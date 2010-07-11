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
#include <QLinkedList>
#include "sys/time.h"

#include "aboutform.h"

#include "wificomms.h"
#include "telemetrythread.h"
#include "systemstatus.h"
#include "AH.h"
#include "receiveconsole.h"
#include "dataplotter.h"
#include "bfimagefeed.h"
#include "dataplotter.h"
#include "datalogger.h"
#include "flightcontrol.h"
#include "gainscontrol.h"
#include "parametercontrol.h"


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
    void on_actionRestart_Logging_triggered();
    void on_actionLoad_Config_triggered();
    void on_actionSave_Config_triggered();
    void on_actionRemove_Last_Plotting_Widget_triggered();
    void on_actionRemove_All_Plotting_Widgets_triggered();
    void on_actionNew_Plotting_Widget_triggered();
    void on_actionAbout_triggered();

    // Telemetry Buttons
    void StartTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    void CloseTelemetry();
    void RetryTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    void TelemetryMonitor();

    // Update Telemetry Slots
   void ProcessHeliState(const timeval timeStamp, const state_t heliState, const int discarded = 0);
   void ProcessAckMessage(const timeval timeStamp, const uint32_t ackType, const int discarded = 0);
   void ProcessCloseMessage(const timeval timeStamp, const int discarded = 0);
   void ProcessFCState(const timeval timeStamp, const fc_state_t fcState, const int discarded = 0);
   void ProcessAPState(const timeval timeStamp, const ap_state_t apState, const int discarded = 0);
   void ProcessParameters(const timeval timeStamp, const loop_parameters_t loopParameters, const int discarded = 0);
   void ProcessGains(const timeval timeStamp, const gains_t loopGains, const int discarded = 0);

   void UpdateWidgets();
private:
    Ui::gcsMainWindow *ui;
    void createDockWindows();
    QString timeStamptoString(const timeval& timeStamp);

    // Docked Widgets Widget
    AHclass* m_ahWidget;
    SystemStatus* m_systemStatusWidget;
    wifiComms* m_wifiCommsWidget;
    ReceiveConsole* m_receiveConsoleWidget;
    DataPlotter* m_dataPlotterWidget;
    bfImageFeed* m_bfImageFeedWidget;
    FlightControl* m_flightControlWidget;
    GainsControl* m_gainsControlWidget;
    ParameterControl* m_parameterControlWidget;

    // Additional Plotting Widgets
    QLinkedList<DataPlotter *> m_plottingWidgets;
    QLinkedList<QDockWidget *> m_plottingDock;

    // Form
    aboutForm m_Aboutfrm;

    // Threads
    TelemetryThread* m_TelemetryThread;

    // Telemetry Timer
    QTimer m_oTelUptimer;
    quint8 m_TelSecCount;
    quint8 m_TelMinCount;
    quint32 m_TelHourCount;

    // GUI Update Timer
    QTimer m_updateTimer;

    // Data Logger Object
    DataLogger m_Data;
};

#endif // GCSMAINWINDOW_H
