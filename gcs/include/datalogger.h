/**
 * \file   datalogger.h
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
 * Definition of the Data Logging Class.
 */

#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QVector>
#include <fstream>

#include "sys/time.h"
#include "state.h"

/** @name Possible Plot Data */
enum PlotData {
    F_PHI,
    F_PHI_DOT,
    F_THETA,
    F_THETA_DOT,
    F_PSI,
    F_PSI_DOT,
    F_X,
    F_X_DOT,
    F_AX,
    F_Y,
    F_Y_DOT,
    F_AY,
    F_Z,
    F_Z_DOT,
    F_AZ,
    VOLTAGE,
    TRACE,
    IMU_ROLL_DOT,
    IMU_PITCH_DOT,
    IMU_YAW_DOT,
    IMU_AX,
    IMU_AY,
    IMU_AZ,
    US_Z,
    ENGINE1,
    ENGINE2,
    ENGINE3,
    ENGINE4,
    FC_CPU,
    RC_LINK,
    REF_PHI,
    REF_THETA,
    REF_PSI,
    REF_X,
    REF_Y,
    REF_Z,
    PHI_ACTIVE,
    THETA_ACTIVE,
    PSI_ACTIVE,
    X_ACTIVE,
    Y_ACTIVE,
    Z_ACTIVE,
    CURVE_COUNT,
    HELI_STATE_RAW_TIME,
    HELI_STATE_TIME,
    FC_STATE_RAW_TIME,
    FC_STATE_TIME,
    AP_STATE_RAW_TIME,
    AP_STATE_TIME,
    DATA_COUNT
};

class DataLogger
{
public:
    void setHeliStateData(const timeval* const timeStamp, const state_t* const heliState);
    void setFCStateData(const timeval* const timeStamp, const fc_state_t* const fcState);
    void setAPStateData(const timeval* const timeStamp, const ap_state_t* const apState);
    void initialiseLogs();
    void clearData();

    QVector<double>* getData();

    DataLogger();
    ~DataLogger();

private:
    /** Vector of Data Vectors */
    QVector<double> m_DataVector[DATA_COUNT];
    double m_zeroTime;

    /** @name Log Files */
    volatile bool m_loggingOn;
    std::ofstream stateOutputFile;
    std::ofstream fcStateOutputFile;
    std::ofstream apStateOutputFile;
};

#endif // DATALOGGER_H
