#include <iostream>
#include <cstdlib>

// ../../../reuse/
#include "../../../reuse/ahns_logger.h"
#include "mockup.h"
#include "imageloader.h"

#include <GL/glut.h>

using namespace std;

// DEFINE ATTITUDE VARIABLES
float amountZ = 0.0f;
float amountX = 0.0f;
float stepSize = 0.0f;
short autoRotateLeft = 2;
short autoPitchUp = 2;
GLuint textID = 0;

// DEFINE ALTITUDE VARIABLES

void initRendering()
{
	AHNS_DEBUG("ENTERED initRendering()");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	AHNS_DEBUG("initRendering: Completed glEnable() functions.");
	
	stepSize = 0.2f;
	stepSize /= 40;
}

void loadTextNumbers()
{
	//Image* image = loadBMP("10deg.bmp");
	Image* image = loadBMP("vtr.bmp");
	textID = loadTexture(image);
	
	cout << (int)textID << endl;
	
	delete image;
}

// KEYPRESS POLL
void handleKeypress(unsigned char key, int x, int y)
{
	AHNS_DEBUG("ENTERED: handleKeypress(unsigned char key, int x, int y)");
	switch (key)
	{
		case 27: //Escape key
			AHNS_DEBUG("handleKeypress: Escape Key pressed, program will now exit.");
			exit(0);
		case 'a':
			autoRotateLeft = 4;
			autoPitchUp = 4;
			amountZ -= 1.0f;
			break;
		case 'd':
			autoRotateLeft = 4;
			autoPitchUp = 4;
			amountZ += 1.0f;
			break;
		case 'A':
			if (autoRotateLeft == 0)	autoRotateLeft = 2;
			else				autoRotateLeft = 1;
			break;
		case 'D':
			if (autoRotateLeft == 1)	autoRotateLeft = 2;
			else				autoRotateLeft = 0;
			break;
		case 'w':
		        if (amountX < 0.9f)	amountX += 0.005f;
			break;
		case 's':
		        if (amountX > -0.9f)	amountX -= 0.005f;
		        break;
		case 'W':
			if (autoPitchUp == 0)	autoPitchUp = 2;
			else			autoPitchUp = 1;
		        break;
		case 'S':
			if (autoPitchUp == 1)	autoPitchUp = 2;
			else			autoPitchUp = 0;
		        break;
		case 'r':
			autoRotateLeft = 3;
			autoPitchUp = 3;
		        break;
		case 'R':
			autoRotateLeft = 3;
			autoPitchUp = 3;
		        break;
		default:
			break;
	}
}


void handleResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, 7.0/5.0, 1.0, 200.0);
}
