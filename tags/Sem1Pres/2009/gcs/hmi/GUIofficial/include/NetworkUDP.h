#ifndef UDP_TRANSMITTING_H
#define UDP_TRANSMITTING_H

/**************************************************/
/* ********** START OF THE HEADER FILE ********** */
/**************************************************/
#include "ui_GUIofficial.h"
#include "ui_UDPconnection.h"
#include "UDPconnection.h"
#include "hmi_packet.h"
#include "ap_packet.h"
#include "ahns_logger.h"
#include "ApControl.h"

#include <../QtNetwork/QUdpSocket>
#include <QString>
#include <QTimer>

struct apCommand
{
	uint8_t  ap_CMD;
	uint8_t  ap_X;
	uint8_t  ap_Y;
	uint16_t ap_ALT;
};

/** 
 * Structure to receive Autopilot packets.
 * Needs to modify according to defined ICD
 */
struct receiverPacket
{
	uint8_t time_HI;	// Defining high time byte
	uint8_t time_MH;	// Defining medium high time byte
	uint8_t time_ML;	// Defining medium low time byte
	uint8_t time_LO;	// Defining low time byte
	uint8_t joyX_HI;
	uint8_t joyX_LO;
	uint8_t joyY_HI;
	uint8_t joyY_LO;
	uint8_t joyZ_HI;
	uint8_t joyZ_LO;
	uint8_t ap_CMD;		// AP Mode: Auto & Stability Augmented
	uint8_t ap_X;
	uint8_t ap_Y;
	uint8_t ap_ALT_HI;
	uint8_t ap_ALT_LO;
};

struct receiverPacket_AP
{
	uint8_t timestamp_HI;
	uint8_t timestamp_MH;
	uint8_t timestamp_ML;
	uint8_t timestamp_LO;
	
	uint8_t altCMD_HI;
	uint8_t altCMD_MH;
	uint8_t altCMD_ML;
	uint8_t altCMD_LO;
	
	uint8_t altTRU_HI;
	uint8_t altTRU_MH;
	uint8_t altTRU_ML;
	uint8_t altTRU_LO;
	
	uint8_t altP_HI;
	uint8_t altP_MH;
	uint8_t altP_ML;
	uint8_t altP_LO;
	
	uint8_t altI_HI;
	uint8_t altI_MH;
	uint8_t altI_ML;
	uint8_t altI_LO;
	
	uint8_t altD_HI;
	uint8_t altD_MH;
	uint8_t altD_ML;
	uint8_t altD_LO;
	
	uint8_t altPID_HI;
	uint8_t altPID_MH;
	uint8_t altPID_ML;
	uint8_t altPID_LO;

	uint8_t batt_lvl_HI;
	uint8_t batt_lvl_LO;

	//uint8_t tgStatus;
};

/* *************************************** */
/* ********** TRANSMITTER CLASS ********** */
/* *************************************** */
class UDPclass_tx : public QMainWindow, private Ui::GUIofficial_MASTER
{
	Q_OBJECT
	
public:

	UDPclass_tx(QWidget* parent = 0);
	~UDPclass_tx();


public slots:

	/* ******** UDP CONNECTION FUNCTIONS ******** */
	void udpTxConnection(QString qszIP, QString qszPort);
	
	/* ******** VARIABLE ACCESS FUNCTIONS ******** */
	short getTxSendCounter();
	joystickPacket getJoyData(/*joystickPacket origPack*/);
	const char* getTxPacket();
	void setConnectionInfo(buffer_connInfo connInfo);

	void transmitData(const char* ptrTxPacket);
	void formatData(joystickPacket jsVal, apCommand apPos);

private:

	int txSendCounter;
	uint8_t txPacket[HMI_BUFFSIZE];
	QUdpSocket *txSocket;
	buffer_connInfo udpConnInfo;
	joystickPacket jsSendStruct;
	APclass AP_Core;
};


/****************************************/
/* ********** RECEIVER CLASS ********** */
/****************************************/
class UDPclass_rx : public QMainWindow, private Ui::GUIofficial_MASTER
{
	Q_OBJECT

public:

	UDPclass_rx(QWidget* parent = 0);
	~UDPclass_rx();

	void udpRxConnection(QString qszPort);
	void udpRxReadData();
	void udpRxProcessData();

	QUdpSocket* get_udpRxSocket();
	//receiverPacket get_udpDataPacket();
	receiverPacket_AP get_udpDataPacket();

signals:
	void signal_dispData();

private slots:
	void udpRxProcessPendingData();


private:

	QUdpSocket *rxSocket;
	buffer_connInfo connInfo;
	
	/* ****** Temporary Variables ****** */
	receiverPacket_AP dataPacket;
	//receiverPacket dataPacket;
	uint8_t buffer[AP_PACKSIZE];
	//uint8_t buffer[HMI_PACKSIZE];
	char* data;
	unsigned short buffer_index;
	unsigned short state;	

};


/************************************************/
/* ********** END OF THE HEADER FILE ********** */
/************************************************/
#endif
