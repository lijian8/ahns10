/**
 * \file   commsconsole.h
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
 * Communications Console Class
 * To be used for display of the packets received, transmitted and discarded.
 */

#ifndef COMMSCONSOLE_H
#define COMMSCONSOLE_H

#include <QWidget>

namespace Ui {
    class CommsConsole;
}

class CommsConsole : public QWidget {
    Q_OBJECT
public:
    CommsConsole(QWidget *parent = 0);
    ~CommsConsole();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CommsConsole *ui;
};

#endif // COMMSCONSOLE_H
