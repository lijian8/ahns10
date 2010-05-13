/**
 * \file   dataplotter.cpp
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
 * Implementation of the Data Plotting Widget.
 */

#include <QResizeEvent>
#include <QCheckBox>

#include "sys/time.h"
#include "state.h"

#include "dataplotter.h"
#include "ui_dataplotter.h"

#include "ahns_logger.h"

DataPlotter::DataPlotter(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::DataPlotter)
{
    AHNS_DEBUG("DataPlotter::DataPlotter(QWidget *parent)");
    ui->setupUi(this);

    int i = 0;

    // Array of all Check boxes
    QCheckBox* checkBoxArray[] =
    {
        ui->filteredRollchkbox,
        ui->filteredRolldchkbox,
        ui->filteredPitchchkbox,
        ui->filteredPitchdchkbox,
        ui->filteredYawchkbox,
        ui->filteredYawdchkbox,
        ui->filteredXchkbox,
        ui->filteredVXchkbox,
        ui->filteredAXchkbox,
        ui->filteredYchkbox,
        ui->filteredVYchkbox,
        ui->filteredAYchkbox,
        ui->filteredZchkbox,
        ui->filteredVZchkbox,
        ui->filteredAZchkbox
    };

    for (i = 0; i < 15; ++i) // eventually curve count
    {
        connect(checkBoxArray[i],SIGNAL(clicked()),this,SLOT(SetActive()));
    }

    // Possible Curve Colours
    QPen cyanPen(QColor(103, 200, 255));	//cyanPen.setWidth(2);
    QPen redPen(QColor(255, 0, 0));		redPen.setStyle(Qt::DashLine);
    QPen greenPen(QColor(0, 255, 0));	        greenPen.setStyle(Qt::DotLine);
    QPen yellowPen(QColor(255, 255, 0));	yellowPen.setStyle(Qt::DashLine);
    QPen pinkPen(QColor(255, 0, 255));	        pinkPen.setStyle(Qt::DashDotLine);

    for (i = 0; i < CURVE_COUNT; ++i)
    {
        // Set Curver Pen
        m_plotCurves[i].setPen(cyanPen);

        // All curves initially not active
        m_activePlot[i] = false;
    }



    this->setMinimumSize(320,240);
}

DataPlotter::~DataPlotter()
{
    delete ui;
}

void DataPlotter::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
  * @brief Plotter Size Hint
  */
QSize DataPlotter::sizeHint() const
{
    return QSize(640,480);
}

/**
  * @brief Resize the widgets
  */
void DataPlotter::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("DataPlotter::resizeEvent (QResizeEvent* e)");
    ui->verticalLayoutWidget->resize(e->size());
    return;
}

/**
  * @brief Ensure new HeliState is available for plotting and plot if needed
  */

void DataPlotter::setHeliStateData(const timeval* const timeStamp, const state_t* heliState)
{
    AHNS_DEBUG("DataPlotter::setHeliStateData(const timeval* timeStamp, const state_t* heliState)");

    // Time
    dataVector[RAW_TIME].push_back(timeStamp->tv_sec + timeStamp->tv_usec*1.0e-6);
    dataVector[TIME].push_back(dataVector[RAW_TIME].last()-dataVector[RAW_TIME].front());

    // Angular Position
    dataVector[F_PHI].push_back(heliState->phi);
    dataVector[F_THETA].push_back(heliState->theta);
    dataVector[F_PSI].push_back(heliState->psi);

    // Angular Rates
    dataVector[F_PHI_DOT].push_back(heliState->p);
    dataVector[F_THETA_DOT].push_back(heliState->q);
    dataVector[F_PSI_DOT].push_back(heliState->r);

    // Linear Positions
    dataVector[F_X].push_back(heliState->x);
    dataVector[F_Y].push_back(heliState->y);
    dataVector[F_Z].push_back(heliState->z);

    // Linear Velocities
    dataVector[F_X_DOT].push_back(heliState->vx);
    dataVector[F_Y_DOT].push_back(heliState->vy);
    dataVector[F_Z_DOT].push_back(heliState->vz);

    // Linear Accelerations
    dataVector[F_AX].push_back(heliState->ax);
    dataVector[F_AY].push_back(heliState->ay);
    dataVector[F_AZ].push_back(heliState->az);

    // Voltage Accelerations
    dataVector[VOLTAGE].push_back(heliState->voltage);

    replot();

    return;
}


/**
  * @brief Replot all active Curve Data
  */

void DataPlotter::replot()
{
    int i = 0;

    for (i = 0; i < CURVE_COUNT; ++i)
    {
        if(m_activePlot[i] == true)
        {
            // Update the Curves
            m_plotCurves[i].setData( dataVector[TIME].data(), dataVector[i].data(), dataVector[i].size() );
            // Attach the curves
            m_plotCurves[i].attach(ui->dataPlotqwtPlot);
        }
        else
        {
            if (m_plotCurves[i].isVisible())
            {
                m_plotCurves[i].detach();
            }
        }
    }

    ui->dataPlotqwtPlot->replot();
    return;
}

/**
  * @brief Slot to update the active graphs after a checkbox tick
  */
void DataPlotter::SetActive()
{
    AHNS_DEBUG("DataPlotter::SetActive()");

    int i = 0;

    // Array of all Check boxes
    QCheckBox* checkBoxArray[] =
    {
        ui->filteredRollchkbox,
        ui->filteredRolldchkbox,
        ui->filteredPitchchkbox,
        ui->filteredPitchdchkbox,
        ui->filteredYawchkbox,
        ui->filteredYawdchkbox,
        ui->filteredXchkbox,
        ui->filteredVXchkbox,
        ui->filteredAXchkbox,
        ui->filteredYchkbox,
        ui->filteredVYchkbox,
        ui->filteredAYchkbox,
        ui->filteredZchkbox,
        ui->filteredVZchkbox,
        ui->filteredAZchkbox
    };

    for (i = 0; i < 15; ++i) // eventaully curve count
    {
        if(checkBoxArray[i]->isChecked())
        {
            m_activePlot[i] = true;
        }
        else
        {
            m_activePlot[i] = false;
        }

    }

    replot();
    return;
}
