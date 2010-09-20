/**
 * \file   datalogger.cpp
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
 * Implementation of the Data Logging Class.
 */

#include <QVector>

#include <ctime>
#include <stdexcept>
#include <fstream>

#include "ahns_logger.h"
#include "datalogger.h"

DataLogger::DataLogger()
{
    // By Default Logging
    initialiseLogs();

    // Preallocate some memory
    int i = 0;
    for (i = 0; i < DATA_COUNT; ++i)
    {
        m_DataVector[i].reserve(50*60*15);
    }
}

DataLogger::~DataLogger()
{
    if (sensorDataOutputFile.is_open())
    {
        sensorDataOutputFile.close();
    }
    if (stateOutputFile.is_open())
    {
        stateOutputFile.close();
    }

    if (fcStateOutputFile.is_open())
    {
        fcStateOutputFile.close();
    }

    if (apStateOutputFile.is_open())
    {
        apStateOutputFile.close();
    }

    if (viconDataOutpitFile.is_open())
    {
        viconDataOutpitFile.close();
    }
}

/**
  * @brief Slot for initialising the logging files
  */
void DataLogger::initialiseLogs()
{
    if(m_loggingOn)
    {
        sensorDataOutputFile.close();
        stateOutputFile.close();
        fcStateOutputFile.close();
        apStateOutputFile.close();
    }

    // Ensure Logging is on
    m_loggingOn = true;

    // Open Log Files
    time_t logFileTime;
    std::time(&logFileTime);
    char logFileName[80];

    // Sensor Data File
    strftime(logFileName, 80, "logs/SENSOR_DATA_%A-%d-%m-%G-%H%M%S.log", localtime(&logFileTime));
    sensorDataOutputFile.open(logFileName);
    if (sensorDataOutputFile.fail())
    {
        AHNS_DEBUG("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
        throw std::runtime_error("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
    }
    else
    {
        sensorDataOutputFile << "AHNS SENSOR MESSAGES LOG FOR " << logFileName << std::endl;
        sensorDataOutputFile << "TIME, IMU_ROLL_DOT, IMU_PITCH_DOT, IMU_YAW_DOT, ";
        sensorDataOutputFile << "IMU_AX, IMU_AY, IMU_AZ, SENSOR_Z, COMPASS_PSI" << std::endl;
    }

    // State File
    strftime(logFileName, 80, "logs/Filtered_States_%A-%d-%m-%G-%H%M%S.log", localtime(&logFileTime));
    stateOutputFile.open(logFileName);
    if (stateOutputFile.fail())
    {
        AHNS_DEBUG("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
        throw std::runtime_error("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
    }
    else
    {
        stateOutputFile << "AHNS STATE MESSAGES LOG FOR " << logFileName << std::endl;
        stateOutputFile << "TIME, F_PHI, F_PHI_DOT, F_THETA, F_THETA_DOT, F_PSI, F_PSI_DOT, F_X, F_X_DOT, ";
        stateOutputFile << "F_AX, F_Y, F_Y_DOT, F_AY, F_Z, F_Z_DOT, F_AZ, VOLTAGE" << std::endl;
    }

    // FC State File
    strftime(logFileName, 80, "logs/FC_States_%A-%d-%m-%G-%H%M%S.log", localtime(&logFileTime));
    fcStateOutputFile.open(logFileName);
    if (fcStateOutputFile.fail())
    {
        AHNS_DEBUG("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
        throw std::runtime_error("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
    }
    else
    {
        fcStateOutputFile << "AHNS FC STATE MESSAGES LOG FOR " << logFileName << std::endl;
        fcStateOutputFile << "TIME, COMMANDED_ENGINE1, COMMANDED_ENGINE2, COMMANDED_ENGINE3, COMMANDED_ENGINE4, ";
        fcStateOutputFile << "RC_LINK, FC_UPTIME, FC_CPU" << std::endl;
    }

    // AP State File
    strftime(logFileName, 80, "logs/AP_State_%A-%d-%m-%G-%H%M%S.log", localtime(&logFileTime));
    apStateOutputFile.open(logFileName);
    if (apStateOutputFile.fail())
    {
        AHNS_DEBUG("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
        throw std::runtime_error("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
    }
    else
    {
        apStateOutputFile << "AHNS AP STATE MESSAGES LOG FOR " << logFileName << std::endl;
        apStateOutputFile << "TIME, REF_PHI, REF_THETA, REF_PSI, REF_X, REF_Y, REF_Z, PHI_ACTIVE, THETA_ACTIVE, ";
        apStateOutputFile << "PSI_ACTIVE, X_ACTIVE, Y_ACTIVE, Z_ACTIVE" << std::endl;
    }

    // Vicon Data File
    strftime(logFileName, 80, "logs/Vicon_State_%A-%d-%m-%G-%H%M%S.log", localtime(&logFileTime));
    viconDataOutpitFile.open(logFileName);
    if (viconDataOutpitFile.fail())
    {
        AHNS_DEBUG("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
        throw std::runtime_error("DataLogger::DataLogger(QWidget *parent) [ FAILED FILE OPEN ]");
    }
    else
    {
        viconDataOutpitFile << "AHNS VICON STATE MESSAGES LOG FOR " << logFileName << std::endl;
        viconDataOutpitFile << "TIME, VICON_X, VICON_Y, VICON_Z, VICON_VX, VICON_VY, VICON_VZ, VICON_PHI, VICON_PSI" << std::endl;
    }


    // Set Common Zero Time
    struct timeval time0;
    gettimeofday(&time0,NULL);
    m_zeroTime = time0.tv_sec + time0.tv_usec*1.0e-6;

    return;
}

/**
  * @brief Ensure new HeliState is available for plotting and plot if needed
  */

void DataLogger::setHeliStateData(const timeval* const timeStamp, const state_t* const heliState)
{
    AHNS_DEBUG("DataLogger::setHeliStateData(const timeval* timeStamp, const state_t* heliState)");

    // Time
    m_DataVector[HELI_STATE_RAW_TIME].push_back(timeStamp->tv_sec + timeStamp->tv_usec*1.0e-6);
    m_DataVector[HELI_STATE_TIME].push_back(m_DataVector[HELI_STATE_RAW_TIME].last()-m_zeroTime);

    // Angular Position
    m_DataVector[F_PHI].push_back(heliState->phi);
    m_DataVector[F_THETA].push_back(heliState->theta);
    m_DataVector[F_PSI].push_back(heliState->psi);

    // Angular Rates
    m_DataVector[F_PHI_DOT].push_back(heliState->p);
    m_DataVector[F_THETA_DOT].push_back(heliState->q);
    m_DataVector[F_PSI_DOT].push_back(heliState->r);

    // Linear Positions
    m_DataVector[F_X].push_back(heliState->x);
    m_DataVector[F_Y].push_back(heliState->y);
    m_DataVector[F_Z].push_back(heliState->z);

    // Linear Velocities
    m_DataVector[F_X_DOT].push_back(heliState->vx);
    m_DataVector[F_Y_DOT].push_back(heliState->vy);
    m_DataVector[F_Z_DOT].push_back(heliState->vz);

    // Linear Accelerations
    m_DataVector[F_AX].push_back(heliState->ax);
    m_DataVector[F_AY].push_back(heliState->ay);
    m_DataVector[F_AZ].push_back(heliState->az);

    // Voltage Accelerations
    m_DataVector[VOLTAGE].push_back(heliState->voltage);
    m_DataVector[TRACE].push_back(heliState->trace);


    // Log the State Data
    if (m_loggingOn)
    {
        stateOutputFile << m_DataVector[HELI_STATE_RAW_TIME].last()-m_DataVector[HELI_STATE_RAW_TIME].front() << "," << heliState->phi << ",";
        stateOutputFile << heliState->p << "," << heliState->theta << "," << heliState->q << ",";
        stateOutputFile << heliState->psi << "," << heliState->r << "," << heliState->x << "," << heliState->vx << ",";
        stateOutputFile << heliState->ax << "," << heliState->y << "," << heliState->vy << "," << heliState->ay << ",";
        stateOutputFile << heliState->z << "," << heliState->vz << "," << heliState->az << "," << heliState->voltage << std::endl;
    }

    return;
}

/**
  * @brief Ensure new FCState is available for plotting and plot if needed
  */

void DataLogger::setFCStateData(const timeval* const timeStamp, const fc_state_t* const fcState)
{
    AHNS_DEBUG("DataLogger::setFCStateData(const timeval* timeStamp, const fc_state_t* fcState)");

    // Time
    m_DataVector[FC_STATE_RAW_TIME].push_back(timeStamp->tv_sec + timeStamp->tv_usec*1.0e-6);
    m_DataVector[FC_STATE_TIME].push_back(m_DataVector[FC_STATE_RAW_TIME].last()-m_zeroTime);

    // Commanded Engine PWM
    m_DataVector[ENGINE1].push_back(fcState->commandedEngine1);
    m_DataVector[ENGINE2].push_back(fcState->commandedEngine2);
    m_DataVector[ENGINE3].push_back(fcState->commandedEngine3);
    m_DataVector[ENGINE4].push_back(fcState->commandedEngine4);

    // RC Link
    m_DataVector[RC_LINK].push_back(fcState->rclinkActive);

    // CPU Usage Rates
    m_DataVector[FC_CPU].push_back(fcState->fcCPUusage);

    // Log the FC State Data
    if (m_loggingOn)
    {
        fcStateOutputFile << m_DataVector[FC_STATE_RAW_TIME].last()-m_DataVector[FC_STATE_RAW_TIME].front() << "," << fcState->commandedEngine1 << ",";
        fcStateOutputFile << fcState->commandedEngine2 << "," << fcState->commandedEngine3 << "," << fcState->commandedEngine4 << ",";
        fcStateOutputFile << (int) fcState->rclinkActive << "," << fcState->fcUptime << "," << (int) fcState->fcCPUusage << std::endl;
    }

    return;
}

/**
  * @brief Log new AP State for plotting and file output
  */

void DataLogger::setAPStateData(const timeval* const timeStamp, const ap_state_t* const apState)
{
    AHNS_DEBUG("DataLogger::setAPStateData(const timeval* timeStamp, const ap_state_t* const apState)");

    // Time
    m_DataVector[AP_STATE_RAW_TIME].push_back(timeStamp->tv_sec + timeStamp->tv_usec*1.0e-6);
    m_DataVector[AP_STATE_TIME].push_back(m_DataVector[AP_STATE_RAW_TIME].last()-m_zeroTime);

    // Reference Angles
    m_DataVector[REF_PHI].push_back(apState->referencePhi);
    m_DataVector[REF_THETA].push_back(apState->referenceTheta);
    m_DataVector[REF_PSI].push_back(apState->referencePsi);

    // Reference Position
    m_DataVector[REF_X].push_back(apState->referenceX);
    m_DataVector[REF_Y].push_back(apState->referenceY);
    m_DataVector[REF_Z].push_back(apState->referenceZ);

    // Active Loops
    m_DataVector[PHI_ACTIVE].push_back(apState->phiActive);
    m_DataVector[THETA_ACTIVE].push_back(apState->thetaActive);
    m_DataVector[PSI_ACTIVE].push_back(apState->psiActive);
    m_DataVector[X_ACTIVE].push_back(apState->xActive);
    m_DataVector[Y_ACTIVE].push_back(apState->yActive);
    m_DataVector[Z_ACTIVE].push_back(apState->zActive);

    // Log the FC State Data
    if (m_loggingOn)
    {
        apStateOutputFile << m_DataVector[AP_STATE_RAW_TIME].last()-m_DataVector[AP_STATE_RAW_TIME].front() << "," << apState->referencePhi << ",";
        apStateOutputFile << apState->referenceTheta << "," << apState->referencePsi << "," << apState->referenceX << ",";
        apStateOutputFile << apState->referenceY << "," << apState->referenceZ << "," << (int) apState->phiActive << ",";
        apStateOutputFile << (int) apState->thetaActive << "," << (int) apState->psiActive << "," << (int) apState->xActive << ",";
        apStateOutputFile << (int) apState->yActive <<"," << (int) apState->zActive << std::endl;
    }

    return;
}

/**
  * @brief Log new Sensor Data for plotting and file output
  */
void DataLogger::setSensorData(const timeval* timeStamp, const sensor_data_t* const sensorData)
{
    AHNS_DEBUG("void DataLogger::setSensorData(const timeval* timeStamp, const state_data_t* const sensorData)");

    // Time
    m_DataVector[SENSOR_RAW_TIME].push_back(timeStamp->tv_sec + timeStamp->tv_usec*1.0e-6);
    m_DataVector[SENSOR_TIME].push_back(m_DataVector[SENSOR_RAW_TIME].last()-m_zeroTime);

    // Angular Rates
    m_DataVector[IMU_ROLL_DOT].push_back(sensorData->p);
    m_DataVector[IMU_PITCH_DOT].push_back(sensorData->q);
    m_DataVector[IMU_YAW_DOT].push_back(sensorData->r);

    // Linear Accelerations
    m_DataVector[IMU_AX].push_back(sensorData->ax);
    m_DataVector[IMU_AY].push_back(sensorData->ay);
    m_DataVector[IMU_AZ].push_back(sensorData->az);

    // Altitude Sensor
    m_DataVector[SENSOR_Z].push_back(sensorData->z);

    // Compass
    m_DataVector[COMPASS_PSI].push_back(sensorData->psi);

    // Log the Sensor Data
    if (m_loggingOn)
    {
        sensorDataOutputFile << m_DataVector[SENSOR_RAW_TIME].last()-m_DataVector[SENSOR_RAW_TIME].front() << "," << sensorData->p << ",";
        sensorDataOutputFile << sensorData->q << "," << sensorData->r << "," << sensorData->ax << ",";
        sensorDataOutputFile << sensorData->ay << "," << sensorData->az << "," << sensorData->z << "," << sensorData->psi << std::endl;
    }
    return;
}

/**
  * @brief Log new Vicon Data for plotting and file output
  */
void DataLogger::setViconData(const timeval* const timeStamp, const vicon_state_t* const viconData)
{
    AHNS_DEBUG("void DataLogger::setViconData(const timeval* const timeStamp, const vicon_state_t* const viconData)");

    // Time
    m_DataVector[VICON_RAW_TIME].push_back(timeStamp->tv_sec + timeStamp->tv_usec*1.0e-6);
    m_DataVector[VICON_TIME].push_back(m_DataVector[VICON_TIME].last()-m_zeroTime);

    // Position
    m_DataVector[VICON_X].push_back(viconData->x);
    m_DataVector[VICON_Y].push_back(viconData->y);
    m_DataVector[VICON_Z].push_back(viconData->z);

    // Velocity
    m_DataVector[VICON_VX].push_back(viconData->vx);
    m_DataVector[VICON_VY].push_back(viconData->vy);
    m_DataVector[VICON_VZ].push_back(viconData->vz);

    // Angles
    m_DataVector[VICON_PHI].push_back(viconData->phi);
    m_DataVector[VICON_THETA].push_back(viconData->theta);
    m_DataVector[VICON_PSI].push_back(viconData->psi);

    // Log the Sensor Data
    if (m_loggingOn)
    {
        viconDataOutpitFile << m_DataVector[VICON_RAW_TIME].last()-m_DataVector[VICON_RAW_TIME].front() << "," << viconData->x << ",";
        viconDataOutpitFile << viconData->y << "," << viconData->z << "," << viconData->vx << ",";
        viconDataOutpitFile << viconData->vy << "," << viconData->vz << "," << viconData->phi << "," << viconData->theta << "," << viconData->psi << std::endl;
    }
    return;
}

/**
  * @brief Get Data Vector Accessor
  */
QVector<double>* DataLogger::getData()
{
    return m_DataVector;
}

/**
  * @brief Clear the logged data
  */
void DataLogger::clearData()
{
    int i = 0;

    for (i = 0; i < DATA_COUNT; ++i)
    {
        m_DataVector[i].clear();
    }
    return;
}
