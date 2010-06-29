/**
 * \file   flightcontrol.h
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
 * Definition of the FlightControl Widget.
 */

#ifndef FLIGHTCONTROL_H
#define FLIGHTCONTROL_H

#include <QWidget>

namespace Ui {
    class FlightControl;
}

class FlightControl : public QWidget {
    Q_OBJECT
public:
    FlightControl(QWidget *parent = 0);
    ~FlightControl();

    QSize sizeHint() const;

protected:
    void changeEvent(QEvent* e);
    void resizeEvent (QResizeEvent* e);

private:
    Ui::FlightControl *ui;

private slots:
    void on_guidanceBtn_clicked();
    void on_xCmdResetBtn_clicked();
    void on_yCmdResetBtn_clicked();
    void on_zCmdResetBtn_clicked();
    void on_rollCmdResetBtn_clicked();
    void on_pitchCmdResetBtn_clicked();
    void on_yawCmdResetBtn_clicked();
    void on_positionBtn_clicked();
    void on_attitudeBtn_clicked();
    void on_rcBtn_clicked();
};

#endif // FLIGHTCONTROL_H
