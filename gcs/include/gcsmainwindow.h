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

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_actionAbout_triggered();

    void StartTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);
    void CloseTelemetry();
    void RetryTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP);

private:
    Ui::gcsMainWindow *ui;
    void createDockWindows();

    //List of Dockable Widgets
    QList<QDockWidget*> m_dockList;

    // Form
    aboutForm* m_Aboutfrm;

    // Threads
    cTelemetryThread* m_oTelemetryThread;

};

#endif // GCSMAINWINDOW_H
