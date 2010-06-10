/**
 * \file   SystemStatus.h
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
 * Header for widget to show the high level system status.
 */

#ifndef SYSTEMSTATUS_H
#define SYSTEMSTATUS_H

#include <QWidget>
#include <QTimer>
#include "ui_systemstatus.h"

#include "state_cpp.h"

/** @name Hardware Constants
  * @todo CONFIRM THE LOCATION AND VALUES OF THESE
*/
#define FC_MAX_VOLTAGE 12.0
#define FC_MIN_VOLTAGE 9.0
#define ENGINE_MAX_US 2000.0
#define ENGINE_MIN_US 1000.0

namespace Ui {
    class SystemStatus;
}

/**
  * @breif Qt SystemStatus Widget
  */
class SystemStatus : public QWidget {
    Q_OBJECT
public:
    SystemStatus(QWidget *parent = 0);
    ~SystemStatus();

    QSize sizeHint() const;

    void loadFCState(const fc_state_t& srcState);
    void loadHeliState(const state_t& srcState);

protected:
    void changeEvent(QEvent* e);
    void resizeEvent (QResizeEvent* e);

private slots:
    void UpdateStatus();

private:
    Ui::SystemStatus *ui;

    /** @name Heli State Variables */
    double m_voltage;
    double m_batteryPercentage;

    /** @name Flight Computer State Variables */
    quint8 m_rcLinkActive;
    quint16 m_commandedEngine1;
    quint16 m_commandedEngine2;
    quint16 m_commandedEngine3;
    quint16 m_commandedEngine4;
    quint64 m_fcUptime;
    quint8  m_fcCPUusage;

    /** @name Update Timer */
    QTimer updateTimer;
};

#endif // SYSTEMSTATUS_H
