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

#define REPLY_TIMEOUT_MS 250 /** Timeout Limit on ACK Replies */
#define RETRY_TIME_MS 50     /** Time between Attempts */

/**
  * @brief Thread Class Inherided from QThread
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

    bool isConnected() const;

signals:
    /** @name Signals for Received Messages */
    void NewHeliState(const timeval timeStamp, const state_t receivedState, const int discarded = 0);
    void NewFCState(const timeval timeStamp, const fc_state_t receivedState, const int discarded = 0);
    void NewAPState(const timeval timeStamp, const ap_state_t receivedState, const int discarded = 0);
    void NewGains(const timeval timeStamp, const gains_t receivedGains, const int discarded = 0);
    void NewParameters(const timeval timeStamp, const loop_parameters_t receivedParameters, const int discarded = 0);
    void NewAckMessage(const timeval timeStamp, const uint32_t ackType, const int discarded = 0);
    void NewCloseMessage(const timeval timeStamp, const int discarded = 0);
    void NewFailSafe(const timeval timeStamp, const int discarded = 0);
    void NewSensorData(const timeval timeStamp, const sensor_data_t receivedSensor, const int discraded = 0);

    /** @name Signals for Tx Messages */
    void SentMessage(uint32_t messageType, bool success = true);

    /** @name Signals for Connection Status */
    void RxEstimate(const double& linkSpeed);
    void TxEstimate(const double& linkSpeed);

protected:

public slots:
    /** Methods for Data Transmission */
    void sendPositionCommand(position_t desiredPosition);
    void sendAttitudeCommand(attitude_t desiredAttitude);
    void sendSetAPConfig(ap_config_t apConfig);
    void sendGains(gains_t desiredGains);
    void sendParameters(loop_parameters_t desiredParameters);
    void sendViconState(const vicon_state_t viconState);
    void sendGetConfig();
    void sendSaveConfig();
    void forwardVicon(int state);

private slots:
    void DataPending();
    void clientInitialise();
    void retrySetAPConfig();
    void retrySendPosition();
    void retrySendAttitude();
    void retrySendGains();
    void retrySendParameters();
    void retryGetConfig();
    void retrySaveConfig();

private:
    bool packetInitialise();
    bool ackSort(const uint32_t& ackType);
    int sendMessage(uint32_t type, const unsigned char* txData = NULL, int txDataByteLength = 0);

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

    /** @name Thread Management */
    QMutex m_mutex;
    volatile bool m_connected;
    volatile bool m_stopped;

    /** @name Received Flags */
    volatile bool m_openReceived;   /**< Flag used to check for COMMAND_ACK received at FC*/
    volatile bool m_closeReceived;  /**< Flag used to check for COMMAND_CLOSE received  at FC*/


    /** @name AP Config Count, Flag and Data */
    quint8 m_configTryCount;        /**< Counter of tries */
    volatile bool m_configReceived; /**< Flag used to check for SET_CONFIG received  at FC*/
    ap_config_t m_txAPConfig;       /**< current Config being transmitted */

    /** @name Position Count, Flag and Data */
    quint8 m_positionTryCount;        /**< Counter of tries */
    volatile bool m_positionReceived; /**< Flag used to check for DESIRED_POSITION received at FC*/
    position_t m_txPosition;          /**< current Config being transmitted */

    /** @name Attitude Count, Flag and Data */
    quint8 m_attitudeTryCount;        /**< Counter of tries */
    volatile bool m_attitudeReceived; /**< Flag used to check for DESIRED_ATTITUDE received at FC*/
    attitude_t m_txAttitude;          /**< current attitude being transmitted */

    /** @name Parameter Count, Flag and Data */
    quint8 m_parametersTryCount;        /**< Counter of tries */
    volatile bool m_parametersReceived; /**< Flag used to check for PARAMETERS received at FC*/
    loop_parameters_t m_txParameters;   /**< current parameters being transmitted */

    /** @name Gains Count, Flag and Data */
    quint8 m_gainsTryCount;        /**< Counter of tries */
    volatile bool m_gainsReceived; /**< Flag used to check for GAINS received at FC*/
    gains_t m_txGains;             /**< current gains being transmitted */

    /** @name Get Config Count, Flag and Data */
    quint8 m_getTryCount;        /**< Counter of tries */
    volatile bool m_getReceived; /**< Flag used to check for GET_CONFIG received at FC*/

    /** @name Save Config Count, Flag and Data */
    quint8 m_saveTryCount;        /**< Counter of tries */
    volatile bool m_saveReceived; /**< Flag used to check for SAVE_CONFIG received at FC*/

    bool m_forwardVicon;
};

#endif // TELEMETRYTHREAD_H
