/**
 * @file   gcsmessages.cpp
 * @author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Implementation of the message handling slots of gcsMainWindow class
 */

#include <QString>
#include <QStringBuilder>
#include "sys/time.h"
#include "state.h"

#include "AH.h"
#include "gcsmainwindow.h"
#include "ahns_logger.h"

/**
  * @brief Process the new state_t message by updating the GUI
  * Widgets Updated:
  *     - Updates the AH
  *     - Updates the recevied console
  * @param timeStamp timeval that stores the timestamp of the current message
  * @param heliState state_t that has all angular quantities in radians and positions in meters
  * @param discarded bool to flag if the message was discarded or not
  */
void gcsMainWindow::ProcessHeliState(const timeval timeStamp, const state_t heliState, const int discarded)
{
    AHNS_DEBUG("gcsMainWindow::ProcessHeliState(const timeval timeStamp, const state_t heliState, const int discarded) [ Thread = " << QThread::currentThreadId() << " ]");
    QString consoleText;
    QString timeStampStr = timeStampToString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ HELI_STATE ]";
        }
        else
        {
            consoleText = timeStampStr % stateToString(heliState);
        }
    }

    m_receiveConsoleWidget->addItem(consoleText,discarded);

    if (!discarded)
    {
        // Update the GUI
        m_ahWidget->setState(&heliState);
        m_Data.setHeliStateData(&timeStamp,&heliState);
        m_systemStatusWidget->loadHeliState(heliState);
    }

    return;
}

/**
  * @brief Slot to receive an ACK Message
  * After the ACK an ackType identifies the nature of ack return.
  */
void gcsMainWindow::ProcessAckMessage(const timeval timeStamp, const uint32_t ackType, const int discarded)
{
    QString timeStampStr = timeStampToString(timeStamp);
    QString consoleText;

    consoleText = timeStampStr % " [ COMMAND_ACK ] " % ackMessageToString(ackType);
    m_receiveConsoleWidget->addItem(consoleText,discarded);

    return;
}

/**
  * @brief Slot to receive a CLOSE Message
  */
void gcsMainWindow::ProcessCloseMessage(const timeval timeStamp, const int discarded)
{
    QString consoleText;
    QString timeStampStr = timeStampToString(timeStamp);
    consoleText = timeStampStr % " [ COMMAND_CLOSE ] ";
    m_receiveConsoleWidget->addItem(consoleText,discarded);

    return;
}

/**
  * @brief Slot to receive a FC_STATE Message
  */
void gcsMainWindow::ProcessFCState(const timeval timeStamp, const fc_state_t fcState, const int discarded)
{
    AHNS_DEBUG("gcsMainWindow::ProcessFCState(const timeval timeStamp, const fc_state_t fcState, const int discarded)");
    QString consoleText;
    QString timeStampStr = timeStampToString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ FC_STATE ]";
        }
        else
        {
            consoleText = timeStampStr % fcStateToString(fcState);
        }
    }

    m_receiveConsoleWidget->addItem(consoleText,discarded);

    if (!discarded)
    {
        // Update the GUI
        m_Data.setFCStateData(&timeStamp,&fcState);
        m_systemStatusWidget->loadFCState(fcState);
    }

    return;
}

/**
  * @brief Slot to receive a AUTOPILOT_STATE Message
  */
void gcsMainWindow::ProcessAPState(const timeval timeStamp, const ap_state_t apState, const int discarded)
{
    AHNS_DEBUG("gcsMainWindow::ProcessAPState(const timeval timeStamp, const ap_state_t apState, const int discarded)");
    QString consoleText;
    QString timeStampStr = timeStampToString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ AUTOPILOT_STATE ]";
        }
        else
        {
            consoleText = timeStampStr % apStateToString(apState);
        }
    }

    m_receiveConsoleWidget->addItem(consoleText,discarded);

    if (!discarded)
    {
        // Update the GUI
        m_Data.setAPStateData(&timeStamp,&apState);
        m_flightControlWidget->SetAPState(&apState);
    }

    return;
}

/**
  * @brief Slot to receive PARAMETERS Message
  */
void gcsMainWindow::ProcessParameters(const timeval timeStamp, const loop_parameters_t loopParameters, const int discarded)
{
    AHNS_DEBUG("void ProcessParameters(const timeval timeStamp, const loop_parameters_t loopParameters, const int discarded)");

    QString consoleText;
    QString timeStampStr = timeStampToString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ PARAMETERS ]";
        }
        else
        {
            consoleText = timeStampStr % parametersToString(loopParameters);
        }
    }

    m_receiveConsoleWidget->addItem(consoleText,discarded);

    if (!discarded)
    {
        // Update the GUI
        m_parameterControlWidget->SetLoopParameters(&loopParameters);
    }

    return;
}

/**
  * @brief Slot to receive GAINS Message
  */
void gcsMainWindow::ProcessGains(const timeval timeStamp, const gains_t loopGains, const int discarded)
{
    AHNS_DEBUG("void ProcessGains(const timeval timeStamp, const gains_t loopGains, const int discarded)");

    QString consoleText;
    QString timeStampStr = timeStampToString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ GAINS ]";
        }
        else
        {
            consoleText = timeStampStr % gainsToString(loopGains);
        }
    }

    m_receiveConsoleWidget->addItem(consoleText,discarded);

    if (!discarded)
    {
        // Update the GUI
        m_gainsControlWidget->SetGains(&loopGains);
    }

    return;
}


/**
* @brief Function to convert timeval to QString
*/
QString gcsMainWindow::timeStampToString(const timeval& timeStamp)
{
    QString timeSec;
    timeSec.setNum(timeStamp.tv_sec + timeStamp.tv_usec*1.0e-6,'G',15);
    return timeSec;
}

QString gcsMainWindow::stateToString(const state_t& heliState)
{
    QString consoleText = QString("[ HELI_STATE ]\n%1 %2 %3\n%4 %5 %6\n%7 %8 %9\n%10 %11 %12\n%13 %14 %15\n%16 %17\n")
                          .arg(QString::number(heliState.phi))
                          .arg(QString::number(heliState.theta))
                          .arg(QString::number(heliState.psi))
                          .arg(QString::number(heliState.p))
                          .arg(QString::number(heliState.q))
                          .arg(QString::number(heliState.r))
                          .arg(QString::number(heliState.x))
                          .arg(QString::number(heliState.y))
                          .arg(QString::number(heliState.z))
                          .arg(QString::number(heliState.vx))
                          .arg(QString::number(heliState.vy))
                          .arg(QString::number(heliState.vz))
                          .arg(QString::number(heliState.ax))
                          .arg(QString::number(heliState.ay))
                          .arg(QString::number(heliState.az))
                          .arg(QString::number(heliState.trace))
                          .arg(QString::number(heliState.voltage));
    return consoleText;
}

QString gcsMainWindow::ackMessageToString(const uint32_t& ackType)
{
    QString aType;

    if (ackType == COMMAND_OPEN)
    {
        aType = "COMMAND_OPEN";
    }
    else if (ackType == SET_CONFIG)
    {
        aType = "SET_CONFIG";
    }
    else if (ackType == GET_CONFIG)
    {
        aType = "GET_CONFIG";
    }
    else if (ackType == SAVE_CONFIG)
    {
        aType = "SAVE_CONFIG";
    }
    else if (ackType == DESIRED_ATTITUDE)
    {
        aType = "DESIRED_ATTITUDE";
    }
    else if (ackType == DESIRED_POSITION)
    {
        aType = "DESIRED_POSITION";
    }
    else if (ackType == PARAMETERS)
    {
        aType = "PARAMETERS";
    }
    else if (ackType == GAINS)
    {
        aType = "GAINS";
    }
    else
    {
        aType = "UNKNOWN";
    }

    return aType;
}

QString gcsMainWindow::fcStateToString(const fc_state_t& fcState)
{
    QString consoleText = QString(" [ FC_STATE ]\n%1 %2 %3 %4\n%5 %6 %7\n")
                          .arg(QString::number(fcState.commandedEngine1))
                          .arg(QString::number(fcState.commandedEngine2))
                          .arg(QString::number(fcState.commandedEngine3))
                          .arg(QString::number(fcState.commandedEngine4))
                          .arg(QString::number(fcState.fcCPUusage))
                          .arg(QString::number(fcState.fcUptime))
                          .arg(QString::number(fcState.rclinkActive));
    return consoleText;
}

QString gcsMainWindow::apStateToString(const ap_state_t& apState)
{
    QString consoleText = QString(" [ AUTOPILOT_STATE ] \n%1 %2 %3\n%4 %5 %6\n%7 %8 %9\n%10 %11 %12\n")
                          .arg(QString::number(apState.referencePhi))
                          .arg(QString::number(apState.referenceTheta))
                          .arg(QString::number(apState.referencePsi))
                          .arg(QString::number(apState.referenceX))
                          .arg(QString::number(apState.referenceY))
                          .arg(QString::number(apState.referenceZ))
                          .arg(QString::number(apState.phiActive))
                          .arg(QString::number(apState.thetaActive))
                          .arg(QString::number(apState.psiActive))
                          .arg(QString::number(apState.xActive))
                          .arg(QString::number(apState.yActive))
                          .arg(QString::number(apState.zActive));
    return consoleText;
}

QString gcsMainWindow::parametersToString(const loop_parameters_t& loopParameters)
{
    QString consoleText = QString(" [ PARAMETERS ]\n%1 %2 %3\n%4 %5 %6\n%7 %8 %9\n%10 %11 %12\n%13 %14 %15\n%16 %17 %18\n")
                          .arg(QString::number(loopParameters.rollMaximum))
                          .arg(QString::number(loopParameters.rollNeutral))
                          .arg(QString::number(loopParameters.rollMinimum))
                          .arg(QString::number(loopParameters.pitchMaximum))
                          .arg(QString::number(loopParameters.pitchNeutral))
                          .arg(QString::number(loopParameters.pitchMinimum))
                          .arg(QString::number(loopParameters.yawMaximum))
                          .arg(QString::number(loopParameters.yawNeutral))
                          .arg(QString::number(loopParameters.yawMinimum))
                          .arg(QString::number(loopParameters.xMaximum))
                          .arg(QString::number(loopParameters.xNeutral))
                          .arg(QString::number(loopParameters.xMinimum))
                          .arg(QString::number(loopParameters.yMaximum))
                          .arg(QString::number(loopParameters.yNeutral))
                          .arg(QString::number(loopParameters.yMinimum))
                          .arg(QString::number(loopParameters.zMaximum))
                          .arg(QString::number(loopParameters.zNeutral))
                          .arg(QString::number(loopParameters.zMinimum));
    return consoleText;
}

QString gcsMainWindow::gainsToString(const gains_t& loopGains)
{
    QString consoleText = QString(" [ GAINS ]\n%1 %2 %3\n%4 %5 %6\n%7 %8 %9\n%10 %11 %12\n%13 %14 %15\n%16 %17 %18\n")
                          .arg(QString::number(loopGains.rollKp))
                          .arg(QString::number(loopGains.rollKi))
                          .arg(QString::number(loopGains.rollKd))
                          .arg(QString::number(loopGains.pitchKp))
                          .arg(QString::number(loopGains.pitchKi))
                          .arg(QString::number(loopGains.pitchKd))
                          .arg(QString::number(loopGains.yawKp))
                          .arg(QString::number(loopGains.yawKi))
                          .arg(QString::number(loopGains.yawKd))
                          .arg(QString::number(loopGains.xKp))
                          .arg(QString::number(loopGains.xKi))
                          .arg(QString::number(loopGains.xKd))
                          .arg(QString::number(loopGains.yKp))
                          .arg(QString::number(loopGains.yKi))
                          .arg(QString::number(loopGains.yKd))
                          .arg(QString::number(loopGains.zKp))
                          .arg(QString::number(loopGains.zKi))
                          .arg(QString::number(loopGains.zKd));
    return consoleText;
}

QString gcsMainWindow::sensorDataToString(const sensor_data_t& sensorData)
{
    QString consoleText = QString(" [ SENSOR_DATA ]\n%1 %2 %3\n%4 %5 %6\n%7 %8\n")
                          .arg(QString::number(sensorData.p))
                          .arg(QString::number(sensorData.q))
                          .arg(QString::number(sensorData.r))
                          .arg(QString::number(sensorData.ax))
                          .arg(QString::number(sensorData.ay))
                          .arg(QString::number(sensorData.az))
                          .arg(QString::number(sensorData.z))
                          .arg(QString::number(sensorData.psi));
    return consoleText;
}

/**
  * @brief Slot to receive SENSOR_DATA Message
  */
void gcsMainWindow::ProcessSensorData(const timeval timeStamp, const sensor_data_t sensorData, const int discarded)
{
    AHNS_DEBUG("void gcsMainWindow::ProcessSensorData(const timeval timeStamp, const sensor_data_t sensorData, const int discarded)");

    QString consoleText;
    QString timeStampStr = timeStampToString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ SENSOR_DATA ]";
        }
        else
        {
            consoleText = timeStampStr % sensorDataToString(sensorData);
        }
    }

    m_receiveConsoleWidget->addItem(consoleText,discarded);

    if (!discarded)
    {
        // Update the GUI
        m_Data.setSensorData(&timeStamp, &sensorData);
    }

    return;
}
