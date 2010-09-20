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
    /** @brief Slots for Telemetry*/
    void buttonBoxChanged(QAbstractButton* btnAbstract);
    void lcdUpdate(const quint32& hourCounter, const quint8& minCounter, const quint8& secCounter);

    /** @brief Slots for Vicon */
    void buttonBoxChangedVicon(QAbstractButton* btnAbstract);
    void lcdUpdateVicon(const quint32& hourCounter, const quint8& minCounter, const quint8& secCounter);

private slots:


signals:
    /** @brief Signals for Telemetry Threading*/
    void ConnectionRetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    void ConnectionClose();
    void ConnectionStart(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);

    /** @brief Signals for Vicon Threading*/
    void ViconConnectionRetry(quint16& serverPort, QString& serverIP);
    void ViconConnectionClose();
    void ViconConnectionStart(quint16& serverPort, QString& serverIP);

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent* e);

private:
    Ui::wifiComms *ui;

    char szHostIP[64];
};

#endif // WIFICOMMS_H
