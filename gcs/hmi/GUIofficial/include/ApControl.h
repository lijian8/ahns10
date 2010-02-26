#ifndef AUTOPILOT_CONTROL_H
#define AUTOPILOT_CONTROL_H

#include "ui_GUIofficial.h"
#include "UDPconnection.h"
#include <linux/joystick.h>

/* *********************************** */
/* ********** DEFINE MACROS ********** */
/* *********************************** */
#define MAX_AXES	8
#define MAX_BUTTONS	6
#define JOY_MAX_LIM	255

/**************************************************/
/* ********** START OF THE HEADER FILE ********** */
/**************************************************/
struct jsStruct
{
	int axs[2];
	int btn[2];
};

struct jsStore
{
	int fd;
	std::map<int, int> axis;
	std::map<int, int> button;
};

class APclass : public QMainWindow, private Ui::GUIofficial_MASTER
{
public:
	APclass(QWidget* parent = 0);
	~APclass();

	QPoint processLateral(QSize frameSize, QPoint cursorPos);
	
	/* ******** JOYSTICK FUNCTIONS ******** */
	void jsInit(jsStore& data);
	void jsFunction(jsStore& data); // Initialising joystick device to return FD value
	joystickPacket get_joystickPosition();
	
private:
	int fd;
	struct jsStore jsHandle;
};

#endif
