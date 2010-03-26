/**
 * @file
 * This file is for n00bs
 */

#include "../../../reuse/ahns_logger.h"

#include "txUDP.h"

/*
extern "C" {
#include "js_code/joystick.h"
}
*/

#include <QString>
#include <QDateTime>
#include <QTime>
#include <qradiobutton.h>
#include <QLabel> //Newly Added
// Libraries for Text Editing
#include <QTextEdit>
#include <QLineEdit>
// ==========================
#include <../QtNetwork/QUdpSocket> // Added for UDP
#include <QByteArray> // Added for UDP
#include <iostream>
#include <iomanip>
#include <ctime>
#include <QLCDNumber>
#include <QProgressBar>

using namespace std;

txUDP::txUDP(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);

	ahns_logger_init("jsTestThread.log");

	AHNS_DEBUG("txUDP::txUDP()");

	// Disable all buttons to start with
	AHNS_DEBUG("txUDP:: disable all buttons to start with.");
	DisableButtons();

	// Set up TIMER for timing UDP transfer
	AHNS_DEBUG("txUDP:: allocating new QTimer");
	UDP_timer = new QTimer(this);
	if (UDP_timer == NULL)
	{
		AHNS_ALERT("Could not allocate memory for new QTimer!");
		exit(1);
	}
	connect(UDP_timer, SIGNAL(timeout()), this, SLOT(SendTimedTxtUDP())); //set up send timer event
	
	jsTestOpr = new jsTestThread();
	if(jsTestOpr != NULL)
	{
		AHNS_DEBUG("txUDP:: before starting jsTestOpr->start()");
		jsTestOpr->start();
		connect(jsTestOpr, SIGNAL(finished()), this, SLOT(QuitProgram()));
	}
	else
	{
		AHNS_ALERT("txUDP:: could not allocate jsTestThread. Not started.");
		exit(1);
	}
	
	jsConstTimer = new QTimer(this);
	if (jsConstTimer != NULL)
	{
		connect(jsConstTimer, SIGNAL(timeout()), this, SLOT(JsConstDisp()));
	}
	else 
	{
		AHNS_ALERT("txUDP:: could not allocate  jsConstTimer. Not connected to anything.");
		exit(1);
	}

	jsTimer = new QTimer(this);
	if (jsTimer != NULL)
	{
		connect(jsTimer, SIGNAL(timeout()), this, SLOT(SendTimedJsUDP()));
	}
	else
	{
		AHNS_ALERT("txUDP:: could not allocate jsTimer and so did not connect it.");
		exit(1);
	}

	recorded = false;

	AHNS_DEBUG("txUDP:: finished the memory allocation and so forth. Ending function");
}

void txUDP::on_closeBtn_clicked()
{
	jsTestOpr->stop();
}

void txUDP::QuitProgram()
{
	AHNS_DEBUG("txUDP::quitProgram()");
	AHNS_DEBUG("txUDP:: PROGRAM ENDED SUCCESSFULLY");
	close();
}

// ================= UDP TRANSFER FUNCTIONS ================== //
// ===================== INITIALISATION ===================== //
void txUDP::on_confirmDetails_clicked()
{
	szIP = IPaddr->text();

	QString szPort = txtPortNo->text();
	sPort = szPort.toUShort();

	// Transmitting Connection
	udpTxtSocket = new QUdpSocket(this);
	udpTxtSocket->connectToHost(szIP, sPort);
	outDisp->append(QByteArray("IP: ") + szIP + QByteArray("    Text Port: ") + szPort);

	EnableButtons();
}

void txUDP::on_sendUDP_clicked()
{
	textUDP = textEnterUDP->text();
	rawTextUDP = textUDP.toAscii();
        QByteArray datagram = rawTextUDP; // + QByteArray::number(2);
	udpTxtSocket->write(datagram.data(), datagram.size());
        orgDatagram->append(tr(datagram.data()));
	textEnterUDP->clear();
}

void txUDP::on_clearUDP_clicked()
{
	orgDatagram->setText(tr("History cleared: Ready to be print again."));
	orgDatagram->clear();
	textEnterUDP->clear();
	UDP_timer->stop();
}

void txUDP::on_timedTxUDP_clicked()
{
	UDP_counter = 0;
	UDP_data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!";
	UDP_timer->start(100); // Transmission frequency (2Hz = 500ms)
	timedTxUDP->setEnabled(false);
}

void txUDP::on_timedTxUDPstop_clicked()
{
	if (UDP_timer->isActive() == true)
	{
		UDP_timer->stop();
		orgDatagram->append(tr("Data package sending was interrupted."));
		timedTxUDP->setEnabled(true);
	}
}

// ============== JOYSTICK UDP SENDING FUNCTIONS ============== //
jsTestThread::jsTestThread()
{
	stopnow = false;
}

void jsTestThread::run()
{
	AHNS_DEBUG("jsTestThread::run()");

	jsInit(jsHandle);
	AHNS_DEBUG("run:: Before while(1) and after jsInit() for jsTestThread.");
	while(!stopnow)
	{
		AHNS_DEBUG("run:: Before passing in of the return file handle and associated values.");
		jsFunction(jsHandle);
	}

	AHNS_DEBUG("How many axies did we see? " << jsHandle.axis.size());
}

void jsTestThread::stop()
{
	stopnow = true;
	AHNS_DEBUG("jsTestThread:: stopnow = true, loop stopped");
}

void txUDP::on_jsLocDisp_clicked()
{
	QString szPort = jsPortNo->text();
	sPort = szPort.toUShort();

	// Transmitting Connection
	udpJsSocket = new QUdpSocket(this);
	udpJsSocket->connectToHost(szIP, sPort);
	outDisp->append(QByteArray("Joystick Port: ") + szPort);

	QString sJsUpRate = jsUpRate->text();
	jsUpdateRate = sJsUpRate.toInt();

	outDisp->append(QByteArray("Joystick Send Rate: ") + sJsUpRate + QByteArray("ms"));
	jsConstTimer->start(1);
	jsLocDisp->setEnabled(false);
	jsUpRate->setEnabled(false);
	jsPortNo->setEnabled(false);
	jsRateSend->setEnabled(true);
}

void txUDP::on_jsRateSend_clicked()
{
}

// ============== INTERNAL PROCESSING FUNCTIONS ============== //
void txUDP::RecPeakPoints()
{
	AHNS_DEBUG("txUDP::RecPeakPoints()");

	QString qsxMax = xMax->text();
	QString qsxMin = xMin->text();
	QString qsyMax = yMax->text();
	QString qsyMin = yMin->text();

	xMax->setEnabled(false);
	xMin->setEnabled(false);
	yMax->setEnabled(false);
	yMin->setEnabled(false);

	int ixMax = qsxMax.toInt();
	int ixMin = qsxMin.toInt();
	int iyMax = qsyMax.toInt();
	int iyMin = qsyMin.toInt();

	AHNS_DEBUG("RecPeakPoints:: xMax Value: " << ixMax);
	AHNS_DEBUG("RecPeakPoints:: xMin Value: " << ixMin);
	AHNS_DEBUG("RecPeakPoints:: yMax Value: " << iyMax);
	AHNS_DEBUG("RecPeakPoints:: yMin Value: " << iyMin);
	
	jsPosX->setRange(ixMin, ixMax);
	jsPosY->setRange(iyMin, iyMax);

	recorded = true;
}

void txUDP::on_recJsCal_clicked()
{
	if (!recorded)
	{
		recJsCal->setFlat(true);
		recJsCal->setEnabled(false);
		RecPeakPoints();
		recJsCal->setText(tr("RECORED!!"));
	}
}

void txUDP::JsConstDisp()
{
//	jsUnResDisp->display((int)jsOpr->jsOutVal);
	jsTestDisp_x->display(jsTestOpr->jsHandle.axis[0]);
	jsTestDisp_y->display(-jsTestOpr->jsHandle.axis[1]);
	jsPosX->setValue(jsTestOpr->jsHandle.axis[0]);
	jsPosY->setValue(-jsTestOpr->jsHandle.axis[1]);
}

void txUDP::EnableButtons()
{
	timedTxUDPstop->setEnabled(true);
	timedTxUDP->setEnabled(true);
	sendUDP->setEnabled(true);
	jsLocDisp->setEnabled(true);
	clearUDP->setEnabled(true);
	jsUpRate->setEnabled(true);
	jsPortNo->setEnabled(true);
	textEnterUDP->setEnabled(true);
}

void txUDP::DisableButtons()
{
	sendUDP->setEnabled(false);
	clearUDP->setEnabled(false);
	timedTxUDP->setEnabled(false);
	timedTxUDPstop->setEnabled(false);
	jsLocDisp->setEnabled(false);
	jsUpRate->setEnabled(false);
	jsRateSend->setEnabled(false);
	jsPortNo->setEnabled(false);
	textEnterUDP->setEnabled(false);
}

void txUDP::SendTimedTxtUDP()
{
	if (UDP_counter < UDP_data.size())
	{
		timedData = UDP_data[UDP_counter] + QByteArray(" "); // Adding a space before the insert of msg number for debugging.
		QByteArray timedDatagram = timedData + QByteArray::number(UDP_counter + 1); // Reformatted the data before sending.
		udpTxtSocket->write(timedDatagram.data(), timedDatagram.size());
		orgDatagram->append(tr(timedDatagram.data()));
		UDP_counter++;
	}
	else
	{
		QByteArray endMsg = "End of data transmission.";
		udpTxtSocket->write(endMsg.data(), endMsg.size());

		UDP_timer->stop();
		orgDatagram->append(endMsg.data());
		timedTxUDP->setEnabled(true);
	}
}

void txUDP::SendTimedJsUDP()
{
	//udpJsSocket->write((char*)&(jsOpr->jsOutVal), sizeof(float));
	//jsSendDisp->display((int)jsOpr->jsOutVal);
}
