/**
 * \file   gainscontrol.h
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
 * Definition of the GainsControl Widget.
 */

#ifndef GAINSCONTROL_H
#define GAINSCONTROL_H

#include <QWidget>

#include "state.h"

namespace Ui {
    class GainsControl;
}

class GainsControl : public QWidget {
    Q_OBJECT
public:
    GainsControl(QWidget *parent = 0);
    ~GainsControl();

signals:
    void sendGains(gains_t loopGains);

public slots:
    void SetGains(const gains_t* const srcGains);

protected:
    void changeEvent(QEvent *e);
    QSize SizeHint() const;
    void resizeEvent(QResizeEvent* e);

private:
    Ui::GainsControl *ui;

private slots:
    /** @name Command Buttons */
    void on_loadBtn_clicked();
    void on_SaveBtn_clicked();
    void on_SendBtn_clicked();
};

#endif // GAINSCONTROL_H
