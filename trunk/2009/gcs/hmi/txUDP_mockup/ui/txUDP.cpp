
#include "txUDP.h"

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

using namespace std;

txUDP::txUDP(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);

	// Disable all buttons to start with
	sendUDP->setEnabled(false);
	clearUDP->setEnabled(false);
	timedTxUDP->setEnabled(false);
	timedTxUDPstop->setEnabled(false);

	// Set up TIMER for timing UDP transfer
	UDP_timer = new QTimer(this);
	connect(UDP_timer, SIGNAL(timeout()), this, SLOT(SendTimedUDP())); //set up send timer event
}

void txUDP::on_closeBtn_clicked()
{
	close();
}

// ================= UDP TRANSFER FUNCTIONS ================== //
// ===================== INITIALISATION ===================== //
void txUDP::on_confirmDetails_clicked()
{
	szIP = IPaddr->text();

	QString szPort = portNo->text();
	sPort = szPort.toUShort();

	// Transmitting Connection
	udpSocket = new QUdpSocket(this);
	udpSocket->connectToHost(szIP, sPort);
	outDisp->append(QByteArray("IP: ") + szIP + QByteArray(" Port: ") + szPort);
	connect(udpSocket, SIGNAL(hostFound()), this, SLOT(EnableButtons()));

	sendUDP->setEnabled(true);
	timedTxUDP->setEnabled(true);
	timedTxUDPstop->setEnabled(true);
}

void txUDP::on_readDisp_clicked()
{
	text = inDisp->text();
	outDisp->setText(text);
}

void txUDP::on_sendUDP_clicked()
{
	textUDP = textEnterUDP->text();
	rawTextUDP = textUDP.toAscii();
        QByteArray datagram = rawTextUDP; // + QByteArray::number(2);
	udpSocket->write(datagram.data(), datagram.size());
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

// ============== INTERNAL PROCESSING FUNCTIONS ============== //
void txUDP::EnableButtons()
{
//	sendUDP->setEnabled(true);
	clearUDP->setEnabled(true);
//	timedTxUDP->setEnabled(true);
//	timedTxUDPstop->setEnabled(true);
}

void txUDP::SendTimedUDP()
{
	if (UDP_counter < UDP_data.size())
	{
		timedData = UDP_data[UDP_counter] + QByteArray(" "); // Adding a space before the insert of msg number for debugging.
		QByteArray timedDatagram = timedData + QByteArray::number(UDP_counter + 1); // Reformatted the data before sending.
		udpSocket->write(timedDatagram.data(), timedDatagram.size());
		orgDatagram->append(tr(timedDatagram.data()));
		UDP_counter++;
	}
	else
	{
		QByteArray endMsg = "End of data transmission.";
		udpSocket->write(endMsg.data(), endMsg.size());

		UDP_timer->stop();
		orgDatagram->append(endMsg.data());
		timedTxUDP->setEnabled(true);
	}
}
