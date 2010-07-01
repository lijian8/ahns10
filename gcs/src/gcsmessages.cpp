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
* @brief Function to convert timeval to QString
*/
QString gcsMainWindow::timeStamptoString(const timeval& timeStamp)
{
    QString timeSec;
    timeSec.setNum(timeStamp.tv_sec + timeStamp.tv_usec*1.0e-6,'G',15);
    return timeSec;
}

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
    QString timeStampStr = timeStamptoString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ HELI_STATE ]";
        }
        else
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

            consoleText = "[ HELI_STATE ] \n\t" % timeStampStr %"\n\t"
                          % phi %" \t " % theta %" \t "% psi %"\n\t"
                          % p %" \t " % q %" \t "% r %"\n\t"
                          % x %" \t " % y %" \t "% z %"\n\t"
                          % vx %" \t " % vy %" \t "% vz %"\n\t"
                          % ax %" \t " % ay %" \t "% az %"\n\t"
                          % trace %" \t " % voltage;
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
    QString consoleText;
    QString aType;
    QString timeStampStr = timeStamptoString(timeStamp);


    switch (ackType)
    {
    case COMMAND_OPEN:
        aType = "COMMAND_OPEN";
        break;
    case AUTOPILOT_STATE:
        aType = "AUTOPILOT_STATE";
        break;
    default:
        aType = "UNKNOWN";
        break;
    }

    consoleText = timeStampStr % "[ COMMAND_ACK ] " % aType;
    m_receiveConsoleWidget->addItem(consoleText,discarded);

    return;
}

/**
  * @brief Slot to receive a CLOSE Message
  */
void gcsMainWindow::ProcessCloseMessage(const timeval timeStamp, const int discarded)
{
    QString consoleText;
    QString timeStampStr = timeStamptoString(timeStamp);
    consoleText = timeStampStr % "[ COMMAND_CLOSE ]";
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
    QString timeStampStr = timeStamptoString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ FC_STATE ]";
        }
        else
        {
            QString e1, e2, e3, e4, rcLinkStatus, fcUptime, fcCPUusage;
            e1.setNum(fcState.commandedEngine1);
            e2.setNum(fcState.commandedEngine2);
            e3.setNum(fcState.commandedEngine3);
            e4.setNum(fcState.commandedEngine4);
            rcLinkStatus.setNum(fcState.rclinkActive);
            fcUptime.setNum(fcState.fcUptime);
            fcCPUusage.setNum(fcState.fcCPUusage);

            consoleText = "[ FC_STATE ] \n\t" % timeStampStr %"\n\t"
                          % e1 %" \t " % e2 %" \t "% e3 %"\n\t"
                          % e4 %" \t " % rcLinkStatus %" \t "% fcUptime %"\n\t"
                          % fcCPUusage;
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
    QString timeStampStr = timeStamptoString(timeStamp);

    if (m_receiveConsoleWidget->receivedShow() || m_receiveConsoleWidget->discardedShow()) // form string if either shown
    {
        if (!m_receiveConsoleWidget->detailShow()) // only keep first line
        {
            consoleText = timeStampStr % " [ AUTOPILOT_STATE ]";
        }
        else
        {
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

            consoleText = "[ AUTOPILOT_STATE ] \n\t" % timeStampStr %"\n\t"
                          % refPhi %" \t " % refTheta %" \t "% refPsi %"\n\t"
                          % refX %" \t " % refY %" \t "% refZ %"\n\t"
                          % actPhi %" \t " % actTheta %" \t "% actPsi %"\n\t"
                          % actX %" \t " % actY %" \t "% actZ %"\n\t";
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
