#include "AH.h"

#if defined(__APPLE__) && defined(__MACH__)
    #include "GLUT/glut.h"
#else
    #include <GL/glut.h>
#endif

#include <QString>
#include <QTime>
#include <QtOpenGL>
#include <QByteArray>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <QSize>
#include <cmath>

#include "ahns_logger.h"

using namespace std;

AHclass::AHclass(QWidget* parent) : QGLWidget(parent)
{
    QWidget::setMinimumSize(200,200);
    // DEFINE VARIABLES
    angRoll = 0.0f;
    angPitch = 0.0f;
    angRollRate = 0.0f;
    angPitchRate = 0.0f;
    altState = 0.0f;
    vertAltPos = 0.0f;
    fadeOut = 0.0f;
    int argc = 0;
    char** argv = NULL;
    glutInit(&argc, argv);
}

AHclass::~AHclass()
{
}

void AHclass::initializeGL()
{
    EnableAntialiase();
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_FLAT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    resize(tempSize.rwidth(), tempSize.rheight());
}

void AHclass::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(10.4f, (double)w/(double)h, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void AHclass::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float bankOffset = 23.5f;

    glTranslatef(0.0f, 0.0f, -5.0f); // Can be modified to obtain the right size and scaling

    /* ******** DRAWING THE ANIMATABLE COMPONENTS ******** */
    glPushMatrix();
    glRotatef(angRoll, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, angPitch, 0.0f);
    ATT_drawBackground_dynamic();
    ATT_drawBackground_static();
    ATT_drawHorizon();
    ATT_drawPitchMarkers();
    glPopMatrix();

    glPushMatrix();
    glRotatef(angRoll, 0.0f, 0.0f, 1.0f);
    ATT_drawBankPointer(bankOffset);
    glPopMatrix();

    /* ******** DRAWING THE STATIC COMPONENTS ******** */
    glPushMatrix();
    ATT_drawBankMarkers(bankOffset);
    ATT_drawCenterPoint();
    ATT_drawAirdataBackground(20.0f+0.5f, 22.0f+0.8f);
    displayAltitudeText(18.4f+0.5f, 22.35f+0.8f);
    glPopMatrix();
}

QSize AHclass::sizeHint() const
{
    return QSize(400, 400);
}

/**
  * @brief Slot for updating complete widget
  * @param heliState Pointer to the helistate from the thread
  */
void AHclass::setState(const state_t* const heliState)
{
    AHNS_DEBUG("AHclass::setState()");

    if (!updating)
    {
        // AH Roll is in degrees
        angRoll = heliState->phi * 180.0/ M_PI;
        angRollRate = heliState->p * 180.0 / M_PI;

        // AH Pitch in radians
        angPitch = -heliState->theta;
        angPitchRate = heliState->q;

        altState = -heliState->z*100.0;
    }

    return;
}
