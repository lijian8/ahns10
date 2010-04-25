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
#include "AH.h"
#include "systemstatus.h"

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
    QDockWidget *dock = new QDockWidget(tr("Artificial Horizon"),this);
    AHclass* AH = new AHclass(dock);
    dock->setWidget(AH);
    addDockWidget(Qt::RightDockWidgetArea,dock);
    ui->menuView->insertAction(0,dock->toggleViewAction());

    //System Status
    dock = new QDockWidget(tr("System Status"),this);
    SystemStatus* oSystemStatus = new SystemStatus(dock);
    dock->setWidget(oSystemStatus);
    addDockWidget(Qt::LeftDockWidgetArea,dock);
    ui->menuView->insertAction(0,dock->toggleViewAction());


    return;
}

void gcsMainWindow::on_actionAbout_triggered()
{
    m_Aboutfrm->show();
    return;
}
