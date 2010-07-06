/**
 * \file   gainscontrol.cpp
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
 * Implementation of the GainsControl Widget.
 */

#include "gainscontrol.h"
#include "ui_gainscontrol.h"
#include <QSize>
#include <QResizeEvent>

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
