/**
 * @file   bfglrenderer.cpp
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
 * Opengl functions to render an image sent from the blackfin
 *
 */

#include "bfglrenderer.h"


/**
  * @brief Constructor
  */
bfglrenderer::bfglrenderer(QWidget *parent) : QGLWidget(parent)
{

}

/**
  * @brief Destructor
  */
bfglrenderer::~bfglrenderer()
{

}

/**
  * @brief Opengl size hint
  */
QSize bfglrenderer::sizeHint() const
{
  return QSize(320, 240);
}

/**
  * @brief connect to the IOhandler
  */
void bfglrenderer::connectIOhandler(IOImageHandler *i)
{
    connect(i,SIGNAL(SIG_imageWritten()),this,SLOT(updateTexture()));
    imHandler = i;
}

/**
  * @brief Initialize the opengl widget
  */
void bfglrenderer::initializeGL()
      {
          glClearColor(0.5, 0.5, 0.5, 0.0);
          glShadeModel(GL_FLAT);
          glEnable(GL_TEXTURE_2D);
      }

/**
  * @brief Resize the opengl widget
  */
void bfglrenderer::resizeGL(int w, int h)
      {
          glViewport(0,0, w, h);
          glMatrixMode(GL_PROJECTION);
          glLoadIdentity();
          glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 15.0);
          glMatrixMode(GL_MODELVIEW);
      }

/**
  * @brief Paint the opengl widget
  */
void bfglrenderer::paintGL()
      {
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          glLoadIdentity();
          glBindTexture(GL_TEXTURE_2D, currentTexture);
          quad(0,0,100,0,100,100,0,100);

      }

/**
  * @brief Update the texture only if an image was received
  */
 void bfglrenderer::updateTexture()
 {
    // update the image
    currentImage.loadFromData(*(imHandler->imarray));
    currentTexture = bindTexture(currentImage);
    //std::cout << "TEXTURE UPDATED" << std::endl;
    //std::cout << currentImage.height() << " " << currentImage.width() << std::endl;
    //resizeGL(currentImage.width(),currentImage.height());
    update();
 }

 /**
   * @brief Create a quad for image viewing
   */
void bfglrenderer::quad(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
                   GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4)
{
   glBegin(GL_QUADS);
   glTexCoord2d(0.0,0.0); glVertex3d(x1, y1, 0);
   glTexCoord2d(1.0,0.0); glVertex3d(x2, y2, 0);
   glTexCoord2d(1.0,1.0); glVertex3d(x3, y3, 0);
   glTexCoord2d(0.0,1.0); glVertex3d(x4, y4, 0);
   glEnd();
}
