/**
 * @file   bfimagefeed.h
 * @author Liam O'Sullivan
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Blackfin camera feed widget header
 *
 */

#ifndef BFIMAGEFEED_H
#define BFIMAGEFEED_H

#include <QWidget>
#include <QGLWidget>
#include "bfcameracomms.h"
#include "bfglrenderer.h"
#include <QtGui>
#include <QLineEdit>

class bfImageFeed : public QWidget
{
    Q_OBJECT

public:
    bfImageFeed(QWidget *parent = 0);
signals:
    void buttonConnect();
    void buttonDisconnect();
private slots:
    void setconnectButtonState();
    void connectServer();
    void updateLabel();
    void sendBFCommand();
private:
    bfglrenderer *glWidget;
    bfcameracomms *bfCam;
    QLineEdit *commandLineEdit;
    QPushButton *connectButton;
    QPushButton *sendButton;
    int state;
    QLabel *fpsText;

};

#endif // BFIMAGEFEED_H
