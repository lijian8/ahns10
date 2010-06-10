/**
 * \file   SystemStatus.cpp
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
 * Implementation of the widget to display high level system status
 */

#include <QResizeEvent>
#include <QStringBuilder>

#include "systemstatus.h"
#include "ui_systemstatus.h"

#include "ahns_logger.h"
#include "ahns_timeformat.h"

SystemStatus::SystemStatus(QWidget *parent) : QWidget(parent), ui(new Ui::SystemStatus)
{
    ui->setupUi(this);

    ui->fcUptimeLcd->display(AHNS_HMS(0,0,0));

    setMinimumSize(290, 130);

    // Force Update at 25Hz
    updateTimer.setInterval(40);
    connect(&updateTimer,SIGNAL(timeout()),this,SLOT(UpdateStatus()));
}

SystemStatus::~SystemStatus()
{
    delete ui;
}

/**
  * @brief Handle Resize of the widget
  */
void SystemStatus::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("SystemStatus::resizeEvent (QResizeEvent* e)");
    ui->gridLayoutWidget->resize(e->size());
    return;
}

void SystemStatus::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * @brief Initial sizing hint of the SystemStatus widget
  */
QSize SystemStatus::sizeHint() const
{
    return QSize(290, 130);
}

/**
  * @brief Function to be called to load a new FC srcState
  */
void SystemStatus::loadFCState(const fc_state_t& srcState)
{
    AHNS_DEBUG("SystemStatus::loadFCState(const fc_state_t& srcState)");
    m_rcLinkActive = srcState.rclinkActive;
    m_commandedEngine1 = srcState.commandedEngine1;
    m_commandedEngine2 = srcState.commandedEngine2;
    m_commandedEngine3 = srcState.commandedEngine3;
    m_commandedEngine4 = srcState.commandedEngine4;
    m_fcUptime = srcState.fcUptime;
    m_fcCPUusage = srcState.fcCPUusage;

    if(!updateTimer.isActive())
    {
     updateTimer.start();
    }

    return;
}

/**
  * @brief Function to be called to load a new Heli srcState
  */
void SystemStatus::loadHeliState(const state_t& srcState)
{
    AHNS_DEBUG("SystemStatus::loadHeliState(const state_t& srcState)");
    m_voltage = srcState.voltage;
    m_batteryPercentage = (srcState.voltage - FC_MIN_VOLTAGE) / (FC_MAX_VOLTAGE - FC_MIN_VOLTAGE) * 100.0;

    if(!updateTimer.isActive())
    {
     updateTimer.start();
    }

    return;
}

void SystemStatus::UpdateStatus()
{
    AHNS_DEBUG("SystemStatus::UpdateStatus()");
    QString labelText;

    if ( m_rcLinkActive == 1)
    {
        ui->rclbl->setText("ON");
        ui->rclinkprgbar->setValue(1);
    }
    else
    {
        ui->rclbl->setText("OFF");
        ui->rclinkprgbar->setValue(0);
    }

    labelText.setNum(m_commandedEngine1);
    labelText = labelText % " us";
    ui->eng1lbl->setText(labelText);
    ui->eng1prgbar->setValue((m_commandedEngine1-ENGINE_MIN_US)/(ENGINE_MAX_US-ENGINE_MIN_US)*100);

    labelText.setNum(m_commandedEngine2);
    labelText = labelText % " us";
    ui->eng2lbl->setText(labelText);
    ui->eng2prgbar->setValue((m_commandedEngine2-ENGINE_MIN_US)/(ENGINE_MAX_US-ENGINE_MIN_US)*100);

    labelText.setNum(m_commandedEngine3);
    labelText = labelText % " us";
    ui->eng3lbl->setText(labelText);
    ui->eng3prgbar->setValue((m_commandedEngine3-ENGINE_MIN_US)/(ENGINE_MAX_US-ENGINE_MIN_US)*100);

    labelText.setNum(m_commandedEngine4);
    labelText = labelText % " us";
    ui->eng4lbl->setText(labelText);
    ui->eng4prgbar->setValue((m_commandedEngine4-ENGINE_MIN_US)/(ENGINE_MAX_US-ENGINE_MIN_US)*100);

    labelText.setNum(m_fcCPUusage);
    ui->fcCPUlbl->setText(labelText);
    ui->fcCPUprgbar->setValue(m_fcCPUusage);

    ui->fcUptimeLcd->display((double)m_fcUptime);

    labelText.setNum(m_voltage);
    ui->voltagelbl->setText(labelText);
    ui->voltageprgbar->setValue(m_batteryPercentage);
    return;
}
