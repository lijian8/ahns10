/**
 * @file   bfglrenderer.h
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
 * Header file which contains Opengl functions to render an image sent from the blackfin
 *
 */

#ifndef BFGLRENDERER_H
#define BFGLRENDERER_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QImage>
#include <iostream>
#include <QObject>
#include <QDataStream>
#include <QBuffer>
#include "ioimagehandler.h"


class bfglrenderer : public QGLWidget {
    Q_OBJECT
public:
    bfglrenderer(QWidget *parent = 0);
    ~bfglrenderer();
    void connectIOhandler(IOImageHandler *i);
    QSize sizeHint() const;

public slots:
    void updateTexture();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void quad(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2, GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4);

private:
    QImage currentImage;
    GLuint currentTexture;
    IOImageHandler *imHandler;
};

#endif // BFGLRENDERER_H
