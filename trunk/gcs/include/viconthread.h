/**
 * \file   viconthread.h
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-09-21 01:34:36 +1000 (Tue, 21 Sep 2010) $
 * $Rev: 394 $
 * $Id: telemetrythread.h 394 2010-09-20 15:34:36Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Header file for the ViconThreads.
 */

#ifndef VICONTHREAD_H
#define VICONTHREAD_H

#include <QObject>
#include <QtGlobal>

#include <QThread>
#include <QMutex>
#include <QTimer>

#include <QHostAddress>
#include <QUdpSocket>

#include "inttypes.h"
#include "state.h"

#define REPLY_TIMEOUT_MS 250 /** Timeout Limit on ACK Replies */
#define RETRY_TIME_MS 50     /** Time between Attempts */

/**
  * @brief Vicon Class Inherided from QThread
  */
class ViconThread : public QThread
{
    Q_OBJECT

public:
    ViconThread(QObject * parent = 0);
    ViconThread(quint16& serverPort, QString& serverIP, QObject * parent = 0);
    ViconThread(quint16& serverPort, QHostAddress& serverIP, QObject * parent = 0);
    ~ViconThread();

    void run();
    void stop();

    quint16 readServerPort() const;
    QHostAddress readServerIP() const;

    bool isConnected() const;

signals:
    /** @name Signals for Received Messages */
    void NewViconState(const vicon_state_t receivedState);

private:
    /** Last Vicon Packet rx Time*/
    struct timeval lastPacket;

    /** Vicon Specific Code */
    int ViconServerConnect();
    void ProcessViconState();

     /** @name Members to configure the sockets*/
    quint16 m_serverPort;
    QHostAddress m_serverIP;

    /** @name Thread Management */
    QMutex m_mutex;
    volatile bool m_connected;
    volatile bool m_stopped;
};
#endif // VICONTHREAD_H
