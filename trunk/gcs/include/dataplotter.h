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
#include <QTimer>

#include "sys/time.h"
#include "state.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>

#include "datalogger.h"

namespace Ui {
    class DataPlotter;
}

class DataPlotter : public QWidget {    

    Q_OBJECT
public:

    DataPlotter(QVector<double>* srcData = 0, QWidget *parent = 0);
    ~DataPlotter();
    QSize sizeHint() const;

signals:

public slots:
    void replot();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent (QResizeEvent* e);

private slots:
    void SetActive();
    void ClearPlots();

private:

    Ui::DataPlotter *ui;

    /** @name Active Plots */
    volatile bool m_activePlot[CURVE_COUNT];

    /** @name Curvers*/
    QwtPlotCurve m_plotCurves[CURVE_COUNT];

    /** @name Plot Grid */
    QwtPlotGrid m_coordinateGrid;

    /** @name Plot Legend */
    QwtLegend m_legend;

    /** Pointer to Data Vectors */
    QVector<double>* m_DataVector;
};

#endif // DATAPLOTTER_H
