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
#include <QMessageBox>
#include <QMetaType>

#include "gcsmainwindow.h"
#include "ui_gcsmainwindow.h"
#include "state.h"
#include "sys/time.h"

// Widgets for the Docks
#include "AH.h"
#include "systemstatus.h"
#include "wificomms.h"
#include "receiveconsole.h"
#include "dataplotter.h"

// Threads
#include "telemetrythread.h"

// AHNS Reuse
#include "ahns_logger.h"

gcsMainWindow::gcsMainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::gcsMainWindow)
{
    ui->setupUi(this);

    // Register Types
    qRegisterMetaType<timeval>("timeval");
    qRegisterMetaType<state_t>("state_t");
    qRegisterMetaType<fc_state_t>("fc_state_t");
    qRegisterMetaType<ap_state_t>("ap_state_t");
    qRegisterMetaType<gains_t>("gains_t");
    qRegisterMetaType<loop_parameters_t>("loop_parameters_t");



    createDockWindows();

    // Actions for Main Menu
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(on_actionAbout_triggered()));

    // Create about form
    m_Aboutfrm = new aboutForm;
    m_Aboutfrm->setStyleSheet(this->styleSheet());

    // TelemetryThread not started yet
    m_TelemetryThread = NULL;

    // Timer signals
    connect(&m_oTelUptimer,SIGNAL(timeout()),this,SLOT(TelemetryMonitor()));

    //timer variables
    m_oTelUptimer.setInterval(1000);
    m_TelSecCount = 0;
    m_TelMinCount = 0;
    m_TelHourCount = 0;
}

gcsMainWindow::~gcsMainWindow()
{
    AHNS_DEBUG("gcsMainWindow::~gcsMainWindow()");
    AHNS_DEBUG("gcsMainWindow::~gcsMainWindow() [Thread Pointer = "  << m_TelemetryThread << " ]");

    if (m_TelemetryThread != NULL)
    {
        m_TelemetryThread->stop();
        m_TelemetryThread->wait();
        delete m_TelemetryThread;
    }

    delete m_Aboutfrm;
    delete ui;
    AHNS_DEBUG("gcsMainWindow::~gcsMainWindow() [ COMPLETED ]")
        }

void gcsMainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
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
    try
    {
        AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Creating Docks ]");
        QDockWidget* dockAH = new QDockWidget(tr("Artificial Horizon"),this);
        QDockWidget* dockSS = new QDockWidget(tr("System Status"),this);
        QDockWidget* dockRC = new QDockWidget(tr("Received Packets"),this);
        QDockWidget* dockWC = new QDockWidget(tr("Wi-Fi Communications"),this);
        QDockWidget* dockDP = new QDockWidget(tr("Data Plotter"),this);

        AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Creating Widgets ]");
        m_ahWidget = new AHclass(dockAH);
        m_systemStatusWidget = new SystemStatus(dockSS);
        m_wifiCommsWidget = new wifiComms(dockWC);
        m_receiveConsoleWidget = new ReceiveConsole(dockRC);
        m_dataPlotterWidget = new DataPlotter(dockDP);

        dockAH->setWidget(m_ahWidget);
        dockSS->setWidget(m_systemStatusWidget);
        dockRC->setWidget(m_receiveConsoleWidget);
        dockWC->setWidget(m_wifiCommsWidget);
        dockDP->setWidget(m_dataPlotterWidget);

        addDockWidget(Qt::RightDockWidgetArea,dockAH);
        addDockWidget(Qt::RightDockWidgetArea,dockSS);
        addDockWidget(Qt::RightDockWidgetArea,dockRC);
        addDockWidget(Qt::RightDockWidgetArea,dockWC);
        addDockWidget(Qt::LeftDockWidgetArea,dockDP);

        //setTabPosition(Qt::RightDockWidgetArea,QTabWidget::South);
        tabifyDockWidget(dockWC,dockRC);

        ui->menuView->insertAction(0,dockAH->toggleViewAction());
        ui->menuView->insertAction(0,dockSS->toggleViewAction());
        ui->menuView->insertAction(0,dockWC->toggleViewAction());
        ui->menuView->insertAction(0,dockRC->toggleViewAction());
        ui->menuView->insertAction(0,dockDP->toggleViewAction());

        AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Connect Slots ]");
        connect(m_wifiCommsWidget,SIGNAL(ConnectionStart(quint16&,QString&,quint16&,QString&)),this,SLOT(StartTelemetry(quint16&,QString&,quint16&,QString&)));
        connect(m_wifiCommsWidget,SIGNAL(ConnectionClose()),this,SLOT(CloseTelemetry()));
        connect(m_wifiCommsWidget,SIGNAL(ConnectionRetry(quint16&,QString&,quint16&,QString&)),this,SLOT(RetryTelemetry(quint16&,QString&,quint16&,QString&)));
        connect(this,SIGNAL(NewTelemetryStatus(quint32,quint8,quint8)),m_wifiCommsWidget,SLOT(lcdUpdate(quint32,quint8,quint8)));
    }
    catch (std::exception& e)
    {
        AHNS_ALERT("gcsMainWindow::createDockWindows() [ CREATE DOCKS AND WIDGETS ERROR " << e.what() << " ]");
    }

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
    AHNS_DEBUG("gcsMainWindow::StartTelemetry() [ MAIN THREAD ID " << (int) QThread::currentThreadId() << " ]");
    if (m_TelemetryThread == NULL)
    {
        try
        {
            // Allocate the Thread
            m_TelemetryThread = new TelemetryThread(serverPort,serverIP,clientPort,clientIP);

            // Speed Information
            connect(m_TelemetryThread,SIGNAL(RxEstimate(double)),m_receiveConsoleWidget,SLOT(RxSpeed(const double&)));

            // Rx Messages
            connect(m_TelemetryThread,SIGNAL(NewHeliState(const timeval, const state_t, const int)),this,SLOT(ProcessHeliState(const timeval, const state_t, const int)));
            connect(m_TelemetryThread,SIGNAL(NewAckMessage(const timeval, const int)),this,SLOT(ProcessAckMessage(const timeval, const int)));

            m_receiveConsoleWidget->clearConsole();
        }
        catch (const std::exception &e)
        {
            AHNS_ALERT("gcsMainWindow::StartTelemetry() [ THREAD START FAILED " << e.what() << " ]");
            m_TelemetryThread = NULL;
            QMessageBox messageBox(QMessageBox::Warning,"Failed Telemetry Launch",e.what());
            messageBox.setStyleSheet(this->styleSheet());
            messageBox.show();
            messageBox.exec();
        }
    }
    else
    {
        AHNS_DEBUG("gcsMainWindow::StartTelemetry() [ FALSE OPEN BUTTON PRESS ]");
        QMessageBox messageBox(QMessageBox::Information,"Telemetry","Telemetry Already Running",QMessageBox::Ok);
        messageBox.setStyleSheet(this->styleSheet());
        messageBox.show();
        messageBox.exec();
    }
    return;
}

/**
  * @brief Slot to connect to the wifiComms widget signal to close down the telemetry thread
  */
void gcsMainWindow::CloseTelemetry()
{
    AHNS_DEBUG("gcsMainWindow::CloseTelemetry()");
    if (m_TelemetryThread != NULL)
    {
        m_TelemetryThread->stop();
        m_TelemetryThread->wait();

        delete m_TelemetryThread;
        m_TelemetryThread = NULL;

        m_oTelUptimer.stop();
    }
    else
    {
        AHNS_DEBUG("gcsMainWindow::CloseTelemetry() [ FALSE CLOSE BUTTON PRESS ]");
        QMessageBox messageBox(QMessageBox::Information,"Telemetry","Telemetry Already Stopped",QMessageBox::Ok);
        messageBox.setStyleSheet(this->styleSheet());
        messageBox.show();
        messageBox.exec();
    }
    return;
}

/**
  * @brief Slot to connect to the wifiComms widget signal to restart the telemetry thread
  */
void gcsMainWindow::RetryTelemetry(quint16& serverPort, QString& serverIP, quint16& clientPort, QString& clientIP)
{
    AHNS_DEBUG("gcsMainWindow::RetryTelemetry()");

    if (m_TelemetryThread != NULL)
    {
        // Stop Thread
        CloseTelemetry();

        // Recreate Thread
        try
        {
            StartTelemetry(serverPort, serverIP, clientPort, clientIP);
        }
        catch (const std::exception &e)
        {
            AHNS_ALERT("gcsMainWindow::ResetTelemetry() [ THREAD RESET FAILED " << e.what() << " ]");
            m_TelemetryThread = NULL;
            QMessageBox messageBox(QMessageBox::Warning,"Failed Telemetry Restart",e.what(),QMessageBox::Ok);
            messageBox.setStyleSheet(this->styleSheet());
            messageBox.show();
            messageBox.exec();
        }
    }
    else
    {
        // Create Thread
        StartTelemetry(serverPort, serverIP, clientPort, clientIP);
    }

    return;
}

/**
  * @brief Telemetry Thread Monitor
  */
void gcsMainWindow::TelemetryMonitor()
{
    if (m_TelemetryThread != NULL)
    {
        if (m_TelemetryThread->isRunning())
        {
            m_TelSecCount++;
            if (m_TelSecCount == 60)
            {
                m_TelSecCount = 0;
                m_TelMinCount++;
                if (m_TelMinCount > 60)
                {
                    m_TelMinCount = 0;
                    m_TelHourCount++;
                }
            }
            emit NewTelemetryStatus(m_TelHourCount,m_TelMinCount,m_TelSecCount);
        }
        else
        {
            AHNS_ALERT("gcsMainWindow::TelemetryMonitor() [ TELEMETRY THREAD DROPPED ]");
            QMessageBox messageBox(QMessageBox::Critical,"Failed Telemetry Thread","Thread Stopped",QMessageBox::Ok);
            messageBox.setParent(this);
            messageBox.show();
            messageBox.exec();
        }
    }
    else
    {
        AHNS_ALERT("gcsMainWindow::TelemetryMonitor() [ TELEMETRY THREAD DELETED EARLY ]");
        QMessageBox messageBox(QMessageBox::Critical,"Failed Telemetry Thread","Thread Deleted",QMessageBox::Ok);
        messageBox.setStyleSheet(this->styleSheet());
        messageBox.show();
        messageBox.exec();
    }
    return;
}
