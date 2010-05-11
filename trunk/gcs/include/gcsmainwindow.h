/**
 * \file   gcsmainwindow.h
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
 * GCS form header file used to define the gcsMainWindow class
 */

#ifndef GCSMAINWINDOW_H
#define GCSMAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QtGlobal>
#include "sys/time.h"

#include "aboutform.h"

#include "wificomms.h"
#include "telemetrythread.h"
#include "systemstatus.h"
#include "AH.h"
#include "receiveconsole.h"


namespace Ui {
    class gcsMainWindow;
}

/**
  * \brief Mainwindow Qt Class for GCS, Generated with QtCreator
  */
class gcsMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    gcsMainWindow(QWidget *parent = NULL);
    ~gcsMainWindow();

signals:

    // Update Child Widgets

    // Timers
    void NewTelemetryStatus(const quint32& hourCount, const quint8& minCount, const quint8& secCount);

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_actionAbout_triggered();

    // Telemetry Buttons
    void StartTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    void CloseTelemetry();
    void RetryTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    void TelemetryMonitor();

    // Update Telemetry Slots
   void ProcessHeliState(timeval* timeStamp, const state_t* heliState, const int discarded = 0);
   void ProcessAckMessage(timeval* timeStamp, const int discarded = 0);

private:
    Ui::gcsMainWindow *ui;
    void createDockWindows();
    QString timeStamptoString(const struct timeval timeStamp);

    //Docked Widgets Widget
    AHclass* m_ahWidget;
    SystemStatus* m_systemStatusWidget;
    wifiComms* m_wifiCommsWidget;
    ReceiveConsole* m_receiveConsoleWidget;

    // Form
    aboutForm* m_Aboutfrm;

    // Threads
    TelemetryThread* m_TelemetryThread;

    // Telemetry Timer
    QTimer m_oTelUptimer;
    quint8 m_TelSecCount;
    quint8 m_TelMinCount;
    quint32 m_TelHourCount;
};

#endif // GCSMAINWINDOW_H
