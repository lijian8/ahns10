/**
 * \file   dataplotter.h
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
 * Declaration of the Data Plotting Widget.
 */

#ifndef DATAPLOTTER_H
#define DATAPLOTTER_H

#include <QWidget>
#include <QVector>

#include "sys/time.h"
#include "state.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_div.h>

namespace Ui {
    class DataPlotter;
}

class DataPlotter : public QWidget {
    Q_OBJECT
public:
    DataPlotter(QWidget *parent = 0);
    ~DataPlotter();
    QSize sizeHint() const;
    void setHeliStateData(const timeval* const timeStamp, const state_t* heliState);
    void replot();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent (QResizeEvent* e);

private slots:
    void SetActive();

private:
    Ui::DataPlotter *ui;

    /** @name Possible Plot Data */
    enum PlotData {
        F_PHI,
        F_PHI_DOT,
        F_THETA,
        F_THETA_DOT,
        F_PSI,
        F_PSI_DOT,
        F_X,
        F_X_DOT,
        F_AX,
        F_Y,
        F_Y_DOT,
        F_AY,
        F_Z,
        F_Z_DOT,
        F_AZ,
        VOLTAGE,
        CURVE_COUNT,
        RAW_TIME,
        TIME,
        DATA_COUNT
    };

    /** Vector of Data Vectors */
    QVector<double> dataVector[DATA_COUNT];

    /** @name Active Plots */
    volatile bool m_activePlot[CURVE_COUNT];

    /** @name Curvers*/
    QwtPlotCurve m_plotCurves[CURVE_COUNT];
};

#endif // DATAPLOTTER_H
