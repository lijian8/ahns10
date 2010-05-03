/**
 * \file   wificomms.h
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
 * Header file for the Wifi Communications widget to manage the wifi
 * communications and launch its telemetry thread.
 */

#ifndef WIFICOMMS_H
#define WIFICOMMS_H

#include <QWidget>

namespace Ui {
    class wifiComms;
}

class wifiComms : public QWidget {
    Q_OBJECT
public:
    wifiComms(QWidget *parent = 0);
    ~wifiComms();

    QSize sizeHint() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::wifiComms *ui;
};

#endif // WIFICOMMS_H
