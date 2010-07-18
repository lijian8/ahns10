/**
 * \file   gainscontrol.cpp
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
 * Implementation of the GainsControl Widget.
 */

#include "gainscontrol.h"
#include "ui_gainscontrol.h"

#include <QSize>
#include <QResizeEvent>

#include "state.h"
#include "ahns_logger.h"

GainsControl::GainsControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GainsControl)
{
    ui->setupUi(this);
}

GainsControl::~GainsControl()
{
    delete ui;
}

void GainsControl::changeEvent(QEvent *e)
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
QSize GainsControl::SizeHint() const
{
    return QSize(420,270);
}


/**
  * @brief Handle resize of the widget
  */
void GainsControl::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("GainsControl::resizeEvent (QResizeEvent* e)");
    ui->layoutWidget->resize(e->size());
    return;
}


/**
  * @breif Slot to accept and display received gains
  */
void GainsControl::SetGains(gains_t const* srcGains)
{
    AHNS_DEBUG("void GainsControl::SetGains(gains_t const* srcGains)");

    // Roll Gains
    ui->rollPBox->setValue(srcGains->rollKp);
    ui->rollIBox->setValue(srcGains->rollKi);
    ui->rollDBox->setValue(srcGains->rollKd);

    // Pitch Gains
    ui->pitchPBox->setValue(srcGains->pitchKp);
    ui->pitchIBox->setValue(srcGains->pitchKi);
    ui->pitchDBox->setValue(srcGains->pitchKd);

    // Yaw Gains
    ui->yawPBox->setValue(srcGains->yawKp);
    ui->yawIBox->setValue(srcGains->yawKi);
    ui->yawDBox->setValue(srcGains->yawKd);

    // x Gains
    ui->xPBox->setValue(srcGains->xKp);
    ui->xIBox->setValue(srcGains->xKi);
    ui->xDBox->setValue(srcGains->xKd);

    // y Gains
    ui->yPBox->setValue(srcGains->yKp);
    ui->yIBox->setValue(srcGains->yKi);
    ui->yDBox->setValue(srcGains->yKd);

    // z Gains
    ui->zPBox->setValue(srcGains->zKp);
    ui->zIBox->setValue(srcGains->zKi);
    ui->zDBox->setValue(srcGains->zKd);

    return;
}

/**
  * @brief Load gains from user selected GCS file
  */
void GainsControl::on_loadBtn_clicked()
{
    AHNS_DEBUG("void GainsControl::on_loadBtn_clicked()");

    return;
}

/**
  * @brief Save GCS gains to user selected file
  */
void GainsControl::on_SaveBtn_clicked()
{
    AHNS_DEBUG("void GainsControl::on_SaveBtn_clicked()");

    return;
}

/**
  * @brief Send Gains from GCS to FC
  */
void GainsControl::on_SendBtn_clicked()
{
    AHNS_DEBUG("void GainsControl::on_SendBtn_clicked()");

    gains_t loopGains;

    // Roll Gains
    loopGains.rollKp = ui->rollPBox->value();
    loopGains.rollKi = ui->rollIBox->value();
    loopGains.rollKd = ui->rollDBox->value();

    // Pitch Gains
    loopGains.pitchKp = ui->pitchPBox->value();
    loopGains.pitchKi = ui->pitchIBox->value();
    loopGains.pitchKd = ui->pitchDBox->value();

    // Yaw Gains
    loopGains.yawKp = ui->yawPBox->value();
    loopGains.yawKi = ui->yawIBox->value();
    loopGains.yawKd = ui->yawDBox->value();

    // x Gains
    loopGains.xKp = ui->xPBox->value();
    loopGains.xKi = ui->xIBox->value();
    loopGains.xKd = ui->xDBox->value();

    // y Gains
    loopGains.yKp = ui->yPBox->value();
    loopGains.yKi = ui->yIBox->value();
    loopGains.yKd = ui->yDBox->value();

    // z Gains
    loopGains.zKp = ui->zPBox->value();
    loopGains.zKi = ui->zIBox->value();
    loopGains.zKd = ui->zDBox->value();

    emit sendGains(loopGains);

    return;
}
