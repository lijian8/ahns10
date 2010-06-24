#include <iostream>
#include <cstdlib>
#include <cmath>

#include "../../../reuse/ahns_logger.h"
#include "AH.h"

#include <GL/glut.h>

using namespace std;


/* ******************************************************* */
/* ********** DRAWING THE ENTIRE AH SCENE/FRAME ********** */
/* ******************************************************* */
void DrawScene()
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
	fadeOutScreen();
	glPopMatrix();
	
	AHNS_DEBUG("drawScene: Up to glutSwapBuffers()");
	glutPostRedisplay();
	glutSwapBuffers();
}
