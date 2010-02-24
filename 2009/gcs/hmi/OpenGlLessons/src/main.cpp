#include <iostream>
#include <stdlib.h>

// ../../../reuse/
#include "../../../reuse/ahns_logger.h"
#include "mockup.h"

#include <GL/glut.h>

using namespace std;

int main(int argc, char** argv)
{
	ahns_logger_init("AH_OpenGL.log"); // Initialise debugger

	// Initialise GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(700, 500);

	// Create the window
	glutCreateWindow("Mockup Artificial Horizon - VERY BASIC");
	initRendering();
	loadTextNumbers();

	// Set Handler Functions
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);

	glutTimerFunc(20, AnimateAH, 0);

	glutMainLoop();

	return 0;
}
