/**
 * \file   SystemStatus.h
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
#include "ui_systemstatus.h"

namespace Ui {
    class SystemStatus;
}

/**
  * @breif Qt SystemStatus Widget
  */
class SystemStatus : public QWidget {
    Q_OBJECT
public:
    SystemStatus(QWidget *parent = 0);
    ~SystemStatus();

    QSize sizeHint() const;

protected:
    void changeEvent(QEvent* e);
    void resizeEvent (QResizeEvent* e);

private:
    Ui::SystemStatus *ui;
};

#endif // SYSTEMSTATUS_H
