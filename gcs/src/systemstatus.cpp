/**
 * \file   systemstatus.cpp
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

#include "systemstatus.h"
#include "ui_systemstatus.h"

SystemStatus::SystemStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemStatus)
{
    ui->setupUi(this);
}

SystemStatus::~SystemStatus()
{
    delete ui;
}

void SystemStatus::changeEvent(QEvent *e)
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
