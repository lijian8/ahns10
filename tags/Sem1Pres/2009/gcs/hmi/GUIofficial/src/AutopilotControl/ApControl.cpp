#include "ApControl.h"
#include "include/GUIofficial.h"
#include "ui_GUIofficial.h"
#include "NetworkUDP.h"
#include "AnnSystem.h"
#include "ahns_logger.h"

#include <QString>
#include <QTimer>
#include <QByteArray>
#include <QColor>
#include <QDragEnterEvent>

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>

using namespace std;

/**
 * Initialises the Autopilot Control class
 * Performs joystick initialisation
 */
APclass::APclass(QWidget* parent)
{
	Q_UNUSED(parent);
	
	jsInit(jsHandle);
}

APclass::~APclass()
{
}

/**
 * @param frameSize Size of the dragging area in pixels, stroed in a QSize format
 * @param cursor Position of the mouse pointer
 * @return retVal
 */
QPoint APclass::processLateral(QSize frameSize, QPoint cursorPos)
{
	int norm_X = (cursorPos.x()*255)/(frameSize.rwidth()-6);
	int norm_Y = (cursorPos.y()*255)/(frameSize.rheight()-6);

	QPoint retVal(norm_X, norm_Y);
	return retVal;
}

/* *************************************** */
/* ********** GENERAL FUNCTIONS ********** */
/* *************************************** */
void GUIofficial::ApControl_Init()
{
	/* ******** SETTING UP THE SIGNAL/SLOT CONNECTIONS ******** */
	connect(ap_slAlt, SIGNAL(valueChanged(int)), this, SLOT(ap_slProcessAltitude()));
	connect(ap_sbAltSet, SIGNAL(valueChanged(int)), this, SLOT(ap_sbProcessAltitude()));
	connect(ap_sbPadLat_SA, SIGNAL(valueChanged(int)), this, SLOT(ap_setPadPrefix()));

	
	//label_grid->setPixmap(QPixmap("resource/objects/testPic.jpg"));
	//label_grid->show();
	ctrl_cursor->setPixmap(QPixmap("resource/objects/ap_cursor.png"));
	ctrl_cursor->show();
	//label_grid->setEnabled(false);
	inRegion = false;
	isUsingJoy = false;
	initPos = true;
	pos_retVal.setX(0);
	pos_retVal.setY(0);
	tmrFltModeIndx = 0;
	cmdPos.ap_CMD = 0;
	cmdPos.ap_X = 128;
	cmdPos.ap_Y = 128;
	cmdPos.ap_ALT = 100;
	ap_dispAlt->display(cmdPos.ap_ALT);
	isPosResetted = false;
	isAltResetted = false;
	ctrl_cursor->move(cmdPos.ap_X, cmdPos.ap_Y);
	ap_actStabDev("MAN", false);
	
	/* ****** Disable Joystick Elements ****** */
	ap_btnPad->setEnabled(false);
	ap_dispJoyAlt_SA->setEnabled(false);
	ap_dispJoyLat_SA->setEnabled(false);
	ap_dispJoyLong_SA->setEnabled(false);
	/* ****** Disable Control Pad Elements ****** */
	ap_btnJoystick->setEnabled(false);
	ap_btnPadReset->setEnabled(false);
	ap_sbPadAlt_SA->setEnabled(false);
	ap_sbPadLat_SA->setEnabled(false);
	ap_sbPadLong_SA->setEnabled(false);
	/* ****** Disable Autonomous Mode Elements ****** */
	ap_enableAutonomous(false);
}

void GUIofficial::ap_actStabDev(QString mode, bool isJoyAct)
{
	if (isJoyAct)
	{
		ap_displayAutopilotMessage(mode, "JOY");
		
		/* ****** Disable Control Pad Elements ****** */
		ap_btnJoystick->setEnabled(false);
		ap_btnPadReset->setEnabled(false);
		ap_sbPadAlt_SA->setEnabled(false);
		ap_sbPadLat_SA->setEnabled(false);
		ap_sbPadLong_SA->setEnabled(false);
		
		/* ****** Enable Joystick Elements ****** */
		ap_btnPad->setEnabled(true);
		ap_dispJoyAlt_SA->setEnabled(true);
		ap_dispJoyLat_SA->setEnabled(true);
		ap_dispJoyLong_SA->setEnabled(true);
	}
	else
	{
		ap_displayAutopilotMessage(mode, "PAD");
	
		/* ****** Disable Joystick Elements ****** */
		ap_btnPad->setEnabled(false);
		ap_dispJoyAlt_SA->setEnabled(false);
		ap_dispJoyLat_SA->setEnabled(false);
		ap_dispJoyLong_SA->setEnabled(false);
		
		/* ****** Enable Control Pad Elements ****** */
		ap_btnJoystick->setEnabled(true);
		ap_btnPadReset->setEnabled(true);
		ap_sbPadAlt_SA->setEnabled(true);
		ap_sbPadLat_SA->setEnabled(true);
		ap_sbPadLong_SA->setEnabled(true);
	}
}

void GUIofficial::ap_enableAutonomous(bool isToEnable)
{
	if (isToEnable)
	{
		ap_btnSetPos->setEnabled(true);
		ap_btnSetAlt->setEnabled(true);
		ap_btnResetAlt->setEnabled(true);
		ap_btnResetPos->setEnabled(true);
		ap_sbAltSet->setEnabled(true);
		ap_dispAlt->setEnabled(true);
		ap_slAlt->setEnabled(true);
	}
	else
	{
		ap_btnSetPos->setEnabled(false);
		ap_btnSetAlt->setEnabled(false);
		ap_btnResetAlt->setEnabled(false);
		ap_btnResetPos->setEnabled(false);
		ap_sbAltSet->setEnabled(false);
		ap_dispAlt->setEnabled(false);
		ap_slAlt->setEnabled(false);
	}
}

void GUIofficial::ap_enableStabilityAug(bool isToEnable)
{
	if (isToEnable)
	{
		ap_btnPad->setEnabled(true);
		ap_dispJoyAlt_SA->setEnabled(true);
		ap_dispJoyLat_SA->setEnabled(true);
		ap_dispJoyLong_SA->setEnabled(true);
		ap_btnJoystick->setEnabled(true);
		ap_btnPadReset->setEnabled(true);
		ap_sbPadAlt_SA->setEnabled(true);
		ap_sbPadLat_SA->setEnabled(true);
		ap_sbPadLong_SA->setEnabled(true);
	}
	else
	{
		ap_btnPad->setEnabled(false);
		ap_dispJoyAlt_SA->setEnabled(false);
		ap_dispJoyLat_SA->setEnabled(false);
		ap_dispJoyLong_SA->setEnabled(false);
		ap_btnJoystick->setEnabled(false);
		ap_btnPadReset->setEnabled(false);
		ap_sbPadAlt_SA->setEnabled(false);
		ap_sbPadLat_SA->setEnabled(false);
		ap_sbPadLong_SA->setEnabled(false);
	}
}

/* ********************************************** */
/* ********** COMMAND BUTTON FUNCTIONS ********** */
/* ********************************************** */
void GUIofficial::on_ap_btnAuto_clicked()
{
	if (cmdPos.ap_CMD != AUTO)
	{
		btnFltMode->setColor(QPalette::ButtonText, ANN_BLACK);
		ann_btnFltMode->setPalette(*btnFltMode);
		cmdPos.ap_CMD = AUTO;
		displaySystemMessage_NORMAL(APCMD_AP);
		ap_displayAutopilotMessage("AUTO");
		ann_flashTimer_FltMode->start(500);
		
		ap_enableAutonomous(true);
		ap_enableStabilityAug(false);
	}
}

void GUIofficial::on_ap_btnStabAug_clicked()
{
	if (cmdPos.ap_CMD != STAB)
	{
		btnFltMode->setColor(QPalette::ButtonText, ANN_BLACK);
		ann_btnFltMode->setPalette(*btnFltMode);
		cmdPos.ap_CMD = STAB;
		displaySystemMessage_NORMAL(APCMD_STAB);
		ap_displayAutopilotMessage("STAB");
		ann_flashTimer_FltMode->start(500);
		
		ap_enableAutonomous(false);
		/* ****** Disable Joystick Elements ****** */
		ap_btnPad->setEnabled(false);
		ap_dispJoyAlt_SA->setEnabled(false);
		ap_dispJoyLat_SA->setEnabled(false);
		ap_dispJoyLong_SA->setEnabled(false);
		
		/* ****** Enable Control Pad Elements ****** */
		ap_btnJoystick->setEnabled(true);
		ap_btnPadReset->setEnabled(true);
		ap_sbPadAlt_SA->setEnabled(true);
		ap_sbPadLat_SA->setEnabled(true);
		ap_sbPadLong_SA->setEnabled(true);
		
	}
}

void GUIofficial::on_ap_btnSetPos_clicked()
{
	if (inRegion)
	{
		if (!isPosResetted)
		{
			if (pos_retVal.x() > 255)	cmdPos.ap_X = 255;
			else if (pos_retVal.x() < 0)	cmdPos.ap_X = 0;
			else				cmdPos.ap_X = pos_retVal.x();
			if (pos_retVal.x() > 255)	cmdPos.ap_Y = 255;
			else if (pos_retVal.x() < 0)	cmdPos.ap_Y = 0;
			else				cmdPos.ap_Y = pos_retVal.y();
		}
		
		displaySystemMessage_NORMAL(APCMD_POS);
	}
	else if (initPos)
	{
		if (pos_retVal.x() > 255)	cmdPos.ap_X = 255;
		else if (pos_retVal.x() < 0)	cmdPos.ap_X = 0;
		if (pos_retVal.x() > 255)	cmdPos.ap_Y = 255;
		else if (pos_retVal.x() < 0)	cmdPos.ap_Y = 0;
		displaySystemMessage_NORMAL(APCMD_POS);
	}
}

void GUIofficial::on_ap_btnSetAlt_clicked()
{
	if (!isAltResetted)
	{
		if (ap_slAlt->value() == ap_sbAltSet->value())
		{
			cmdPos.ap_ALT = ap_slAlt->value();
			ap_dispAlt->display(cmdPos.ap_ALT);
			displaySystemMessage_NORMAL(APCMD_ALT);
		}
		else	displaySystemMessage_ERROR(APCMD_ALT_ERR);
	}
	else
	{
		displaySystemMessage_NORMAL(APCMD_ALT);
	}
}

void GUIofficial::on_ap_btnResetPos_clicked()
{
	cmdPos.ap_X = 128;
	cmdPos.ap_Y = 128;
	isPosResetted = true;
	ctrl_cursor->move(cmdPos.ap_X, cmdPos.ap_Y);
	displaySystemMessage_NORMAL(APCMD_RESET, "Position");
}

void GUIofficial::on_ap_btnResetAlt_clicked()
{
	cmdPos.ap_ALT = 100;
	ap_sbAltSet->setValue(100);
	ap_slAlt->setValue(100);
	isAltResetted = true;
	displaySystemMessage_NORMAL(APCMD_RESET, "Altitude");
}

void GUIofficial::on_ap_btnJoystick_clicked()
{
	isUsingJoy = true;
	ap_actStabDev("JOYSTICK", isUsingJoy);
	displaySystemMessage_NORMAL(APCMD_SA_DEV, "Joystick");
}

void GUIofficial::on_ap_btnPad_clicked()
{
	isUsingJoy = false;
	ap_actStabDev("S/AUG", isUsingJoy);
	displaySystemMessage_NORMAL(APCMD_SA_DEV, "On Screen Control Pad");
}

void GUIofficial::on_ap_btnPadReset_clicked()
{
}

/* *********************************************** */
/* ********** MESSAGE DISPLAY FUNCTIONS ********** */
/* *********************************************** */
void GUIofficial::ap_displayAutopilotMessage(QString mode, QString stab)
{
	QString formatMode, formatStab;
	formatMode = QString(tr("MODE: %1").arg(mode));
	formatStab = QString(tr("STAB DEV: %1").arg(stab));
	ap_tbApMsg->clear();
	ap_tbApMsg->append(formatMode);
	ap_tbApMsg->append(formatStab);
}

/* **************************************** */
/* ********** TRACKPAD FUNCTIONS ********** */
/* **************************************** */
void GUIofficial::mousePressEvent(QMouseEvent* event)
{
	int frame_x = ap_frAutoPos->pos().x() + HMI_Tabs->pos().x();
	int frame_y = ap_frAutoPos->pos().y() + HMI_Tabs->pos().y() + 50;

	QPoint frame(frame_x+12, frame_y+12);

	if (event->button() == Qt::LeftButton)
	{
		if	((event->pos().x() > frame_x) && (event->pos().y() > frame_y)
		&&	(event->pos().x() < frame_x + ap_frAutoPos->size().rwidth()) && (event->pos().y() < frame_y + ap_frAutoPos->size().rheight()))
		{
			ctrl_cursor->move(event->pos() - frame);
		}
	}
}

void GUIofficial::mouseReleaseEvent(QMouseEvent* event)
{
	int frame_x = ap_frAutoPos->pos().x() + HMI_Tabs->pos().x();
	int frame_y = ap_frAutoPos->pos().y() + HMI_Tabs->pos().y() + 50;
	QPoint frame(frame_x, frame_y);

	if	((event->pos().x() > frame_x) && (event->pos().y() > frame_y)
	&&	(event->pos().x() < frame_x + ap_frAutoPos->size().rwidth()) && (event->pos().y() < frame_y + ap_frAutoPos->size().rheight()))
	{
		pos_retVal = AP_Core.processLateral(ap_frAutoPos->size(), event->pos()-frame);
		//cerr << ">>>> " << pos_retVal.x() << " " << pos_retVal.y() << endl;
		inRegion = true;
		isPosResetted = false;
		isAltResetted = false;
		initPos = false;
	}
	else	inRegion = false;
}

void GUIofficial::mouseMoveEvent(QMouseEvent* event)
{
	int frame_x = ap_frAutoPos->pos().x() + HMI_Tabs->pos().x();
	int frame_y = ap_frAutoPos->pos().y() + HMI_Tabs->pos().y() + 50;
	QPoint frame((frame_x+12), (frame_y+12));
	
	if	((event->pos().x() > frame_x) && (event->pos().y() > frame_y)
	&&	(event->pos().x() < frame_x + ap_frAutoPos->size().rwidth()) && (event->pos().y() < frame_y + ap_frAutoPos->size().rheight()))
	{
		ctrl_cursor->move(event->pos() - frame);
	}
}

/* *************************************************** */
/* ********** STABILITY AUGMENTED FUNCTIONS ********** */
/* *************************************************** */
void GUIofficial::ap_slProcessAltitude()
{
	ap_sbAltSet->setValue(ap_slAlt->value());
}

void GUIofficial::ap_sbProcessAltitude()
{
	ap_slAlt->setValue(ap_sbAltSet->value());
}

void GUIofficial::ap_setPadPrefix()
{
	/* ******** DETERMINE LATERAL RATE POSITION ******** */
	if (ap_sbPadLat_SA->value() < 0)	ap_sbPadLat_SA->setPrefix("L");
	else if (ap_sbPadLat_SA->value() > 0)	ap_sbPadLat_SA->setPrefix("R");
	else					ap_sbPadLat_SA->setPrefix(" ");
}

joystickPacket GUIofficial::get_controlPadPosition()
{
	joystickPacket padPos;
	padPos.joy_X = (ap_sbPadLat_SA->value() + 100)*50;
	padPos.joy_Y = (ap_sbPadLong_SA->value() + 100)*50;
	padPos.joy_Z = (ap_sbPadAlt_SA->value() + 30)*125;
	
	return padPos;
}
