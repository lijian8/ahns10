#include "include/GUIofficial.h"
#include "ui_UDPconnection.h"
#include "UDPconnection.h"
#include "AH.h"

#include <QWidget>
#include <QDesktopWidget>
#include <QString>
#include <QDateTime>
#include <QTime>
#include <QCloseEvent>
#include <QLabel> //Newly Added
#include <QGLWidget>
#include <QSound>
#include <QLineEdit>
// ==========================
#include <QTextBrowser>
#include <QByteArray> // Added for UDP
#include <iostream>
#include <iomanip>
#include <ctime>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_div.h>

using namespace std;

/* ************************************** */
/* ************************************** */
/* ********** FORM CONSTRUCTOR ********** */
/* ************************************** */
/* ************************************** */
GUIofficial::GUIofficial(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);

	ahns_logger_init("GUIofficial.log");
	
	AHNS_DEBUG("GUIofficial::GUIofficial(QWidget* parent)");
	
	/* ******** CREATE NEW VARIABLES ******** */
	AH_Core = new AHclass;
	AH_Timer = new QTimer(this);
	udpConnDialog = new ConnectionDialog();
	udpRxBlock = new UDPclass_rx;
	udpTxTimer = new QTimer(this);
	sw_System = new QTimer(this);
	timer_System = new QTimer(this);
	stat_bar = new QTimer(this);
	pbAltDisp = new QPalette(AH_pbAltDisp->palette());
	
	/* ******** EXECUTING INTIALISATION FUNCTIONS ******** */
	Variable_Init();
	Display_Init();
	General_Init();
	Annunciation_Init();
	ApControl_Init();
	Plots_Init();

	/* ******** SET UP ANY INTERNAL CONNECTIONS ******** */
	//connect(AH_Timer, SIGNAL(timeout()), this, SLOT(dbSetAttUpdate()));
	connect(sw_System, SIGNAL(timeout()), this, SLOT(dispfuncStopwatch()));
	connect(timer_System, SIGNAL(timeout()), this, SLOT(dispfuncTimer()));
	/* ******** SET UP ANY EXTERNAL CONNECTIONS ******** */
	AHNS_DEBUG("Before udpTxTimer = udpTxBlock.getUdpTimer();");
	connect(udpTxTimer, SIGNAL(timeout()), this, SLOT(UdpTx_SendData()));
	AHNS_DEBUG("After connect(udpTxTimer, SIGNAL(timeout()), this, SLOT(UdpTx_SendData()))");
	connect(stat_bar, SIGNAL(timeout()), this, SLOT(updateStatus()));
	connect(actionNetwork_Connection, SIGNAL(triggered()), this, SLOT(UDPdialog()));
	connect(udpConnDialog, SIGNAL(signal_resumesMainWidgets()), this, SLOT(resumesMainWidgets()));
	connect(udpConnDialog, SIGNAL(signal_activatesMainWidgets()), this, SLOT(activatesMainWidgets()));
	connect(udpRxBlock, SIGNAL(signal_dispData()), this, SLOT(UdpRx_DisplayData()));
	/* ******** SET UP ANY DEBUGGING CONNECTIONS ******** */
	connect(dbAH_Banking, SIGNAL(valueChanged(int)), dbAH_BankInd, SLOT(setNum(int)));
	connect(dbAH_Pitching, SIGNAL(valueChanged(int)), dbAH_PitchInd, SLOT(setNum(int)));
	connect(dbAH_Altitude, SIGNAL(valueChanged(int)), dbAH_AltInd, SLOT(setNum(int)));
	
	

	/* ******** SET UP TIMER INITIALISATION ******** */
	//AH_Timer->start(40);
	AH_Core->tempSize = AH_Output->size();
	AH_Core->show();
	//AH_Output->setWidget(AH_Core);
}


/* ******************************************************* */
/* ********** VARIABLE INITIALISATION FUNCTIONS ********** */
/* ******************************************************* */
void GUIofficial::Variable_Init()
{
	bankRate = 0;
	pitchRate = 0;
	altPos = 0;
	cbFirst = cb_enableRx->isChecked();
	
	isStarted = false;
	isResetted = false;
	cntMilliSec = 0;
	cntSec = 0;
	cntMin = 0;
	
	isTmrStarted = false;
	isTmrResetted = false;
	cntTmrSec = 0;
	cntTmrMin = 0;
	
	isTmrSS = false;
	isTmrSet = false;
}


/* ****************************************************** */
/* ********** DISPLAY INITIALISATION FUNCTIONS ********** */
/* ****************************************************** */
void GUIofficial::Display_Init()
{
	AHNS_DEBUG("GUIofficial::Display_Init()");
	int desktop_H = desktopInfo.screenGeometry().height();
	AHNS_DEBUG("Display_Init():: screenGeometry.height:   " << desktop_H);
	int desktop_W = desktopInfo.screenGeometry().width();
	AHNS_DEBUG("Display_Init():: screenGeometry.width:   " << desktop_W);
	int GUIofficial_H = size().height();
	AHNS_DEBUG("Display_init():: size().height():   " << GUIofficial_H);
	int GUIofficial_W = size().width();
	AHNS_DEBUG("Display_init():: size().width():   " << GUIofficial_W);
	move(desktop_W/2 - GUIofficial_W/2, desktop_H/2 - GUIofficial_H/2);
	
	sprintf(tempTimeChar, "%02d", cntMin);
	tempQStr = tempTimeChar;
	dbAH_BankInd->setNum(0);
	dbAH_PitchInd->setNum(0);
	dbAH_AltInd->setNum(0);
	disp_swMilliSec->display(tempQStr);
	disp_swSec->display(tempQStr);
	disp_swMin->display(tempQStr);
	disp_tmrSec->display(tempQStr);
	disp_tmrMin->display(tempQStr);
	
	AH_pbAltDisp->setValue(AH_Core->get_AltitudeState());
	AH_lcdAltDisp->display(AH_pbAltDisp->value());
}

void GUIofficial::General_Init()
{
	displaySystemMessage_NORMAL(HMI_START);
	stat_bar->start(40);
	udp_JS.joy_X = 127;
	udp_JS.joy_Y = 127;
	udp_JS.joy_Z = 0;
}


/* ***************************************** */
/* ********** PROGRAM EXIT BUTTON ********** */
/* ***************************************** */
void GUIofficial::closeEvent(QCloseEvent* event)
{
	Annunciation_Exit();

	if (udpTxTimer->isActive())	udpTxTimer->stop();
	if (AH_Timer->isActive())	AH_Timer->stop();
	if (sw_System->isActive())	sw_System->stop();
	if (timer_System->isActive())	timer_System->stop();
	if (db_altTimer->isActive())	db_altTimer->stop();
	if (stat_bar->isActive())	stat_bar->stop();

	delete AH_Core;
	delete udpTxTimer;
	delete AH_Timer;
	delete sw_System;
	delete timer_System;
	delete udpRxBlock;
	delete pbAltDisp;
	delete stat_bar;
	
	udpConnDialog->close();
	event->accept();
}

void GUIofficial::on_btnProgExit_clicked()
{
	//QSound::play("resource/audioSamples/smb_gameover.wav");
	close();
}

/* *********************************************** */
/* ********** GENERAL PROGRAM FUNCTIONS ********** */
/* *********************************************** */
void GUIofficial::resumesMainWidgets()
{
	connInfo = udpConnDialog->get_ConnectionInfo(); // Pass across value from UDP connection dialog to tx class
	if ((!connInfo.buf_txIP.isEmpty()) && (!connInfo.buf_txPort.isEmpty()) && (!connInfo.buf_rxPort.isEmpty()))
	{
		udpTxBlock.udpTxConnection(connInfo.buf_txIP, connInfo.buf_txPort);
		udpRxBlock->udpRxConnection(connInfo.buf_rxPort);
		displaySystemMessage_NORMAL(UDP_CONNECTION);
		udpTxTimer->start(50);
		SystemMessages_Disp->setEnabled(true);
	}
	else
	{
		dbUDP_Disp->append(QByteArray("ERROR: Incorrect IP address and Port numbers"));
	}

}

void GUIofficial::activatesMainWidgets()
{
	setEnabled(true);
}

void GUIofficial::displaySystemMessage_NORMAL(unsigned short dispCategory_NORM, QString msg)
{
	QString qstrTime = currentTimestamp.currentTime().toString("HH:mm:ss");
	QString qstrMsgDisp;
	
	switch (dispCategory_NORM)
	{
		case HMI_START:
			qstrMsgDisp = QString("%1$ AHNS 2009 HMI Graphical User Interface Started").arg(qstrTime);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case UDP_CONNECTION:
			qstrMsgDisp = QString("%1$ Host IP address: %2").arg(qstrTime).arg(connInfo.buf_txIP);
			SystemMessages_Disp->append(qstrMsgDisp);
			qstrMsgDisp = QString("%1$ Host port number: %2").arg(qstrTime).arg(connInfo.buf_txPort);
			SystemMessages_Disp->append(qstrMsgDisp);
			qstrMsgDisp = QString("%1$ Client port number: %2").arg(qstrTime).arg(connInfo.buf_rxPort);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case UDP_RXSTREAM:
			qstrMsgDisp = QString("%1$ Network Status: %2").arg(qstrTime).arg(msg);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case TIMER_SET:
			sprintf(tempTimeChar_sec, "%02d", cntTmrSec);
			sprintf(tempTimeChar_min, "%02d", cntTmrMin);
			qstrMsgDisp = QString("%1$ Timer SET: %2:%3").arg(qstrTime).arg(tempTimeChar_min).arg(tempTimeChar_sec);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case TIMER_INFO:
			qstrMsgDisp = QString("%1$ Time remaining: %2:%3").arg(qstrTime).arg(tempTimeChar_min).arg(tempTimeChar_sec);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case TIMER_END:
			qstrMsgDisp = QString("%1$ Timer time END").arg(qstrTime);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case APCMD_AP:
			qstrMsgDisp = QString("%1$ Autopilot ENGAGED").arg(qstrTime);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case APCMD_STAB:
			qstrMsgDisp = QString("%1$ Stability Augmentation ENGAGED").arg(qstrTime);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case APCMD_RESET:
			qstrMsgDisp = QString("%1$ Autonomous %2 RESET").arg(qstrTime).arg(msg);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case APCMD_POS:
			qstrMsgDisp = QString("%1$ Autonomous Position SET: X = %2, Y = %3").arg(qstrTime).arg((cmdPos.ap_X*200)/255).arg((cmdPos.ap_Y*200)/255);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case APCMD_ALT:
			qstrMsgDisp = QString("%1$ Autopilot Altitude SET: %2cm").arg(qstrTime).arg(cmdPos.ap_ALT);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case APCMD_SA_DEV:
			qstrMsgDisp = QString("%1$ Switched to stability augmentation device: %2").arg(qstrTime).arg(msg);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case ANNUN_BATT_LVL:
			qstrMsgDisp = QString("%1$ Battery Level: %2% remaining").arg(qstrTime).arg(msg);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case PLOTS_CLEAR:
			qstrMsgDisp = QString("%1$ Plotting Areas: Cleared").arg(qstrTime);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		default:
			AHNS_DEBUG("displaySystemMessage_NORMAL(unsigned short dispCategory_NORM, QString msg):: No correct macro selected");
			break;
	}
}

void GUIofficial::displaySystemMessage_WARNING(unsigned short dispCategory_WARN, QString msg)
{
	QString qstrTime = currentTimestamp.currentTime().toString("HH:mm:ss");
	QString qstrMsgDisp;
	
	switch (dispCategory_WARN)
	{
		case ANNUN_BATT_LOW:
			qstrMsgDisp = QString("%1$ BATT LOW: %2% remaining").arg(qstrTime).arg(msg);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		case ANNUN_BATT_CRT:
			qstrMsgDisp = QString("%1$ BATT CRIT: %2% remaining").arg(qstrTime).arg(msg);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		default:
			AHNS_DEBUG("displaySystemMessage_ERROR(unsigned short dispCategory_ERROR, QString msg):: No correct macro selected");
			break;
	}
}

void GUIofficial::displaySystemMessage_ERROR(unsigned short dispCategory_ERROR, QString msg)
{
	Q_UNUSED(msg);
	
	QString qstrTime = currentTimestamp.currentTime().toString("HH:mm:ss");
	QString qstrMsgDisp;
	
	switch (dispCategory_ERROR)
	{
		case APCMD_ALT_ERR:
			qstrMsgDisp = QString("%1$ UNSUCCESSFUL: Error occurred when setting the autopilot altitude").arg(qstrTime);
			SystemMessages_Disp->append(qstrMsgDisp);
			break;
		default:
			AHNS_DEBUG("displaySystemMessage_ERROR(unsigned short dispCategory_ERROR, QString msg):: No correct macro selected");
			break;
	}
}

/* ************************************* */
/* ********** TIMER FUNCTIONS ********** */
/* ************************************* */
/* ******** START/STOP BUTTON FUNCTION ******** */
void GUIofficial::on_btn_tmrSS_clicked()
{
	if ((!isTmrStarted) && ((cntTmrSec != 0) || (cntTmrMin != 0)))
	{
		timer_System->start(1000);
		isTmrStarted = true;
		isTmrSS = false;
		isTmrSet = false;
	}
	else
	{
		timer_System->stop();
		isTmrStarted = false;
		isTmrSS = true;
	}
	
	isTmrResetted = false;
}

/* ******** SET/RESET BUTTON FUNCTION ******** */
void GUIofficial::on_btn_tmrSet_clicked()
{
	tempQStr = sbMinSet->text();
	cntTmrMin = tempQStr.toInt();
	tempQStr = sbSecSet->text();
	cntTmrSec = tempQStr.toInt();
	timer_System->stop();
	db_altTimer->stop();
	isTmrStarted = false;
	isTmrResetted = true;
	isTmrSet = true;
	displaySystemMessage_NORMAL(TIMER_SET);
	dispfuncTimer();	
}

/* ******** TIMER DISPLAY FUNCTION ******** */
void GUIofficial::dispfuncTimer()
{
	if (!isTmrResetted)
	{
		if ((cntTmrSec != 0) || (cntTmrMin != 0))
		{
			cntTmrSec -= 1;
			if (cntTmrSec < 0)
			{
				cntTmrMin -= 1;
				cntTmrSec = 59;
			}
		}
	}

	sprintf(tempTimeChar_sec, "%02d", cntTmrSec);
	tempQStr = tempTimeChar_sec;
	disp_tmrSec->display(tempQStr);
	
	sprintf(tempTimeChar_min, "%02d", cntTmrMin);
	tempQStr = tempTimeChar_min;
	disp_tmrMin->display(tempQStr);
	
	if ((!isTmrSS) && (!isTmrSet))
	{
		//if ((cntTmrMin == 0) && (cntTmrSec == 30))				QSound::play("resource/audioSamples/tmr_oneMin.wav");
		//else if ((cntTmrMin == 0) && (cntTmrSec == 15))				QSound::play("resource/audioSamples/tmr_oneMin.wav");
		//else if ((cntTmrMin == 0) && (cntTmrSec <= 10) && (cntTmrSec > 0))	QSound::play("resource/audioSamples/tmr_<10sec.wav");
		if ((cntTmrSec == 0) && (cntTmrMin == 0))
		{
			timer_System->stop();
			displaySystemMessage_NORMAL(TIMER_END);
			isTmrStarted = false;
			//QSound::play("resource/audioSamples/tmr_end.wav");
			//QSound::play("resource/audioSamples/smb_mariodie.wav");
		}
	}
	
	if ((cntTmrMin == 2) && (cntTmrSec == 0))
	{
		displaySystemMessage_NORMAL(TIMER_INFO);
		ann_flashTimer_Tmr2Min->start(500);
	}
	else if ((cntTmrMin == 1) && (cntTmrSec == 0))
	{
		displaySystemMessage_NORMAL(TIMER_INFO);
		ann_flashTimer_Tmr1Min->start(500);
	}
	else if ((cntTmrMin == 0) && (cntTmrSec == 30))
	{
		displaySystemMessage_NORMAL(TIMER_INFO);
		ann_flashTimer_Tmr30Sec->start(500);
	}

	isTmrSS = false;
	isTmrSet = false;
}


/* ***************************************** */
/* ********** STOPWATCH FUNCTIONS ********** */
/* ***************************************** */
/* ******** START BUTTON FUNCTION ******** */
void GUIofficial::on_btn_swStart_clicked()
{
	if (!isStarted)
	{
		sw_System->start(10);
		isStarted = true;
	}
	
	isResetted = false;
	
	db_altTimer->start(40);
}

/* ******** PAUSE BUTTON FUNCTION ******** */
void GUIofficial::on_btn_swPause_clicked()
{
	if (isStarted)
	{
		sw_System->stop();
		isStarted = false;
	}
	
	isResetted = false;
}

/* ******** RESET BUTTON FUNCTION ******** */
void GUIofficial::on_btn_swReset_clicked()
{
	if (isStarted)
	{
		timer_System->stop();
		isStarted = false;
	}
	
	isResetted = true;
	cntMilliSec = 0;
	cntSec = 0;
	cntMin = 0;
	dispfuncStopwatch();
}

/* ******** STOPWATCH DISPLAY FUNCTION ******** */
void GUIofficial::dispfuncStopwatch()
{
	if (!isResetted)
	{
		cntMilliSec += 1;
		if (cntMilliSec == 100)
		{
			cntSec += 1;
			cntMilliSec = 0;
		}
		if (cntSec == 60)
		{
			cntMin += 1;
			cntSec = 0;
		}
	}	
	
	
	sprintf(tempTimeChar, "%02d", cntMilliSec);
	tempQStr = tempTimeChar;
	disp_swMilliSec->display(tempQStr);
	
	sprintf(tempTimeChar, "%02d", cntSec);
	tempQStr = tempTimeChar;
	disp_swSec->display(tempQStr);
	
	sprintf(tempTimeChar, "%02d", cntMin);
	tempQStr = tempTimeChar;
	disp_swMin->display(tempQStr);
}


void GUIofficial::pbAlt_updateAltBar(int altVal)
{
	altVal /= 10;
	if (altVal <= 80)
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_ORANGE);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	//else if ((altVal > 170) && (altVal <= 200))
	//{
	//	pbAltDisp->setColor(QPalette::Highlight, ANN_YELLOW);
	//	AH_pbAltDisp->setPalette(*pbAltDisp);
	//}
	else if ((altVal > 180) && (altVal <= 200))
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_ORANGE);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	else if (altVal > 200)
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_RED);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	else
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_GREEN);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	
	if (altVal >= 200)
	{
		ann_procAltMax(true);
		ann_procAltMin(false);
		ann_procAltSafe(false);
	}
	else if (altVal <= 100)
	{
		ann_procAltMin(true);
		ann_procAltMax(false);
		ann_procAltSafe(false);
	}
	else// if ((altVal < 200) && (altVal > 100))
	{
		ann_procAltMax(false);
		ann_procAltMin(false);
		ann_procAltSafe(true);
	}
	
	AH_pbAltDisp->setValue(altVal);
	AH_lcdAltDisp->display(AH_pbAltDisp->value());
}
/* ******************************************************* */
/* ******************************************************* */
/* ********** ALL DEBUGGING FUNCTIONS AND TOOLS ********** */
/* ******************************************************* */
/* ******************************************************* */
void GUIofficial::dbSetAttUpdate()
{
	tempQStr = dbAH_BankInd->text();
	bankRate = tempQStr.toInt();
	tempQStr = dbAH_PitchInd->text();
	pitchRate = tempQStr.toInt();
	tempQStr = dbAH_AltInd->text();
	altPos = tempQStr.toInt();
	AH_Core->angRollRate = (float)(((bankRate/50.0f)*30)/50);
	AH_Core->angPitchRate = (float)(((-pitchRate/50.0f)*10)/50);
	AH_Core->vertAltPos = (float)(altPos);
	AH_Core->UpdateRoll();
	
	if (AH_Core->get_AltitudeState() <= 80)
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_ORANGE);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	else if ((AH_Core->get_AltitudeState() > 250) && (AH_Core->get_AltitudeState() <= 350))
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_YELLOW);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	else if ((AH_Core->get_AltitudeState() > 350) && (AH_Core->get_AltitudeState() <= 400))
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_ORANGE);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	else if (AH_Core->get_AltitudeState() > 400)
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_RED);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	else
	{
		pbAltDisp->setColor(QPalette::Highlight, ANN_GREEN);
		AH_pbAltDisp->setPalette(*pbAltDisp);
	}
	
	AH_pbAltDisp->setValue(AH_Core->get_AltitudeState());
	AH_lcdAltDisp->display(AH_pbAltDisp->value());
}
