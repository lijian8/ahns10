
#ifndef _IRIDIUMTESTCTRL_FORM_H_
#define _IRIDIUMTESTCTRL_FORM_H_

//#include "../../../reuse/ahns_logger.h"

#include "ui_txUDPGUI.h"
extern "C" {
#include "js_code/joystick.h"
}

#include <QWidget>
#include <QString>
#include <QMap>
#include <QThread>
#include <QDialog>
// ==================================
#include <QTimer> // ADDED FOR TIMER
#include <QTime>
#include <QCheckBox>
#include <QTextEdit>
#include <QProgressBar>
#include <../QtNetwork/QUdpSocket> // Added for UDP
#include <../QtNetwork/QAbstractSocket>
// ==================================
#include <fstream>
#include <stdlib.h>

class QCheckBox;

struct jsStruct
{
	int axs[2];
	int btn[2];
};

struct jsStore
{
	int fd;
	std::map</*key*/ int, /*data type*/ int> axis;
	std::map<int, int> button;
};

class jsTestThread : public QThread
{

public:

	jsTestThread();
	~jsTestThread(){}

	void stop();
	struct jsStore jsHandle;

private:
	void run();
	void jsInit(jsStore& data);
	void jsFunction(jsStore& data);

	bool stopnow;

};

class txUDP : public QMainWindow, private Ui::txSectUDP
{
	Q_OBJECT

public:
	
	txUDP(QWidget* parent = 0);
	~txUDP(){}
		
private slots:

	// GENERAL BUTTON SLOTS
	void on_closeBtn_clicked();
//	void on_readDisp_clicked();

	// UDP BUTTON SLOTS
	void on_sendUDP_clicked();
	void on_clearUDP_clicked();
	void on_timedTxUDP_clicked();
	void on_timedTxUDPstop_clicked();
	void on_confirmDetails_clicked();

	// UDP MEMBER FUNCTIONS
	void SendTimedTxtUDP();
	void EnableButtons();
	void DisableButtons();

	// JOYSTICK UDP BUTTON SLOTS
	void on_jsLocDisp_clicked();
	void on_jsRateSend_clicked();

	// JOYSTICK UDP FUNCTIONS
	void JsConstDisp();
	void SendTimedJsUDP();
	void QuitProgram();
	void RecPeakPoints();
	void on_recJsCal_clicked();
	
private:

	// UDP PRIVATE DATA MEMBER
	QUdpSocket *udpTxtSocket;
	QByteArray datagram;
	QString label;
	QString textUDP;
	QByteArray rawTextUDP;
	QByteArray UDP_data;
	QString szIP;
	quint16 sPort;

	// TIMER DATA MEMBER
	QTimer *UDP_timer;
	QByteArray timedData;
	short UDP_counter;

	// JOYSTICK UDP
	QUdpSocket *udpJsSocket;
	bool recorded;

	//JoyThread *jsOpr;
	jsTestThread *jsTestOpr;
	QTimer *jsTimer;
	QTimer *jsConstTimer;
	int jsUpdateRate;
};



#endif

