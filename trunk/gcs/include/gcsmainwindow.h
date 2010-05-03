/**
 * \file   gcsmainwindow.h
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
 * GCS form header file used to define the gcsMainWindow class
 */

#ifndef GCSMAINWINDOW_H
#define GCSMAINWINDOW_H

#include <QMainWindow>
#include "aboutform.h"

namespace Ui {
    class gcsMainWindow;
}

/**
  * \brief Mainwindow Qt Class for GCS, Generated with QtCreator
  */
class gcsMainWindow : public QMainWindow {
    Q_OBJECT
public:
    gcsMainWindow(QWidget *parent = 0);
    ~gcsMainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::gcsMainWindow *ui;
    void createDockWindows();

    //List of Dockable Widgets
    QList<QDockWidget*> dockList;

    // Form
    aboutForm* m_Aboutfrm;

private slots:
    void on_actionAbout_triggered();
};

#endif // GCSMAINWINDOW_H
