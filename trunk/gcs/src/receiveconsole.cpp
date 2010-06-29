/**
 * @file   receiveconsole.cpp
 * @author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Receive Console widget to display the packets received and disgarded by the GCS
 */

#include "ahns_logger.h"

#include <QResizeEvent>

#include "include/receiveconsole.h"
#include "ui_receiveconsole.h"

ReceiveConsole::ReceiveConsole(QWidget *parent) : QWidget(parent), ui(new Ui::ReceiveConsole)
{
    ui->setupUi(this);
    setMinimumSize(440,190);
    m_packetCount = 0;
    m_discardedCount = 0;

    ui->Speedlbl->setText("0 kBs");
    ui->receivedChkbox->setChecked(true);
    ui->discardedChkbox->setChecked(true);
}

ReceiveConsole::~ReceiveConsole()
{
    delete ui;
}

void ReceiveConsole::changeEvent(QEvent *e)
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
  * @brief Console Size Hint
  */
QSize ReceiveConsole::sizeHint() const
{
    return QSize(440,190);
}

/**
  * @brief Resize the widgets
  * Stretch the scroll area but don't expand the titles vertically
  */
void ReceiveConsole::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("ReceiveConsole::resizeEvent (QResizeEvent* e)");
    ui->verticalLayoutWidget->resize(e->size());
    return;
}

/**
  *  @brief Queries the state of the received packet box
  */
bool ReceiveConsole::receivedShow() const
{
    return ui->receivedChkbox->isChecked();
}

/**
  *  @brief Queries the state of the discarded packet box
  */
bool ReceiveConsole::discardedShow() const
{
    return ui->discardedChkbox->isChecked();
}

/**
  *  @brief Queries the state of the details packet box
  */
bool ReceiveConsole::detailShow() const
{
    return ui->detailsChkbox->isChecked();
}

/**
  * @brief Function to add new item text to the ui list
  */
void ReceiveConsole::addItem(QString& item, const int& discarded)
{

    if (discarded != 0)
    {
        m_discardedCount++;
        m_packetCount++;

        if (discarded == -1)
        {
            item.push_back(" -> DISCARDED LATE PACKET");
        }
        else
        {
            item.push_back(" -> DISCARDED DUPLICATE PACKET");
        }

        if (discardedShow())
        {
            ui->listWidget->addItem(item);
        }
    }
    else if (receivedShow())
    {
        ui->listWidget->addItem(item);
        m_packetCount++;
    }
    else
    {
         m_packetCount++;
    }

    ui->discardedlcdNumber->display((double) m_discardedCount);
    ui->receivedlcdNumber->display((double) m_packetCount);
    ui->listWidget->scrollToBottom();
    return;
}

/**
  * @brief Clear the Console and counters
  */
void ReceiveConsole::clearConsole()
{
    m_discardedCount = 0;
    m_packetCount = 0;
    ui->discardedlcdNumber->display((double) m_discardedCount);
    ui->receivedlcdNumber->display((double) m_packetCount);
    ui->listWidget->clear();
    ui->Speedlbl->setText("0 Bs");
    return;
}

/**
  * @brief Update the link speed display
  */
void ReceiveConsole::RxSpeed(const double& linkSpeed)
{
    QString linkSpeedstr;
    linkSpeedstr.setNum(linkSpeed,'f',2);
    linkSpeedstr.push_back(" Bs");
    ui->Speedlbl->setText(linkSpeedstr);
    return;
}
