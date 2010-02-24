#include <iostream>
#include <cstdlib>
#include <cmath>
#include "../../../reuse/ahns_logger.h"
#include "AH.h"
#include "GL/glut.h"

using namespace std;

float colorConv(int RGB)
{
	return (float)RGB/255;
}

void fadeOutScreen()
{
	glPushMatrix();
	
	COL_FADE;
	glTranslatef(0.0f, 0.0f, 3.0f);
	
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, -100.0f, 0.0f);
	glVertex3f(-100.0f, 100.0f, 0.0f);
	glVertex3f(100.0f, 100.0f, 0.0f);
	glVertex3f(100.0f, -100.0f, 0.0f);
	glEnd();
	
	glPopMatrix();
}

void fadeOutOnExit(int unused)
{
	fadeOut += 0.02f;
	glutTimerFunc(20, fadeOutOnExit, 1);
	if (fadeOut > 1.1f)	exit(0);
}

void EnableAntialiase()
{
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
}
