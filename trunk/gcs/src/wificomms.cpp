/**
 * \file   wificomms.cpp
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
 * Wifi Communications widget to:
 *   - Receive the desired server IP/hostname
 *   - Launch the wifi telemetry thread
 *   - report on the wifi status
 */

#include "include/wificomms.h"
#include "ui_wificomms.h"

wifiComms::wifiComms(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wifiComms)
{
    ui->setupUi(this);
}

wifiComms::~wifiComms()
{
    delete ui;
}

void wifiComms::changeEvent(QEvent *e)
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
  * \brief Initial sizing hint of the wifi widget
  */
QSize wifiComms::sizeHint() const
{
    return QSize(630, 270);;
}
