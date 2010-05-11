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

#include "systemstatus.h"
#include "ui_systemstatus.h"

#include "ahns_logger.h"
#include "ahns_timeformat.h"

SystemStatus::SystemStatus(QWidget *parent) : QWidget(parent), ui(new Ui::SystemStatus)
{
    ui->setupUi(this);

    ui->fcUptimeLcd->display(AHNS_HMS(0,0,0));

    setMinimumSize(342, 180);
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
    return QSize(342, 180);
}
