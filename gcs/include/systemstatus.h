/**
 * \file   systemstatus.h
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
 * Header for widget to show the high level system status.
 */

#ifndef SYSTEMSTATUS_H
#define SYSTEMSTATUS_H

#include <QWidget>

namespace Ui {
    class systemStatus;
}

/**
  * \breif Qt systemStatus Widget
  */
class systemStatus : public QWidget {
    Q_OBJECT
public:
    systemStatus(QWidget *parent = 0);
    ~systemStatus();

    QSize sizeHint() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::systemStatus *ui;
};

#endif // SYSTEMSTATUS_H
