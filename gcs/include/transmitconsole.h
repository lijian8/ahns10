/**
 * @file   transmitconsole.h
 * @author Tim Molloy
 *
 * $Author: tlmolloy@gmail.com $
 * $Date: 2010-07-11 14:05:08 +1000 (Sun, 11 Jul 2010) $
 * $Rev: 228 $
 * $Id: receiveconsole.cpp 228 2010-07-11 04:05:08Z tlmolloy@gmail.com $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Definition of Transmit Console widget to display the packets transmitted by the GCS
 */

#ifndef TRANSMITCONSOLE_H
#define TRANSMITCONSOLE_H

#include <QWidget>

namespace Ui {
    class TransmitConsole;
}

class TransmitConsole : public QWidget
{
    Q_OBJECT

public:
    TransmitConsole(QWidget *parent = 0);
    ~TransmitConsole();

    bool transmittedShow() const;
    bool detailShow() const;
    void clearConsole();

    QSize sizeHint() const;
    void resizeEvent (QResizeEvent* e);

public slots:
    void TxSpeed(const double& linkSpeed);
    void SentItem(uint32_t messageType, bool success = true);

private:
    Ui::TransmitConsole *ui;
    quint64 m_packetCount;
};

#endif // TRANSMITCONSOLE_H
