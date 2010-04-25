#ifndef ARTIFICIAL_HORIZON_H
#define ARTIFICIAL_HORIZON_H

/**************************************************/
/* ********** START OF THE HEADER FILE ********** */
/**************************************************/
#include <QGLWidget>
#include <QSize>
#include <QTimer>
#include <QScrollArea>


// DEFINING SCREEN MACROS
#define SCRN_WIDTH 700
#define SCRN_HEIGHT 700

// DEFINING COLOUR MACROS
#define COL_BLACK glColor3f(0.0f, 0.0f, 0.0f)
#define COL_WHITE glColor3f(1.0f, 1.0f, 1.0f)
//#define COL_SKYBLUE glColor3f(colorConv(0), colorConv(7), colorConv(109))
#define COL_SKYBLUE glColor3f(colorConv(0), colorConv(10), colorConv(150))
#define COL_BROWN glColor3f(colorConv(111), colorConv(73), colorConv(0))
//#define COL_BROWN glColor3f(colorConv(80), colorConv(52), colorConv(0))
#define COL_PINK glColor3f(colorConv(201), colorConv(47), colorConv(195))
#define COL_GRAY glColor3f(colorConv(61), colorConv(61), colorConv(61))
#define COL_YELLOW glColor3f(colorConv(255), colorConv(222), colorConv(0))
#define COL_GREEN glColor3f(colorConv(0), colorConv(255), colorConv(48))
#define COL_FADE glColor4f(0.0f, 0.0f, 0.0f, fadeOut)

// DEFINE LINE WIDTH MACROS
#define LWIDTH_HORIZON glLineWidth(2.5f)
#define LWIDTH_PITCHMKR_10 glLineWidth(1.0f)
#define LWIDTH_PITCHMKR_5 glLineWidth(1.0f)
#define LWIDTH_BANKMKR glLineWidth(4.0f)
#define LWIDTH_CNTRPT glLineWidth(2.5f)
#define LWIDTH_ALTMKR glLineWidth(1.5f)
#define LWIDTH_DEFAULT glLineWidth(1.0f)

// DEFINE OTHER MACROS
#define MKR_W 0.15f
#define PI 3.14159265358979
#define DEG2RAD 3.14159265358979/180
#define RAD2DEG 180/3.14159265358979
#define ONTOP 0.000000001f


class AHclass : public QGLWidget
{
	Q_OBJECT
	
public:
	AHclass(QWidget* parent = 0);
        virtual ~AHclass();
	
	QSize sizeHint() const;
	/* ****** DEFINING PUBLIC FUNCTIONS ******/
	void UpdateRoll();
	int get_AltitudeState();
	
	/* ****** DEFINING PUBLIC VARIABLES ******/
	/* **** ATTITUDE STATE VARIABLES ****/
	float angRoll;
	float angPitch;
	float angRollRate;
	float angPitchRate;
	float altState;
	float vertAltPos;
	float fadeOut;
	QSize tempSize;
	
	
private slots:

	/* ****** INITIALISATION FUNCTIONS ******/
	void EnableAntialiase();


	/* ****** DRAWING FUNCTIONS ******/
	void ATT_drawBackground_dynamic();
	void ATT_drawBackground_static();
	void ATT_drawBankMarkers(float offsetDist);
	void ATT_drawBankPointer(float offsetDist);
	void ATT_drawHorizon();
	void ATT_drawPitchMarkers();
	void ATT_drawCenterPoint();
	void ATT_drawAirdataBackground(float posX, float posY);
	void insertText(const char *message);
	void displayMessage(float posX, float posY, const char *message, float scale);
	void displayAltitudeText(float posX, float posY);
	
	void ATT_pitchMarkers_10DegInc(int mult);
	void ATT_pitchMarkers_5DegInc(int mult);
	

	/* ****** OTHER FUNCTIONS ******/
	float colorConv(int RGB); // Colour Conversion Function
//	void fadeOutOnExit(int unused);
//	void fadeOutScreen();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void keyPressEvent(QKeyEvent *keyPressed);
};


/************************************************/
/* ********** END OF THE HEADER FILE ********** */
/************************************************/

#endif
