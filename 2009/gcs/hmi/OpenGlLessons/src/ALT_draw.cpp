#include <iostream>
#include <cstdlib>

// ../../../reuse/
#include "../../../reuse/ahns_logger.h"
#include "mockup.h"

#include <GL/glut.h>

using namespace std;

/*extern float amountZ;
extern float amountX;
extern float stepSize;
extern short autoRotateLeft;
extern short autoPitchUp;*/

void ALT_drawHeightMark_L(int mult)
{
	float mlt = (float)(5*mult);

	glPushMatrix();
	glTranslatef(0.3f, 0.1f, 0.31f);
	glScalef(0.5f, 0.5f, 1.0f);

	LWIDTH_ALTMKR;
	glBegin(GL_LINES);

	COL_WHITE;

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);

	glEnd();
	glPopMatrix();
}

void ALT_drawBackground()
{
	glPushMatrix();
	glTranslatef(0.3f, -0.5f, 0.101f);
	glBegin(GL_QUADS);
	
	COL_GRAY;
	
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glVertex3f(0.2f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	
	glEnd();
	glPopMatrix();
}
