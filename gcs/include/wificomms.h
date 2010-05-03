/**
 * \file   wificomms.h
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
 * Header file for the Wifi Communications widget to manage the wifi
 * communications and launch its telemetry thread.
 */

#ifndef WIFICOMMS_H
#define WIFICOMMS_H

#include <QWidget>
#include <QAbstractButton>
#include <QTimer>

namespace Ui {
    class wifiComms;
}

class wifiComms : public QWidget {
    Q_OBJECT
public:
    wifiComms(QWidget *parent = 0);
    ~wifiComms();

    QSize sizeHint() const;

public slots:
    void buttonBoxChanged(QAbstractButton* btnAbstract);

private slots:
    void lcdUpdate();

signals:
    void sigConnectionRetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    void sigConnectionClose();
    void sigConnectionStart(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::wifiComms *ui;

    char szHostIP[64];

    // Timer for UPTIME LCD
    QTimer m_oUptimer;
    quint8 m_secCount;
    quint8 m_minCount;
    quint32 m_hourCount;
};

#endif // WIFICOMMS_H
