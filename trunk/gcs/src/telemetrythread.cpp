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

#include "telemetrythread.h"
#include "ahns_logger.h"

#include "state.h"
#include "commands.h"

#include <sys/time.h>
#include <sys/socket.h>
#include "inttypes.h"
#include <netinet/in.h>


/**
  * @brief Default constructor - does nothing
  */
TelemetryThread::TelemetryThread()
{
    AHNS_DEBUG("TelemetryThread::TelemetryThread()");
    m_stopped = false;
    moveToThread(this);
}

/**
  * @brief Constructor to start a new UDP Telemetry Thread from ints
  * @param serverPort The port of the server that the connection will be to
  * @param serverIP The IP of the server; gumstix heliconnect 192.168.2.2
  * @param clientPort The port of the client that will be used
  * @param clientIP The IP address of the client, should be autodetected
  */
TelemetryThread::TelemetryThread(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP) : QThread()
{
    AHNS_DEBUG("TelemetryThread::TelemetryThread(server,client)");
    QMutexLocker lock(&m_mutex);
    m_stopped = false;

    moveToThread(this);

    // Assign members
    m_serverIP.setAddress(serverIP);
    m_serverPort = serverPort;
    m_clientIP.setAddress(clientIP);
    m_clientPort = clientPort;

    // Create the Sockets
    rxInitialise();
    txInitialise();

}

/**
  * @brief Parameter Copy Constructor
  * @param serverPort Port to connect to on the server
  * @param serverIP QHostAddress of the server
  * @param clientPort Port of the client (ground control station) to use
  * @param clientIP QHostAddress of the client ground control station
  */
TelemetryThread::TelemetryThread(quint16& serverPort, QHostAddress& serverIP, quint16& clientPort, QHostAddress& clientIP) : QThread()
{
    AHNS_DEBUG("TelemetryThread::TelemetryThread(server,client QHostAddress)");
    QMutexLocker lock(&m_mutex);
    m_stopped = false;

    moveToThread(this);

    // Assign members
    m_serverIP = serverIP;
    m_serverPort = serverPort;
    m_clientIP = clientIP;
    m_clientPort = clientPort;
}

/**
  * @brief Destructor stops the thread
  */
TelemetryThread::~TelemetryThread()
{
    AHNS_DEBUG("TelemetryThread::~TelemetryThread()");
    QMutexLocker locker(&m_mutex);
    m_stopped = true;

    if (m_rxSocket != NULL)
    {
        m_rxSocket->close();
        delete m_rxSocket;
        m_rxSocket = NULL;
    }

    if(m_txSocket != NULL)
    {
        m_txSocket->close();
        delete m_txSocket;
        m_txSocket = NULL;
    }
}

/**
  * @brief Loops the telemetry thread routine
  */
void TelemetryThread::run()
{
    AHNS_DEBUG("TelemetryThread::run() [ STARTED ]");

    while (!m_stopped)
    {
        //AHNS_DEBUG("TelemetryThread::run() [ NOT STOPPED ]");
        msleep(20);
    }

    AHNS_DEBUG("TelemetryThread::run() [ COMPLETED ] ");
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

/**
  * @brief Initialise the UDP Socket for transmission
  * Creates the m_txSocket
  */
void TelemetryThread::txInitialise()
{
    AHNS_DEBUG("TelemetryThread::txInitialise()");
    try
    {
        m_txSocket = new QUdpSocket();
        //m_txSocket->connectToHost(m_severIP,m_serverPort);
    }
    catch(const std::exception& e)
    {
        AHNS_ALERT("TelemetryThread::txInitialise() [ TX PORT FAILED ] " << e.what() );
    }
    return;
}

/**
  * @brief Initialise the UDP Socket for receiver from server
  * Binds the m_rxSocket to a local port
  */
void TelemetryThread::rxInitialise()
{
    AHNS_DEBUG("TelemetryThread::rxInitialise()");
    try
    {
        m_rxSocket = new QUdpSocket();
        m_rxSocket->bind(m_clientPort);
        connect(m_rxSocket,SIGNAL(readyRead()),this,SLOT(DataPending()));
    }
    catch(const std::exception& e)
    {
        AHNS_ALERT("TelemetryThread::rxInitialise() [ RX PORT FAILED ] " << e.what() );
    }

    return;
}

/**
  * @brief Slot to be called when new data arrives on m_rxSocket
  */
void TelemetryThread::DataPending()
{
    AHNS_DEBUG("TelemetryThread::DataPending()");

    while(m_rxSocket->hasPendingDatagrams())
    {
        struct timeval *timeStamp = NULL;
        uint32_t messageType = -1;

        // Create Buffer
        QByteArray byteBuffer(m_rxSocket->pendingDatagramSize(),0);
        m_rxSocket->readDatagram(byteBuffer.data(),byteBuffer.size());
        char* buffer = byteBuffer.data();

        // Determine Message Time
        timeStamp = (struct timeval*) buffer;
        timeStamp->tv_sec = ntohl(timeStamp->tv_sec);
        timeStamp->tv_usec = ntohl(timeStamp->tv_usec);
        buffer += sizeof(struct timeval);
        AHNS_DEBUG("TelemetryThread::DataPending() { Message Time" << timeStamp->tv_sec << timeStamp->tv_usec <<" }");

        // Determine Message Type
        messageType = ntohl(*(uint32_t*) buffer);
        buffer += sizeof(uint32_t);
        AHNS_DEBUG("TelemetryThread::DataPending() { Message Type" << messageType << " }");

        // Parse
    }
    return;
}
