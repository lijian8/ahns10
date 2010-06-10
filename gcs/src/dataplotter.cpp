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
#include <QVector>

#include <ctime>
#include <stdexcept>
#include <fstream>

#include "sys/time.h"
#include "state_cpp.h"

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
        ui->fcCPUchkbox, \
        ui->fcRCchkbox\
    };

/**
  * @brief Constructor
  */
DataPlotter::DataPlotter(QVector<double>* srcData, QWidget *parent) : QWidget(parent), ui(new Ui::DataPlotter)
{
    AHNS_DEBUG("DataPlotter::DataPlotter(QWidget *parent)");
    ui->setupUi(this);
    int i = 0;

    // Array of all Check boxes
    CHECK_BOX_ARRAY;

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
        case RC_LINK:
            m_plotCurves[i].setTitle(QwtText("RC Link [1/0]"));
            break;
        default:
            AHNS_ALERT("DataPlotter::DataPlotter() [ UNHANDLED CURVE ]");
        }
        m_plotCurves[i].setSymbol(QwtSymbol());
    }

    connect(ui->Clearbtn,SIGNAL(clicked()),this,SLOT(ClearPlots()));

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

    // Address of Data
    m_DataVector = srcData;

    // Update Plots at 25Hz
    updatePlotTimer.setInterval(40);
    connect(&updatePlotTimer,SIGNAL(timeout()),this,SLOT(replot()));
    updatePlotTimer.start();
}

DataPlotter::~DataPlotter()
{

    AHNS_DEBUG("DataPlotter::~DataPlotter");

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
                while ((j < pointLimit) && (j < m_DataVector[i].size()))
                {
                    timePoints[j] = m_DataVector[HELI_STATE_TIME][m_DataVector[HELI_STATE_TIME].size()-j-1];
                    dataPoints[j] = m_DataVector[i][m_DataVector[i].size()-j-1];
                    j++;
                }
                m_plotCurves[i].setData(timePoints, dataPoints, j);
                break;
            case ENGINE1:
            case ENGINE2:
            case ENGINE3:
            case ENGINE4:
            case FC_CPU:
            case RC_LINK:
                j = 0;
                while ( (j < pointLimit) && (j < m_DataVector[i].size()))
                {
                    timePoints[j] = m_DataVector[FC_STATE_TIME][m_DataVector[FC_STATE_TIME].size()-j-1];
                    dataPoints[j] = m_DataVector[i][m_DataVector[i].size()-j-1];
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
  * @brief Slot to clear plots after button click
  */
void DataPlotter::ClearPlots()
{
    AHNS_DEBUG("DataPlotter::ClearPlots()");

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
    }

    replot();
    return;
}
