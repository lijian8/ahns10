/**
 * \file   commsconsole.cpp
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
 * Communications Console Class
 * To be used for display of the packets received, transmitted and discarded.
 */

#include "include/commsconsole.h"
#include "ui_commsconsole.h"

CommsConsole::CommsConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommsConsole)
{
    ui->setupUi(this);
}

CommsConsole::~CommsConsole()
{
    delete ui;
}

void CommsConsole::changeEvent(QEvent *e)
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
