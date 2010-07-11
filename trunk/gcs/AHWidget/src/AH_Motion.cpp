#include <iostream>
#include <cstdlib>
#include "AH.h"

#include <GLUT/glut.h>
#include <QKeyEvent>
#include <QScrollArea>

using namespace std;

void AHclass::keyPressEvent(QKeyEvent *keyPressed)
{
	//QScrollArea::keyPressEvent(keyPressed);
	switch (keyPressed->key())
	{
		case Qt::Key_Left:
			angRoll += 10.0f;
			cout << "LEFT KEY PRESS." << endl;
			//if (angPitch >= -90.0f*DEG2RAD)	angPitch -= 1.0f*DEG2RAD;
			//updateGL();
		        break;
		default:
			//QScrollArea::keyPressEvent(keyPressed);
			break;
	}
}

void AHclass::UpdateRoll()
{
        updating = true;
	angRoll += angRollRate;
	if ((angPitch <= 90.0f*DEG2RAD) && (angPitch >= -90.0f*DEG2RAD))	angPitch += angPitchRate*DEG2RAD;
	else if ((angPitch <= 90.0f*DEG2RAD) && (angPitchRate > 0))		angPitch += angPitchRate*DEG2RAD;
	else if ((angPitch >= 90.0f*DEG2RAD) && (angPitchRate < 0))		angPitch += angPitchRate*DEG2RAD;
	
	/* ***** WORKING OUT ALTITUDE POSITION ******/
        if (altState < vertAltPos)						altState += 1.0f;
	else if (altState > vertAltPos)						altState -= 1.0f;
	
        //updateGL();
        update();
        updating = false;
}
