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
#include "state.h"

namespace Ui {
    class FlightControl;
}

class FlightControl : public QWidget {
    Q_OBJECT
public:
    FlightControl(QWidget *parent = 0);
    ~FlightControl();

    QSize sizeHint() const;

signals:
    void sendSetAPConfig(ap_config_t apConfig);
    void sendPosition(position_t desiredPosition);
    void sendAttitude(attitude_t desiredAttitude);

public slots:
    void SetAPState(const ap_state_t* const srcState);

protected:
    void changeEvent(QEvent* e);
    void resizeEvent (QResizeEvent* e);

private:
    Ui::FlightControl *ui;

private slots:

    /** Uplink Commands */
    void on_sendSetConfigBtn_clicked();
    void on_sendPositionBtn_clicked();
    void on_sendAttitudeBtn_clicked();

    /** Quick restore buttons */
    void on_xCmdResetBtn_clicked();
    void on_yCmdResetBtn_clicked();
    void on_zCmdResetBtn_clicked();
    void on_rollCmdResetBtn_clicked();
    void on_pitchCmdResetBtn_clicked();
    void on_yawCmdResetBtn_clicked();

    /** Mode Change Buttons */
    void on_guidanceBtn_clicked();
    void on_positionBtn_clicked();
    void on_attitudeBtn_clicked();
    void on_rcBtn_clicked();
};

#endif // FLIGHTCONTROL_H
