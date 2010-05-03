/**
 * \file   gcsmainwindow.cpp
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
 * GCS form implementation for gcsMainWindow class
 */

#include <QtGui>

#include "gcsmainwindow.h"
#include "ui_gcsmainwindow.h"

// Widgets for the Docks
#include "AH.h"
#include "systemstatus.h"
#include "wificomms.h"

//AHNS Reuse
#include "ahns_logger.h"

gcsMainWindow::gcsMainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::gcsMainWindow)
{
    ui->setupUi(this);

    createDockWindows();

    // Actions for Main Menu
    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(on_actionAbout_triggered()));

    // Create about form
    m_Aboutfrm = new aboutForm;

}

gcsMainWindow::~gcsMainWindow()
{
    delete m_Aboutfrm;
    delete ui;
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
  * \brief Create all dockable widgets to enable later toggling of display
  */
void gcsMainWindow::createDockWindows()
{
    // Artifical Horizon
    AHNS_DEBUG("Creating AH Widget");
    QDockWidget *dockAH = new QDockWidget(tr("Artificial Horizon"),this);
    AHclass* AH = new AHclass(dockAH);
    dockAH->setWidget(AH);
    dockList << dockAH; // keep the object in a list
    addDockWidget(Qt::RightDockWidgetArea,dockAH);

    ui->menuView->insertAction(0,dockAH->toggleViewAction());

    // AH Slots
    AHNS_DEBUG("Connecting AH Slots");

    //System Status
    AHNS_DEBUG("Creating System Status Widget");
    QDockWidget *dockSS = new QDockWidget(tr("System Status"),this);
    systemStatus* oSystemStatus = new systemStatus(dockSS);
    dockSS->setWidget(oSystemStatus);
    dockList << dockSS; // keep in the list
    addDockWidget(Qt::LeftDockWidgetArea,dockSS);
    ui->menuView->insertAction(0,dockSS->toggleViewAction());
    // addDockWidget(Qt::LeftDockWidgetArea,dock);

    //System Status Slots
    AHNS_DEBUG("Connecting System Status Slots");

    //wifiComms
    AHNS_DEBUG("Creating wifiComms Widget");
    QDockWidget *dockWC = new QDockWidget(tr("Wi-Fi Communications"),this);
    wifiComms* owifiComms = new wifiComms(dockWC);
    dockWC->setWidget(owifiComms); 
    dockList << dockWC; //keep in the list
    addDockWidget(Qt::BottomDockWidgetArea,dockWC);
    ui->menuView->insertAction(0,dockWC->toggleViewAction());

    //wifi Comms Slots
    AHNS_DEBUG("Connecting wifiComms Slots");
  return;
}

void gcsMainWindow::on_actionAbout_triggered()
{
    m_Aboutfrm->show();
    return;
}
