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

/**
  * \brief Thead Class Inherided from QThread
  */
class TelemetryThread : public QThread
{
    Q_OBJECT
public:
    TelemetryThread();
    TelemetryThread(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    TelemetryThread(quint16& serverPort, QHostAddress& serverIP, quint16& clientPort, QHostAddress& clientIP);
    ~TelemetryThread();

    void run();
    void stop();

    quint16 readClientPort() const;
    QHostAddress readClientIP() const;
    quint16 readServerPort() const;
    QHostAddress readServerIP() const;
signals:

protected:

private slots:
    void DataPending();

private:

    void txInitialise();
    void rxInitialise();

    quint16 m_serverPort;
    quint16 m_clientPort;
    QHostAddress m_serverIP;
    QHostAddress m_clientIP;

    QMutex m_mutex;
    volatile bool m_stopped;

    QUdpSocket* m_txSocket;
    QUdpSocket* m_rxSocket;

};

#endif // TELEMETRYTHREAD_H
