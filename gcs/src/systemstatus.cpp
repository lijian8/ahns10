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
#include "MCUCommands.h"

#define PROGRESS_OK ""
#define PROGRESS_WARNING "QProgressBar { color: orange }"
#define PROGRESS_CRITICAL "QProgressBar { color: red }"

SystemStatus::SystemStatus(QWidget *parent) : QWidget(parent), ui(new Ui::SystemStatus)
{
    ui->setupUi(this);

    ui->fcUptimeLcd->display(AHNS_HMS(0,0,0));

    setMinimumSize(290, 130);
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

    return;
}

/**
  * @brief Function to be called to load a new Heli srcState
  */
void SystemStatus::loadHeliState(const state_t& srcState)
{
    AHNS_DEBUG("SystemStatus::loadHeliState(const state_t& srcState)");
    m_voltage = srcState.voltage;
    //m_batteryPercentage = (srcState.voltage - FC_MIN_VOLTAGE) / (FC_MAX_VOLTAGE - FC_MIN_VOLTAGE) * 100.0;

    return;
}

void SystemStatus::UpdateStatus()
{
    AHNS_DEBUG("SystemStatus::UpdateStatus()");

    if (this->isVisible())
    {
        if (m_rcLinkActive == AUTOPILOT)
        {
            ui->rclbl->setStyleSheet("QLabel { background-color: green }");
        }
        else if (m_rcLinkActive == MANUAL_DEBUG)
        {
            ui->rclbl->setStyleSheet("QLabel { background-color: red }");
        }
        else if (m_rcLinkActive == AUGMENTED)
        {
            ui->rclbl->setStyleSheet("QLabel { background-color: blue }");
        }
        else if(m_rcLinkActive == FAIL_SAFE)
        {
             ui->rclbl->setStyleSheet("QLabel { background-color: yellow }");
        }

        ui->eng1prgbar->setValue((m_commandedEngine1-ENGINE_MIN_US)/(ENGINE_MAX_US-ENGINE_MIN_US)*100);
        if ( ui->eng1prgbar->value() > 90 )
        {
            ui->eng1prgbar->setStyleSheet(PROGRESS_CRITICAL);
        }
        else if ( ui->eng1prgbar->value() > 75)
        {
            ui->eng1prgbar->setStyleSheet(PROGRESS_WARNING);
        }
        else
        {
            ui->eng1prgbar->setStyleSheet(PROGRESS_OK);
        }

        ui->eng2prgbar->setValue((m_commandedEngine2-ENGINE_MIN_US)/(ENGINE_MAX_US-ENGINE_MIN_US)*100);
        if ( ui->eng2prgbar->value() > 90 )
        {
            ui->eng2prgbar->setStyleSheet(PROGRESS_CRITICAL);
        }
        else if ( ui->eng2prgbar->value() > 75)
        {
            ui->eng2prgbar->setStyleSheet(PROGRESS_WARNING);
        }
        else
        {
            ui->eng2prgbar->setStyleSheet(PROGRESS_OK);
        }

        ui->eng3prgbar->setValue((m_commandedEngine3-ENGINE_MIN_US)/(ENGINE_MAX_US-ENGINE_MIN_US)*100);
        if ( ui->eng3prgbar->value() > 90 )
        {
            ui->eng3prgbar->setStyleSheet(PROGRESS_CRITICAL);
        }
        else if ( ui->eng3prgbar->value() > 75)
        {
            ui->eng3prgbar->setStyleSheet(PROGRESS_WARNING);
        }
        else
        {
            ui->eng3prgbar->setStyleSheet(PROGRESS_OK);
        }

        ui->eng4prgbar->setValue((m_commandedEngine4-ENGINE_MIN_US)/(ENGINE_MAX_US-ENGINE_MIN_US)*100);
        if ( ui->eng4prgbar->value() > 90 )
        {
            ui->eng4prgbar->setStyleSheet(PROGRESS_CRITICAL);
        }
        else if ( ui->eng4prgbar->value() > 75)
        {
            ui->eng4prgbar->setStyleSheet(PROGRESS_WARNING);
        }
        else
        {
            ui->eng4prgbar->setStyleSheet(PROGRESS_OK);
        }

        ui->fcCPUprgbar->setValue(m_fcCPUusage);

        ui->fcUptimeLcd->display((double)m_fcUptime);

        QString labelText;
        labelText.setNum(m_voltage,'g',4);
        ui->batterylbl->setText(labelText % " V");
    }
    return;
}
