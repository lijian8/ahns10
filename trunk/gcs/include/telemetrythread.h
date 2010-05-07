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
 * Header file for the TelemetryThread.
 */

#ifndef TELEMETRYTHREAD_H
#define TELEMETRYTHREAD_H

#include <QObject>
#include <QtGlobal>

#include <QThread>
#include <QMutex>
#include <QTimer>

#include <QHostAddress>
#include <QUdpSocket>

#include "inttypes.h"
#include "state.h"

/**
  * \brief Thead Class Inherided from QThread
  */
class TelemetryThread : public QThread
{
    Q_OBJECT

public:
    TelemetryThread(QObject * parent = 0);
    TelemetryThread(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP, QObject * parent = 0);
    TelemetryThread(quint16& serverPort, QHostAddress& serverIP, quint16& clientPort, QHostAddress& clientIP, QObject * parent = 0);
    ~TelemetryThread();

    void run();
    void stop();

    quint16 readClientPort() const;
    QHostAddress readClientIP() const;
    quint16 readServerPort() const;
    QHostAddress readServerIP() const;

signals:
    void NewHeliState(const state_t* receivedState);

protected:

public slots:

private slots:
    void DataPending();

private:
    bool clientInitialise();

    // Methods for Data Transmission
    int sendMessage(uint32_t type, const char* txData = NULL, int txDataByteLength = 0);

    quint16 m_serverPort;
    quint16 m_clientPort;
    QHostAddress m_serverIP;
    QHostAddress m_clientIP;

    QMutex m_mutex;
    volatile bool m_stopped;

    QUdpSocket* m_socket;

};

#endif // TELEMETRYTHREAD_H
