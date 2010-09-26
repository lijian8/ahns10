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
#include <QMessageBox>

#include "telemetrythread.h"
#include "ahns_logger.h"
#include "ahns_timeformat.h"

#include <sys/time.h>
#include <sys/socket.h>
#include "inttypes.h"
#include <netinet/in.h>
#include <stdexcept>
#include <ctime>

#include "state.h"
#include "primitive_serialisation.h"
#include "commands.h"

/**
  * @brief Minimal constructor
  * @param parent The object to be set as the parent of the thread, default 0
  */
TelemetryThread::TelemetryThread(QObject * parent) : QThread(parent)
{
    AHNS_DEBUG("TelemetryThread::TelemetryThread(QObject)");
    m_stopped = false;
    m_connected = true;
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
    m_connected = true;

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
    m_connected = true;

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
    quint8 tryCounter = 0;

    if (m_socket != NULL)
    {
        if (isConnected())
        {
            // Send Close Notification to Server
            m_closeReceived = false;
            while ((!m_closeReceived) && (tryCounter < 5))
            {
                tryCounter++;
                sendMessage(COMMAND_CLOSE);
                msleep(1000); // wait for reply
                DataPending();
            }
            if(!m_closeReceived)
            {
                AHNS_ALERT("TelemetryThread::~TelemetryThread() [ SERVER NOTIFICATION FAILED ]");
            }
            else
            {
                m_connected = false;
            }
            m_closeReceived = false; // reset flag
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
void TelemetryThread::clientInitialise()
{
    AHNS_DEBUG("TelemetryThread::clientInitialise()");

    // Initialisation of Flags
    m_configReceived = true;
    m_attitudeReceived = true;
    m_positionReceived = true;
    m_parametersReceived = true;
    m_gainsReceived = true;
    m_saveReceived = true;
    m_getReceived = true;

    try
    {
        // Create Socket
        m_socket = new QUdpSocket(this);
        m_socket->bind(m_clientPort,QUdpSocket::ShareAddress);

        // Connect Data Received Signals
        connect(m_socket,SIGNAL(readyRead()),this,SLOT(DataPending()));

        // Send Connect Request to Server
        quint8 tryCounter = 0;
        m_openReceived = false;
        while ((!m_openReceived) && (tryCounter < 4) && (!m_stopped))
        {
            tryCounter++;
            sendMessage(COMMAND_OPEN);
            msleep(500); // wait for reply
            DataPending();
        }
        if (!m_openReceived)
        {
            throw std::runtime_error("Connection Timed Out");
        }
        m_openReceived = false; // reset flag
    }
    catch(const std::exception& e)
    {
        m_connected = false;
        AHNS_ALERT("TelemetryThread::clientInitialise() [ CLIENT INITIALISATION FAILED ] " << e.what() );
    }
    return;
}

/**
  * @brief Initialise the packet tracking variables
  */
bool TelemetryThread::packetInitialise()
{
    AHNS_DEBUG("TelemetryThread::packetInitialise()");
    bool bReturn = true;
    int i = 0;

    for(i = 0; i < COMMAND_MAX; ++i)
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
  * @param txData Network integer formatted, unsigned char* of data to be transmitted
  * @param txDataByteLength Length in Bytes of the data to be transmitted
  * @return Number of bytes successfully transmitted, -1 for failed
  */

int TelemetryThread::sendMessage(uint32_t type, const unsigned char* txData, int txDataByteLength)
{
    AHNS_DEBUG("TelemetryThread::sendMessage(uint32_t type, char* txData, int txDataByteLength)");

    // Time Stamp
    struct timeval timeStamp;
    gettimeofday(&timeStamp, 0);

    // Pack Time stamp but keep track of started
    char buffer[2*sizeof(int32_t)];
    char* msgTimePointer = buffer;

    // 32-bit Transmission of Time
    //int32_t tv_sec, tv_usec;
    //tv_sec = htonl((int32_t) timeStamp.tv_sec);
    //tv_usec = htonl((int32_t) timeStamp.tv_usec);

    PackInt32((unsigned char*) buffer,(int32_t) timeStamp.tv_sec);
    PackInt32((unsigned char*) &buffer[sizeof(int32_t)],(int32_t) timeStamp.tv_usec);

    // Message 
    type = htonl(type);
    uint32_t *typePointer = &type;
    char* msgTypePointer = (char*) typePointer;

    // Form Datagram
    QByteArray datagram = QByteArray::fromRawData(msgTimePointer,2*sizeof(int32_t));
    datagram += QByteArray::fromRawData(msgTypePointer,sizeof(type));

    // Data
    if (( txData != NULL ) || (txData != 0))
    {
        datagram += QByteArray::fromRawData((char*) txData,txDataByteLength);
    }

    // Speed Data
    m_txData += sizeof(2*sizeof(int32_t)) + sizeof(type) + txDataByteLength;
    time_t currentTime = time(NULL);
    if (timeFirstTxPacket == 0)
    {
        timeFirstTxPacket = time(NULL);
        currentTime = timeFirstTxPacket;
    }

    emit TxEstimate((const double&) (m_txData/difftime(currentTime,timeFirstTxPacket)/1024));

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

    timeval timeStamp;
    int32_t tv_sec, tv_usec;

    uint32_t messageType = -1;
    char* buffer = NULL;
    int discarded = 0;

    while(m_socket->hasPendingDatagrams())
    {        
        // Create Byte Array
        QByteArray byteBuffer(m_socket->pendingDatagramSize(),0);
        m_socket->readDatagram(byteBuffer.data(),byteBuffer.size());
        buffer = byteBuffer.data();

        // Rx Message Time - 32 bit transmission
        buffer += UnpackInt32((unsigned char*) buffer, &tv_sec);
        buffer += UnpackInt32((unsigned char*) buffer, &tv_usec);
        timeStamp.tv_sec = (int64_t) tv_sec;
        timeStamp.tv_usec = (int64_t) tv_usec;

        // Estimate Speed
        if (timeFirstRxPacket == 0)
        {
            timeFirstRxPacket = std::time(NULL);
        }
        time_t currentTime = time(NULL);
        m_rxData += byteBuffer.size();
        emit RxEstimate((const double&) (m_rxData/difftime(currentTime,timeFirstRxPacket)/1024));

        // Determine Message Type
        messageType = ntohl(*(uint32_t*) buffer);
        buffer += sizeof(uint32_t);


        if (messageType < COMMAND_MAX) // Message Type Valid
        {
            if (timercmp(&lastPacket[messageType],&timeStamp, <)) // timeStamp after lastPacket
            {
                //AHNS_DEBUG("TelemetryThread::DataPending() [ PACKET IN ORDER ]");
                discarded = false;
                lastPacket[messageType].tv_sec =  timeStamp.tv_sec;
                lastPacket[messageType].tv_usec =  timeStamp.tv_usec;
            }
            else if (timercmp(&lastPacket[messageType],&timeStamp, >))
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
                uint32_t ackType;
                UnpackUInt32((unsigned char*) buffer, &ackType);
                ackSort(ackType);
                emit NewAckMessage(timeStamp, ackType, discarded);
                break;
            case COMMAND_CLOSE:
                m_closeReceived = true;
                emit NewCloseMessage(timeStamp,discarded);
            case HELI_STATE:
                emit NewHeliState(timeStamp, *(state_t*) buffer, discarded);
                break;
            case FC_STATE:
                fc_state_t receivedState;
                UnpackFCState((unsigned char*) buffer, &receivedState);
                emit NewFCState(timeStamp, receivedState, discarded);
                break;
            case AUTOPILOT_STATE:
                ap_state_t receivedAPState;
                UnpackAPState((unsigned char*) buffer, &receivedAPState);
                emit NewAPState(timeStamp, receivedAPState, discarded);
                break;
            case FAILSAFE:
                emit NewFailSafe(timeStamp,discarded);
                break;
            case GAINS:
                gains_t receivedGains;
                UnpackGains((unsigned char*) buffer, &receivedGains);
                emit NewGains(timeStamp, receivedGains, discarded);
                break;
            case PARAMETERS:
                loop_parameters_t receivedParameters;
                UnpackParameters((unsigned char*) buffer, &receivedParameters);
                emit NewParameters(timeStamp, receivedParameters, discarded);
                break;
            case SENSOR_DATA:
                sensor_data_t receivedSensor;
                UnpackSensorData((unsigned char*) buffer, &receivedSensor);
                emit NewSensorData(timeStamp, receivedSensor, discarded);
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

/**
  * @brief Accessor function for m_connect
  */
bool TelemetryThread::isConnected() const
{
    return m_connected;
}

/**
  * @brief Method to determine the nature of the acknowledgement
  */

bool TelemetryThread::ackSort(const uint32_t& ackType)
{
    bool bRet = true;

    if (ackType == COMMAND_OPEN)
    {
        m_openReceived = true;
    }
    else if (ackType == SET_CONFIG)
    {
        m_configReceived = true;
    }
    else if (ackType == GET_CONFIG)
    {
        m_getReceived = true;
    }
    else if (ackType == SAVE_CONFIG)
    {
        m_saveReceived = true;
    }
    else if (ackType == DESIRED_ATTITUDE)
    {
        m_attitudeReceived = true;
    }
    else if (ackType == DESIRED_POSITION)
    {
        m_positionReceived = true;
    }
    else if (ackType == PARAMETERS)
    {
        m_parametersReceived = true;
    }
    else if (ackType == GAINS)
    {
        m_gainsReceived = true;
    }
    else
    {
        bRet = false;
    }

    return bRet;
}

/**
  * @brief Send Position Setpoint if not awaiting acknowledgement
  */
void TelemetryThread::sendPositionCommand(position_t desiredPosition)
{
    AHNS_DEBUG("void TelemetryThread::sendPositionCommand(position_t desiredPosition)");

    unsigned char buffer[sizeof(position_t)];

    if (m_positionReceived)
    {
        m_txPosition = desiredPosition;

        // Send the Message
        PackPositionData(buffer, &m_txPosition);
        sendMessage(DESIRED_POSITION, buffer, sizeof(position_t));

        // Await Reply
        m_positionReceived = false;
        m_positionTryCount = 1;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySendPosition()));
    }
    else
    {
        AHNS_DEBUG("void TelemetryThread::sendPositionCommand(position_t desiredPosition) [ STILL WAITING ]");
    }

    return;
}

/**
  * @brief Monitor desired Position acknowledgement
  */
void TelemetryThread::retrySendPosition()
{
    AHNS_DEBUG("void TelemetryThread::retrySendPosition()");

    unsigned char buffer[sizeof(position_t)];

    if ((!m_positionReceived) && (m_positionTryCount < (REPLY_TIMEOUT_MS/RETRY_TIME_MS)))
    {
        m_positionTryCount++;

        // Send the Message
        PackPositionData(buffer, &m_txPosition);
        sendMessage(DESIRED_POSITION, buffer, sizeof(position_t));

        // Await Reply
        m_positionReceived = false;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySendPosition()));
    }
    else // stopped either due to count or recieved
    {
        if(m_positionReceived)
        {
            AHNS_DEBUG("void TelemetryThread::retrySendPosition() [ SUCCESS ]");
            emit SentMessage(DESIRED_POSITION);
        }
        else
        {
            m_positionReceived = true;
            AHNS_DEBUG("void TelemetryThread::retrySendPosition() [ FAILED ]");
            emit SentMessage(DESIRED_POSITION,false);
        }
    }
    return;
}


/**
  * @brief Send Attitude Setpoint if not awaiting acknowledgement
  */
void TelemetryThread::sendAttitudeCommand(attitude_t desiredAttitude)
{
    AHNS_DEBUG("void TelemetryThread::sendAttitudeCommand(attitude_t desiredAttitude)");

    unsigned char buffer[sizeof(position_t)];

    if (m_attitudeReceived)
    {
        m_txAttitude = desiredAttitude;

        // Send the Message
        PackAttitudeData(buffer, &m_txAttitude);
        sendMessage(DESIRED_ATTITUDE, buffer, sizeof(attitude_t));

        // Await Reply
        m_attitudeReceived = false;
        m_attitudeTryCount = 1;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySendAttitude()));
    }
    else
    {
        AHNS_DEBUG("void TelemetryThread::sendPositionCommand(position_t desiredPosition) [ STILL WAITING ]");
    }

    return;
}

/**
  * @brief Monitor desired Attitude acknowledgement
  */
void TelemetryThread::retrySendAttitude()
{
    AHNS_DEBUG("void TelemetryThread::retrySendAttitude()");

    unsigned char buffer[sizeof(position_t)];

    if ((!m_attitudeReceived) && (m_attitudeTryCount < (REPLY_TIMEOUT_MS/RETRY_TIME_MS)))
    {
        m_attitudeTryCount++;

        // Send the Message
        PackAttitudeData(buffer, &m_txAttitude);
        sendMessage(DESIRED_ATTITUDE, buffer, sizeof(attitude_t));

        // Await Reply
        m_attitudeReceived = false;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySendAttitude()));
    }
    else // stopped either due to count or recieved
    {
        if(m_attitudeReceived)
        {
            AHNS_DEBUG("void TelemetryThread::retrySendAttitude() [ SUCCESS ]");
            emit SentMessage(DESIRED_ATTITUDE);
        }
        else
        {
            m_attitudeReceived = true;
            AHNS_DEBUG("void TelemetryThread::retrySendAttitude() [ FAILED ]");
            emit SentMessage(DESIRED_ATTITUDE,false);
        }
    }
    return;
}

/**
  * @brief Send Gains and await acknowledgement
  */
void TelemetryThread::sendGains(gains_t desiredGains)
{
    AHNS_DEBUG("void TelemetryThread::sendGains(gains_t desiredGains)");

    unsigned char buffer[sizeof(gains_t)];

    if (m_gainsReceived)
    {
        m_txGains = desiredGains;

        // Send the Message
        PackGains(buffer, &m_txGains);
        sendMessage(GAINS, buffer, sizeof(gains_t));

        // Await Reply
        m_gainsReceived = false;
        m_gainsTryCount = 1;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySendGains()));
    }
    else
    {
        AHNS_DEBUG("void TelemetryThread::sendGains(gains_t desiredGains) [ STILL WAITING ]");
    }

    return;
}

/**
  * @brief Monitor Gains sent for Acknowledgement
  */
void TelemetryThread::retrySendGains()
{
    AHNS_DEBUG("void TelemetryThread::retrySendGains()");

    unsigned char buffer[sizeof(gains_t)];

    if ((!m_gainsReceived) && (m_gainsTryCount < (REPLY_TIMEOUT_MS/RETRY_TIME_MS)))
    {
        m_gainsTryCount++;

        // Send the Message
        PackGains(buffer, &m_txGains);
        sendMessage(GAINS, buffer, sizeof(gains_t));

        // Await Reply
        m_gainsReceived = false;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySendGains()));
    }
    else // stopped either due to count or recieved
    {
        if(m_gainsReceived)
        {
            AHNS_DEBUG("void TelemetryThread::retrySendGains() [ SUCCESS ]");
            emit SentMessage(GAINS);
        }
        else
        {
            m_gainsReceived = true;
            AHNS_DEBUG("void TelemetryThread::retrySendGains() [ FAILED ]");
            emit SentMessage(GAINS,false);
        }
    }

    return;
}



/**
  * @brief Send Loop parameters and await acknowledgement
  */
void TelemetryThread::sendParameters(loop_parameters_t desiredParameters)
{
    AHNS_DEBUG("void TelemetryThread::sendParameters(loop_parameters_t desiredParameters)");

    unsigned char buffer[sizeof(loop_parameters_t)];

    if (m_parametersReceived)
    {
        m_txParameters = desiredParameters;

        // Send the Message
        PackParameters(buffer, &m_txParameters);
        sendMessage(PARAMETERS, buffer, sizeof(loop_parameters_t));

        // Await Reply
        m_parametersReceived = false;
        m_parametersTryCount = 1;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySendParameters()));
    }
    else
    {
        AHNS_DEBUG("void TelemetryThread::sendParameters(loop_parameters_t desiredParameters) [ STILL WAITING ]");
    }

    return;
}

/**
  * @brief Monitor Parameters sent for Acknowledgement
  */
void TelemetryThread::retrySendParameters()
{
    AHNS_DEBUG("void TelemetryThread::retrySendParameters()");

    unsigned char buffer[sizeof(loop_parameters_t)];

    if ((!m_parametersReceived) && (m_parametersTryCount < (REPLY_TIMEOUT_MS/RETRY_TIME_MS)))
    {
        m_parametersTryCount++;

        // Send the Message
        PackParameters(buffer, &m_txParameters);
        sendMessage(PARAMETERS, buffer, sizeof(loop_parameters_t));

        // Await Reply
        m_parametersReceived = false;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySendParameters()));
    }
    else // stopped either due to count or recieved
    {
        if(m_parametersReceived)
        {
            AHNS_DEBUG("void TelemetryThread::retrySendParameters() [ SUCCESS ]");
            emit SentMessage(PARAMETERS);
        }
        else
        {
            m_parametersReceived = true;
            AHNS_DEBUG("void TelemetryThread::retrySendParameters() [ FAILED ]");
            emit SentMessage(PARAMETERS, false);
        }
    }

    return;
}

/**
  * @brief Send AP configuration if not waiting for acknowledgement
  */
void TelemetryThread::sendSetAPConfig(ap_config_t apConfig)
{
    AHNS_DEBUG("void TelemetryThread::sendSetAPConfig(ap_config_t apConfig)");

    unsigned char buffer[sizeof(ap_config_t)];

    if (m_configReceived)
    {
        m_txAPConfig = apConfig;

        // Send the Message
        PackAPConfigData(buffer, &m_txAPConfig);
        sendMessage(SET_CONFIG, buffer, sizeof(ap_config_t));

        // Await Reply
        m_configReceived = false;
        m_configTryCount = 1;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySetAPConfig()));
    }
    else
    {
        AHNS_DEBUG("void TelemetryThread::sendSetAPConfig(ap_config_t apConfig) [ STILL WAITING ]");
    }
    return;
}

/**
  * @brief Monitor SET_CONFIG configuration acknowledgement
  */
void TelemetryThread::retrySetAPConfig()
{
    AHNS_DEBUG("void TelemetryThread::retrySetAPConfig(ap_config_t apConfig)");

    unsigned char buffer[sizeof(ap_config_t)];

    if ((!m_configReceived) && (m_configTryCount < (REPLY_TIMEOUT_MS/RETRY_TIME_MS)))
    {
        m_configTryCount++;

        // Send the Message
        PackAPConfigData(buffer, &m_txAPConfig);
        sendMessage(SET_CONFIG, buffer, sizeof(ap_config_t));

        // Await Reply
        m_configReceived = false;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySetAPConfig()));
    }
    else // stopped either due to count or recieved
    {
        if(m_configReceived)
        {
            AHNS_DEBUG("void TelemetryThread::retrySetAPConfig(ap_config_t apConfig) [ SUCCESS ]");
            emit SentMessage(SET_CONFIG);
        }
        else
        {
            m_configReceived = true;
            AHNS_DEBUG("void TelemetryThread::retrySetAPConfig(ap_config_t apConfig) [ FAILED " << m_configTryCount << " ]");
            emit SentMessage(SET_CONFIG,false);
        }
    }
    return;
}


/**
  * @brief Send request of complete AP config data
  */
void TelemetryThread::sendGetConfig()
{
    AHNS_DEBUG("void TelemetryThread::sendGetConfig()");

    if (m_getReceived)
    {
        // Send the Message
        sendMessage(GET_CONFIG);

        // Await Reply
        m_getReceived = false;
        m_getTryCount = 1;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retryGetConfig()));
    }
    else
    {
        AHNS_DEBUG("void TelemetryThread::sendGetConfig() [ STILL WAITING ]");
    }

    return;
}

/**
  * @brief Monitor Get Config acknowledgement
  */
void TelemetryThread::retryGetConfig()
{
    AHNS_DEBUG("void TelemetryThread::retryGetConfigParameters()");

    if ((!m_getReceived) && (m_getTryCount < (REPLY_TIMEOUT_MS/RETRY_TIME_MS)))
    {
        m_getTryCount++;

        // Send the Message
        sendMessage(GET_CONFIG);

        // Await Reply
        m_getReceived = false;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retryGetConfig()));
    }
    else // stopped either due to count or recieved
    {
        if(m_getReceived)
        {
            AHNS_DEBUG("void TelemetryThread::retryGetConfigParameters() [ SUCCESS ]");
            emit SentMessage(GET_CONFIG);
        }
        else
        {
            m_getReceived = true;
            AHNS_DEBUG("void TelemetryThread::retryGetConfigParameters() [ FAILED " << m_getTryCount << " ]");
            emit SentMessage(GET_CONFIG, false);
        }
    }

    return;
}

/**
  * @brief Send Command for FC to save all settings
  */
void TelemetryThread::sendSaveConfig()
{
    AHNS_DEBUG("void TelemetryThread::sendSaveConfig()");

    if (m_saveReceived)
    {
        // Send the Message
        sendMessage(SAVE_CONFIG);

        // Await Reply
        m_saveReceived = false;
        m_saveTryCount = 1;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySaveConfig()));
    }
    else
    {
        AHNS_DEBUG("void TelemetryThread::sendSaveConfig() [ STILL WAITING ]");
    }
    
    return;
}

/**
  * @brief Monitor Save Config acknowledgement
  */
void TelemetryThread::retrySaveConfig()
{
    AHNS_DEBUG("void TelemetryThread::retrySaveConfig()");

    if ((!m_saveReceived) && (m_saveTryCount < (REPLY_TIMEOUT_MS/RETRY_TIME_MS)))
    {
        m_saveTryCount++;

        // Send the Message
        sendMessage(SAVE_CONFIG);

        // Await Reply
        m_saveReceived = false;
        QTimer::singleShot(RETRY_TIME_MS,this,SLOT(retrySaveConfig()));
    }
    else // stopped either due to count or recieved
    {
        if(m_saveReceived)
        {
            AHNS_DEBUG("void TelemetryThread::retrySaveConfig() [ SUCCESS ]");
            emit SentMessage(SAVE_CONFIG);
        }
        else
        {
            m_saveReceived = true;
            AHNS_DEBUG("void TelemetryThread::retrySaveConfig() [ FAILED " << m_saveTryCount << " ]");
            emit SentMessage(SAVE_CONFIG, false);
        }
    }

    return;
}

/**
  * @brief Send the Vicon Data
  * Data will be sent fast enough to ensure a lost packet
  * will not affect controller performance.
  */
void TelemetryThread::sendViconState(const vicon_state_t viconState)
{
  AHNS_DEBUG("void TelemetryThread::sendViconState()");

  unsigned char buffer[sizeof(vicon_state_t)];

  AHNS_ALERT("Vicon Position: " << viconState.x << " " << viconState.y << " " << viconState.z);
  AHNS_ALERT("Vicon Velocity: " << viconState.vx << " " << viconState.vy << " " << viconState.vz);
  AHNS_ALERT("Vicon Angles: " << viconState.phi << " " << viconState.theta << " " << viconState.psi);

  // Send the Message
  PackViconState(buffer, &viconState);
  sendMessage(VICON_STATE, buffer, sizeof(vicon_state_t));
  emit SentMessage(VICON_STATE);
}
