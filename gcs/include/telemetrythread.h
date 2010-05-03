/**
 * \file   telemetrythread.h
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
 * Header file for the cTelemetryThread.
 */

#ifndef TELEMETRYTHREAD_H
#define TELEMETRYTHREAD_H

#include <QtGlobal>
#include <QThread>
#include <QObject>
#include <QHostAddress>
#include <QMutex>
#include <QTimer>

/**
  * \brief Thead Class Inherided from QThread
  */
class cTelemetryThread : public QThread
{
    Q_OBJECT
public:
    cTelemetryThread();
    cTelemetryThread(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    cTelemetryThread(quint16& serverPort, QHostAddress& serverIP, quint16& clientPort, QHostAddress& clientIP);
    ~cTelemetryThread();

    void run();
    void stop();

    quint16 readClientPort();
    QHostAddress readClientIP() const;
    quint16 readServerPort() const;
    QHostAddress readServerIP() const;

protected:

private:
    quint16 m_serverPort;
    quint16 m_clientPort;
    QHostAddress m_serverIP;
    QHostAddress m_clientIP;

    QMutex m_mutex;
    volatile bool bStopped;

};

#endif // TELEMETRYTHREAD_H
