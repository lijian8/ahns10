/**
 * \file   flightcontrol.cpp
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
 * Implementation of the FlightControl Widget.
 */

#include <QResizeEvent>

#include "include/flightcontrol.h"
#include "ui_flightcontrol.h"

#include "ahns_logger.h"

FlightControl::FlightControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FlightControl)
{
    AHNS_DEBUG("FlightControl::FlightControl(QWidget *parent)");

    ui->setupUi(this);
}

FlightControl::~FlightControl()
{
    AHNS_DEBUG("FlightControl::~FlightControl()");
    delete ui;
}

void FlightControl::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QSize FlightControl::sizeHint() const
{
    return QSize(405,315);
}

void FlightControl::resizeEvent (QResizeEvent* e)
{
    ui->layoutWidget->resize(e->size());
    return;
}


/**
  * @brief Change Loop configuration to RC mode, disabling all loops.
  */
void FlightControl::on_rcBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_rcBtn_clicked()");

    // enable user to change active loops
    ui->rollChkbox->setEnabled(true);
    ui->pitchChkbox->setEnabled(true);
    ui->yawChkbox->setEnabled(true);
    ui->zChkbox->setEnabled(true);

    // enable loop setpoints
    ui->rollCmdBox->setEnabled(true);
    ui->pitchCmdBox->setEnabled(true);
    ui->yawCmdBox->setEnabled(true);
    ui->xCmdBox->setEnabled(false);
    ui->yCmdBox->setEnabled(false);
    ui->zCmdBox->setEnabled(true);

    ui->rollCmdResetBtn->setEnabled(true);
    ui->pitchCmdResetBtn->setEnabled(true);
    ui->yawCmdResetBtn->setEnabled(true);
    ui->xCmdResetBtn->setEnabled(false);
    ui->yCmdResetBtn->setEnabled(false);
    ui->zCmdResetBtn->setEnabled(true);

    // Unselect all
    ui->rollChkbox->setChecked(false);
    ui->pitchChkbox->setChecked(false);
    ui->yawChkbox->setChecked(false);
    
    ui->xChkbox->setChecked(false);
    ui->yChkbox->setChecked(false);
    ui->zChkbox->setChecked(false);

    ui->sendAttitudeBtn->setEnabled(true);
    ui->sendPositionBtn->setEnabled(true);
    
    // Force Upload
    ui->sendSetConfigBtn->click();

    return;
}

/**
  * @brief Change loop selection to Attitude hold mode
  */
void FlightControl::on_attitudeBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_attitudeBtn_clicked()");

    // enable user to change active loops
    ui->rollChkbox->setEnabled(true);
    ui->pitchChkbox->setEnabled(true);
    ui->yawChkbox->setEnabled(true);
    ui->zChkbox->setEnabled(true);

    // enable loop setpoints
    ui->rollCmdBox->setEnabled(true);
    ui->pitchCmdBox->setEnabled(true);
    ui->yawCmdBox->setEnabled(true);
    ui->xCmdBox->setEnabled(false);
    ui->yCmdBox->setEnabled(false);
    ui->zCmdBox->setEnabled(true);

    ui->rollCmdResetBtn->setEnabled(true);
    ui->pitchCmdResetBtn->setEnabled(true);
    ui->yawCmdResetBtn->setEnabled(true);
    ui->xCmdResetBtn->setEnabled(false);
    ui->yCmdResetBtn->setEnabled(false);
    ui->zCmdResetBtn->setEnabled(true);

    // select angle loops
    ui->rollChkbox->setChecked(true);
    ui->pitchChkbox->setChecked(true);
    ui->yawChkbox->setChecked(true);

    ui->xChkbox->setChecked(false);
    ui->yChkbox->setChecked(false);
    ui->zChkbox->setChecked(false);

    // enable all command buttons
    ui->sendAttitudeBtn->setEnabled(true);
    ui->sendPositionBtn->setEnabled(true);

    // Force Upload
    ui->sendSetConfigBtn->click();


    return;
}

/**
  * @brief Change loop selection to Position hold mode
  */
void FlightControl::on_positionBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_positionBtn_clicked()");

    // enable user to change active loops
    ui->rollChkbox->setEnabled(false);
    ui->pitchChkbox->setEnabled(false);
    ui->yawChkbox->setEnabled(false);
    ui->zChkbox->setEnabled(false);

    // enable loop setpoints
    ui->rollCmdBox->setEnabled(false);
    ui->pitchCmdBox->setEnabled(false);
    ui->yawCmdBox->setEnabled(false);
    ui->xCmdBox->setEnabled(true);
    ui->yCmdBox->setEnabled(true);
    ui->zCmdBox->setEnabled(true);

    ui->rollCmdResetBtn->setEnabled(false);
    ui->pitchCmdResetBtn->setEnabled(false);
    ui->yawCmdResetBtn->setEnabled(false);
    ui->xCmdResetBtn->setEnabled(true);
    ui->yCmdResetBtn->setEnabled(true);
    ui->zCmdResetBtn->setEnabled(true);

    // select angle and altitude loops
    ui->rollChkbox->setChecked(true);
    ui->pitchChkbox->setChecked(true);
    ui->yawChkbox->setChecked(true);

    ui->xChkbox->setChecked(true);
    ui->yChkbox->setChecked(true);
    ui->zChkbox->setChecked(true);

    // disable attitude command button
    ui->sendAttitudeBtn->setEnabled(false);
    ui->sendPositionBtn->setEnabled(true);

    // Force Upload
    ui->sendSetConfigBtn->click();

    return;
}

/**
  * @brief Change loop selection to Guidance mode
  */
void FlightControl::on_guidanceBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_guidanceBtn_clicked()");

    // select angle and guidance loops
    ui->rollChkbox->setChecked(true);
    ui->pitchChkbox->setChecked(true);
    ui->yawChkbox->setChecked(true);    
    ui->xChkbox->setChecked(true);
    ui->yChkbox->setChecked(true);
    ui->zChkbox->setChecked(true);

    // disable loop setpoints
    ui->rollCmdBox->setEnabled(false);
    ui->pitchCmdBox->setEnabled(false);
    ui->yawCmdBox->setEnabled(false);
    ui->xCmdBox->setEnabled(false);
    ui->yCmdBox->setEnabled(false);
    ui->zCmdBox->setEnabled(false);

    ui->rollCmdResetBtn->setEnabled(false);
    ui->pitchCmdResetBtn->setEnabled(false);
    ui->yawCmdResetBtn->setEnabled(false);
    ui->xCmdResetBtn->setEnabled(false);
    ui->yCmdResetBtn->setEnabled(false);
    ui->zCmdResetBtn->setEnabled(false);

    // disable user from changing active loops
    ui->rollChkbox->setEnabled(false);
    ui->pitchChkbox->setEnabled(false);
    ui->yawChkbox->setEnabled(false);
    ui->zChkbox->setEnabled(false);

    // disable all command buttons
    ui->sendAttitudeBtn->setEnabled(false);
    ui->sendPositionBtn->setEnabled(false);

    // Force Upload
    ui->sendSetConfigBtn->click();

    return;
}

/**
  * @brief Returns the Roll set point to 0 degrees
  */
void FlightControl::on_rollCmdResetBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_rollCmdResetBtn_clicked()");
    ui->rollCmdBox->setValue(0.00);

    return;
}

/**
  * @brief Returns the Pitch set point to 0 degrees
  */
void FlightControl::on_pitchCmdResetBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_pitchCmdResetBtn_clicked()");
    ui->pitchCmdBox->setValue(0.00);

    return;
}

/**
  * @brief Returns the Yaw set point to 0 degrees
  */
void FlightControl::on_yawCmdResetBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_yawCmdResetBtn_clicked()");
    ui->yawCmdBox->setValue(0.00);

    return;
}

/**
  * @brief Returns the X set point to 0 m
  */
void FlightControl::on_xCmdResetBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_xCmdResetBtn_clicked()");
    ui->xCmdBox->setValue(0.00);

    return;
}

/**
  * @brief Returns the Y set point to 0 m
  */
void FlightControl::on_yCmdResetBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_yCmdResetBtn_clicked()");
    ui->yCmdBox->setValue(0.00);

    return;
}

/**
  * @brief Returns the Z set point to 1.5 m
  */
void FlightControl::on_zCmdResetBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_zCmdResetBtn_clicked()");
    ui->zCmdBox->setValue(1.50);

    return;
}

/**
  * @brief Handle ap state from server
  */
void FlightControl::SetAPState(const ap_state_t* const srcState)
{
    AHNS_DEBUG("void FlightControl::SetAPState(const ap_state_t* const srcState)");

    // Show set points if disabled
    if (!ui->rollCmdBox->isEnabled())
    {
        ui->rollCmdBox->setValue(srcState->referencePhi);
    }

    if (!ui->pitchCmdBox->isEnabled())
    {
        ui->pitchCmdBox->setValue(srcState->referenceTheta);
    }

    if (!ui->yawCmdBox->isEnabled())
    {
        ui->yawCmdBox->setValue(srcState->referencePsi);
    }

    if (!ui->xCmdBox->isEnabled())
    {
        ui->xCmdBox->setValue(srcState->referenceX);
    }

    if (!ui->yCmdBox->isEnabled())
    {
        ui->yCmdBox->setValue(srcState->referenceY);
    }

    if (!ui->zCmdBox->isEnabled())
    {
        ui->zCmdBox->setValue(srcState->referenceZ);
    }

    // Update the lights
    if (srcState->phiActive)
    {
        ui->rollActivelbl->setStyleSheet("QLabel { background-color: green }");
    }
    else
    {
        ui->rollActivelbl->setStyleSheet("QLabel { background-color: red }");
    }

    if (srcState->thetaActive)
    {
        ui->pitchActivelbl->setStyleSheet("QLabel { background-color: green }");
    }
    else
    {
        ui->pitchActivelbl->setStyleSheet("QLabel { background-color: red }");
    }

    if (srcState->psiActive)
    {
        ui->yawActivelbl->setStyleSheet("QLabel { background-color: green }");
    }
    else
    {
        ui->yawActivelbl->setStyleSheet("QLabel { background-color: red }");
    }

    if (srcState->xActive)
    {
        ui->xActivelbl->setStyleSheet("QLabel { background-color: green }");
    }
    else
    {
        ui->xActivelbl->setStyleSheet("QLabel { background-color: red }");
    }

    if (srcState->yActive)
    {
        ui->yActivelbl->setStyleSheet("QLabel { background-color: green }");
    }
    else
    {
        ui->yActivelbl->setStyleSheet("QLabel { background-color: red }");
    }

    if (srcState->zActive)
    {
        ui->zActivelbl->setStyleSheet("QLabel { background-color: green }");
    }
    else
    {
        ui->zActivelbl->setStyleSheet("QLabel { background-color: red }");
    }

    return;
}

/**
  * @brief Collect and Send the Active Loop configuration
  */
void FlightControl::on_sendSetConfigBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_sendSetConfigBtn_clicked()");

    ap_config_t txConfig;

    txConfig.phiActive = ui->rollChkbox->isChecked();
    txConfig.thetaActive = ui->pitchChkbox->isChecked();
    txConfig.psiActive = ui->yawChkbox->isChecked();
    txConfig.xActive = ui->xChkbox->isChecked();
    txConfig.yActive = ui->yChkbox->isChecked();
    txConfig.zActive = ui->zChkbox->isChecked();

    emit sendSetAPConfig(txConfig);

    return;
}

/**
  * @brief Collect and send the position setpoints
  */
void FlightControl::on_sendPositionBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_sendPositionBtn_clicked()");

    position_t txPosition;

    txPosition.x = ui->xCmdBox->value();
    txPosition.y = ui->yCmdBox->value();
    txPosition.z = ui->zCmdBox->value();

    emit sendPosition(txPosition);

    return;
}

/**
  * @brief Collect and send the attitude setpoints
  */
void FlightControl::on_sendAttitudeBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_sendAttitudeBtn_clicked()");

    attitude_t txAttitude;

    txAttitude.phi = ui->rollCmdBox->value()*M_PI/180.0;
    txAttitude.theta = ui->pitchCmdBox->value()*M_PI/180.0;
    txAttitude.psi = ui->yawCmdBox->value()*M_PI/180.0;

    emit sendAttitude(txAttitude);

    return;
}

/**
  * @brief Command for FC to save all AP parameters, gains, references and config
  */
void FlightControl::on_saveBtn_clicked()
{
    AHNS_DEBUG("void FlightControl::on_saveBtn_clicked()");
    emit sendSaveConfig();
    return;
}
