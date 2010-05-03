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

systemStatus::systemStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::systemStatus)
{
    ui->setupUi(this);
}

systemStatus::~systemStatus()
{
    delete ui;
}

void systemStatus::changeEvent(QEvent *e)
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
