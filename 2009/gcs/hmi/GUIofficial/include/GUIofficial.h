#ifndef GUI_OFFICIAL_H
#define GUI_OFFICIAL_H

#include "ui_GUIofficial.h"
#include "ui_UDPconnection.h"
#include "UDPconnection.h"
#include "ApControl.h"

#include "AH.h"
#include "NetworkUDP.h"
#include "AnnSystem.h"
#include "ahns_logger.h"

#include <QGLWidget>
#include <QDesktopWidget>
#include <QWidget>
#include <QString>
#include <QMap>
#include <QThread>
#include <QDialog>
#include <QFile>
#include <QTimer> // ADDED FOR TIMER
#include <QTime>
#include <QCheckBox>
#include <QTextEdit>
#include <QPen>
// ==================================
#include <vector>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_div.h>

/* *********************************** */
/* ********** DEFINE MACROS ********** */
/* *********************************** */
/* ******** DEFINING NORMAL MACROS ******** */
#define HMI_START	0
#define UDP_CONNECTION	1
#define TIMER_INFO	2
#define TIMER_END	3
#define APCMD_AP	4	// Autopilot command - Autopilot Control
#define APCMD_STAB	5	// Autopilot command - Stability Augmentation
#define APCMD_RESET	6
#define APCMD_POS	7
#define APCMD_ALT	8
#define TIMER_SET	9
#define APCMD_SA_DEV	10
#define ANNUN_BATT_LVL	11
#define UDP_RXSTREAM	12
#define PLOTS_CLEAR	13

/* ******** DEFINING WARNING MACROS ******** */
#define ANNUN_BATT_LOW	0
#define ANNUN_BATT_CRT	1

/* ******** DEFINING ERROR MACROS ******** */
#define APCMD_ALT_ERR	0

#define COL_DEFAULT QColor(229, 229, 229)

class GUIofficial : public QMainWindow, private Ui::GUIofficial_MASTER
{
	Q_OBJECT

public:
	
	GUIofficial(QWidget* parent = 0);
	~GUIofficial(){}

	/* ******** PUBLIC VARIABLE DEFINITION ******** */
	QTimer *AH_Timer;

private slots:

	/* ******** INITIALISATION FUNCTION DEFINITION ******** */
	void Variable_Init();
	void Display_Init();
	void General_Init();
	void Annunciation_Init();
	void ApControl_Init();
	void Plots_Init();

	/* *********************************************** */
	/* ********** GENERAL BUTTON DEFINITION ********** */
	/* *********************************************** */
	/* ******** BUTTON SLOT DEFINITION ******** */
	void on_btnProgExit_clicked();
	void on_btn_swStart_clicked();
	void on_btn_swPause_clicked();
	void on_btn_swReset_clicked();
	void on_btn_tmrSet_clicked();
	void on_btn_tmrSS_clicked();
	void on_ann_btnBattStat_clicked();	
	
	void dispfuncStopwatch();
	void dispfuncTimer();
	void ann_TestON();
	void ann_TestOFF();

	/* **************************************************** */
	/* ********** NETWORK TRANSMITTION FUNCTIONS ********** */
	/* **************************************************** */
	/* ******** EXTERNAL DIALOG FUNCTIONS ******** */
	void UDPdialog();
	
	/* ******** RECEPTION BUTTONS ******** */
	void on_dbUDP_btnSend_clicked();
	void on_dbUDP_btnStop_clicked();
	void on_dbUDP_btnClear_clicked();
	
	/* ******** DISPLAY NOTIFICATION and OTHER FUNCTIONS ******** */
	void UdpTx_SendData();
	void UdpRx_DisplayData();
	void resumesMainWidgets();
	void activatesMainWidgets();
	void displaySystemMessage_NORMAL(unsigned short dispCategory_NORM, QString msg = 0);
	void displaySystemMessage_WARNING(unsigned short dispCategory_WARN, QString msg = 0);
	void displaySystemMessage_ERROR(unsigned short dispCategory_ERROR, QString msg = 0);
	
	/* ******** ANNUNCIATION SYSTEM FUNCTIONS ******** */
	void ann_procBatt(int battVal);
	void Annunciation_Exit();
	void ann_procAltMax(bool flag);
	void ann_procAltMin(bool flag);
	void ann_procBattStat();
	void ann_procFltMode();
	void ann_procLatBound();
	void ann_procAltSafe(bool flag);
	void ann_procTmr1Min();
	void ann_procTmr2Min();
	void ann_procTmr30Sec();
	
	/* ******** AUTOPILOT/STAB AUG SYSTEM COMMANDS ******** */
	void on_ap_btnAuto_clicked();
	void on_ap_btnStabAug_clicked();
	void on_ap_btnSetAlt_clicked();
	void on_ap_btnSetPos_clicked();
	void on_ap_btnResetPos_clicked();
	void on_ap_btnResetAlt_clicked();
	void on_ap_btnJoystick_clicked();
	void on_ap_btnPad_clicked();
	void on_ap_btnPadReset_clicked();
	void ap_slProcessAltitude();
	void ap_sbProcessAltitude();
	void ap_displayAutopilotMessage(QString mode, QString stab = "N/A");
	void ap_actStabDev(QString mode, bool isJoyAct);
	void ap_setPadPrefix();
	void ap_enableAutonomous(bool isToEnable);
	void ap_enableStabilityAug(bool isToEnable);
	joystickPacket get_controlPadPosition();
	
	/* ******** LOGGING PLOTTING FUNCTIONS ******** */
	void on_plot_btnAltClear_clicked();
	void plots_alt_seekHistory();
	void plots_altPID_seekHistory();
	void plots_formatPlots();
	void plots_updatePlot(receiverPacket_AP packBuffer);
	
	/* ******** OTHER FUNCTIONS ******** */
	void pbAlt_updateAltBar(int altVal);
	void updateStatus();
	
	/* ******** DEBUGGING FUNCTIONS ******** */
	void dbSetAttUpdate();
	void db_animateAH();
	


private:
	
	/* ******** PRIVATE VARIABLE DEFINITION ******** */
	/* ****** ATTITUDE STATE VARIABLES ******/
	int bankRate;
	int pitchRate;
	int altPos;
	/* ****** TIMER VARIABLES ******/
	QTimer* sw_System;
	bool isStarted;
	bool isResetted;
	int cntMilliSec;
	int cntSec;
	int cntMin;
	
	QTimer* timer_System;
	bool isTmrStarted;
	bool isTmrResetted;
	bool isTmrSS;
	bool isTmrSet;
	int cntTmrSec;
	int cntTmrMin;
	
	/* ****** ANNUNCIATOR PALETTE VARIABLES ******/
	QPalette* pbBattLevel;
	QPalette* pbAltDisp;
	QPalette* lbTel;
	QPalette* lbGrdPulse;
	QPalette* btnAltMax;
	QPalette* btnAltMin;
	QPalette* btnBattStat;
	QPalette* btnFltMode;
	QPalette* btnLatBound;
	QPalette* btnAltSafe;
	QPalette* btnTmr1Min;
	QPalette* btnTmr2Min;
	QPalette* btnTmr30Sec;
	/* ****** GENERAL ANNUNCIATOR VARIABLES ******/
	//QTimer* ann_flashTimer_Telemetry;
	//QTimer* ann_flashTimer_GrdPulse;
	//QTimer* ann_flashTimer_AltMax;
	//QTimer* ann_flashTimer_AltMin;
	QTimer* ann_flashTimer_BattStat;
	QTimer* ann_flashTimer_FltMode;
	QTimer* ann_flashTimer_LatBound;
	//QTimer* ann_flashTimer_SinkRate;
	QTimer* ann_flashTimer_Tmr1Min;
	QTimer* ann_flashTimer_Tmr2Min;
	QTimer* ann_flashTimer_Tmr30Sec;
	unsigned short firstTime;
	unsigned short tmr1MinIndx;
	unsigned short tmr2MinIndx;
	unsigned short tmr30SecIndx;
	unsigned short tmrFltModeIndx;
	unsigned short tmrBattStatIndx;
	uint8_t tgStatus;
	
	/* ****** OTHER VARIABLES ******/
	buffer_connInfo connInfo;
	QDesktopWidget desktopInfo;
	ConnectionDialog* udpConnDialog;
	QTime currentTimestamp;
	bool inRegion;
	bool initPos;
	bool isPosResetted; // Belongs to Autopilot Module: Flag to determine whether the position button was pressed
	bool isAltResetted; // Belongs to Autopilot Module: Flag to determine whether the altitude button was pressed
	bool isUsingJoy;
	apCommand cmdPos;
	joystickPacket udp_JS;
	receiverPacket_AP receivedPacket;

	/* ****** DEBUGGING VARIABLES ******/
	QTimer* db_altTimer;
	QTimer* stat_bar;
	
	/* ****** PLOT VARIABLES ******/
	QwtPlotCurve qpc_altCMD;	//("Commanded Altitude");
	QwtPlotCurve qpc_altTRU;	//("Actual/True Altitude");
	QwtPlotCurve qpc_altPID;	//("PID Components Summed");
	QwtPlotCurve qpc_altPID_P;	//("PID P Component");
	QwtPlotCurve qpc_altPID_I;	//("PID I Component");
	QwtPlotCurve qpc_altPID_D;	//("PID D Component");
	std::vector<double> vec_plotAlt;
	std::vector<double> vec_plotAlt_PID;
	int count;
	int reScale;
	bool altClear;
	bool network_firstRun;
	bool isPlotRunning;

	/* ****** MODULE VARIABLES ******/
	ConnectionDialog* GUI_dialog_UDP;
	UDPclass_tx* dialog_UDP;
	AHclass* AH_Core;
	UDPclass_tx udpTxBlock;
	UDPclass_rx* udpRxBlock;
	APclass AP_Core;
	QByteArray sendData;
	ahnsStatus theStatus;
	
	/* ****** TEMPORARY VARIABLES ******/
	QString tempQStr;
	QChar* tempQChar;
	QTimer* udpTxTimer;
	QUdpSocket* tempRxSocket;
	char tempTimeChar[3];
	char tempTimeChar_min[3];
	char tempTimeChar_sec[3];
	bool cbFirst;
	QPoint pos_retVal;
	
protected:
	void closeEvent(QCloseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
};




#endif

