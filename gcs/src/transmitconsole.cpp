/**
 * @file   TransmitConsole.cpp
 * @author Tim Molloy
 *
 * $Author: tlmolloy@gmail.com $
 * $Date: 2010-07-11 14:05:08 +1000 (Sun, 11 Jul 2010) $
 * $Rev: 228 $
 * $Id: TransmitConsole.cpp 228 2010-07-11 04:05:08Z tlmolloy@gmail.com $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Transmit Console widget to display the packets transmitted by the GCS
 */

#include <QResizeEvent>

#include "include/transmitconsole.h"
#include "ui_transmitconsole.h"

#include "commands.h"
#include "ahns_logger.h"

TransmitConsole::TransmitConsole(QWidget *parent) : QWidget(parent), ui(new Ui::TransmitConsole)
{
    ui->setupUi(this);
    setMinimumSize(440,190);
    m_packetCount = 0;

    ui->Speedlbl->setText("0 kBps");
    ui->transmittedChkbox->setChecked(false);
}

TransmitConsole::~TransmitConsole()
{
    delete ui;
}

/**
  * @brief Console Size Hint
  */
QSize TransmitConsole::sizeHint() const
{
    return QSize(440,190);
}

/**
  * @brief Resize the widgets
  * Stretch the scroll area but don't expand the titles vertically
  */
void TransmitConsole::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("TransmitConsole::resizeEvent (QResizeEvent* e)");
    ui->verticalLayoutWidget->resize(e->size());
    return;
}

/**
  *  @brief Queries the state of the transmitted packet box
  */
bool TransmitConsole::transmittedShow() const
{
    return ui->transmittedChkbox->isChecked();
}

/**
  *  @brief Queries the state of the details packet box
  */
bool TransmitConsole::detailShow() const
{
    return ui->detailsChkbox->isChecked();
}

/**
  * @brief Function to add new item text to the ui list
  */
void TransmitConsole::SentItem(uint32_t messageType, bool success)
{
    QString item;
    if (transmittedShow())
    {
        switch (messageType)
        {
        case DESIRED_POSITION:
            item.append("[ DESIRED_POSITION ]");
            break;
        case DESIRED_ATTITUDE:
            item.append("[ DESIRED_ATTITUDE ]");
            break;
        case GAINS:
            item.append("[ GAINS ]");
            break;
        case PARAMETERS:
            item.append("[ PARAMETERS ]");
            break;
        case SET_CONFIG:
            item.append("[ SET_CONFIG ]");
            break;
        case GET_CONFIG:
            item.append("[ GET_CONFIG ]");
            break;
        case SAVE_CONFIG:
            item.append("[ SAVE_CONFIG ]");
            break;
        default:
            AHNS_DEBUG("void TransmitConsole::SentItem(uint32_t messageType, bool success) [ UNKNOWN ]");
        }

        switch (success)
        {
        case true:
            item.append(" SUCCESS");
            break;
        case false:
            item.append(" FAIL");
            break;
        }

        ui->listWidget->addItem(item);
        m_packetCount++;
    }
    else
    {
         m_packetCount++;
    }

    ui->transmittedlcdNumber->display((double) m_packetCount);
    ui->listWidget->scrollToBottom();
    return;
}

/**
  * @brief Clear the Console and counters
  */
void TransmitConsole::clearConsole()
{
    m_packetCount = 0;
    ui->transmittedlcdNumber->display((double) m_packetCount);
    ui->listWidget->clear();
    ui->Speedlbl->setText("0 kBps");
    return;
}

/**
  * @brief Update the link speed display
  */
void TransmitConsole::TxSpeed(const double& linkSpeed)
{
    QString linkSpeedstr;
    linkSpeedstr.setNum(linkSpeed,'f',2);
    linkSpeedstr.push_back(" kBps");
    ui->Speedlbl->setText(linkSpeedstr);
    return;
}
