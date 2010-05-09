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

#include "aboutform.h"
#include "telemetrythread.h"


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

    // Messages
    void NewAHState(const float& newRoll,const float& newRollRate,const float& newPitch,const float& newPitchRate, const float& newAltState);


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
   void UpdateHeliState(const state_t* heliState);



private:
    Ui::gcsMainWindow *ui;
    void createDockWindows();

    //List of Dockable Widgets
    QList<QDockWidget*> m_dockList;


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
