
#include "groundstationform.h"

extern "C" {
#include "driver/joystick.h" 
}

#include <QString>
#include <QDateTime>
#include <QColor>
#include <QLCDNumber>
#include <QTime>
#include <QStatusBar>
#include <qradiobutton.h>
#include <QLabel> //Newly Added
// Libraries for Text Editing
#include <QTextEdit>
#include <QLineEdit>
// ==========================
#include <../QtNetwork/QUdpSocket> // Added for UDP
#include <QByteArray> // Added for UDP
#include <QThread>
#include <iostream>
#include <iomanip>
#include <ctime>

using namespace std;

int tCounter = 0;

GroundStationForm::GroundStationForm(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);
	
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(About()));
	connect(actionExit, SIGNAL(triggered()), this, SLOT(on_closeBtn_clicked()));
	
	clock = new QTimer(this);
	connect(clock, SIGNAL(timeout()), this, SLOT(incTimer())); //set up send timer event
	
	jsTimer = new QTimer(this);
	connect(jsTimer, SIGNAL(timeout()), this, SLOT(dispJoystick())); //setup joystick reading according to timer

	jsOpr = new jsThread();
	jsOpr->start();

	// UDP CONNECTIONS
	// Receiver Connection
	udpSocketrx = new QUdpSocket(this);
	udpSocketrx->bind(45454);
        connect(udpSocketrx, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	
	// Transmitting Connection
	udpSocket = new QUdpSocket(this);
	udpSocket->connectToHost("127.0.0.1", 45454);
	
	// AHNS About
	abt = new AhnsAbout();
}

void GroundStationForm::About()
{
	abt->show();
}

void GroundStationForm::on_dispText_clicked()
{
	// label = "Hello World!!";	
}

void GroundStationForm::on_dispNumber_clicked()
{
	numDisp->display(1108);
	numDisp->setSmallDecimalPoint(true);
	numDisp->display(1005);
	decFloat->setNum(12.34);
}

void GroundStationForm::on_timerStart_clicked()
{
	timerStart->setEnabled(false);
	clock->start(1000);
	txtDisp->setText("Timer Started");
}

void GroundStationForm::on_timerPause_clicked()
{
	clock->stop();
	timerStart->setEnabled(true);
	txtDisp->setText("Timer Paused");
}

void GroundStationForm::on_timerReset_clicked()
{
	clock->stop();
	timerStart->setEnabled(true);
	tCounter = 0;
	txtDisp->setText("Timer Resetted");
	timerDisplay();
}

void GroundStationForm::timerDisplay()
{
	timerDisp->display(tCounter);
}

void GroundStationForm::on_closeBtn_clicked()
{
	close();
}

// ================= UDP TRANSFER FUNCTIONS ================== //
void GroundStationForm::on_sendUDPmsg_clicked()
{
	text = inDisp->text();
	outDisp->setText(text);
}

void GroundStationForm::on_timedJoystick_clicked()
{
	jsTimer->start(200);
	jsDisp->setNum(jsOpr->jsTempVal);
}

void GroundStationForm::dispJoystick()
{
	jsDisp->setNum(jsOpr->jsTempVal);
}

// ====================================

void jsThread::run()
{
	float jsVal;
	int joyFD;

	joyFD = initJoystick();

	while(!detStop)
	{
		jsVal = handle_joystick(joyFD);
		insJsVal(jsVal);
	}
}

jsThread::jsThread()
{
	detStop = false;
}

/*void jsThread::jsStop()
{
	detStop = true;
}*/

void jsThread::insJsVal(float jsVal)
{
	jsTempVal = jsVal;
}

// ==================================

void GroundStationForm::on_UDPop_clicked()
{
	sendUdpData(); // ADDED BREAKPOINT 1
}

void GroundStationForm::on_clearUDP_clicked()
{
	statusLabel->setText("\n");
	orgDatagram->setText(tr("History cleared: Ready to be print again."));
}

// ============== INTERNAL PROCESSING FUNCTIONS ============== //
void GroundStationForm::incTimer()
{
	tCounter += 1;
	timerDisplay();
}

void GroundStationForm::sendUdpData()
{
	textUDP = textEnterUDP->text();
	rawTextUDP = textUDP.toAscii();
        QByteArray datagram = rawTextUDP + QByteArray::number(2);
	udpSocket->write(datagram.data(), datagram.size());
        orgDatagram->setText(tr(datagram.data())); // ADDED BREAKPOINT 2

	cout << "Debugging purpose - Data: " << datagram.data() << " Size: " << datagram.size() << endl;
}

void GroundStationForm::processPendingDatagrams()
{
	cout << "Pending Datagrams: " << udpSocketrx->hasPendingDatagrams() << endl;
	while (udpSocketrx->hasPendingDatagrams())
	{
		QByteArray datagramRx;
		datagramRx.resize(udpSocketrx->pendingDatagramSize());
		udpSocketrx->readDatagram(datagramRx.data(), datagramRx.size());
		statusLabel->setText(tr("Received datagram: \"%1\"").arg(datagramRx.data()));
	}
}
