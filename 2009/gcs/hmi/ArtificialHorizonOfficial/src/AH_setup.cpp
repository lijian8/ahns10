#include <iostream>
#include <cstdlib>

// ../../../reuse/
#include "../../../reuse/ahns_logger.h"
#include "AH.h"

#include <GL/glut.h>

using namespace std;

// DEFINE ATTITUDE VARIABLES
float angRoll = 0.0f;
float angPitch = 0.0f;
float altState = 0.0f;
float fadeOut = 0.0f;

// DEFINE ALTITUDE VARIABLES

void InitRendering()
{
	AHNS_DEBUG("ENTERED initRendering()");
	EnableAntialiase();
	EnableAntialiase();
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_FLAT);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	AHNS_DEBUG("initRendering: Completed glEnable() functions.");
}

void InitVariables()
{
	angRoll = 0.0f;
	angPitch = 0.0f;
}

// KEYPRESS POLL
void HandleKeypress(unsigned char key, int x, int y)
{
	AHNS_DEBUG("ENTERED: handleKeypress(unsigned char key, int x, int y)");
	switch (key)
	{
		case 27: //Escape key
			AHNS_DEBUG("handleKeypress: Escape Key pressed, program will now exit.");
			glutTimerFunc(20, fadeOutOnExit, 1);
			//exit(0);
			break;
		case 'w':
		        if (angPitch <= 90.0f*DEG2RAD)	angPitch += 1.0f*DEG2RAD;
			break;
		case 's':
		        if (angPitch >= -90.0f*DEG2RAD)	angPitch -= 1.0f*DEG2RAD;
		        break;
		case 'a':
			angRoll -= 1.0f;
			break;
		case 'd':
			angRoll += 1.0f;
			break;
		case 'q':
			if (altState <= 500.0f)	altState += 1.0f;
			else			altState += 0.0f;
		        break;
		case 'z':
			if (altState <= 0.0f)			altState += 0.0f;
			else					altState -= 1.0f;
		        break;
		case 'r':
			angPitch = 0.0f;
			angRoll = 0.0f;
			altState = 0.0f;
			break;
		default:
			break;
	}
}


void HandleResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(10.4f, 5.0f/5.0f, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
