/**
 * \file   parametercontrol.cpp
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-07-01 23:44:05 +1000 (Thu, 01 Jul 2010) $
 * $Rev: 224 $
 * $Id: flightcontrol.h 224 2010-07-01 13:44:05Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Implementation of the ParameterControl Widget.
 */

#include "parametercontrol.h"
#include "ui_parametercontrol.h"

#include "state.h"
#include "ahns_logger.h"

#include <QResizeEvent>

ParameterControl::ParameterControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterControl)
{
    ui->setupUi(this);
}

ParameterControl::~ParameterControl()
{
    delete ui;
}

void ParameterControl::changeEvent(QEvent *e)
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

/**
  * @brief SizeHint resize
  */
QSize ParameterControl::SizeHint() const
{
    return QSize(420,270);
}


/**
  * @brief Handle resize of the widget
  */
void ParameterControl::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("ParameterControl::resizeEvent (QResizeEvent* e)");
    ui->layoutWidget->resize(e->size());
    return;
}

/**
  * @brief Slot to accept and display received parameters
  */
void ParameterControl::SetLoopParameters(const loop_parameters_t* const srcParameters)
{
    AHNS_DEBUG("void ParameterControl::SetLoopParameters(const loop_parameters_t* const srcParameters)");

    // Set Roll Boxes
    ui->rollMaxBox->setValue(srcParameters->rollMaximum);
    ui->rollMinBox->setValue(srcParameters->rollMinimum);
    ui->rollNeutralBox->setValue(srcParameters->rollNeutral);

    // Set Pitch Boxes
    ui->pitchMaxBox->setValue(srcParameters->pitchMaximum);
    ui->pitchMinBox->setValue(srcParameters->pitchMinimum);
    ui->pitchNeutralBox->setValue(srcParameters->pitchNeutral);

    // Set Yaw Boxes
    ui->yawMaxBox->setValue(srcParameters->yawMaximum);
    ui->yawMinBox->setValue(srcParameters->yawMinimum);
    ui->yawNeutralBox->setValue(srcParameters->yawNeutral);

    // Set x Boxes
    ui->xMaxBox->setValue(srcParameters->xMaximum);
    ui->xMinBox->setValue(srcParameters->xMinimum);
    ui->xNeutralBox->setValue(srcParameters->xNeutral);

    // Set y Boxes
    ui->yMaxBox->setValue(srcParameters->yMaximum);
    ui->yMinBox->setValue(srcParameters->yMinimum);
    ui->yNeutralBox->setValue(srcParameters->yNeutral);

    // Set z Boxes
    ui->zMaxBox->setValue(srcParameters->zMaximum);
    ui->zMinBox->setValue(srcParameters->zMinimum);
    ui->zNeutralBox->setValue(srcParameters->zNeutral);


    return;
}

/**
  * @brief Loads parameters from a user selected file on the GCS.
  */
void ParameterControl::on_loadBtn_clicked()
{
    AHNS_DEBUG("void ParameterControl::on_loadBtn_clicked()");
    return;
}

/**
  * @brief Save parameters on a user selected file on the GCS
  */
void ParameterControl::on_SaveBtn_clicked()
{
    AHNS_DEBUG("void ParameterControl::on_SaveBtn_clicked()");

    return;
}

/**
  * @brief Send the Parameters in the GCS to the Flight Computer
  */
void ParameterControl::on_SendBtn_clicked()
{
    AHNS_DEBUG("void ParameterControl::on_SendBtn_clicked()");

    loop_parameters_t loopConfig;

    // Set Roll Parameters
    loopConfig.rollMaximum = ui->rollMaxBox->value();
    loopConfig.rollMinimum = ui->rollMinBox->value();
    loopConfig.rollNeutral = ui->rollNeutralBox->value();

    // Set Pitch Parameters
    loopConfig.pitchMaximum = ui->pitchMaxBox->value();
    loopConfig.pitchMinimum = ui->pitchMinBox->value();
    loopConfig.pitchNeutral = ui->pitchNeutralBox->value();

    // Set Yaw Parameters
    loopConfig.yawMaximum = ui->yawMaxBox->value();
    loopConfig.yawMinimum = ui->yawMinBox->value();
    loopConfig.yawNeutral = ui->yawNeutralBox->value();

    // Set x Parameters
    loopConfig.xMaximum = ui->xMaxBox->value();
    loopConfig.xMinimum = ui->xMinBox->value();
    loopConfig.xNeutral = ui->xNeutralBox->value();

    // Set y Parameters
    loopConfig.yMaximum = ui->yMaxBox->value();
    loopConfig.yMinimum = ui->yMinBox->value();
    loopConfig.yNeutral = ui->yNeutralBox->value();

    // Set z Parameters
    loopConfig.zMaximum = ui->zMaxBox->value();
    loopConfig.zMinimum = ui->zMinBox->value();
    loopConfig.zNeutral = ui->zNeutralBox->value();

    emit sendParameters(loopConfig);
    return;
}
