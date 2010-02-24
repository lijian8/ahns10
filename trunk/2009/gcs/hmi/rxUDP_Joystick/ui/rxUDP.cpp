
#include "rxUDP.h"

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

rxUDP::rxUDP(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);

	// UDP CONNECTIONS
	// Receiver Connection
	udpTxtSocket = new QUdpSocket(this);
	udpTxtSocket->bind(45454);
        connect(udpTxtSocket, SIGNAL(readyRead()), this, SLOT(processTxtPendingDatagrams()));

	udpJsSocket = new QUdpSocket(this);
	udpJsSocket->bind(45455);
	connect(udpJsSocket, SIGNAL(readyRead()), this, SLOT(processJsPendingDatagrams()));
	
}

void rxUDP::on_dispClear_clicked()
{
	statusLabel->clear();
}

void rxUDP::on_closeBtn_clicked()
{
	close();
}

// ================= UDP TRANSFER FUNCTIONS ================== //
void rxUDP::processTxtPendingDatagrams()
{
	while (udpTxtSocket->hasPendingDatagrams())
	{
		QByteArray txtDatagram;
		txtDatagram.resize(udpTxtSocket->pendingDatagramSize());
		udpTxtSocket->readDatagram(txtDatagram.data(), txtDatagram.size());
		statusLabel->append(QString(tr(txtDatagram.data())));
	}
}

void rxUDP::processJsPendingDatagrams()
{
	while (udpJsSocket->hasPendingDatagrams())
	{
		float jsDatagram;
		udpJsSocket->readDatagram((char*)&jsDatagram, sizeof(float));
		jsRxDisp->display(jsDatagram);
	}
}
