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
        ui->tracechkbox, \
        ui->imuRollchkbox, \
        ui->imuPitchchkbox, \
        ui->imuYawchkbox, \
        ui->imuAXchkbox, \
        ui->imuAYchkbox, \
        ui->imuAZchkbox, \
        ui->engine1chkbox, \
        ui->engine2chkbox, \
        ui->engine3chkbox, \
        ui->engine4chkbox, \
        ui->fcCPUchkbox, \
        ui->fcRCchkbox, \
        ui->refRollchkbox, \
        ui->refPitchchkbox, \
        ui->refYawchkbox, \
        ui->refXchkbox, \
        ui->refYchkbox, \
        ui->refZchkbox, \
        ui->rollActivechkbox, \
        ui->pitchActivechkbox, \
        ui->yawActivechkbox, \
        ui->xActivechkbox, \
        ui->yActivechkbox, \
        ui->zActivechkbox \
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

    // Connect CheckBox Slots
    for (i = 0; i < CURVE_COUNT; ++i)
    {
        connect(checkBoxArray[i],SIGNAL(clicked()),this,SLOT(SetActive()));
        m_plotCurves[i].setSymbol(QwtSymbol());
    }

    connect(ui->Clearbtn,SIGNAL(clicked()),this,SLOT(ClearPlots()));

    m_plotCurves[F_PHI].setTitle(QwtText("Roll Angle [rad]"));
    m_plotCurves[F_PHI_DOT].setTitle(QwtText("Roll Rate [rad/s]"));
    m_plotCurves[F_THETA].setTitle(QwtText("Pitch Angle [rad]"));
    m_plotCurves[F_THETA_DOT].setTitle(QwtText("Pitch Rate [rad/s]"));
    m_plotCurves[F_PSI].setTitle(QwtText("Yaw Angle [rad]"));
    m_plotCurves[F_PSI_DOT].setTitle(QwtText("Yaw Rate [rad/s]"));
    m_plotCurves[F_X].setTitle(QwtText("x Position [m]"));
    m_plotCurves[F_X_DOT].setTitle(QwtText("x Velocity [m/s]"));
    m_plotCurves[F_AX].setTitle(QwtText("x Accelertaion [m/s]"));
    m_plotCurves[F_Y].setTitle(QwtText("y Position [m]"));
    m_plotCurves[F_Y_DOT].setTitle(QwtText("y Velocity [m/s]"));
    m_plotCurves[F_AY].setTitle(QwtText("y Accelertaion [m/s/s]"));
    m_plotCurves[F_Z].setTitle(QwtText("z Position [m]"));
    m_plotCurves[F_Z_DOT].setTitle(QwtText("z Velocity [m]"));
    m_plotCurves[F_AZ].setTitle(QwtText("z Accelertaion [m]"));
    m_plotCurves[VOLTAGE].setTitle(QwtText("Voltage [V]"));
    m_plotCurves[TRACE].setTitle(QwtText("Trace"));

    m_plotCurves[IMU_ROLL_DOT].setTitle(QwtText("IMU Roll Rate [deg/s]"));
    m_plotCurves[IMU_PITCH_DOT].setTitle(QwtText("IMU Pitch Rate [deg/s]"));
    m_plotCurves[IMU_YAW_DOT].setTitle(QwtText("IMU Yaw Rate [deg/s]"));
    m_plotCurves[IMU_AX].setTitle(QwtText("IMU X Acceleration [g's]"));
    m_plotCurves[IMU_AY].setTitle(QwtText("IMU Y Acceleration [g's]"));
    m_plotCurves[IMU_AZ].setTitle(QwtText("IMU Z Acceleration [g's]"));

    m_plotCurves[ENGINE1].setTitle(QwtText("Engine 1 Commanded [us]"));
    m_plotCurves[ENGINE2].setTitle(QwtText("Engine 2 Commanded [us]"));
    m_plotCurves[ENGINE3].setTitle(QwtText("Engine 3 Commanded [us]"));
    m_plotCurves[ENGINE4].setTitle(QwtText("Engine 4 Commanded [us]"));
    m_plotCurves[FC_CPU].setTitle(QwtText("CPU Usage [%]"));
    m_plotCurves[RC_LINK].setTitle(QwtText("RC Link [1/0]"));

    m_plotCurves[REF_PHI].setTitle(QwtText("Reference Roll Angle [rad]"));
    m_plotCurves[REF_THETA].setTitle(QwtText("Reference Pitch Angle [rad]"));
    m_plotCurves[REF_PSI].setTitle(QwtText("Reference Yaw Angle [rad]"));
    m_plotCurves[REF_X].setTitle(QwtText("Reference X Position [m]"));
    m_plotCurves[REF_Y].setTitle(QwtText("Reference Y Position [m]"));
    m_plotCurves[REF_Z].setTitle(QwtText("Reference Z Position [m]"));
    m_plotCurves[PHI_ACTIVE].setTitle(QwtText("Roll Active [1/0]"));
    m_plotCurves[THETA_ACTIVE].setTitle(QwtText("Pitch Active [1/0]"));
    m_plotCurves[PSI_ACTIVE].setTitle(QwtText("Yaw Active [1/0]"));
    m_plotCurves[X_ACTIVE].setTitle(QwtText("X Active [1/0]"));
    m_plotCurves[Y_ACTIVE].setTitle(QwtText("Y Active [1/0]"));
    m_plotCurves[Z_ACTIVE].setTitle(QwtText("Z Active [1/0]"));

    // Possible Curve Pens
    /// \@todo Assign static colours and line styles
    srand(time(NULL));
    for (i = 0; i < CURVE_COUNT; ++i)
    {
        QPen randomPen(QColor(rand() % 185, rand() % 185, rand() % 185));

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
    return QSize(880,480);
}

/**
  * @brief Resize the widgets
  */
void DataPlotter::resizeEvent (QResizeEvent* e)
{
    //AHNS_DEBUG("DataPlotter::resizeEvent (QResizeEvent* e)");

    ui->verticalLayoutWidget->resize(e->size());

    // resize internal tab widgets
    ui->gridLayoutWidget->resize(e->size().width(),77);
    ui->gridLayoutWidget_2->resize(e->size().width(),77);
    ui->gridLayoutWidget_3->resize(e->size().width(),77);
    ui->gridLayoutWidget_4->resize(e->size().width(),77);
    ui->gridLayoutWidget_5->resize(e->size().width(),77);
    ui->gridLayoutWidget_6->resize(e->size().width(),77);
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

    if (this->isVisible())
    {
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
                case TRACE:
                    j = 0;
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
                    {
                        timePoints[j] = m_DataVector[HELI_STATE_TIME][m_DataVector[HELI_STATE_TIME].size()-j-1];
                        dataPoints[j] = m_DataVector[i][m_DataVector[i].size()-j-1];
                        j++;
                    }
                    m_plotCurves[i].setData(timePoints, dataPoints, j);
                    break;
                case IMU_ROLL_DOT:
                case IMU_PITCH_DOT:
                case IMU_YAW_DOT:
                case IMU_AX:
                case IMU_AY:
                case IMU_AZ:
                    j = 0;
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
                    {
                        timePoints[j] = m_DataVector[SENSOR_TIME][m_DataVector[SENSOR_TIME].size()-j-1];
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
                case REF_PHI:
                case REF_THETA:
                case REF_PSI:
                case REF_X:
                case REF_Y:
                case REF_Z:
                case PHI_ACTIVE:
                case THETA_ACTIVE:
                case PSI_ACTIVE:
                case X_ACTIVE:
                case Y_ACTIVE:
                case Z_ACTIVE:
                    j = 0;
                    while ( (j < pointLimit) && (j < m_DataVector[i].size()))
                    {
                        timePoints[j] = m_DataVector[AP_STATE_TIME][m_DataVector[AP_STATE_TIME].size()-j-1];
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
    }
    return;
}

/**
  * @brief Slot to update the active graphs after a checkbox tick
  */
void DataPlotter::SetActive()
{
    AHNS_DEBUG("DataPlotter::SetActive()");

    int i = 0;
    int activeCount = 0;

    // Array of all Check boxes
    CHECK_BOX_ARRAY;

    for (i = 0; i < CURVE_COUNT; ++i)
    {
        if(checkBoxArray[i]->isChecked())
        {
            m_activePlot[i] = true;
            activeCount++;
        }
        else
        {
            m_activePlot[i] = false;
        }
    }

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
