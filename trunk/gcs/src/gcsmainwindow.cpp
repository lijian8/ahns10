/**
 * @file   gcsmainwindow.cpp
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
 * GCS form implementation for gcsMainWindow class
 */

#include <QtGui>

#include "gcsmainwindow.h"
#include "ui_gcsmainwindow.h"

// Widgets for the Docks
#include "AH.h"
#include "systemstatus.h"
#include "wificomms.h"

// Threads
#include "telemetrythread.h"

//AHNS Reuse
#include "ahns_logger.h"

gcsMainWindow::gcsMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::gcsMainWindow)
{
    ui->setupUi(this);

    createDockWindows();

    // Actions for Main Menu
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(on_actionAbout_triggered()));

    // Create about form
    m_Aboutfrm = new aboutForm;

    // TelemetryThread not started yet
    m_oTelemetryThread = NULL;
}

gcsMainWindow::~gcsMainWindow()
{
    AHNS_DEBUG("gcsMainWindow::~gcsMainWindow()")
    AHNS_DEBUG("gcsMainWindow::~gcsMainWindow() [Thread Pointer = "  << m_oTelemetryThread << " ]");

    if (m_oTelemetryThread != NULL)
    {
      m_oTelemetryThread->stop();
      while (m_oTelemetryThread->isRunning());
      delete m_oTelemetryThread;
    }

    delete m_Aboutfrm;
    delete ui;
    AHNS_DEBUG("gcsMainWindow::~gcsMainWindow() [ COMPLETED ]")
}

void gcsMainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * @brief Create all dockable widgets to enable later toggling of display
  */
void gcsMainWindow::createDockWindows()
{
    AHNS_DEBUG("gcsMainWindow::createDockWindows()");

    AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Creating AH Widget ]");
    QDockWidget *dockAH = new QDockWidget(tr("Artificial Horizon"),this);
    AHclass* AH = new AHclass(dockAH);
    dockAH->setWidget(AH);
    m_dockList << dockAH; // keep the object in a list
    addDockWidget(Qt::RightDockWidgetArea,dockAH);
    ui->menuView->insertAction(0,dockAH->toggleViewAction());

    AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Creating System Status Widget ]");
    QDockWidget *dockSS = new QDockWidget(tr("System Status"),this);
    systemStatus* oSystemStatus = new systemStatus(dockSS);
    dockSS->setWidget(oSystemStatus);
    m_dockList << dockSS; // keep in the list
    addDockWidget(Qt::LeftDockWidgetArea,dockSS);
    ui->menuView->insertAction(0,dockSS->toggleViewAction());
    // addDockWidget(Qt::LeftDockWidgetArea,dock);

    AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Creating wifiComms Widget ]");
    QDockWidget *dockWC = new QDockWidget(tr("Wi-Fi Communications"),this);
    wifiComms* owifiComms = new wifiComms(dockWC);
    dockWC->setWidget(owifiComms); 
    m_dockList << dockWC; //keep in the list
    addDockWidget(Qt::BottomDockWidgetArea,dockWC);
    ui->menuView->insertAction(0,dockWC->toggleViewAction());

//    AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Creating telemetry Widget ]");
//    QDockWidget *dockTS = new QDockWidget(tr("Telemetry Status"),this);
//    telemetryStatus* otelemetryStatus = new telemetryStatus(dockTS);
//    dockTS->setWidget(otelemetryStatus);
//    m_dockList << dockTS; //keep in the list
//    addDockWidget(Qt::BottomDockWidgetArea,dockTS);
//    ui->menuView->insertAction(0,dockTS->toggleViewAction());

    AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Connect Slots ]");
    connect(owifiComms,SIGNAL(sigConnectionStart(quint16&,QString&,quint16&,QString&)),this,SLOT(StartTelemetry(quint16&,QString&,quint16&,QString&)));
    connect(owifiComms,SIGNAL(sigConnectionClose()),this,SLOT(CloseTelemetry()));
    connect(owifiComms,SIGNAL(sigConnectionRetry(quint16&,QString&,quint16&,QString&)),this,SLOT(RetryTelemetry(quint16&,QString&,quint16&,QString&)));

    AHNS_DEBUG("gcsMainWindow::createDockWindows() [ COMPLETED ]");
  return;
}

/**
  * @brief Action to accompany the menu bar's Help->About
  */
void gcsMainWindow::on_actionAbout_triggered()
{
    AHNS_DEBUG("gcsMainWindow::on_actionAbout_triggered()");
    m_Aboutfrm->show();
    return;
}

/**
  * @brief Slot to Connect to the wifiComms widget signal to start the telemetry thread
  */
void gcsMainWindow::StartTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP)
{
    AHNS_DEBUG("gcsMainWindow::StartTelemetry()");

    if (m_oTelemetryThread == NULL)
    {
        try
        {
          m_oTelemetryThread = new cTelemetryThread(serverPort,serverIP,clientPort,clientIP);
          m_oTelemetryThread->start();
        }
        catch (const std::exception &e)
        {
            AHNS_ALERT("gcsMainWindow::StartTelemetry() [ THREAD START FAILED " << e.what() << " ]");
            m_oTelemetryThread = NULL;
        }
    }
    else
    {
      AHNS_DEBUG("gcsMainWindow::StartTelemetry() [ FALSE APPLY BUTTON PRESS ]");
    }
    return;
}

/**
  * @brief Slot to connect to the wifiComms widget signal to close down the telemetry thread
  */
void gcsMainWindow::CloseTelemetry()
{
    AHNS_DEBUG("gcsMainWindow::CloseTelemetry()");
    if (m_oTelemetryThread != NULL)
    {
        m_oTelemetryThread->stop();
        while(m_oTelemetryThread->isRunning());

        delete m_oTelemetryThread;
        m_oTelemetryThread = NULL;
    }
    else
    {
        AHNS_DEBUG("gcsMainWindow::CloseTelemetry() [ FALSE CLOSE BUTTON PRESS ]");
    }
    return;
}

/**
  * @brief Slot to connect to the wifiComms widget signal to restart the telemetry thread
  */
void gcsMainWindow::RetryTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP)
{
    AHNS_DEBUG("gcsMainWindow::RetryTelemetry()");

    if (m_oTelemetryThread != NULL)
    {
      // Stop Thread
      m_oTelemetryThread->stop();
      while(m_oTelemetryThread->isRunning());

      // Delete Thread
      delete m_oTelemetryThread;
      m_oTelemetryThread = NULL;

      // Recreate Thread
      try
      {
        m_oTelemetryThread = new cTelemetryThread(serverPort,serverIP,clientPort,clientIP);
        m_oTelemetryThread->start();
      }
      catch (const std::exception &e)
      {
        AHNS_ALERT("gcsMainWindow::ResetTelemetry() [ THREAD RESET FAILED " << e.what() << " ]");
        m_oTelemetryThread = NULL;
      }
    }
    else
    {
        // Create Thread
        try
        {
          m_oTelemetryThread = new cTelemetryThread(serverPort,serverIP,clientPort,clientIP);
          m_oTelemetryThread->start();
        }
        catch (const std::exception &e)
        {
          AHNS_ALERT("gcsMainWindow::RetryTelemetry() [ THREAD RESET FAILED " << e.what() << " ]");
          m_oTelemetryThread = NULL;
        }
    }

    return;
}
