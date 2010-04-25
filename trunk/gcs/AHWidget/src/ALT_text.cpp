#include <iostream>
#include <cstdlib>
#include "AH.h"
#include <cstdio>

#include <GL/glut.h>

using namespace std;


/* ********************************************************* */
/* ********** DISPLAYING THE ALTITUDE INFORMATION ********** */
/* ********************************************************* */
void AHclass::displayAltitudeText(float posX, float posY)
{
	char insChar[8];
	
	glPushMatrix();
	LWIDTH_DEFAULT;
	COL_GREEN;
	glTranslatef(0.0f, 0.0f, ONTOP*2);
	sprintf(insChar, "%dcm", (int)altState);
	if (altState >= 100)				posX += 0.9f;
	else if ((altState < 100) && (altState >= 10))	posX += 2*0.9f;
	else if ((altState < 10) && (altState >= 0))	posX += 3*0.9f;
	displayMessage(posX*DEG2RAD, posY*DEG2RAD, insChar, 1.5f);
	glPopMatrix();
}

int AHclass::get_AltitudeState()
{
	return (int)altState;
}
