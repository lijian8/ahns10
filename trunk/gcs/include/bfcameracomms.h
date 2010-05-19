#ifndef BFCAMERACOMMS_H
/**
 * @file   bfcameracomms.h
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
 * Blackfin camera communication library header
 *
 */

#define BFCAMERACOMMS_H

#include <QTcpSocket>
#include <iostream>
#include <cmath>
#include <QDataStream>
#include <QImage>
#include <QTime>
#include "ioimagehandler.h"

#define DEFAULT_CAM_IP "192.168.1.3"
#define DEFAULT_CAM_PORT 10001

class bfcameracomms : public IOImageHandler {
    Q_OBJECT
public:
    bfcameracomms();
    ~bfcameracomms();
    void server_connect(QString server);
    qreal fps;

signals:
    void SIG_imageReceived();
    void SIG_NextImage();

protected:

private slots:
    void sendImageRequest();
    void getImage();
    void updateImage();
    void server_disconnect();

private:
    QTcpSocket tcpSocket;
    QImage *image_received;
    QTime t;
    int timeTaken;
    int framesReceived;

};

#endif // BFCAMERACOMMS_H
