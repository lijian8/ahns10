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
        ui->sensorZchkbox, \
        ui->compassPSIchkbox, \
        ui->viconXchkbox, \
        ui->viconYchkbox, \
        ui->viconZchkbox, \
        ui->viconVXchkbox, \
        ui->viconVYchkbox, \
        ui->viconVZchkbox, \
        ui->viconRollchkbox, \
        ui->viconPitchchkbox, \
        ui->viconYawchkbox, \
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
  * @brief DataPlotter Constructor
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

        // All curves initially not active
        m_activePlot[i] = false;
    }

    connect(ui->Clearbtn,SIGNAL(clicked()),this,SLOT(ClearPlots()));

    ui->dataPlotqwtPlot->setCanvasBackground(QColor(255,255,255));
    QPen tempPen(QColor(0,0,0));

    tempPen.setStyle(Qt::DashLine);
    tempPen.setStyle(Qt::DashDotLine);
    tempPen.setStyle(Qt::DashDotDotLine);

    m_plotCurves[F_PHI].setTitle(QwtText(ui->filteredRollchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,0,255));
    m_plotCurves[F_PHI].setPen(tempPen);

    m_plotCurves[F_PHI_DOT].setTitle(QwtText(ui->filteredRolldchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,0,119));
    m_plotCurves[F_PHI_DOT].setPen(tempPen);

    m_plotCurves[F_THETA].setTitle(QwtText(ui->filteredPitchchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,153,0));
    m_plotCurves[F_THETA].setPen(tempPen);

    m_plotCurves[F_THETA_DOT].setTitle(QwtText(ui->filteredPitchdchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,85,0));
    m_plotCurves[F_THETA_DOT].setPen(tempPen);

    m_plotCurves[F_PSI].setTitle(QwtText(ui->filteredYawchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,0,0));
    m_plotCurves[F_PSI].setPen(tempPen);

    m_plotCurves[F_PSI_DOT].setTitle(QwtText(ui->filteredYawdchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(119,0,0));
    m_plotCurves[F_PSI_DOT].setPen(tempPen);

    m_plotCurves[F_X].setTitle(QwtText("x Position [m]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,204,51));
    m_plotCurves[F_X].setPen(tempPen);

    m_plotCurves[F_X_DOT].setTitle(QwtText("x Velocity [m/s]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,153,0));
    m_plotCurves[F_X_DOT].setPen(tempPen);

    m_plotCurves[F_AX].setTitle(QwtText("x Accelertaion [m/s]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,102,0));
    m_plotCurves[F_AX].setPen(tempPen);

    m_plotCurves[F_Y].setTitle(QwtText("y Position [m]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,102,255));
    m_plotCurves[F_Y].setPen(tempPen);

    m_plotCurves[F_Y_DOT].setTitle(QwtText("y Velocity [m/s]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,102,102));
    m_plotCurves[F_Y_DOT].setPen(tempPen);

    m_plotCurves[F_AY].setTitle(QwtText("y Accelertaion [m/s/s]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,0,102));
    m_plotCurves[F_AY].setPen(tempPen);

    m_plotCurves[F_Z].setTitle(QwtText("z Position [m]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,153,0));
    m_plotCurves[F_Z].setPen(tempPen);

    m_plotCurves[F_Z_DOT].setTitle(QwtText("z Velocity [m]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,102,51));
    m_plotCurves[F_Z_DOT].setPen(tempPen);

    m_plotCurves[F_AZ].setTitle(QwtText("z Accelertaion [m]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(153,0,102));
    m_plotCurves[F_AZ].setPen(tempPen);

    m_plotCurves[VOLTAGE].setTitle(QwtText("Voltage [V]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(102,102,153));
    m_plotCurves[VOLTAGE].setPen(tempPen);

    m_plotCurves[TRACE].setTitle(QwtText("Trace"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(153,102,0));
    m_plotCurves[TRACE].setPen(tempPen);

    m_plotCurves[IMU_ROLL_DOT].setTitle(QwtText(ui->imuRollchkbox->text()));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(153,102,0));
    m_plotCurves[IMU_ROLL_DOT].setPen(tempPen);

    m_plotCurves[IMU_PITCH_DOT].setTitle(QwtText(ui->imuPitchchkbox->text()));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(0,102,102));
    m_plotCurves[IMU_PITCH_DOT].setPen(tempPen);

    m_plotCurves[IMU_YAW_DOT].setTitle(QwtText(ui->imuYawchkbox->text()));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(102,102,51));
    m_plotCurves[IMU_YAW_DOT].setPen(tempPen);

    m_plotCurves[IMU_AX].setTitle(QwtText("IMU X Acceleration [g's]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(0,153,0));
    m_plotCurves[IMU_AX].setPen(tempPen);

    m_plotCurves[IMU_AY].setTitle(QwtText("IMU Y Acceleration [g's]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(255,102,102));
    m_plotCurves[IMU_AY].setPen(tempPen);

    m_plotCurves[IMU_AZ].setTitle(QwtText("IMU Z Acceleration [g's]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(51,102,102));
    m_plotCurves[IMU_AZ].setPen(tempPen);

    m_plotCurves[SENSOR_Z].setTitle(QwtText("Ultrasonic Sensor Z [m]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(255,51,102));
    m_plotCurves[SENSOR_Z].setPen(tempPen);

    m_plotCurves[COMPASS_PSI].setTitle(QwtText("Compass Heading [deg]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(0,150,102));
    m_plotCurves[COMPASS_PSI].setPen(tempPen);

    m_plotCurves[VICON_X].setTitle(QwtText(ui->viconXchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(102,150,0));
    m_plotCurves[VICON_X].setPen(tempPen);

    m_plotCurves[VICON_Y].setTitle(QwtText(ui->viconYchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,0,255));
    m_plotCurves[VICON_Y].setPen(tempPen);

    m_plotCurves[VICON_Z].setTitle(QwtText(ui->viconZchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,255,255));
    m_plotCurves[VICON_Z].setPen(tempPen);

    m_plotCurves[VICON_VX].setTitle(QwtText(ui->viconVXchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,0,255));
    m_plotCurves[VICON_VX].setPen(tempPen);

    m_plotCurves[VICON_VY].setTitle(QwtText(ui->viconVYchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(50,10,255));
    m_plotCurves[VICON_VY].setPen(tempPen);

    m_plotCurves[VICON_VZ].setTitle(QwtText(ui->viconVZchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(5,50,255));
    m_plotCurves[VICON_VZ].setPen(tempPen);

    m_plotCurves[VICON_PHI].setTitle(QwtText(ui->viconRollchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,255,0));
    m_plotCurves[VICON_PHI].setPen(tempPen);

    m_plotCurves[VICON_THETA].setTitle(QwtText(ui->viconPitchchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(50,255,100));
    m_plotCurves[VICON_THETA].setPen(tempPen);

    m_plotCurves[VICON_PSI].setTitle(QwtText(ui->viconYawchkbox->text()));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(25,150,100));
    m_plotCurves[VICON_PSI].setPen(tempPen);

    m_plotCurves[ENGINE1].setTitle(QwtText("Engine 1 Commanded [us]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,102,255));
    m_plotCurves[ENGINE1].setPen(tempPen);

    m_plotCurves[ENGINE2].setTitle(QwtText("Engine 2 Commanded [us]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,0,0));
    m_plotCurves[ENGINE2].setPen(tempPen);

    m_plotCurves[ENGINE3].setTitle(QwtText("Engine 3 Commanded [us]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(0,102,51));
    m_plotCurves[ENGINE3].setPen(tempPen);

    m_plotCurves[ENGINE4].setTitle(QwtText("Engine 4 Commanded [us]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(102,51,204));
    m_plotCurves[ENGINE4].setPen(tempPen);

    m_plotCurves[FC_CPU].setTitle(QwtText("CPU Usage [%]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(255,255,255));
    m_plotCurves[FC_CPU].setPen(tempPen);

    m_plotCurves[RC_LINK].setTitle(QwtText("RC Link [1/0]"));
    tempPen.setStyle(Qt::SolidLine);
    tempPen.setColor(QColor(255,120,255));
    m_plotCurves[RC_LINK].setPen(tempPen);

    m_plotCurves[REF_PHI].setTitle(QwtText("Reference Roll Angle [rad]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(102,0,0));
    m_plotCurves[REF_PHI].setPen(tempPen);

    m_plotCurves[REF_THETA].setTitle(QwtText("Reference Pitch Angle [rad]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(153,0,255));
    m_plotCurves[REF_THETA].setPen(tempPen);

    m_plotCurves[REF_PSI].setTitle(QwtText("Reference Yaw Angle [rad]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(0,0,102));
    m_plotCurves[REF_PSI].setPen(tempPen);

    m_plotCurves[REF_X].setTitle(QwtText("Reference X Position [m]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(153,51,0));
    m_plotCurves[REF_X].setPen(tempPen);

    m_plotCurves[REF_Y].setTitle(QwtText("Reference Y Position [m]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(51,0,153));
    m_plotCurves[REF_Y].setPen(tempPen);

    m_plotCurves[REF_Z].setTitle(QwtText("Reference Z Position [m]"));
    tempPen.setStyle(Qt::DashLine);
    tempPen.setColor(QColor(0,153,51));
    m_plotCurves[REF_Z].setPen(tempPen);

    m_plotCurves[PHI_ACTIVE].setTitle(QwtText("Roll Active [1/0]"));
    tempPen.setStyle(Qt::DashDotLine);
    tempPen.setColor(QColor(255,51,255));
    m_plotCurves[PHI_ACTIVE].setPen(tempPen);

    m_plotCurves[THETA_ACTIVE].setTitle(QwtText("Pitch Active [1/0]"));
    tempPen.setStyle(Qt::DashDotLine);
    tempPen.setColor(QColor(255,102,0));
    m_plotCurves[THETA_ACTIVE].setPen(tempPen);

    m_plotCurves[PSI_ACTIVE].setTitle(QwtText("Yaw Active [1/0]"));
    tempPen.setStyle(Qt::DashDotLine);
    tempPen.setColor(QColor(102,204,255));
    m_plotCurves[PSI_ACTIVE].setPen(tempPen);

    m_plotCurves[X_ACTIVE].setTitle(QwtText("X Active [1/0]"));
    tempPen.setStyle(Qt::DashDotLine);
    tempPen.setColor(QColor(204,102,255));
    m_plotCurves[X_ACTIVE].setPen(tempPen);

    m_plotCurves[Y_ACTIVE].setTitle(QwtText("Y Active [1/0]"));
    tempPen.setStyle(Qt::DashDotLine);
    tempPen.setColor(QColor(51,0,255));
    m_plotCurves[Y_ACTIVE].setPen(tempPen);

    m_plotCurves[Z_ACTIVE].setTitle(QwtText("Z Active [1/0]"));
    tempPen.setStyle(Qt::DashDotLine);
    tempPen.setColor(QColor(255,200,150));
    m_plotCurves[Z_ACTIVE].setPen(tempPen);

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

    const int pointLimit = 3000;
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
                    j = 0;
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
                    {
                        timePoints[j] = m_DataVector[HELI_STATE_TIME][m_DataVector[HELI_STATE_TIME].size()-j-1];
                        dataPoints[j] = 180.0/M_PI*m_DataVector[i][m_DataVector[i].size()-j-1];
                        j++;
                    }
                    m_plotCurves[i].setData(timePoints, dataPoints, j);
                    break;
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
                case SENSOR_Z:
                case COMPASS_PSI:
                    j = 0;
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
                    {
                        timePoints[j] = m_DataVector[SENSOR_TIME][m_DataVector[SENSOR_TIME].size()-j-1];
                        dataPoints[j] = m_DataVector[i][m_DataVector[i].size()-j-1];
                        j++;
                    }
                    m_plotCurves[i].setData(timePoints, dataPoints, j);
                    break;
                case VICON_X:
                case VICON_Y:
                case VICON_Z:
                case VICON_VX:
                case VICON_VY:
                case VICON_VZ:
                    j = 0;
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
                    {
                        timePoints[j] = m_DataVector[VICON_TIME][m_DataVector[VICON_TIME].size()-j-1];
                        dataPoints[j] = m_DataVector[i][m_DataVector[i].size()-j-1];
                        j++;
                    }
                    m_plotCurves[i].setData(timePoints, dataPoints, j);
                    break;
                case VICON_PHI:
                case VICON_THETA:
                case VICON_PSI:
                    j = 0;
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
                    {
                        timePoints[j] = m_DataVector[VICON_TIME][m_DataVector[VICON_TIME].size()-j-1];
                        dataPoints[j] = 180.0/M_PI*m_DataVector[i][m_DataVector[i].size()-j-1];
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
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
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
                    j = 0;
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
                    {
                        timePoints[j] = m_DataVector[AP_STATE_TIME][m_DataVector[AP_STATE_TIME].size()-j-1];
                        dataPoints[j] = 180.0/M_PI*m_DataVector[i][m_DataVector[i].size()-j-1];
                        j++;
                    }
                    m_plotCurves[i].setData(timePoints, dataPoints, j);
                    break;
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
                    while ((j < pointLimit) && (j < m_DataVector[i].size()))
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
