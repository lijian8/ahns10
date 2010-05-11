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

// Declare the Types for signal/slot use
Q_DECLARE_METATYPE(timeval)
Q_DECLARE_METATYPE(state_t)
Q_DECLARE_METATYPE(fc_state_t)
Q_DECLARE_METATYPE(ap_state_t)
Q_DECLARE_METATYPE(gains_t)
Q_DECLARE_METATYPE(loop_parameters_t)

/**
  * @brief Function to convert timeval to QString
  */
QString gcsMainWindow::timeStamptoString(const struct timeval timeStamp)
{
    QString timeSec;
    timeSec.setNum(timeStamp.tv_sec);

    QString timeuSec;
    timeuSec.setNum(timeStamp.tv_usec);
    return timeSec % ":" % timeuSec;
}

/**
  * @brief Process the new state_t message by updating the GUI
  * Widgets Updated:
  *     - Updates the AH
  *     - Updates the recevied console
  * @param timeStamp timeval that stores the timestamp of the current message
  * @param heliState state_t* that has all angular quantities in radians and positions in meters
  * @param discarded bool to flag if the message was discarded or not
  */
void gcsMainWindow::ProcessHeliState(timeval* timeStamp, const state_t* heliState, const int discarded)
{
    QString consoleText;

    QString timeStampStr = timeStamptoString(*timeStamp);

    QString phi;
    phi.setNum(heliState->phi,'f',2);
    QString theta;
    theta.setNum(heliState->theta,'f',2);
    QString psi;
    psi.setNum(heliState->psi,'f',2);

    QString p;
    p.setNum(heliState->p,'f',2);
    QString q;
    q.setNum(heliState->q,'f',2);
    QString r;
    r.setNum(heliState->r,'f',2);

    QString x;
    x.setNum(heliState->x,'f',2);
    QString y;
    y.setNum(heliState->y,'f',2);
    QString z;
    z.setNum(heliState->z,'f',2);

    QString vx;
    vx.setNum(heliState->vx,'f',2);
    QString vy;
    vy.setNum(heliState->vy,'f',2);
    QString vz;
    vz.setNum(heliState->vz,'f',2);

    QString ax;
    ax.setNum(heliState->ax,'f',2);
    QString ay;
    ay.setNum(heliState->ay,'f',2);
    QString az;
    az.setNum(heliState->az,'f',2);

    QString trace;
    trace.setNum(heliState->trace,'f',2);
    QString voltage;
    voltage.setNum(heliState->voltage,'f',2);

    consoleText = "[ HELI_STATE ] \n\t" % timeStampStr %"\n\t"
                  % phi %" \t " % theta %" \t "% psi %"\n\t"
                  % p %" \t " % q %" \t "% r %"\n\t"
                  % x %" \t " % y %" \t "% z %"\n\t"
                  % vx %" \t " % vy %" \t "% vz %"\n\t"
                  % ax %" \t " % ay %" \t "% az %"\n\t"
                  % trace %" \t " % voltage;

    m_receiveConsoleWidget->addItem(consoleText,discarded);

    if (!discarded)
    {
        float newRoll = (float) heliState->phi;
        float newRollRate = (float) heliState->p;
        float newPitch = (float) heliState->theta;
        float newPitchRate = (float) heliState->q;
        float newAltState = (float) heliState->z;

        m_ahWidget->setState(newRoll,newRollRate,newPitch,newPitchRate,newAltState);
    }

    // the chores
    delete heliState;
    delete timeStamp;
    return;
}

/**
  * @brief Slot to receive an ACK Message
  */
void gcsMainWindow::ProcessAckMessage(timeval* timeStamp, const int discarded)
{
    QString consoleText;
    QString timeStampStr = timeStamptoString(*timeStamp);
    consoleText = "[ COMMAND_ACK ] \n\t" % timeStampStr ;
    m_receiveConsoleWidget->addItem(consoleText,discarded);

    m_TelSecCount = 0;
    m_TelMinCount = 0;
    m_TelHourCount = 0;
    m_oTelUptimer.start();

    delete timeStamp;
    return;
}
