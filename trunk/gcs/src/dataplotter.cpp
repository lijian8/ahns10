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
#include <ctime>
#include <stdexcept>
#include <fstream>

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
        ui->filteredAZchkbox,
        ui->engine1chkbox,
        ui->engine2chkbox,
        ui->engine3chkbox,
        ui->engine4chkbox,
        ui->voltagechkbox,
        ui->fcCPUchkbox
    };

    for (i = 0; i < CURVE_COUNT; ++i)
    {
        connect(checkBoxArray[i],SIGNAL(clicked()),this,SLOT(SetActive()));
    }

    connect(ui->Clearbtn,SIGNAL(clicked()),this,SLOT(ClearAll()));

    // Possible Curve Pens
    srand(time(NULL));
    for (i = 0; i < CURVE_COUNT; ++i)
    {
        QPen randomPen(QColor(50 + rand() % 206,50 + rand() % 206, 50 + rand() % 206));

        switch (rand() % 5 + 1)
        {
        case 1:
            randomPen.setStyle(Qt::SolidLine);
            break;
        case 2:
            randomPen.setStyle(Qt::DashLine);
            break;
        case 3:
            randomPen.setStyle(Qt::DotLine);
            break;
        case 4:
            randomPen.setStyle(Qt::DashDotLine);
            break;
        case 5:
            randomPen.setStyle(Qt::DashDotDotLine);
            break;
        }

        // Set Curver Pen
        m_plotCurves[i].setPen(randomPen);

        // All curves initially not active
        m_activePlot[i] = false;
    }


    initialiseLogs();

    this->setMinimumSize(320,240);
}

void DataPlotter::initialiseLogs()
{
    // Close Files if open
    if (stateOutputFile.is_open())
    {
        stateOutputFile.close();
    }

    // Open Log Files
    time_t logFileTime;
    std::time(&logFileTime);
    char logFileName[80];

    // State File
    strftime(logFileName, 80, "logs/Filtered_States_%A-%d-%m-%G-%H%M%S.log", localtime(&logFileTime));
    stateOutputFile.open(logFileName);
    if (stateOutputFile.fail())
    {
        AHNS_DEBUG("DataPlotter::DataPlotter(QWidget *parent) [ FAILED FILE OPEN ]");
        throw std::runtime_error("DataPlotter::DataPlotter(QWidget *parent) [ FAILED FILE OPEN ]");
    }
    else
    {
        stateOutputFile << "AHNS STATE MESSAGES LOG FOR " << logFileName << std::endl;
        stateOutputFile << "TIME, F_PHI, F_PHI_DOT, F_THETA, F_THETA_DOT, F_PSI, F_PSI_DOT, F_X, F_X_DOT,";
        stateOutputFile << "F_AX, F_Y, F_Y_DOT, F_AY, F_Z, F_Z_DOT, F_AZ, VOLTAGE" << std::endl;
    }
    return;
}

DataPlotter::~DataPlotter()
{

    AHNS_DEBUG("DataPlotter::~DataPlotter");
    if (stateOutputFile.is_open())
    {
        stateOutputFile.close();
    }
    delete ui;
}

void DataPlotter::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
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
    dataVector[HELI_STATE_RAW_TIME].push_back(timeStamp->tv_sec + timeStamp->tv_usec*1.0e-6);
    dataVector[HELI_STATE_TIME].push_back(dataVector[HELI_STATE_RAW_TIME].last()-dataVector[HELI_STATE_RAW_TIME].front());

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


    // Log the State Data
    stateOutputFile << dataVector[HELI_STATE_RAW_TIME].last()-dataVector[HELI_STATE_RAW_TIME].front() << "," << heliState->phi << ",";
    stateOutputFile << heliState->p << "," << heliState->theta << "," << heliState->q << ",";
    stateOutputFile << heliState->psi << "," << heliState->r << "," << heliState->x << "," << heliState->vx << ",";
    stateOutputFile << heliState->ax << "," << heliState->y << "," << heliState->vy << "," << heliState->ay << ",";
    stateOutputFile << heliState->z << "," << heliState->vz << "," << heliState->az << "," << heliState->voltage << std::endl;
    replot();

    return;
}


/**
  * @brief Replot all active Curve Data
  */

void DataPlotter::replot()
{
    AHNS_DEBUG("DataPlotter::replot()");
    int i = 0;

    for (i = 0; i < CURVE_COUNT; ++i)
    {
        if(m_activePlot[i] == true)
        {
            // Update the Curves
            switch (i)
            {
            case F_PHI:
            case F_PHI_DOT:
            case F_THETA:
            case F_THETA_DOT:
            case F_PSI:
            case F_PSI_DOT:
            case F_X:
            case F_X_DOT:
            case F_AX:
            case F_Y:
            case F_Y_DOT:
            case F_AY:
            case F_Z:
            case F_Z_DOT:
            case F_AZ:
            case VOLTAGE:
                m_plotCurves[i].setData( dataVector[HELI_STATE_TIME].data(), dataVector[i].data(), dataVector[i].size() );
                break;
            case ENGINE1:
            case ENGINE2:
            case ENGINE3:
            case ENGINE4:
            case FC_CPU:
                m_plotCurves[i].setData( dataVector[FC_STATE_TIME].data(), dataVector[i].data(), dataVector[i].size() );
                break;
            }


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
        ui->filteredAZchkbox,
        ui->engine1chkbox,
        ui->engine2chkbox,
        ui->engine3chkbox,
        ui->engine4chkbox,
        ui->voltagechkbox,
        ui->fcCPUchkbox
    };

    for (i = 0; i < CURVE_COUNT; ++i)
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


/**
  * @brief Slot clear all Graphing and Data After button click
  */
void DataPlotter::ClearAll()
{
    AHNS_DEBUG("DataPlotter::ClearAll()");

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
        ui->filteredAZchkbox,
        ui->engine1chkbox,
        ui->engine2chkbox,
        ui->engine3chkbox,
        ui->engine4chkbox,
        ui->voltagechkbox,
        ui->fcCPUchkbox
    };

    for (i = 0; i < CURVE_COUNT; ++i)
    {
        // UnClick the Checkboxes
        if(checkBoxArray[i]->isChecked())
        {
            checkBoxArray[i]->click();
        }

        // Deactive the plot
        m_activePlot[i] = false;

        // Clear the Data
        dataVector[i].clear();
    }

    initialiseLogs();
    replot();
    return;
}
