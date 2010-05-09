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

#include "state.h"
#include "commands.h"

#include <sys/time.h>
#include <sys/socket.h>
#include "inttypes.h"
#include <netinet/in.h>
#include <stdexcept>


/**
  * @brief Minimal constructor
  * @param parent The object to be set as the parent of the thread, default 0
  */
TelemetryThread::TelemetryThread(QObject * parent) : QThread(parent)
{
    AHNS_DEBUG("TelemetryThread::TelemetryThread(QObject)");
    m_stopped = false;
    //moveToThread(this);
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

    //moveToThread(this);

    // Assign members
    m_serverIP.setAddress(serverIP);
    m_serverPort = serverPort;
    m_clientIP.setAddress(clientIP);
    m_clientPort = clientPort;

    // Create and pair the socket
    if (!clientInitialise())
    {
        throw std::runtime_error("Client Initialisation Failed");
    }
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

    //moveToThread(this);

    // Assign members
    m_serverIP = serverIP;
    m_serverPort = serverPort;
    m_clientIP = clientIP;
    m_clientPort = clientPort;

    // Create and pair the socket
    if (!clientInitialise())
    {
        throw std::runtime_error("Client Initialisation Failed");
    }
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
        //delete m_socket;
        //m_socket = NULL;
    }
}

/**
  * @brief Loops the telemetry thread routine
  */
void TelemetryThread::run()
{
    AHNS_DEBUG("TelemetryThread::run() [ STARTED ]");
    AHNS_DEBUG("TelemetryThread::run() [ TELEMETRY THREAD ID " << (int) QThread::currentThreadId() << " ]");

    while (!m_stopped)
    {
        // Check for telemetry Heart Beat
        msleep(500);
    }

    AHNS_DEBUG("TelemetryThread::run() [ COMPLETED ] ");
    return;
}

/**
  * @brief Initialise the UDP Socket and open connection to the server
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
        if (sendMessage(COMMAND_OPEN) < 0)
        {
            throw std::runtime_error("Server Request Failed");
        }
    }
    catch(const std::exception& e)
    {
        AHNS_ALERT("TelemetryThread::clientInitialise() [ CLIENT INITIALISATION FAILED ] " << e.what() );
        bReturn = false;
    }
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

    return m_socket->writeDatagram(datagram,m_serverIP,m_serverPort);
}

/**
  * @brief Slot to be called when new data arrives on m_socket
  * The main thread is notified of the datagram's arrival through the emission of
  * a suitable signal.
  */
void TelemetryThread::DataPending()
{
    AHNS_DEBUG("TelemetryThread::DataPending()");

    struct timeval *timeStamp = NULL;
    uint32_t messageType = -1;
    char* buffer = NULL;

    while(m_socket->hasPendingDatagrams())
    {
        // Create Byte Array
        QByteArray byteBuffer(m_socket->pendingDatagramSize(),0);
        m_socket->readDatagram(byteBuffer.data(),byteBuffer.size());
        buffer = byteBuffer.data();

        // Determine Message Time
        timeStamp = (struct timeval*) buffer;
        timeStamp->tv_sec = ntohl(timeStamp->tv_sec);
        timeStamp->tv_usec = ntohl(timeStamp->tv_usec);
        buffer += sizeof(struct timeval);
        AHNS_DEBUG("TelemetryThread::DataPending() [ Message Time " << timeStamp->tv_sec << ":" << timeStamp->tv_usec <<" ]");

        // Determine Message Type
        messageType = ntohl(*(uint32_t*) buffer);
        buffer += sizeof(uint32_t);
        AHNS_DEBUG("TelemetryThread::DataPending() [ Message Type " << messageType << " ]");

       // Parse Structure
       switch (messageType)
       {
       case COMMAND_ACK:
           break;
       case HELI_STATE:
           emit NewHeliState((const state_t*) buffer);
           break;
       default:
           AHNS_ALERT("TelemetryThread::DataPending() [ MESSAGE TYPE PARSE FAILED ]");
       }
    }
    return;
}

/**
  * @brief Method to call to stop the thread
  */
void TelemetryThread::stop()
{
    AHNS_DEBUG("TelemetryThread::stop()");

    QMutexLocker locker(&m_mutex);
    m_stopped = true;
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
