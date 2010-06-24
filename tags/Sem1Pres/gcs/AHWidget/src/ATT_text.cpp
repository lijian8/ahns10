#include <iostream>
#include <cstdlib>
#include "AH.h"

#include <GL/glut.h>

using namespace std;


/* ********************************************** */
/* ********** ALGORITHM TO INSERT TEXT ********** */
/* ********************************************** */
void AHclass::insertText(const char *message)
{
	while(*message) glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *message++);
}

/* *********************************************************************** */
/* ********** DISPLAYING THE IMAGE WITH GIVEN POSITION AND SIZE ********** */
/* *********************************************************************** */
void AHclass::displayMessage(float posX, float posY, const char *message, float scale)
{
	glPushMatrix();
	glTranslatef(posX-(1.6f*DEG2RAD), posY-(0.8f*DEG2RAD), ONTOP);
	glScalef(0.0001, 0.0001, 0.0001);
	glScalef(scale, scale, scale);
	insertText(message);
	glPopMatrix();
}

