/**
 * \file   telemetrythread.cpp
 * \author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * cTelemetryThread class implementation
 */

#include <QThread>
#include <QMutexLocker>

#include "telemetrythread.h"
#include "ahns_logger.h"
#include "state.h"

/**
  * \brief Default constructor - does nothing
  */
cTelemetryThread::cTelemetryThread()
{
    AHNS_DEBUG("cTelemetryThread::cTelemetryThread()");
    bStopped = false;
    moveToThread(this);
}

/**
  * @brief Constructor to start a new UDP Telemetry Thread from ints
  * @param serverPort The port of the server that the connection will be to
  * @param serverIP The IP of the server; gumstix heliconnect 192.168.2.2
  * @param clientPort The port of the client that will be used
  * @param clientIP The IP address of the client, should be autodetected
  */
cTelemetryThread::cTelemetryThread(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP) : QThread()
{
    QMutexLocker lock(&m_mutex);
    AHNS_DEBUG("cTelemetryThread::cTelemetryThread(server,client)");
    bStopped = false;

    m_serverIP.setAddress(serverIP);
    m_serverPort = serverPort;

    m_clientIP.setAddress(clientIP);
    m_clientPort = clientPort;
    moveToThread(this);
}

/**
  * @brief Parameter Copy Constructor
  */
cTelemetryThread::cTelemetryThread(quint16& serverPort, QHostAddress& serverIP, quint16& clientPort, QHostAddress& clientIP) :QThread()
{
    QMutexLocker lock(&m_mutex);
    AHNS_DEBUG("cTelemetryThread::cTelemetryThread(server,client QHostAddress)");
    bStopped = false;

    m_serverIP = serverIP;
    m_serverPort = serverPort;

    m_clientIP = clientIP;
    m_clientPort = clientPort;
    moveToThread(this);
}

/**
  * \brief Destructor stops the thread
  */
cTelemetryThread::~cTelemetryThread()
{
    QMutexLocker locker(&m_mutex);
    bStopped = true;
}

/**
  * \brief Loops the telemetry thread routine
  */
void cTelemetryThread::run()
{
    AHNS_DEBUG("cTelemetryThread::run() [ STARTED ]");

    while (!bStopped)
    {
        //AHNS_DEBUG("cTelemetryThread::run() [ NOT STOPPED ]");
    }

    AHNS_DEBUG("cTelemetryThread::run() [ COMPLETED ] ");
    return;
}

void cTelemetryThread::stop()
{
    AHNS_DEBUG("cTelemetryThread::stop()");

    QMutexLocker locker(&m_mutex);
    bStopped = true;
    return;
}

quint16 cTelemetryThread::readClientPort()
{
    return m_clientPort;
}

QHostAddress cTelemetryThread::readClientIP() const
{
    return m_clientIP;
}

quint16 cTelemetryThread::readServerPort() const
{
    return m_serverPort;
}

QHostAddress cTelemetryThread::readServerIP() const
{
    return m_serverIP;
}
