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
#include "commands.h"

/**
  * @brief Thead Class Inherided from QThread
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
    /** @name Signals for Received Messages */
    void NewHeliState(timeval* timeStamp, const state_t* receivedState, const int discarded = 0);
    void NewFCState(timeval* timeStamp, const fc_state_t* receivedState, const int discarded = 0);
    void NewAPState(timeval* timeStamp, const ap_state_t* receivedState, const int discarded = 0);
    void NewRollGain(timeval* timeStamp, const gains_t* receivedGains, const int discarded = 0);
    void NewPitchGain(timeval* timeStamp, const gains_t* receivedGains, const int discarded = 0);
    void NewYawGain(timeval* timeStamp, const gains_t* receivedGains, const int discarded = 0);
    void NewGuidanceXGain(timeval* timeStamp, const gains_t* receivedGains, const int discarded = 0);
    void NewGuidanceYGain(timeval* timeStamp, const gains_t* receivedGains, const int discarded = 0);
    void NewGuidanceZGain(timeval* timeStamp, const gains_t* receivedGains, const int discarded = 0);
    void NewRollParameters(timeval* timeStamp, const loop_parameters_t* receivedParameters, const int discarded = 0);
    void NewPitchParameters(timeval* timeStamp, const loop_parameters_t* receivedParameters, const int discarded = 0);
    void NewYawParameters(timeval* timeStamp, const loop_parameters_t* receivedParameters, const int discarded = 0);
    void NewGuidanceXParameters(timeval* timeStamp, const loop_parameters_t* receivedParameters, const int discarded = 0);
    void NewGuidanceYParameters(timeval* timeStamp, const loop_parameters_t* receivedParameters, const int discarded = 0);
    void NewGuidanceZParameters(timeval* timeStamp, const loop_parameters_t* receivedParameters, const int discarded = 0);

    void NewAckMessage(timeval* timeStamp, const int discarded = 0);
    void NewFailSafe(timeval* timeStamp, const int discarded = 0);


    void RxEstimate(const double& linkSpeed);

protected:

public slots:
    // Methods for Data Transmission
    int sendMessage(uint32_t type, const char* txData = NULL, int txDataByteLength = 0);

private slots:
    void DataPending();
    bool clientInitialise();

private:
    bool packetInitialise();

    /** Array to Track last packet of message type corresponding to the array element*/
    struct timeval lastPacket[COMMAND_MAX+1];

     /** @name Members to configure the sockets*/
    quint16 m_serverPort;
    quint16 m_clientPort;
    QHostAddress m_serverIP;
    QHostAddress m_clientIP;
    QUdpSocket* m_socket;

    /** @name Current Link Details */
    quint64 m_rxData;
    time_t timeFirstRxPacket;
    quint64 m_txData;
    time_t timeFirstTxPacket;

    // Thread Management
    QMutex m_mutex;
    volatile bool m_stopped;

    volatile bool m_ackReceived; /**< Flag used to check for COMMAND_ACK received */
};

#endif // TELEMETRYTHREAD_H
