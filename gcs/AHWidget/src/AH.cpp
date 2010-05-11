#include "AH.h"
#include "GL/glut.h"
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
    QWidget::setMinimumSize(400,400);
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
  * @param newRoll Roll Angle in radians
  * @param newRollRate Roll Rate in radians per sec
  * @param newPitch Pitch Angle in radians
  * @param newPitchRate Pitch Rate in radians per sec
  * @param newAltState Altitude in meters
  */
void AHclass::setState(const float newRoll,const float newRollRate,const float newPitch,const float newPitchRate, const float newAltState)
{
    AHNS_DEBUG("AHclass::setState()");
    // Roll is in degrees, pitch in radians
    angRoll = newRoll * M_PI / 180.0;
    angRollRate = newRollRate* M_PI / 180.0;

    angPitch = newPitch;
    angPitchRate = newPitchRate;

    altState = newAltState*100.0;

    UpdateRoll();
    return;
}
