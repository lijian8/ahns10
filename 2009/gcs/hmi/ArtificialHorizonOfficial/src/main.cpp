#include <iostream>
#include <cstdlib>

#include "../../../reuse/ahns_logger.h"
#include "AH.h"

#include <GL/glut.h>

using namespace std;

int main(int argc, char** argv)
{
	ahns_logger_init("AH_OpenGL.log"); // Initialise debugger

	// Initialise GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SCRN_WIDTH, SCRN_HEIGHT);

	// Create the window
	glutCreateWindow("Official Artificial Horizon");
	InitRendering();
	InitVariables();

	// Set Handler Functions
	//glutDisplayFunc(DrawScene);
	glutKeyboardFunc(HandleKeypress);
	glutReshapeFunc(HandleResize);
	
	glutIdleFunc(DrawScene);
	
	// Start Timers
	//glutTimerFunc(20, fadeOutOnExit, 1);

	glutMainLoop();

	return 0;
}
