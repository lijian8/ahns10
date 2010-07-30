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

    consoleText = timeStampStr % "[ COMMAND_ACK ] " % ackMessageToString(ackType);
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
    consoleText = timeStampStr % "[ COMMAND_CLOSE ] ";
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
        m_receiveConsoleWidget->addItem(consoleText,discarded);
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
        m_receiveConsoleWidget->addItem(consoleText,discarded);
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
    QString phi;
    phi.setNum(heliState.phi,'f',2);
    QString theta;
    theta.setNum(heliState.theta,'f',2);
    QString psi;
    psi.setNum(heliState.psi,'f',2);

    QString p;
    p.setNum(heliState.p,'f',2);
    QString q;
    q.setNum(heliState.q,'f',2);
    QString r;
    r.setNum(heliState.r,'f',2);

    QString x;
    x.setNum(heliState.x,'f',2);
    QString y;
    y.setNum(heliState.y,'f',2);
    QString z;
    z.setNum(heliState.z,'f',2);

    QString vx;
    vx.setNum(heliState.vx,'f',2);
    QString vy;
    vy.setNum(heliState.vy,'f',2);
    QString vz;
    vz.setNum(heliState.vz,'f',2);

    QString ax;
    ax.setNum(heliState.ax,'f',2);
    QString ay;
    ay.setNum(heliState.ay,'f',2);
    QString az;
    az.setNum(heliState.az,'f',2);

    QString trace;
    trace.setNum(heliState.trace,'f',2);
    QString voltage;
    voltage.setNum(heliState.voltage,'f',2);

    QString consoleText = "[ HELI_STATE ]\n"
                  % phi %" \t " % theta %" \t "% psi %"\n"
                  % p %" \t " % q %" \t "% r %"\n"
                  % x %" \t " % y %" \t "% z %"\n"
                  % vx %" \t " % vy %" \t "% vz %"\n"
                  % ax %" \t " % ay %" \t "% az %"\n"
                  % trace %" \t " % voltage;
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
    QString consoleText;

    QString e1, e2, e3, e4, rcLinkStatus, fcUptime, fcCPUusage;
    e1.setNum(fcState.commandedEngine1);
    e2.setNum(fcState.commandedEngine2);
    e3.setNum(fcState.commandedEngine3);
    e4.setNum(fcState.commandedEngine4);
    rcLinkStatus.setNum(fcState.rclinkActive);
    fcUptime.setNum(fcState.fcUptime);
    fcCPUusage.setNum(fcState.fcCPUusage);

    consoleText = "[ FC_STATE ]\n"
                  % e1 %" \t " % e2 %" \t "% e3 %"\n"
                  % e4 %" \t " % rcLinkStatus %" \t "% fcUptime %"\n"
                  % fcCPUusage;

    return consoleText;
}

QString gcsMainWindow::apStateToString(const ap_state_t& apState)
{
    QString consoleText;
    QString refPhi, refTheta, refPsi, refX, refY, refZ, actPhi, actTheta, actPsi, actX, actY, actZ;

    refPhi.setNum(apState.referencePhi);
    refTheta.setNum(apState.referenceTheta);
    refPsi.setNum(apState.referencePsi);

    refX.setNum(apState.referenceX);
    refY.setNum(apState.referenceY);
    refZ.setNum(apState.referenceZ);

    actPhi.setNum(apState.phiActive);
    actTheta.setNum(apState.thetaActive);
    actPsi.setNum(apState.psiActive);

    actX.setNum(apState.xActive);
    actY.setNum(apState.yActive);
    actZ.setNum(apState.zActive);

    consoleText = "[ AUTOPILOT_STATE ] \n"
                  % refPhi %" \t " % refTheta %" \t "% refPsi %"\n"
                  % refX %" \t " % refY %" \t "% refZ %"\n"
                  % actPhi %" \t " % actTheta %" \t "% actPsi %"\n"
                  % actX %" \t " % actY %" \t "% actZ %"\n";

    return consoleText;
}

QString gcsMainWindow::parametersToString(const loop_parameters_t& loopParameters)
{
    QString consoleText;

    QString rollMax, rollNeutral, rollMin,
    pitchMax, pitchNeutral, pitchMin,
    yawMax, yawNeutral, yawMin,
    xMax, xNeutral, xMin,
    yMax, yNeutral, yMin,
    zMax, zNeutral, zMin;

    rollMax.setNum(loopParameters.rollMaximum);
    rollNeutral.setNum(loopParameters.rollNeutral);
    rollMin.setNum(loopParameters.rollMinimum);

    pitchMax.setNum(loopParameters.pitchMaximum);
    pitchNeutral.setNum(loopParameters.pitchNeutral);
    pitchMin.setNum(loopParameters.pitchMinimum);

    yawMax.setNum(loopParameters.yawMaximum);
    yawNeutral.setNum(loopParameters.yawNeutral);
    yawMin.setNum(loopParameters.yawMinimum);

    xMax.setNum(loopParameters.xMaximum);
    xNeutral.setNum(loopParameters.xNeutral);
    xMin.setNum(loopParameters.xMinimum);

    yMax.setNum(loopParameters.yMaximum);
    yNeutral.setNum(loopParameters.yNeutral);
    yMin.setNum(loopParameters.yMinimum);

    zMax.setNum(loopParameters.zMaximum);
    zNeutral.setNum(loopParameters.zNeutral);
    zMin.setNum(loopParameters.zMinimum);

    consoleText = "[ PARAMETERS ]\n"
                  % rollMax %" \t " % rollNeutral %" \t " % rollMin %"\n"
                  % pitchMax %" \t " % pitchNeutral %" \t " % pitchMin %"\n"
                  % yawMax %" \t " % yawNeutral %" \t " % yawMin %"\n"
                  % xMax %" \t " % xNeutral %" \t " % xMin % "\n"
                  % yMax %" \t " % yNeutral %" \t " % yMin % "\n"
                  % zMax %" \t " % zNeutral %" \t " % zMin % "\n";

    return consoleText;
}

QString gcsMainWindow::gainsToString(const gains_t& loopGains)
{
    QString consoleText;
    QString rollP, rollI, rollD,
    pitchP, pitchI, pitchD,
    yawP, yawI, yawD,
    xP, xI, xD,
    yP, yI, yD,
    zP, zI, zD;

    rollP.setNum(loopGains.rollKp);
    rollI.setNum(loopGains.rollKi);
    rollD.setNum(loopGains.rollKd);

    pitchP.setNum(loopGains.pitchKp);
    pitchI.setNum(loopGains.pitchKi);
    pitchD.setNum(loopGains.pitchKd);

    yawP.setNum(loopGains.yawKp);
    yawI.setNum(loopGains.yawKi);
    yawD.setNum(loopGains.yawKd);

    xP.setNum(loopGains.xKp);
    xI.setNum(loopGains.xKi);
    xD.setNum(loopGains.xKd);

    yP.setNum(loopGains.yKp);
    yI.setNum(loopGains.yKi);
    yD.setNum(loopGains.yKd);

    zP.setNum(loopGains.zKp);
    zI.setNum(loopGains.zKi);
    zD.setNum(loopGains.zKd);

    consoleText = "[ GAINS ]\n"
                  % rollP %" \t " % rollI %" \t " % rollD %"\n"
                  % pitchP %" \t " % pitchI %" \t " % pitchD %"\n"
                  % yawP %" \t " % yawI %" \t " % yawD %"\n"
                  % xP %" \t " % xI %" \t " % xD % "\n"
                  % yP %" \t " % yI %" \t " % yD % "\n"
                  % zP %" \t " % zI %" \t " % zD % "\n";

    return consoleText;
}

QString gcsMainWindow::sensorDataToString(const sensor_data_t& sensorData)
{
    QString consoleText;
    QString p,q,r,ax,ay,az;

    p.setNum(sensorData.p);
    q.setNum(sensorData.q);
    r.setNum(sensorData.r);
    ax.setNum(sensorData.ax);
    ay.setNum(sensorData.ay);
    az.setNum(sensorData.az);

    consoleText = "[ SENSOR_DATA ]\n"
                  % p %" \t " % q %" \t " % r %"\n"
                  % ax %" \t " % ay %" \t " % az %"\n";

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
        m_receiveConsoleWidget->addItem(consoleText, discarded);
        m_Data.setSensorData(&timeStamp, &sensorData);
    }

    return;
}
