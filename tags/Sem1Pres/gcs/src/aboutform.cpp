/**
 * \file
 * \author Tim Molloy
 *
 * Last Modified by: $Author$
 *
 * $LastChangedDate$
 * $Rev$
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Header for the AHNS GCS About Form
 */

#include "aboutform.h"
#include "ui_aboutform.h"

#include "ahns_logger.h"

/**
  * @brief About form constructor
  */
aboutForm::aboutForm(QWidget *parent) : QWidget(parent), ui(new Ui::aboutForm)
{
    AHNS_DEBUG("aboutForm::aboutForm()");
    ui->setupUi(this);
}

/**
  * @brief About form destructor
  */
aboutForm::~aboutForm()
{
    delete ui;
}

void aboutForm::changeEvent(QEvent *e)
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
