/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Basic lue paramShapes" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <iostream>
#include <stdlib.h> //Needed for "exit" function

//Include OpenGL header files, so that we can use OpenGL
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

//Called when a key is pressed
void handleKeypress(unsigned char key, //The key that was pressed
					int x, int y) {    //The current mouse coordinates
	switch (key) {
		case 27: //Escape key
			exit(0); //Exit the program
	}
}

//Initializes 3D rendering
void initRendering() {
	//Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
}

//Called when the window is resized
void handleResize(int w, int h) {
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(45.0,                  //The camera angle
				   (double)w / (double)h, //The width-to-height ratio
				   1.0,                   //The near z clipping coordinate
				   200.0);                //The far z clipping coordinate
}

float red = 1.0f;
float blue = 0.0f;
float green = 0.0f;
int cnt = 0;
void upColor(int value)
{
	if (cnt == 0)
	{
		red = 0.0f;
		blue = 1.0f;
		green = 0.0f;
	}
	else if (cnt == 1)
	{
		red = 0.0f;
		blue = 0.0f;
		green = 1.0f;
	}
	else if (cnt == 2)
	{
		red = 1.0f;
		blue = 0.0f;
		green = 0.0f;
		cnt = 0;
	}

	cnt += 1;
	glutPostRedisplay();
	glutTimerFunc(1000, upColor, 0);
}

float _angle = 30.0f;
float _cameraAngle = 10.0f;

//Draws the 3D scene
void drawScene() {
	//Clear information from last draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	glLoadIdentity(); //Reset the drawing perspective
	glRotatef(-_cameraAngle, 0.1, -0.1f, 1.0f);
	glTranslatef(0.0f, 0.0f, -5.0f);
	glScalef(0.5f, 0.5f, 0.5f);

	glPushMatrix();// Save the transformation state, so that the next line can be changed according to the shape
	glTranslatef(0.0f, -1.0f, 0.0f);
	glRotatef(-_angle, 0.0f, 0.0f, 1.0f);

	glBegin(GL_QUADS); //Begin quadrilateral coordinates
	
	glColor3f(red, blue, green);
	glVertex3f(-0.7f, -0.5f, 0.0f);
	glVertex3f(0.7f, -0.5f, 0.0f);
	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(-0.4f, 0.5f, 0.0f);
	
	glEnd(); //End quadrilateral coordinates
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.3f,0.3f, 0.0f);
	glRotatef(_angle*5, 0.0f, 2.0f, 3.0f);
	glBegin(GL_TRIANGLES); //Begin triangle coordinates

	glColor3f(0.9f, 0.65f, 0.8f);
	glVertex3f(-0.2f, 0.0f, 0.0f);
	glVertex3f(-0.1f, -0.2f, 0.0f);
	glVertex3f(0.0f, 0.2f, 0.0f);

	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glVertex3f(0.1f, -0.2f, 0.0f);

	glVertex3f(0.1f, -0.2f, 0.0f);
	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(-0.1f, -0.2f, 0.0f);

	glEnd();
	glPopMatrix();

/*
	//EXCERCISE PENTAGON
	glVertex3f(0.1f, 0.3f, -8.0f);
	glVertex3f(0.2f, 0.1f, -8.0f);
	glVertex3f(0.3f, 0.3f, -8.0f);

	glVertex3f(0.1f, 0.3f, -8.0f);
	glVertex3f(0.3f, 0.5f, -8.0f);
	glVertex3f(0.3f, 0.3f, -8.0f);

	glVertex3f(0.3f, 0.5f, -8.0f);
	glVertex3f(0.5f, 0.3f, -8.0f);
	glVertex3f(0.3f, 0.3f, -8.0f);

	glVertex3f(0.4f, 0.1f, -8.0f);
	glVertex3f(0.5f, 0.3f, -8.0f);
	glVertex3f(0.3f, 0.3f, -8.0f);

	glVertex3f(0.2f, 0.1f, -8.0f);
	glVertex3f(0.4f, 0.1f, -8.0f);
	glVertex3f(0.3f, 0.3f, -8.0f);

	glEnd();
*/

	glPushMatrix();
	glTranslatef(1.0f, 1.0f, 0.0f);
	glTranslatef(0.5f, 0.0f, 0.0f);
	glRotatef(_angle*0.5, 0.0f, 0.0f, 1.0f);
	//glScalef(0.4f, 0.4f, 0.4f);
	glBegin(GL_TRIANGLES);

	//Pentagon
	glColor3f(0.0f, 0.75f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor3f(0.1f, 0.5f, 1.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glColor3f(0.8f, 0.2f, 0.25f);
	glVertex3f(-0.5f, 0.0f, 0.0f);
	
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	
	glVertex3f(-0.5f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);

	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.0f, 1.0f, 0.0f);
	glTranslatef(0.5f, 0.0f, 0.0f);
	glRotatef(_angle*3, 1.0f, 2.0f, 3.0f);
	glBegin(GL_TRIANGLES);

	//Triangle
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	
	glEnd(); //End triangle coordinates
	
	glPopMatrix();

	glutSwapBuffers(); //Send the 3D scene to the screen
}

bool forward = true;
void update(int value)
{
	_angle += 5.0f;
	if (_angle > 2*360)	_angle -= 2*360;

	if (forward)
	{
		if ((_cameraAngle > 0) & (_cameraAngle < 180))
		{
			_cameraAngle += 2.0f;
		}
		else if (_cameraAngle == 180)
		{
			forward = false;
			_cameraAngle -= 2.0f;
		}
		else
		{
			_cameraAngle = 0;
		}
	}
	else if (!forward)
	{
		if ((_cameraAngle > 0) & (_cameraAngle < 180))
		{
			_cameraAngle -= 2.0f;
		}
		else if (_cameraAngle == 0)
		{
			forward = true;
			_cameraAngle += 2.0f;
		}
		else
		{
			_cameraAngle = 0;
			forward = true;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400); //Set the window size
	
	//Create the window
	glutCreateWindow("Basic Shapes - videotutorialsrock.com");
	initRendering(); //Initialize rendering
	
	//Set handler functions for drawing, keypresses, and window resizes
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);

	glutTimerFunc(1000, upColor, 0);
	glutTimerFunc(25, update, 0);
	glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
	return 0; //This line is never reached
}









