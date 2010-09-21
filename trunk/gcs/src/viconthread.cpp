/**
 * @file   telemetrythread.cpp
 * @author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-08-14 14:58:08 +1000 (Sat, 14 Aug 2010) $
 * $Rev: 317 $
 * $Id: telemetrythread.cpp 317 2010-08-14 04:58:08Z tlmolloy $
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

#include "viconthread.h"
#include "ahns_logger.h"
#include "ahns_timeformat.h"

#include <sys/time.h>
#include <sys/socket.h>
#include "inttypes.h"
#include <netinet/in.h>
#include <stdexcept>
#include <ctime>

#include "state.h"
#include "Client.h"

/**
  * @brief Default constructor
  * @param parent The object to be set as the parent of the thread, default 0
  */
ViconThread::ViconThread(QObject * parent)
    : QThread(parent), m_stopped(false), m_connected(true)
{
    AHNS_DEBUG("ViconThread::ViconThread(QObject)");

    moveToThread(this);
    start();
}

/**
  * @brief Constructor to start a new UDP Telemetry Thread from ints
  * @param serverPort The port of the server that the connection will be to
  * @param serverIP The IP of the server; gumstix heliconnect 192.168.2.2
  * @param parent The object to be set as the parent of the thread, default 0
  */
ViconThread::ViconThread(quint16& serverPort, QString& serverIP, QObject * parent)
    : QThread(parent), m_stopped(false), m_connected(true),  m_serverIP(serverIP), m_serverPort(serverPort)
{
    AHNS_DEBUG("ViconThread::ViconThread(server,client)");
    QMutexLocker lock(&m_mutex);

    moveToThread(this);
    start();
}

/**
  * @brief Parameter Copy Constructor
  * @param serverPort Port to connect to on the server
  * @param serverIP QHostAddress of the server
  * @param parent The object to be set as the parent of the thread, default 0
  */
ViconThread::ViconThread(quint16& serverPort, QHostAddress& serverIP, QObject * parent)
    : QThread(parent), m_stopped(false), m_connected(true),  m_serverIP(serverIP), m_serverPort(serverPort)
{
    AHNS_DEBUG("ViconThread::ViconThread(server,client QHostAddress)");
    QMutexLocker lock(&m_mutex);

    moveToThread(this);
    start();
}

/**
  * @brief Destructor stops the thread
  * Sockets are children of the thread so they will be closed and destroyed automatically
  */
ViconThread::~ViconThread()
{
    AHNS_DEBUG("ViconThread::~ViconThread()");
    QMutexLocker locker(&m_mutex);

    if (m_socket != NULL)
    {
        m_socket->close();
    }
}

/**
  * @brief Loops the telemetry thread routine
  *
  * Begins by attempting to connect to the server with  a 25 sec timeout.
  */
void ViconThread::run()
{
    AHNS_DEBUG("ViconThread::run() [ STARTED ]");
    AHNS_DEBUG("ViconThread::run() [ VICON THREAD ID " << (int) QThread::currentThreadId() << " ]");

    while (!m_stopped)
    {
        //exec();
    }

    AHNS_DEBUG("ViconThread::run() [ COMPLETED ] ");
    return;
}

/**
  * @brief Method to call to stop the thread
  *
  * This needs to be called from the ViconThread thread to avoid blocking the GUI
  */
void ViconThread::stop()
{
    AHNS_DEBUG("ViconThread::stop()");

    QMutexLocker locker (&m_mutex);
    m_stopped = true;
    quit();

    return;
}

/**
  * @brief Accessort function for m_serverPort
  */
quint16 ViconThread::readServerPort() const
{
    return m_serverPort;
}

/**
  * @brief Accessor function for m_serverIP
  */
QHostAddress ViconThread::readServerIP() const
{
    return m_serverIP;
}

/**
  * @brief Accessor function for m_connect
  */
bool ViconThread::isConnected() const
{
    return m_connected;
}
