#include <iostream>
#include <cstdlib>
#include "AH.h"
//#include "AH_Alt.h"
//#include "GL/glut.h"

using namespace std;

/* ***************************************************** */
/* ********** COLOUR UNIT CONVERSION FUNCTION ********** */
/* ***************************************************** */
float AHclass::colorConv(int RGB)
{
	return (float)RGB/255;
}

/*float AHclass_ALT::colorConv(int RGB)
{
	return (float)RGB/255;
}

/* *************************************************** */
/* ********** ANTIALIASE ENABLING FUNCTIONS ********** */
/* *************************************************** */
void AHclass::EnableAntialiase()
{
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
}

/*void AHclass_ALT::EnableAntialiase()
{
	glHint(GL_LINE_SMOOTH, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
}*/

