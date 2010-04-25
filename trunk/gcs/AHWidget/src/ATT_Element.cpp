#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <cmath>

#include "AH.h"

#include <GL/glut.h>

using namespace std;


/* ********************************************************************** */
/* ********** DRAWING THE BACKGROUND OF THE ARTIFICIAL HORIZON ********** */
/* ********************************************************************** */
/* ******** DRAWING THE SECTIONS THAT ANIMATES ******** */
void AHclass::ATT_drawBackground_dynamic()
{
	/* ****** DRAWING THE SKY COMPONENT ****** */
	glPushMatrix();
	
	COL_SKYBLUE;
	glBegin(GL_QUADS);
	glVertex3f(-180.0*2*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(180.0*2*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(180.0*2*DEG2RAD, 90.0*2*DEG2RAD, 0.0f);
	glVertex3f(-180.0*2*DEG2RAD, 90.0*2*DEG2RAD, 0.0f);
	glEnd();
	
	glPopMatrix();
	
	/* ****** DRAWING THE EARTH COMPONENT ****** */
	glPushMatrix();
	
	COL_BROWN;
	glBegin(GL_QUADS);
	glVertex3f(-180.0*2*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(180.0*2*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(180.0*2*DEG2RAD, -90.0*2*DEG2RAD, 0.0f);
	glVertex3f(-180.0*2*DEG2RAD, -90.0*2*DEG2RAD, 0.0f);
	glEnd();
	
	glPopMatrix();
}

/* ******** DRAWING THE SECTIONS THAT ARE STATIC ******** */
void AHclass::ATT_drawBackground_static() // NEEDS TO BE ADDED
{
	/* ****** DRAWING THE BACK SIDEBAR COMPONENTS ****** */
	/* ******   (If required in the new design)   ****** */
	/* >>>>>>>>>>>>>>> NEEDS TO BE ADDED <<<<<<<<<<<<<<< */
}

/* ***************************************** */
/* ********** DRAWING THE HORIZON ********** */
/* ***************************************** */
/* *********** 1e-6 Units In Front ********* */
void AHclass::ATT_drawHorizon()
{
	glPushMatrix();
	
	COL_WHITE;
	LWIDTH_HORIZON;
	
	glBegin(GL_LINES);
	glVertex3f(-180.0*2*DEG2RAD, 0.0f, ONTOP);
	glVertex3f(180.0*2*DEG2RAD, 0.0f, ONTOP);
	glEnd();
	
	glPopMatrix();
}

/* *********************************************** */
/* ********** DRAWING THE PITCH MARKERS ********** */
/* *********************************************** */
/* ******** DRAWING MARKERS WITH TEN DEGREE INCREMENTS ******** */
/* ******************** 1e-6 Units In Front ******************* */
void AHclass::ATT_pitchMarkers_10DegInc(int mult)
{
	float degOffset = mult*10.0f*DEG2RAD;
	//cout << degOffset << endl;
	
	glPushMatrix();
	glTranslatef(0.0f, degOffset, ONTOP);
	
	COL_WHITE;
	LWIDTH_PITCHMKR_10;
	 
	/* ****** POSITIONING THE VERTICAL TICKS (SKY)****** */
	if (mult > 0)
	{
		/* **** LEFT VERTICAL TICK **** */
		glPushMatrix();
		glTranslatef(-11.0f*DEG2RAD, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, -1.0f*DEG2RAD, 0.0f);
		glEnd();
		glPopMatrix();
		/* **** RIGHT VERTICAL TICK **** */
		glPushMatrix();
		glTranslatef(11.0f*DEG2RAD, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, -1.0f*DEG2RAD, 0.0f);
		glEnd();
		glPopMatrix();
	}
	/* ****** POSITIONING THE VERTICAL TICKS (GND)****** */
	else if (mult < 0)
	{
		/* **** LEFT VERTICAL TICK **** */
		glPushMatrix();
		glTranslatef(-11.0f*DEG2RAD, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f*DEG2RAD, 0.0f);
		glEnd();
		glPopMatrix();
		/* **** RIGHT VERTICAL TICK **** */
		glPushMatrix();
		glTranslatef(11.0f*DEG2RAD, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f*DEG2RAD, 0.0f);
		glEnd();
		glPopMatrix();
	}
	
	/* ****** POSITIONING THE HORIZONTAL BARS ****** */
	/* **** LEFT HORIZONTAL BAR **** */
	glPushMatrix();
	glTranslatef(-7.0f*DEG2RAD, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(-4.0f*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(4.0f*DEG2RAD, 0.0f, 0.0f);
	glEnd();
	glPopMatrix();
	/* **** RIGHT HORIZONTAL BAR **** */
	glPushMatrix();
	glTranslatef(7.0f*DEG2RAD, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(-4.0f*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(4.0f*DEG2RAD, 0.0f, 0.0f);
	glEnd();
	glPopMatrix();
	
	glPopMatrix();
}

/* ******** DRAWING MARKERS WITH FIVE DEGREE INCREMENTS ******** */
/* ******************** 1e-6 Units In Front ******************** */
void AHclass::ATT_pitchMarkers_5DegInc(int mult)
{
	float degOffset = 0.0f;
	
	if (mult < 0)		degOffset = 5.0f;
	else if(mult > 0)	degOffset = -5.0f;
	degOffset = (10.0f*mult + degOffset)*DEG2RAD;
	
	glPushMatrix();
	glTranslatef(0.0f, degOffset, ONTOP);
	
	COL_WHITE;
	LWIDTH_PITCHMKR_5;
	
	glBegin(GL_LINES);
	glVertex3f(-4.0f*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(4.0f*DEG2RAD, 0.0f, 0.0f);
	glEnd();
	
	glPopMatrix();
}

/* ******** DRAWING THE COMBINED MARKERS ******** */
void AHclass::ATT_drawPitchMarkers()
{
	glPushMatrix();
	char *insChar = NULL;

	char tickLabel[9][4] = {"10", "20", "30", "40", "50", "60", "70", "80", "90"};
	for (int iMkr = 1; iMkr < 10; ++iMkr)
	{
		ATT_pitchMarkers_5DegInc(iMkr);
		ATT_pitchMarkers_5DegInc(-iMkr);
		ATT_pitchMarkers_10DegInc(iMkr);
		ATT_pitchMarkers_10DegInc(-iMkr);
		insChar = &tickLabel[iMkr-2][4];
		COL_WHITE;
		displayMessage(0, iMkr*10*DEG2RAD, insChar, 2.5f);
		displayMessage(0, -iMkr*10*DEG2RAD, insChar, 2.5f);
	}

	glPopMatrix();
}

/* ************************************************* */
/* ********** DRAWING THE BANKING MARKERS ********** */
/* ************************************************* */
void AHclass::ATT_drawBankMarkers(float offsetDist)
{
	glPushMatrix();
	float offsetLength = 1.0f;
	
	glTranslatef(0.0f, 0.0f, ONTOP);
	COL_WHITE;
	
	glPushMatrix();
	LWIDTH_BANKMKR;
	glBegin(GL_LINES);
	glVertex3f(0.0f, offsetDist*DEG2RAD, 0.0f);
	glVertex3f(0.0f, (offsetDist + offsetLength)*DEG2RAD, 0.0f);
	glEnd();
	glPopMatrix();
	
	for (float iBank = 10.0f; iBank <= 90.0f; iBank += 5.0f)
	{
		if ((iBank == 10.0f) || (iBank == 20.0f) || (iBank == 30.0f) ||
		    (iBank == 45.0f) || (iBank == 60.0f) || (iBank == 90.0f))
		{
			if (iBank == 90.0f)				offsetLength += 1.0f;
			else if ((iBank == 45.0f) || (iBank == 60.0f))	offsetLength += 0.7f;
			
			glPushMatrix();
			glRotatef(iBank, 0.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
			glVertex3f(0.0f, offsetDist*DEG2RAD, 0.0f);
			glVertex3f(0.0f, (offsetDist + offsetLength)*DEG2RAD, 0.0f);
			glEnd();
			glPopMatrix();

			glPushMatrix();
			glRotatef(-iBank, 0.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
			glVertex3f(0.0f, offsetDist*DEG2RAD, 0.0f);
			glVertex3f(0.0f, (offsetDist + offsetLength)*DEG2RAD, 0.0f);
			glEnd();
			glPopMatrix();
		}
	}
	
	glPopMatrix();
}

/* ************************************************* */
/* ********** DRAWING THE BANKING POINTER ********** */
/* ************************************************* */
void AHclass::ATT_drawBankPointer(float offsetDist)
{
	glPushMatrix();
	
	glTranslatef(0.0f, (offsetDist - 0.15f)*DEG2RAD, ONTOP);
	COL_YELLOW;
	
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.6f*0.75f*DEG2RAD, -2.0f*0.75f*DEG2RAD, 0.0f);
	glVertex3f(0.6f*0.75f*DEG2RAD, -2.0f*0.75f*DEG2RAD, 0.0f);
	glEnd();
	glPopMatrix();
	
	glPopMatrix();
}

/* ******************************************************* */
/* ********** DRAWING THE CENTER AIRCRAFT/POINT ********** */
/* ******************************************************* */
void AHclass::ATT_drawCenterPoint()
{
	glPushMatrix();
	
	glTranslatef(0.0f, 0.0f, ONTOP);
	glScalef(1.2f, 1.2f, 1.0f);

	COL_YELLOW;
	LWIDTH_CNTRPT;
	
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-1.5f*DEG2RAD, -2.0f*DEG2RAD, 0.0f);
	glVertex3f(-1.5f*DEG2RAD, -2.0f*DEG2RAD, 0.0f);
	glVertex3f(-3.0f*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(-3.0f*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(-8.0f*DEG2RAD, 0.0f, 0.0f);
	
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.5f*DEG2RAD, -2.0f*DEG2RAD, 0.0f);
	glVertex3f(1.5f*DEG2RAD, -2.0f*DEG2RAD, 0.0f);
	glVertex3f(3.0f*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(3.0f*DEG2RAD, 0.0f, 0.0f);
	glVertex3f(8.0f*DEG2RAD, 0.0f, 0.0f);
	glEnd();
	
	glPopMatrix();
}

/* ***************************************************** */
/* ********** DRAWING THE AIR DATA BACKGROUND ********** */
/* ***************************************************** */
void AHclass::ATT_drawAirdataBackground(float posX, float posY)
{
	glPushMatrix();
	glTranslatef(posX*DEG2RAD, posY*DEG2RAD, ONTOP*1.0f);
	
	COL_BLACK;
	
	glBegin(GL_QUADS);
	glVertex3f(-3.0f*DEG2RAD, -1.0f*DEG2RAD, 0.0f);
	glVertex3f(-3.0f*DEG2RAD, 1.0f*DEG2RAD, 0.0f);
	glVertex3f(3.0f*DEG2RAD, 1.0f*DEG2RAD, 0.0f);
	glVertex3f(3.0f*DEG2RAD, -1.0f*DEG2RAD, 0.0f);
	glEnd();
	
	glPopMatrix();
}



