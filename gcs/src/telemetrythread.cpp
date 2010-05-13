/**
 * @file   telemetrythread.cpp
 * @author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * TelemetryThread class implementation
 */

#include <QThread>
#include <QMutexLocker>
#include <QtNetwork>

#include "telemetrythread.h"
#include "ahns_logger.h"
#include "ahns_timeformat.h"

#include "state.h"
#include "commands.h"

#include <sys/time.h>
#include <sys/socket.h>
#include "inttypes.h"
#include <netinet/in.h>
#include <stdexcept>
#include <ctime>


/**
  * @brief Minimal constructor
  * @param parent The object to be set as the parent of the thread, default 0
  */
TelemetryThread::TelemetryThread(QObject * parent) : QThread(parent)
{
    AHNS_DEBUG("TelemetryThread::TelemetryThread(QObject)");
    m_stopped = false;
    packetInitialise();

    moveToThread(this);
    start();
}

/**
  * @brief Constructor to start a new UDP Telemetry Thread from ints
  * @param serverPort The port of the server that the connection will be to
  * @param serverIP The IP of the server; gumstix heliconnect 192.168.2.2
  * @param clientPort The port of the client that will be used
  * @param clientIP The IP address of the client, should be autodetected
  * @param parent The object to be set as the parent of the thread, default 0
  */
TelemetryThread::TelemetryThread(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP, QObject * parent) : QThread(parent)
{
    AHNS_DEBUG("TelemetryThread::TelemetryThread(server,client)");
    QMutexLocker lock(&m_mutex);
    m_stopped = false;

    // Assign members
    m_serverIP.setAddress(serverIP);
    m_serverPort = serverPort;
    m_clientIP.setAddress(clientIP);
    m_clientPort = clientPort;
    packetInitialise();

    moveToThread(this);
    start();
}

/**
  * @brief Parameter Copy Constructor
  * @param serverPort Port to connect to on the server
  * @param serverIP QHostAddress of the server
  * @param clientPort Port of the client (ground control station) to use
  * @param clientIP QHostAddress of the client ground control station
  * @param parent The object to be set as the parent of the thread, default 0
  */
TelemetryThread::TelemetryThread(quint16& serverPort, QHostAddress& serverIP, quint16& clientPort, QHostAddress& clientIP, QObject * parent) : QThread(parent)
{
    AHNS_DEBUG("TelemetryThread::TelemetryThread(server,client QHostAddress)");
    QMutexLocker lock(&m_mutex);
    m_stopped = false;

    // Assign members
    m_serverIP = serverIP;
    m_serverPort = serverPort;
    m_clientIP = clientIP;
    m_clientPort = clientPort;
    packetInitialise();

    moveToThread(this);
    start();
}

/**
  * @brief Destructor stops the thread
  * Sockets are children of the thread so they will be closed and destroyed automatically
  */
TelemetryThread::~TelemetryThread()
{
    AHNS_DEBUG("TelemetryThread::~TelemetryThread()");
    QMutexLocker locker(&m_mutex);
    m_stopped = true;

    if (m_socket != NULL)
    {
        if(sendMessage(COMMAND_CLOSE) < 0) // Tell server the socket is closing
        {
            AHNS_ALERT("TelemetryThread::~TelemetryThread() [ SERVER NOTIFICATION FAILED ]");
        }
        m_socket->close();
    }
}

/**
  * @brief Loops the telemetry thread routine
  *
  * Begins by attempting to connect to the server with  a 25 sec timeout.
  */
void TelemetryThread::run()
{
    AHNS_DEBUG("TelemetryThread::run() [ STARTED ]");
    AHNS_DEBUG("TelemetryThread::run() [ TELEMETRY THREAD ID " << (int) QThread::currentThreadId() << " ]");

    QTimer::singleShot(0,this,SLOT(clientInitialise()));

    while (!m_stopped)
    {
        exec();
    }

    AHNS_DEBUG("TelemetryThread::run() [ COMPLETED ] ");
    return;
}

/**
  * @brief Initialise the UDP Socket and Join Server
  * Binds the m_socket to a local port, m_clientPort
  * The connection to m_socket->readReady must be Qt::DirectConnection
  * the socket is therefore best created as a child of the TelemetryThread
  * @return True for a successful server pairing, false otherwise
  */
bool TelemetryThread::clientInitialise()
{
    AHNS_DEBUG("TelemetryThread::clientInitialise()");
    bool bReturn = true;

    try
    {
        // Create Socket
        m_socket = new QUdpSocket(this);
        m_socket->bind(m_clientPort,QUdpSocket::ShareAddress);

        // Connect Data Received Signals
        connect(m_socket,SIGNAL(readyRead()),this,SLOT(DataPending()));

        // Send Connect Request to Server
        quint8 tryCounter = 0;
        m_ackReceived = false;
        while ((!m_ackReceived) && (tryCounter < 50) && (!m_stopped))
        {
            tryCounter++;
            sendMessage(COMMAND_OPEN);
            msleep(250); // wait for reply
            DataPending();
        }
        m_ackReceived = false; // reset flag
    }
    catch(const std::exception& e)
    {
        AHNS_ALERT("TelemetryThread::clientInitialise() [ CLIENT INITIALISATION FAILED ] " << e.what() );
        bReturn = false;
    }
    return bReturn;
}

/**
  * @brief Initialise the packet tracking variables
  */
bool TelemetryThread::packetInitialise()
{
    AHNS_DEBUG("TelemetryThread::packetInitialise()");
    bool bReturn = true;
    int i = 0;

    for(i = 0; i <= COMMAND_MAX; ++i)
    {
        lastPacket[i].tv_sec = 0;
        lastPacket[i].tv_usec = 0;
    }

    m_rxData = 0;
    m_txData = 0;
    timeFirstRxPacket = 0;
    timeFirstTxPacket = 0;

    return bReturn;
}

/**
  * @brief Send time stamped, data of a given command type
  * @param type Message type as defined in commands.h
  * @param txData Network integer formatted, char* of data to be transmitted
  * @param txDataByteLength Length in Bytes of the data to be transmitted
  * @return Number of bytes successfully transmitted, -1 for failed
  */

int TelemetryThread::sendMessage(uint32_t type, const char* txData, int txDataByteLength)
{
    AHNS_DEBUG("TelemetryThread::sendMessage(uint32_t type, char* txData, int txDataByteLength)");

    // Time Stamp
    struct timeval timeStamp;
    gettimeofday(&timeStamp, 0 );
    timeStamp.tv_sec	= htonl(timeStamp.tv_sec);
    timeStamp.tv_usec	= htonl(timeStamp.tv_usec);
    struct timeval* timeStampPointer = &timeStamp;
    char* msgTimePointer = (char*) timeStampPointer;

    // Message Type
    type = htonl(type);
    uint32_t *typePointer = &type;
    char* msgTypePointer = (char*) typePointer;

    // Form Datagram
    QByteArray datagram = QByteArray::fromRawData(msgTimePointer,sizeof(timeStamp));
    datagram += QByteArray::fromRawData(msgTypePointer,sizeof(type));

    // Data
    if (( txData != NULL ) || (txData != 0))
    {
        datagram += QByteArray::fromRawData(txData,txDataByteLength);
    }

    // Speed Data
    m_txData += sizeof(timeStamp) + sizeof(type) + txDataByteLength;
    if (timeFirstTxPacket == 0)
    {
        timeFirstTxPacket = time(NULL);
    }

    return m_socket->writeDatagram(datagram,m_serverIP,m_serverPort);
}

/**
  * @brief Slot to be called when new data arrives on m_socket
  * The main thread is notified of the datagram's arrival through the emission of
  * a suitable signal.
  */
void TelemetryThread::DataPending()
{
    AHNS_DEBUG("TelemetryThread::DataPending() [ Thead = " << QThread::currentThreadId() << " ]");

    timeval* timeStampPointer = NULL;
    uint32_t messageType = -1;
    char* buffer = NULL;
    int discarded = 0;

    while(m_socket->hasPendingDatagrams())
    {

        // Create Byte Array
        QByteArray byteBuffer(m_socket->pendingDatagramSize(),0);
        m_socket->readDatagram(byteBuffer.data(),byteBuffer.size());
        buffer = byteBuffer.data();

        // Determine Message Time
        timeStampPointer = (struct timeval*) buffer;
        buffer += sizeof(struct timeval);

        timeStampPointer->tv_sec = ntohl(timeStampPointer->tv_sec);
        timeStampPointer->tv_usec = ntohl(timeStampPointer->tv_usec);


        // Estimate Speed
        if (timeFirstRxPacket == 0)
        {
            timeFirstRxPacket = std::time(NULL);
        }
        time_t currentTime = time(NULL);
        m_rxData += byteBuffer.size();
        emit RxEstimate((const double&) (m_rxData/difftime(currentTime,timeFirstRxPacket)));

        // Determine Message Type
        messageType = ntohl(*(uint32_t*) buffer);
        buffer += sizeof(uint32_t);


        if ((0 <= messageType) && (messageType <= 256)) // Message Type Valid
        {
            if (timercmp(&lastPacket[messageType],timeStampPointer, <)) // timeStampPointer after lastPacket
            {
                //AHNS_DEBUG("TelemetryThread::DataPending() [ PACKET IN ORDER ]");
                discarded = false;
                lastPacket[messageType].tv_sec =  timeStampPointer->tv_sec;
                lastPacket[messageType].tv_usec =  timeStampPointer->tv_usec;
            }
            else if (timercmp(&lastPacket[messageType],timeStampPointer, >))
            {
                discarded = -1;
                //AHNS_DEBUG("TelemetryThread::DataPending() [ LATE PACKET ]");
            }
            else
            {
                discarded = -2;
                //AHNS_DEBUG("TelemetryThread::DataPending() [ DUPLICATE PACKET ]");
            }

            // Parse Structure
            switch (messageType)
            {
            case COMMAND_ACK:
                m_ackReceived = true; 
                emit NewAckMessage(*timeStampPointer,discarded);
                break;
            case HELI_STATE:
                emit NewHeliState(*timeStampPointer, *(state_t*) buffer, discarded);
                break;
            case FC_STATE:
                emit NewFCState(*timeStampPointer, *(fc_state_t*) buffer, discarded);
                break;
            case AUTOPILOT_STATE:
                emit NewAPState(*timeStampPointer, *(ap_state_t*) buffer, discarded);
                break;
            case FAILSAFE:
                emit NewFailSafe(*timeStampPointer,discarded);
                break;
            case ATTITUDE_GAIN_ROLL:
                emit NewRollGain(*timeStampPointer, *(gains_t*) buffer, discarded);
                break;
            case ATTITUDE_GAIN_PITCH:
                emit NewPitchGain(*timeStampPointer, *(gains_t*) buffer, discarded);
                break;
            case ATTITUDE_GAIN_YAW:
                emit NewYawGain(*timeStampPointer, *(gains_t*) buffer, discarded);
                break;
            case GUIDANCE_GAIN_X:
                emit NewGuidanceXGain(*timeStampPointer, *(gains_t*) buffer, discarded);
                break;
            case GUIDANCE_GAIN_Y:
                emit NewGuidanceYGain(*timeStampPointer, *(gains_t*) buffer, discarded);
                break;
            case GUIDANCE_GAIN_Z:
                emit NewGuidanceZGain(*timeStampPointer, *(gains_t*) buffer, discarded);
                break;
            case ATTITUDE_PARAMETERS_ROLL:
                emit NewRollParameters(*timeStampPointer, *(loop_parameters_t*) buffer, discarded);
                break;
            case ATTITUDE_PARAMETERS_PITCH:
                emit NewPitchParameters(*timeStampPointer, *(loop_parameters_t*) buffer, discarded);
                break;
            case ATTITUDE_PARAMETERS_YAW:
                emit NewYawParameters(*timeStampPointer, *(loop_parameters_t*) buffer, discarded);
                break;
            case GUIDANCE_PARAMETERS_X:
                emit NewGuidanceXParameters(*timeStampPointer, *(loop_parameters_t*) buffer, discarded);
                break;
            case GUIDANCE_PARAMETERS_Y:
                emit NewGuidanceYParameters(*timeStampPointer, *(loop_parameters_t*) buffer, discarded);
                break;
            case GUIDANCE_PARAMETERS_Z:
                emit NewGuidanceZParameters(*timeStampPointer, *(loop_parameters_t*) buffer, discarded);
                break;
            default:
                AHNS_DEBUG("TelemetryThread::DataPending() [ MESSAGE TYPE NOT MATCHED ]");
                break;
            }
        }
        else
        {
            discarded = -3;
            AHNS_ALERT("TelemetryThread::DataPending() [ UNKNOWN MESSAGE TYPE ]");
        }
    }
    return;
}

/**
  * @brief Method to call to stop the thread
  *
  * This needs to be called from the telemetry thread to avoid blocking the GUI
  */
void TelemetryThread::stop()
{
    AHNS_DEBUG("TelemetryThread::stop()");

    QMutexLocker locker (&m_mutex);
    m_stopped = true;
    quit();

    return;
}

/**
  * @brief Accessor function for m_clientPort
  */
quint16 TelemetryThread::readClientPort() const
{
    return m_clientPort;
}

/**
  * @brief Accessor function for m_clientIP
  */
QHostAddress TelemetryThread::readClientIP() const
{
    return m_clientIP;
}

/**
  * @brief Accessort function for m_serverPort
  */
quint16 TelemetryThread::readServerPort() const
{
    return m_serverPort;
}

/**
  * @brief Accessor function for m_serverIP
  */
QHostAddress TelemetryThread::readServerIP() const
{
    return m_serverIP;
}
