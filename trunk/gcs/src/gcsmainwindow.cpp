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
#include "bfimagefeed.h"

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
    AHNS_DEBUG("gcsMainWindow::~gcsMainWindow() [ COMPLETED ]");
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
        dockAH->setObjectName("Artificial Horizon");
        QDockWidget* dockSS = new QDockWidget(tr("System Status"),this);
        dockSS->setObjectName("System Status");
        QDockWidget* dockRC = new QDockWidget(tr("Received Packets"),this);
        dockRC->setObjectName("Received Packets");
        QDockWidget* dockWC = new QDockWidget(tr("Wi-Fi Communications"),this);
        dockWC->setObjectName("Wi-Fi Communications");
        QDockWidget* dockDP = new QDockWidget(tr("Data Plotter"),this);
        dockDP->setObjectName("Data Plotter");
        QDockWidget* dockBF = new QDockWidget(tr("Blackfin"),this);
        dockBF->setObjectName("Blackfin Camera feed");

        m_bfImageFeedWidget = new bfImageFeed(dockBF);


        AHNS_DEBUG("gcsMainWindow::createDockWindows() [ Creating Widgets ]");
        m_ahWidget = new AHclass(dockAH);
        m_systemStatusWidget = new SystemStatus(dockSS);
        m_wifiCommsWidget = new wifiComms(dockWC);
        m_receiveConsoleWidget = new ReceiveConsole(dockRC);
        m_dataPlotterWidget = new DataPlotter(m_Data.getData(),dockDP);

        dockAH->setWidget(m_ahWidget);
        dockSS->setWidget(m_systemStatusWidget);
        dockRC->setWidget(m_receiveConsoleWidget);
        dockWC->setWidget(m_wifiCommsWidget);
        dockDP->setWidget(m_dataPlotterWidget);
        dockBF->setWidget(m_bfImageFeedWidget);

        addDockWidget(Qt::RightDockWidgetArea,dockAH);
        addDockWidget(Qt::RightDockWidgetArea,dockSS);
        addDockWidget(Qt::RightDockWidgetArea,dockRC);
        addDockWidget(Qt::RightDockWidgetArea,dockWC);
        addDockWidget(Qt::LeftDockWidgetArea,dockDP);
        addDockWidget(Qt::RightDockWidgetArea,dockBF);


        //setTabPosition(Qt::RightDockWidgetArea,QTabWidget::South);
        tabifyDockWidget(dockWC,dockRC);
        tabifyDockWidget(dockAH,dockBF);

        ui->menuView->insertAction(0,dockAH->toggleViewAction());
        ui->menuView->insertAction(0,dockSS->toggleViewAction());
        ui->menuView->insertAction(0,dockWC->toggleViewAction());
        ui->menuView->insertAction(0,dockRC->toggleViewAction());
        ui->menuView->insertAction(0,dockDP->toggleViewAction());

        ui->menuView->insertAction(0,dockBF->toggleViewAction());

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
  * @brief Create new data plotting widget docks
  */

void gcsMainWindow::on_actionNew_Plotting_Widget_triggered()
{
    AHNS_DEBUG("gcsMainWindow::on_actionNew_Plotting_Widget_triggered()");

    QString tempStr;
    tempStr.setNum(m_plottingWidgets.size() + 1);
    // Dock
    m_plottingDock.push_back(new QDockWidget(QString(tr("Data Plotter") % "(" % tempStr % ")"),this));
    m_plottingDock.last()->setObjectName(QString(tr("Data Plotter") % "(" % tempStr % ")"));
    // Widget
    m_plottingWidgets.push_back(new DataPlotter(m_Data.getData(),m_plottingDock.last()));
    //*m_plottingWidgets.last() = *m_dataPlotterWidget;
    // Dock widget
    m_plottingDock.last()->setWidget(m_plottingWidgets.last());


    addDockWidget(Qt::LeftDockWidgetArea,m_plottingDock.last());
    //m_plottingDock.last()->setFloating(true);
    // m_plottingDock.last()->show();

    ui->menuView->insertAction(0,m_plottingDock.last()->toggleViewAction());
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
  * @brief Remove all Plotting Widgets
  */
void gcsMainWindow::on_actionRemove_All_Plotting_Widgets_triggered()
{
    AHNS_DEBUG("gcsMainWindow::on_actionRemove_All_Plotting_Widgets_triggered()");

    QLinkedList<QDockWidget*>::iterator iter = m_plottingDock.begin();
    for (iter = m_plottingDock.begin(); iter != m_plottingDock.end(); ++iter)
    {
        (*iter)->setAttribute(Qt::WA_DeleteOnClose);
        (*iter)->close();
    }
    m_plottingDock.clear();
    m_plottingWidgets.clear();

    return;
}

/**
  * @brief Remove last Plotting Widgets
  */
void gcsMainWindow::on_actionRemove_Last_Plotting_Widget_triggered()
{
    AHNS_DEBUG("gcsMainWindow::on_actionRemove_Last_Plotting_Widget_triggered()");

    m_plottingDock.last()->setAttribute(Qt::WA_DeleteOnClose);
    m_plottingDock.last()->close();
    m_plottingDock.removeLast();
    m_plottingWidgets.removeLast();
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
            connect(m_TelemetryThread,SIGNAL(NewCloseMessage(const timeval, const int)),this,SLOT(ProcessCloseMessage(const timeval, const int)));
            connect(m_TelemetryThread,SIGNAL(NewFCState(const timeval, const fc_state_t, const int)),this,SLOT(ProcessFCState(const timeval, const fc_state_t, const int)));
            m_receiveConsoleWidget->clearConsole();

            // Start the timer
            m_TelSecCount = 0;
            m_TelMinCount = 0;
            m_TelHourCount = 0;
            m_oTelUptimer.start();
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
        if ((m_TelemetryThread->isRunning()) && (m_TelemetryThread->isConnected()))
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
            QMessageBox messageBox(QMessageBox::Critical,"Telemetry Thread Dropped","Thread Timed Out or Stopped",QMessageBox::Ok);
            messageBox.setStyleSheet(this->styleSheet());
            messageBox.show();
            messageBox.exec();

            m_oTelUptimer.stop();
            // Not Connected so don't run
            if (m_TelemetryThread != NULL)
            {
                m_TelemetryThread->stop();
                m_TelemetryThread->wait();
                delete m_TelemetryThread;
                m_TelemetryThread = NULL;
            }
        }
    }
    else
    {
        AHNS_ALERT("gcsMainWindow::TelemetryMonitor() [ TELEMETRY THREAD DELETED EARLY ]");
        QMessageBox messageBox(QMessageBox::Critical,"Failed Telemetry Thread","Thread Deleted",QMessageBox::Ok);
        messageBox.setStyleSheet(this->styleSheet());
        messageBox.show();
        messageBox.exec();
        m_oTelUptimer.stop();
    }
    return;
}

/**
  * @brief Menu slot to save config
  */
void gcsMainWindow::on_actionSave_Config_triggered()
{
    AHNS_DEBUG("void gcsMainWindow::on_actionSave_Config_triggered()");
    QString fileName;
    QFile file;
    QString msg;
    bool writeSuccess = false;

    fileName = QFileDialog::getSaveFileName(this, tr("Save layout"));
    if (!fileName.isEmpty())
    {
        file.setFileName(fileName);

        if (!file.open(QFile::WriteOnly))
        {
            msg = tr("Failed to open %1\n%2").arg(fileName).arg(file.errorString());
            QMessageBox::warning(this, tr("Save Error"), msg);
            AHNS_DEBUG("void gcsMainWindow::on_actionSave_Config_triggered() [ OPEN FAILED ]");
        }
        else
        {
            QByteArray geo_data = saveGeometry();
            QByteArray layout_data = saveState();

            writeSuccess = file.putChar((uchar)geo_data.size());
            if (writeSuccess)
            {
                writeSuccess = file.write(geo_data) == geo_data.size();
            }

            if (writeSuccess)
            {
                writeSuccess = file.write(layout_data) == layout_data.size();
            }

            if (!writeSuccess)
            {
                msg = tr("Error writing to %1\n%2") .arg(fileName).arg(file.errorString());
                QMessageBox::warning(this, tr("Save Error"), msg);
                AHNS_DEBUG("void gcsMainWindow::on_actionSave_Config_triggered() [ SAVE FAILED ]");
            }
            file.close();
        }
    }
    return;
}

void gcsMainWindow::on_actionLoad_Config_triggered()
{
    AHNS_DEBUG("void gcsMainWindow::on_actionLoad_Config_triggered()");

    QString fileName;
    bool readSuccess = false;
    fileName = QFileDialog::getOpenFileName(this, tr("Load layout"));

    if (!fileName.isEmpty())
    {

        QFile file(fileName);
        if (!file.open(QFile::ReadOnly))
        {
            QString msg = tr("Failed to open %1\n%2").arg(fileName).arg(file.errorString());
            QMessageBox::warning(this, tr("Load Error"), msg);
            AHNS_DEBUG("void gcsMainWindow::on_actionLoad_Config_triggered() [ LOAD ERROR ]");
        }
        else
        {

            uchar geo_size;
            QByteArray geo_data;
            QByteArray layout_data;

            readSuccess = file.getChar((char*)&geo_size);
            if (readSuccess)
            {
                geo_data = file.read(geo_size);
                readSuccess  = geo_data.size() == geo_size;
            }
            if (readSuccess)
            {
                layout_data = file.readAll();
                readSuccess  = layout_data.size() > 0;
            }

            if (readSuccess )
            {
                readSuccess  = restoreGeometry(geo_data);
            }
            if (readSuccess )
            {
                readSuccess  = restoreState(layout_data);
            }

            if (!readSuccess)
            {
                QString msg = tr("Error reading %1").arg(fileName);
                QMessageBox::warning(this, tr("Load Error"), msg);
                AHNS_DEBUG("void gcsMainWindow::on_actionLoad_Config_triggered() [ READ ERROR ]");
            }
        }
    }
    return;
}

/**
  * @brief Restart the Data Logging
  */
void gcsMainWindow::on_actionRestart_Logging_triggered()
{
    m_Data.clearData();
    m_Data.initialiseLogs();
}
