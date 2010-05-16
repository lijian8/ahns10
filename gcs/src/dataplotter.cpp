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

#include <qwt_symbol.h>

#include "dataplotter.h"
#include "ui_dataplotter.h"

#include "ahns_logger.h"

#define   CHECK_BOX_ARRAY  QCheckBox* checkBoxArray[] = { \
ui->filteredRollchkbox, \
        ui->filteredRolldchkbox, \
        ui->filteredPitchchkbox, \
        ui->filteredPitchdchkbox, \
        ui->filteredYawchkbox, \
        ui->filteredYawdchkbox, \
        ui->filteredXchkbox, \
        ui->filteredVXchkbox, \
        ui->filteredAXchkbox, \
        ui->filteredYchkbox, \
        ui->filteredVYchkbox, \
        ui->filteredAYchkbox, \
        ui->filteredZchkbox, \
        ui->filteredVZchkbox, \
        ui->filteredAZchkbox, \
        ui->voltagechkbox, \
        ui->engine1chkbox, \
        ui->engine2chkbox, \
        ui->engine3chkbox, \
        ui->engine4chkbox, \
        ui->fcCPUchkbox \
    };

/**
  * @brief Constructor
  */
DataPlotter::DataPlotter(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::DataPlotter)
{
    AHNS_DEBUG("DataPlotter::DataPlotter(QWidget *parent)");
    ui->setupUi(this);
    int i = 0;

    // Array of all Check boxes
    CHECK_BOX_ARRAY;

    // Keep At 25Hz
    updatePlotTimer.setInterval(40);
    connect(&updatePlotTimer,SIGNAL(timeout()),this,SLOT(replot()));

    setMinimumSize(640,480);

    // Connect CheckBox Slots
    for (i = 0; i < CURVE_COUNT; ++i)
    {
        connect(checkBoxArray[i],SIGNAL(clicked()),this,SLOT(SetActive()));

        // Update the Legend Items
        switch (i)
        {
        case F_PHI:
            m_plotCurves[i].setTitle(QwtText("Roll Angle [rad]"));
            break;
        case F_PHI_DOT:
            m_plotCurves[i].setTitle(QwtText("Roll Rate [rad/s]"));
            break;
        case F_THETA:
            m_plotCurves[i].setTitle(QwtText("Pitch Angle [rad]"));
            break;
        case F_THETA_DOT:
            m_plotCurves[i].setTitle(QwtText("Pitch Rate [rad/s]"));
            break;
        case F_PSI:
            m_plotCurves[i].setTitle(QwtText("Yaw Angle [rad]"));
            break;
        case F_PSI_DOT:
            m_plotCurves[i].setTitle(QwtText("Yaw Rate [rad/s]"));
            break;
        case F_X:
            m_plotCurves[i].setTitle(QwtText("x Position [m]"));
            break;
        case F_X_DOT:
            m_plotCurves[i].setTitle(QwtText("x Velocity [m/s]"));
            break;
        case F_AX:
            m_plotCurves[i].setTitle(QwtText("x Accelertaion [m/s]"));
            break;
        case F_Y:
            m_plotCurves[i].setTitle(QwtText("y Position [m]"));
            break;
        case F_Y_DOT:
            m_plotCurves[i].setTitle(QwtText("y Velocity [m/s]"));
            break;
        case F_AY:
            m_plotCurves[i].setTitle(QwtText("y Accelertaion [m/s/s]"));
            break;
        case F_Z:
            m_plotCurves[i].setTitle(QwtText("z Position [m]"));
            break;
        case F_Z_DOT:
            m_plotCurves[i].setTitle(QwtText("z Velocity [m]"));
            break;
        case F_AZ:
            m_plotCurves[i].setTitle(QwtText("z Accelertaion [m]"));
            break;
        case VOLTAGE:
            m_plotCurves[i].setTitle(QwtText("Voltage [V]"));
            break;
        case ENGINE1:
            m_plotCurves[i].setTitle(QwtText("Engine 1 Commanded [ms]"));
            break;
        case ENGINE2:
            m_plotCurves[i].setTitle(QwtText("Engine 2 Commanded [ms]"));
            break;
        case ENGINE3:
            m_plotCurves[i].setTitle(QwtText("Engine 3 Commanded [ms]"));
            break;
        case ENGINE4:
            m_plotCurves[i].setTitle(QwtText("Engine 4 Commanded [ms]"));
            break;
        case FC_CPU:
            m_plotCurves[i].setTitle(QwtText("CPU Usage [%]"));
            break;
        }
        m_plotCurves[i].setSymbol(QwtSymbol());
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
            //randomPen.setStyle(Qt::DotLine);
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

    // Legend
    ui->dataPlotqwtPlot->insertLegend(&m_legend,QwtPlot::BottomLegend);

    // Grid
    QPen gridPen(QColor(190,190,190));
    gridPen.setStyle(Qt::DotLine);

    m_coordinateGrid.enableX(true);
    m_coordinateGrid.enableY(true);

    m_coordinateGrid.setMajPen(gridPen);
    m_coordinateGrid.setMinPen(gridPen);
    m_coordinateGrid.attach(ui->dataPlotqwtPlot);

    // By Default no Logging
    m_loggingOn = false;
    //initialiseLogs();
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
  * @brief Slot for initialising the logging files
  */
void DataPlotter::initialiseLogs()
{

    // Logging is Set
    m_loggingOn = true;

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
    if (m_loggingOn)
    {
        stateOutputFile << dataVector[HELI_STATE_RAW_TIME].last()-dataVector[HELI_STATE_RAW_TIME].front() << "," << heliState->phi << ",";
        stateOutputFile << heliState->p << "," << heliState->theta << "," << heliState->q << ",";
        stateOutputFile << heliState->psi << "," << heliState->r << "," << heliState->x << "," << heliState->vx << ",";
        stateOutputFile << heliState->ax << "," << heliState->y << "," << heliState->vy << "," << heliState->ay << ",";
        stateOutputFile << heliState->z << "," << heliState->vz << "," << heliState->az << "," << heliState->voltage << std::endl;
    }
    if (!updatePlotTimer.isActive())
    {
        updatePlotTimer.start();
    }

    emit newPlottingData(dataVector);
    return;
}


/**
  * @brief Replot all active curve data
  */

void DataPlotter::replot()
{
    AHNS_DEBUG("DataPlotter::replot()");
    int i = 0;
    int j = 0;

    const int pointLimit = 1500;
    double timePoints[pointLimit];
    double dataPoints[pointLimit];

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
                j = 0;
                while ((j < pointLimit) && (j < dataVector[i].size()))
                {
                    timePoints[j] = dataVector[HELI_STATE_TIME][dataVector[HELI_STATE_TIME].size()-j-1];
                    dataPoints[j] = dataVector[i][dataVector[i].size()-j-1];
                    j++;
                }
                m_plotCurves[i].setData(timePoints, dataPoints, j);
                break;
            case ENGINE1:
            case ENGINE2:
            case ENGINE3:
            case ENGINE4:
            case FC_CPU:
                j = 0;
                while ( (j < pointLimit) && (j < dataVector[i].size()))
                {
                    timePoints[j] = dataVector[FC_STATE_TIME][dataVector[FC_STATE_TIME].size()-j-1];
                    dataPoints[j] = dataVector[i][dataVector[i].size()-j-1];
                    j++;
                }
                m_plotCurves[i].setData(timePoints, dataPoints, j);
                break;
            }

            // Attach the curves
            m_plotCurves[i].attach(ui->dataPlotqwtPlot);
        }
        else
        {
            m_plotCurves[i].detach();
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
    CHECK_BOX_ARRAY;

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
    CHECK_BOX_ARRAY;

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

    // Restart Logging
    if (m_loggingOn)
    {
        initialiseLogs();
    }

    replot();
    return;
}

/**
  * @brief Copy the last data points
  */
void DataPlotter::copyNewData(const QVector<double>* const srcData)
{
    AHNS_DEBUG("DataPlotter::copyNewData(const QVector<double>* const srcData)");
    int i = 0;
    for ( i = 0; i < DATA_COUNT; ++i)
    {
        if (!srcData[i].isEmpty())
        {
            dataVector[i].push_back(srcData[i].last());
        }
    }

    if (!updatePlotTimer.isActive())
    {
        updatePlotTimer.start();
    }

    return;
}

/**
  * @brief Assignment Operator to copy the dataVector between objects
  */
DataPlotter& DataPlotter::operator=(const DataPlotter& srcDataPlotter)
                                   {
    AHNS_DEBUG("DataPlotter::operator=(const DataPlotter& srcDataPlotter)");
    int i = 0;
    for ( i = 0; i < DATA_COUNT; ++i)
    {
        dataVector[i] = srcDataPlotter.dataVector[i];
    }

    return *this;
}
