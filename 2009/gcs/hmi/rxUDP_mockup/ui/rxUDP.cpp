#include "../../reuse/AP_HMI.h"
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
	udpSocket = new QUdpSocket(this);
	udpSocket->bind(45454);
	state = S1;
	buffer_index = 0;
	data = new char;
	//*buffer = new uint8_t;
	//*dataPacket = new uint8_t;
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
	
}

rxUDP::~rxUDP()
{
	//delete data;
	//delete buffer;
	//delete dataPacket;
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
void rxUDP::processPendingDatagrams()
{
	while (udpSocket->hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(udpSocket->pendingDatagramSize());
		udpSocket->readDatagram(datagram.data(), datagram.size());
		//statusLabel->setText(tr("Received datagram: \"%1\"").arg(datagram.data()));

		//udpSocket->readDatagram(data, udpSocket->pendingDatagramSize());
		//cout << (char)*data << endl;
		//statusLabel->append(QString(tr(datagram.data())));
		//statusLabel->append(QString(tr("1")));
		//data = datagram.data();
		data = datagram.data();
		readData();
	}
}

void rxUDP::readData()
{
	unsigned short prcData = (unsigned short)*data & 0xFF;
	//statusLabel->append(QString(prcData));

	switch (state)
	{
		case S1:
			if (prcData == SYNC_1)		state = S2;
			else				state = S1;
			//cout << "S1" << endl;
		break;
		case S2:
			if (prcData == SYNC_2)		state = S3;
			else				state = S1;
			//cout << "S2" << endl;
		break;
		case S3:
			if (prcData == SYNC_3)		state = S4;
			else				state = S1;
			//cout << "S3" << endl;
		break;
		case S4:
			if (prcData == SYNC_4)		state = S5;
			else				state = S1;
			//cout << "S4" << endl;
		break;
		case S5: // Receiving data			
			if (buffer_index < PACKSIZE)
			{
				//cout << "S5          " << buffer_index << endl;
				buffer[buffer_index] = prcData;
				buffer_index++;
				state = S5;
				if (buffer_index == PACKSIZE)
				{
					buffer_index = 0;
					state = S6;
				}
			}
			/*else
			{
				//processData();
				cout << "Go S6 " << buffer_index << endl;
			}*/
			//cout << "S5 " << buffer_index << endl;
		break;
		case S6:
			if (prcData == SYNC_1)		state = S7;
			else
			{
				buffer_index = 0;
				state = S1;
			}
			//cout << "S6" << endl;
		break;
		case S7:
			if (prcData == SYNC_2)		state = S8;
			else
			{
				buffer_index = 0;
				state = S1;
			}
			//cout << "S7" << endl;
		break;
		case S8:
			if (prcData == SYNC_3)		state = S9;
			else
			{
				buffer_index = 0;
				state = S1;
			}
			//cout << "S8" << endl;
		break;
		case S9:
			if (prcData == SYNC_4)
			{
				processData();
				state = S5;
			}
			else
			{
				buffer_index = 0;
				state = S1;
			}
			//cout << "S9" << endl;
		break;
	}

}

void rxUDP::processData()
{
	/* ******** JOYSTICK POSITION ******** */
	dataPacket[0] = buffer[0];	// X position HIGH
	dataPacket[1] = buffer[1];	// X position LOW
	dataPacket[2] = buffer[2];	// Y position HIGH
	dataPacket[3] = buffer[3];	// Y position LOW
	dataPacket[4] = buffer[4];	// Z position HIGH
	dataPacket[5] = buffer[5];	// Z position LOW

	QString temp;
	cout << "Summed: ";
	for (int ii = 0; ii < 6; ii++)
	{
		if (((ii+1)%2) == 0)
		{
			cout << (unsigned short)((dataPacket[ii-1] << 8) | dataPacket[ii]) << " ";
			temp = QString("%1").arg((dataPacket[ii-1] << 8) | dataPacket[ii]);
			statusLabel->append(temp);
			
		}
	}
	cout << endl;
}










