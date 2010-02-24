#include <iostream>
#include <cstdlib>

#include "../../../reuse/ahns_logger.h"
#include "mockup.h"
#include "imageloader.h"

#include <GL/glut.h>
#include <cmath>

using namespace std;

// DEFINE ATTITUDE VARIABLES
extern float amountZ;
extern float amountX;
extern float stepSize;
extern short autoRotateLeft;
extern short autoPitchUp;
extern GLuint textID;

// DEFINE ALTITUDE VARIABLES
//extern

void ATT_drawSky()
{
	AHNS_DEBUG("ENTERED: ATT_drawSky()");

	AHNS_DEBUG("drawSky: Assigned sky colour COL_SKYBLUE");

	glBegin(GL_QUADS);

	COL_SKYBLUE;

        glVertex3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);

	glEnd();
}

void ATT_drawEarth()
{
	glBegin(GL_QUADS);

	COL_BROWN;

	//New Points
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glEnd();

}

void ATT_drawCtrMarker()
{
	LWIDTH_CTRMKR;
	glBegin(GL_LINES);
	COL_WHITE;
	glVertex3f(-1.0f, 0.0 , 0.0f);
	glVertex3f(1.0f, 0.0 , 0.0f);
	glEnd();
}

void ATT_drawAngleMark_S(int mult)
{
	float mlt = 0.0f;
	if (mult > 0)	mlt = (float)(0.1f*2*mult - 0.1f);
	if (mult < 0)	mlt = (float)(0.1f*2*mult + 0.1f);

	glPushMatrix();
	glTranslatef(0.0f, mlt, 0.0f);

	LWIDTH_ANGMKRS;
	glBegin(GL_LINES);

	COL_WHITE;

	glVertex3f(-MKR_W*0.22, 0.0 , 0.0f);
	glVertex3f(MKR_W*0.22, 0.0 , 0.0f);

	glEnd();
	glPopMatrix();
}

void ATT_drawAngleMark_M(int mult)
{
	float mlt = 0.0f;
	if (mult > 0)	mlt = (float)(0.1f*4*mult - 0.2f);
	if (mult < 0)	mlt = (float)(0.1f*4*mult + 0.2f);
	
	glPushMatrix();
	glTranslatef(0.0f, mlt, 0.0f);

	LWIDTH_ANGMKRM;
	glBegin(GL_LINES);

	COL_WHITE;

	glVertex3f(-MKR_W*0.4, 0.0f, 0.0f);
	glVertex3f(MKR_W*0.4, 0.0f, 0.0f);

	glEnd();
	glPopMatrix();
}

void ATT_drawAngleMark_L(int mult)
{
	glPushMatrix();
	glTranslatef(0.0f, 0.1f*4*mult, 0.0f);

	LWIDTH_ANGMKRL;
	glBegin(GL_LINES);

	COL_WHITE;

	glVertex3f(-MKR_W, 0.0 , 0.0f);
	glVertex3f(MKR_W, 0.0 , 0.0f);

	glEnd();
	glPopMatrix();
}

void drawSidebar()
{
	glPushMatrix();
	glTranslatef(-0.3f, 0.0f, 0.1f);
	glBegin(GL_QUADS);

	COL_BLACK;

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-0.5f, -1.0f, 0.0f);
	glVertex3f(-0.5f, 1.0f, 0.0f);

	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.3f, 0.0f, 0.1f);
	glBegin(GL_QUADS);

        COL_BLACK;

        glVertex3f(0.0f, -1.0f, 0.0f);
        glVertex3f(0.5f, -1.0f, 0.0f);
        glVertex3f(0.5f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);

        glEnd();
        glPopMatrix();
}

void ATT_drawCenterPoint_cross()
{
	COL_PINK;
	
	glTranslatef(0.0f, 0.0f, 0.2f);
	
	LWIDTH_XHAIR;
	
	glPushMatrix();
	glBegin(GL_LINES);
	glVertex3f(-0.2f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glVertex3f(0.0f, -0.2f, 0.0f);
	glVertex3f(0.0f, 0.2f, 0.0f);
	glEnd();
	glPopMatrix();
}

void ATT_drawCenterPoint_dot()
{
	float radius = 0.01f;
	float angle = 0.0f;
	int x = 0;
	int y = 0;
	
	COL_PINK;
	
	glTranslatef(0.0f, 0.0f, 0.2f);
	
	LWIDTH_XHAIR;
	
	glPushMatrix();
	glTranslatef(-0.05f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.18f, 0.0f, 0.0f);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.05f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.18f, 0.0f, 0.0f);
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0f, -0.05f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -0.18f, 0.0f);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.05f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.18f, 0.0f);
	glEnd();
	glPopMatrix();

	glBegin(GL_TRIANGLE_FAN);
	
	for (int ii = 0; ii <= 360; ii += 20)
	{
		angle = ii * (PI/180);
		glVertex3f(x - cos(angle)*radius, y - sin(angle)*radius, 0.0f);
	}
	glEnd();

}

void drawTextNumbers()
{
	//cout << (int)textID << endl;

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.5f);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textID);
	glScalef(0.5f, 0.5f, 0.5f);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Blocky texture mapping (GL_NEAREST)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.165f, -0.12f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.165f, -0.12f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.165f, 0.12f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.165f, 0.12f, 0.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void ATT_drawMarkers()
{
	glTranslatef(0.0f, 0.0f, 0.1f);

        glPushMatrix();
        ATT_drawCtrMarker();
        glPopMatrix();

	glPushMatrix();
	glScalef(0.9f, 0.4f, 0.0f);
	for (int ii = 0; ii <= 18; ii++)
	{
		ATT_drawAngleMark_S(ii);
		ATT_drawAngleMark_S(-ii);
	}
	
	for (int ii = 0; ii <= 9; ii++)
	{
		ATT_drawAngleMark_L(ii);
		ATT_drawAngleMark_M(ii);
		ATT_drawAngleMark_L(-ii);
		ATT_drawAngleMark_M(-ii);
	}
	glPopMatrix();
	
	//drawTextNumbers();
}

void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// STATIC SECTION
	glTranslatef(0.0f, 0.0f, -1.6f);
	glPushMatrix();
	ATT_drawCenterPoint_cross();
	drawSidebar();
	ALT_drawBackground();
	glPopMatrix();

	// ANIMATED SECTION
	glPushMatrix();
	glRotatef(amountZ, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, amountX, 0.0f);

	// Draw Angle Markings
	ATT_drawMarkers();

	// Two sidebars are to be drawn
	ALT_drawHeightMark_L(0);
	
	// Sky Component
	glPushMatrix();
	glScalef(2.0f, 2.0f, 0.0f);
	ATT_drawSky();
	glPopMatrix();

	// Earth Component
	glPushMatrix();
	glScalef(2.0f, 2.0f, 0.0f);
	ATT_drawEarth();
	glPopMatrix();
	
	glPopMatrix();

	AHNS_DEBUG("drawScene: Up to glutSwapBuffers()");
	glutSwapBuffers();
}


void updateRotate()
{
	if (autoRotateLeft == 1)
	{
		amountZ -= 1.0;
	}
	else if (autoRotateLeft == 0)
	{
		amountZ += 1.0f;
	}
	else if	(autoRotateLeft == 3)	amountZ = 0;

	if (autoPitchUp == 1)
	{
		if (amountX < 1.435f)	amountX += stepSize;
	}
	else if (autoPitchUp == 0)
	{
		if (amountX > -1.435f)	amountX -= stepSize;
	}
	else if	(autoPitchUp == 3)	amountX = 0;
}


void AnimateAH(int extra)
{
	updateRotate();
	drawScene();
	glutTimerFunc(20, AnimateAH, 0);
}
