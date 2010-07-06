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
