#ifndef MOCKUP_H
#define MOCKUP_H

#include <GL/glut.h>

// DEFINING COLOUR MACROS
#define COL_BLACK glColor3f(0.0f, 0.0f, 0.0f)
#define COL_WHITE glColor3f(1.0f, 1.0f, 1.0f)
#define COL_SKYBLUE glColor3f(colorConv(0), colorConv(145), colorConv(226))
#define COL_BROWN glColor3f(colorConv(140), colorConv(80), colorConv(20))
#define COL_PINK glColor3f(colorConv(201), colorConv(47), colorConv(195))
#define COL_GRAY glColor3f(colorConv(61), colorConv(61), colorConv(61))

// DEFINE LINE WIDTH MACROS
#define LWIDTH_CTRMKR glLineWidth(5.0)
#define LWIDTH_ANGMKRL glLineWidth(2.0)
#define LWIDTH_ANGMKRM glLineWidth(2.0)
#define LWIDTH_ANGMKRS glLineWidth(2.0)
#define LWIDTH_XHAIR glLineWidth(3.0)
#define LWIDTH_ALTMKR glLineWidth(1.5)

// DEFINE OTHER MACROS
#define MKR_W 0.15f
#define PI 3.14159265358979

void handleKeypress(unsigned char key, int x, int y);
void handleResize(int w, int h);
void drawScene();
void initRendering();
void AnimateAH(int extra);
void loadTextNumbers();
float colorConv(int RGB);

void ALT_drawHeightMark_L(int mult);
void ALT_drawBackground();

#endif
