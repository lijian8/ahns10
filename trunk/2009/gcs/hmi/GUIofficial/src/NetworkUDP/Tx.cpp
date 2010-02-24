#include "include/GUIofficial.h"
#include "NetworkUDP.h"
#include "UDPconnection.h"
#include "ahns_logger.h"

#include <QString>
#include <QTimer>
#include <QByteArray>
#include <../QtNetwork/QUdpSocket>
#include <QTextEdit>
#include <QLineEdit>

#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

UDPclass_tx::UDPclass_tx(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);
	
	txSendCounter = 0;
	txSocket = new QUdpSocket(this);
}

UDPclass_tx::~UDPclass_tx()
{
	delete txSocket;
}


/* ************************************** */
/* ********** PUBLIC FUNCTIONS ********** */
/* ************************************** */
/* ******************************** */
/* ******* SETUP CONNECTION ******* */
/* ******************************** */
void UDPclass_tx::udpTxConnection(QString qszIP, QString qszPort)
{
	AHNS_DEBUG("UDPclass_tx::udpTxConnection(QString qszIP, QString qszPort)");
	AHNS_DEBUG("UDPclass_tx::udpTxConnection(QString qszIP, QString qszPort):   " << qszPort.toUShort());
	txSocket->disconnectFromHost();
	txSocket->connectToHost(qszIP, qszPort.toUShort());
}

/* ****************************************** */
/* ******* DEFINE TRANSMISSION FORMAT ******* */
/* ****************************************** */
void UDPclass_tx::formatData(joystickPacket jsVal, apCommand apPos)
{
	AHNS_DEBUG("UDPclass_tx::formatData(short jsX, short jsY, short jsZ)");
	
	int ii = 0;
	QTime timeStamp;
	int sendTS = timeStamp.currentTime().hour()*3.6e6 + timeStamp.currentTime().minute()*6.0e4 + timeStamp.currentTime().second()*1.0e3 + timeStamp.currentTime().msec();
	sendTS &= 0xEFFFFFFF; 				// Making sure that it is only 32 bytes (with MSB = 0)
	
	/* ******** FORMAT SYNC BYTES ******** */
	txPacket[ii]   = HMI_SYNC_1;
	txPacket[++ii] = HMI_SYNC_2;
	txPacket[++ii] = HMI_SYNC_3;
	txPacket[++ii] = HMI_SYNC_4;
	
	/* ******** FORMAT TIMESTAMP BYTES ******** */
	txPacket[++ii] = (sendTS & 0x7F000000) >> 24;	// High Timestamp Bytes
	txPacket[++ii] = (sendTS & 0x00FF0000) >> 16;	// MHigh Timestamp Bytes
	txPacket[++ii] = (sendTS & 0x0000FF00) >> 8;	// MLow Timestamp Bytes
	txPacket[++ii] = sendTS & 0x000000FF;		// Low Timestamp Bytes
	
	/* ******** FORMAT JOYSTICK BYTES ******** */
	txPacket[++ii] = (jsVal.joy_X & 0xFF00) >> 8; 	// High X bytes
	txPacket[++ii] = jsVal.joy_X & 0x00FF;
	txPacket[++ii] = (jsVal.joy_Y & 0xFF00) >> 8;
	txPacket[++ii] = jsVal.joy_Y & 0x00FF;
	txPacket[++ii] = (jsVal.joy_Z & 0xFF00) >> 8;
	txPacket[++ii] = jsVal.joy_Z & 0x00FF;

	/* ******** FORMAT AUTOPILOT COMMAND BYTES ******** */
	txPacket[++ii] = apPos.ap_CMD;
	txPacket[++ii] = (apPos.ap_X*200)/255;
	txPacket[++ii] = (apPos.ap_Y*200)/255;
	txPacket[++ii] = (apPos.ap_ALT & 0xFF00) >> 8; // Sending HI alt byte
	txPacket[++ii] = apPos.ap_ALT & 0x00FF;	// Sending LO alt byte
}

/* ************************************** */
/* ******* DEFINE JOYSTICK VALUES ******* */
/* ************************************** */
joystickPacket UDPclass_tx::getJoyData(/*joystickPacket origPack*/)
{
	return AP_Core.get_joystickPosition();
}

/* ********************************** */
/* ******* TRANSMIT DATAGRAMS ******* */
/* ********************************** */
void UDPclass_tx::transmitData(const char* ptrTxPacket)
{
	txSocket->write(ptrTxPacket + txSendCounter, 1);
	txSendCounter++;
}

/* ******************************************************* */
/* ********** PRIVATE VARIABLE ACCESS FUNCTIONS ********** */
/* ******************************************************* */
const char* UDPclass_tx::getTxPacket()
{
	txSendCounter = 0;
	return (char*)txPacket;
}

short UDPclass_tx::getTxSendCounter()
{
	return txSendCounter;
}

void UDPclass_tx::setConnectionInfo(buffer_connInfo connInfo)
{
	udpConnInfo = connInfo;
}

/* ********************************************** */
/* ********** GENERAL ACCESS FUNCTIONS ********** */
/* ********************************************** */
void GUIofficial::UdpTx_SendData()
{
	AHNS_DEBUG("GUIofficial::UdpTx_SendData()");
	AHNS_DEBUG("UdpTx_SendData():: udp_JS = udpTxBlock.getJoyData()  " << udp_JS.joy_X);
	if (isUsingJoy)
	{
		udp_JS = AP_Core.get_joystickPosition();
		
		ap_dispJoyAlt_SA->display(udp_JS.joy_Z/100);
		/* ******** CHECK LATERAL CONTROL CONDITIONS ******** */
		if (((udp_JS.joy_X/50) - 100) < 0)	ap_dispJoyLat_SA->display(QString("%1%2").arg("R").arg(-((udp_JS.joy_X/50) - 100)));
		else					ap_dispJoyLat_SA->display(QString("%1%2").arg("L").arg((udp_JS.joy_X/50) - 100));
		ap_dispJoyLong_SA->display((udp_JS.joy_Y/50) - 100);
		
		db_animateAH();
	}
	else		udp_JS = get_controlPadPosition();
	udpTxBlock.formatData(udp_JS, cmdPos);
	const char* ptrTxPacket = udpTxBlock.getTxPacket();
	for (int ii=0; ii<HMI_BUFFSIZE; ii++)
	{
		udpTxBlock.transmitData(ptrTxPacket);
	}
	
}
