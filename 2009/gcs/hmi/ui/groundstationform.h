
#ifndef _IRIDIUMTESTCTRL_FORM_H_
#define _IRIDIUMTESTCTRL_FORM_H_

#include "ui_basegroundstation.h"
#include "ui_ahnsabout.h"

#include <QWidget>
#include <QString>
#include <QMap>
#include <QThread>
#include <QSlider>
#include <QDialog>
// ==================================
#include <QTimer> // ADDED FOR TIMER
#include <QTime>
#include <QCheckBox>
#include <QTextEdit>
#include <../QtNetwork/QUdpSocket> // Added for UDP
#include <../QtNetwork/QAbstractSocket>
// ==================================
#include <fstream>
extern "C" {
#include "driver/joystick.h"
}

class QCheckBox;

class AhnsAbout : public QWidget, private Ui::ahnsAbout
{
	Q_OBJECT
	
public:

	AhnsAbout(QWidget* parent = 0){setupUi(this);}
	~AhnsAbout(){}
	
private slots:

	void on_OkBtn_clicked(){this->close();}

};

class jsThread : public QThread
{
public:

	jsThread();
	~jsThread(){}

	float jsTempVal;

private:

	void run();
	void insJsVal(float jsVal);
	bool detStop;
	float jsVal;
};


class GroundStationForm : public QMainWindow, private Ui::BaseGroundStation
{
	Q_OBJECT

public:
	
	GroundStationForm(QWidget* parent = 0);
	~GroundStationForm(){}
		
private slots:

	// BUTTON SLOTS
	void About();
	void on_dispText_clicked();
	void on_dispNumber_clicked();
	void on_closeBtn_clicked();

	// Timers
	void on_timerStart_clicked();
	void on_timerPause_clicked();
	void on_timerReset_clicked();
	void on_timedJoystick_clicked();
	void incTimer();
	void timerDisplay();

	// UDP Mockup
	void on_sendUDPmsg_clicked();
	void on_UDPop_clicked();
	void sendUdpData();
	//void receiveUdpData();
	void processPendingDatagrams();
	void on_clearUDP_clicked();
	//void readUDPtextBox();
	//void OnBytesWritten();

	void dispJoystick();
	//void constRunJoystick();
	

private:

	AhnsAbout *abt;
	int *pNumPad;
	QUdpSocket *udpSocket;
	QUdpSocket *udpSocketrx;
	QByteArray datagram;
	QTimer *clock; // Timers
	QTimer *jsTimer; // Joystick timer
	float jsVal; // Joystick value
	short joyFD; // Joystick File Descriptor

	QThread *multiRunJoy;

	jsThread *jsOpr;

	QString label;
	QString text;
	QString textUDP;
	QByteArray rawTextUDP;
	double dispVal;
	int numPadVal;

};
#endif

