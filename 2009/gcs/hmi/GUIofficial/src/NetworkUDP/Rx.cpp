#include "include/GUIofficial.h"
#include "NetworkUDP.h"


#include <QString>
#include <QTime>
#include <QByteArray>
#include <../QtNetwork/QUdpSocket>

#include <iostream>
#include <iomanip>

using namespace std;

UDPclass_rx::UDPclass_rx(QWidget* parent)
: QMainWindow(parent)
{
	rxSocket = new QUdpSocket(this);
	connect(rxSocket, SIGNAL(readyRead()), this, SLOT(udpRxProcessPendingData()));
	state = AP_STATE_1;
	buffer_index = 0;
}

UDPclass_rx::~UDPclass_rx()
{
	delete rxSocket;
}

/* *************************************** */
/* ********** PRIVATE FUNCTIONS ********** */
/* *************************************** */
void UDPclass_rx::udpRxProcessPendingData()
{
	while (rxSocket->hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(rxSocket->pendingDatagramSize());
		rxSocket->readDatagram(datagram.data(), datagram.size());
		data = datagram.data();
		udpRxReadData();
	}
}

void UDPclass_rx::udpRxReadData()
{
	AHNS_DEBUG("UDPclass_rx::udpRxReadData()");
	
	unsigned short prcData = (unsigned short)*data & 0xFF;	// Ensure only LOW byte is used
	AHNS_DEBUG("udpRxReadData(): Process prcData: " << prcData);
	AHNS_DEBUG("udpRxReadData(): State: " << state);

	switch (state)
	{
		case AP_STATE_1:
			if (prcData == AP_SYNC_1)	state = AP_STATE_2;
			else				state = AP_STATE_1;
			break;
		case AP_STATE_2:
			if (prcData == AP_SYNC_2)	state = AP_STATE_3;
			else if (prcData == AP_SYNC_1)	state = AP_STATE_2;
			else				state = AP_STATE_1;
			break;
		case AP_STATE_3:
			if (prcData == AP_SYNC_3)	state = AP_STATE_4;
			else				state = AP_STATE_1;
			break;
		case AP_STATE_4:
			if (prcData == AP_SYNC_4)	state = AP_STATE_5;
			else				state = AP_STATE_1;
			break;
		case AP_STATE_5: // Receiving data			
			if (buffer_index < AP_PACKSIZE)
			//if (buffer_index < HMI_PACKSIZE)
			{
				buffer[buffer_index] = prcData;
				buffer_index++;
				state = AP_STATE_5;
				if (buffer_index == AP_PACKSIZE)
				//if (buffer_index == HMI_PACKSIZE)
				{
					buffer_index = 0;
					state = AP_STATE_6;
				}
			}
			AHNS_DEBUG("udpRxReadData(): case AP_STATE_5");
			break;
		case AP_STATE_6:
			if (prcData == AP_SYNC_1)		state = AP_STATE_7;
			else
			{
				buffer_index = 0;
				state = AP_STATE_1;
			}
			AHNS_DEBUG("udpRxReadData(): case AP_STATE_6");
			break;
		case AP_STATE_7:
			if (prcData == AP_SYNC_2)		state = AP_STATE_8;
			else
			{
				buffer_index = 0;
				state = AP_STATE_1;
			}
			AHNS_DEBUG("udpRxReadData(): case AP_STATE_7");
			break;
		case AP_STATE_8:
			if (prcData == AP_SYNC_3)	
			{
				AHNS_DEBUG("udpRxReadData(): case AP_STATE_9 - " << prcData);
				state = AP_STATE_9;
			}
			else
			{
				buffer_index = 0;
				state = AP_STATE_1;
			}
			AHNS_DEBUG("udpRxReadData(): case AP_STATE_8");
			break;
		case AP_STATE_9:
			if (prcData == AP_SYNC_4)
			{
				AHNS_DEBUG("udpRxReadData(): case AP_STATE_9 - before udpRxProcessData()");
				udpRxProcessData();
				AHNS_DEBUG("udpRxReadData(): case AP_STATE_9 - after udpRxProcessData()");
				state = AP_STATE_5;
			}
			else
			{
				buffer_index = 0;
				state = AP_STATE_1;
			}
			AHNS_DEBUG("udpRxReadData(): case AP_STATE_9");
			break;
		default:
			break;
	cerr << "out" << endl;
	}
}

void UDPclass_rx::udpRxProcessData()
{
	/* ******** JOYSTICK POSITION ******** */
	AHNS_DEBUG("UDPclass_rx::udpRxProcessData()");
	memcpy(&dataPacket, buffer, AP_PACKSIZE);
	//memcpy(&dataPacket, buffer, HMI_PACKSIZE);
	AHNS_DEBUG("udpRxProcessData(): memcpy(&dataPacket, buffer, AP_PACKSIZE)");
	emit signal_dispData();
	AHNS_DEBUG("udpRxProcessData(): emit signal");
}

/* ********************************************* */
/* ********** PUBLIC ACCESS FUNCTIONS ********** */
/* ********************************************* */
void UDPclass_rx::udpRxConnection(QString qszPort)
{
	AHNS_DEBUG("UDPclass_rx::udpRxConnection(QString qszPort)");
	rxSocket->disconnectFromHost();
	if (!qszPort.isEmpty())	rxSocket->bind(qszPort.toUShort());
	else			AHNS_DEBUG("UDPclass_rx::udpRxConnection(QString qszPort):  is empty");
}

//receiverPacket UDPclass_rx::get_udpDataPacket()
receiverPacket_AP UDPclass_rx::get_udpDataPacket()
{
	return dataPacket;
}

QUdpSocket* UDPclass_rx::get_udpRxSocket()
{
	return rxSocket;
}

/* ********************************************** */
/* ********** GENERAL ACCESS FUNCTIONS ********** */
/* ********************************************** */
void GUIofficial::on_dbUDP_btnSend_clicked()
{
	if ((!connInfo.buf_txIP.isEmpty()) && (!connInfo.buf_txPort.isEmpty()) && (!connInfo.buf_rxPort.isEmpty()))
	{
		//udpTxBlock.formatData(7000, 664, 185, cmdPos);
		AHNS_DEBUG("on_dbUDP_btnSend_clicked():: udpTxBlock.formatData(7000, 664, 185)");
		udpTxTimer->start(40);
		AHNS_DEBUG("on_dbUDP_btnSend_clicked():: udpTxTimer->start(100)");
	}
	else	AHNS_DEBUG("on_dbUDP_btnSend_clicked():: connInfo is empty");
}

void GUIofficial::on_dbUDP_btnStop_clicked()
{
	udpTxTimer->stop();
}

void GUIofficial::on_dbUDP_btnClear_clicked()
{
	db_Disp2->clear();
}

void GUIofficial::UdpRx_DisplayData()
{
	AHNS_DEBUG("GUIofficial::UdpRx_DisplayData()");
	
	if (cb_enableRx->isChecked()) // Enable and disable UDP receiving
	{
		//receiverPacket formatPacket = udpRxBlock->get_udpDataPacket();
		receiverPacket_AP formatPacket = udpRxBlock->get_udpDataPacket();

		/* ******** CONVERTING TIME TO HH:MM:SS.MS ******** */
		int tempTime = (formatPacket.timestamp_HI << 24) | (formatPacket.timestamp_MH << 16) | (formatPacket.timestamp_ML << 8) | (formatPacket.timestamp_LO);
		short tempTime_hrs  = tempTime/3600000;
		short tempTime_mins = (tempTime - (tempTime_hrs*3600000))/60000;
		short tempTime_secs = (tempTime - (tempTime_hrs*3600000) - (tempTime_mins*60000))/1000;
		short tempTime_msec = tempTime - (tempTime_hrs*3600000) - (tempTime_mins*60000) - (tempTime_secs*1000);
		char timeBuffer[16];
		sprintf(timeBuffer, "%02i:%02i:%02i.%03i", tempTime_hrs, tempTime_mins, tempTime_secs, tempTime_msec);
		db_LCDdisp->display(QString(timeBuffer));
		
		ann_procBatt((int)(formatPacket.batt_lvl_HI << 8 | formatPacket.batt_lvl_LO));
		if (network_firstRun)
		{
			reScale = 0;
			count = 0;
			altClear = false;
			vec_plotAlt.clear();
			vec_plotAlt_PID.clear();
			pl_alt->setAxisScale(2, 0, 10); // X Axis ID: 2
			pl_alt_PID->setAxisScale(2, 0, 10); // X Axis ID: 2
			pl_alt_histBar->setMaximum(50);
			pl_alt_histBar->setValue(50);
			pl_altPID_histBar->setMaximum(50);
			pl_altPID_histBar->setValue(50);
		}
		network_firstRun = false;
		plots_updatePlot(formatPacket);
		pbAlt_updateAltBar((int)((formatPacket.altTRU_HI << (3*8)) | (formatPacket.altTRU_MH << (2*8)) | (formatPacket.altTRU_ML << (1*8)) | (formatPacket.altTRU_LO << (0*8))));
		//pbAlt_updateAltBar(((int)((formatPacket.altCMD_HI << (3*8)) | (formatPacket.altCMD_MH << (2*8)) | (formatPacket.altCMD_ML << (1*8)) | (formatPacket.altCMD_LO << (0*8)))) + 1); // For debugging
		
		// receiverPacket_AP algorithm ********************************************************************************************
		QString tempDisp[8];
		tempDisp[0] = QString("Time %1").arg(timeBuffer);
		tempDisp[1] = QString("AltCMD %1").arg((formatPacket.altCMD_HI << (3*8)) | (formatPacket.altCMD_MH << (2*8)) | (formatPacket.altCMD_ML << (1*8)) | (formatPacket.altCMD_LO << (0*8)));
		tempDisp[2] = QString("AltTRU %1").arg((formatPacket.altTRU_HI << (3*8)) | (formatPacket.altTRU_MH << (2*8)) | (formatPacket.altTRU_ML << (1*8)) | (formatPacket.altTRU_LO << (0*8)));
		tempDisp[3] = QString("Alt P %1").arg(((formatPacket.altP_HI << (3*8)) | (formatPacket.altP_MH << (2*8)) | (formatPacket.altP_ML << (1*8)) | (formatPacket.altP_LO << (0*8)))/100);
		tempDisp[4] = QString("Alt I %1").arg(((formatPacket.altI_HI << (3*8)) | (formatPacket.altI_MH << (2*8)) | (formatPacket.altI_ML << (1*8)) | (formatPacket.altI_LO << (0*8)))/100);
		tempDisp[5] = QString("Alt D %1").arg(((formatPacket.altD_HI << (3*8)) | (formatPacket.altD_MH << (2*8)) | (formatPacket.altD_ML << (1*8)) | (formatPacket.altD_LO << (0*8)))/100);
		tempDisp[6] = QString("Alt PID %1").arg(((formatPacket.altPID_HI << (3*8)) | (formatPacket.altPID_MH << (2*8)) | (formatPacket.altPID_ML << (1*8)) | (formatPacket.altPID_LO << (0*8)))/100);
		tempDisp[7] = QString("Batt %1").arg(formatPacket.batt_lvl_HI << 8 | formatPacket.batt_lvl_LO);
		//tempDisp[7] = QString("Telemetry & GP Status: %1").arg(formatPacket.tgStatus, 0, 16).toUpper();
	
		for (int ii=0; ii<8; ii++)	db_Disp2->append(tempDisp[ii]);
	}

	// receiverPacket algorithm ***********************************************************************************************
	/*QString tempDisp[7];
	AHNS_DEBUG("UdpRx_DisplayData():: formatPacket " << (short)formatPacket.ap_CMD);
	AHNS_DEBUG("UdpRx_DisplayData():: formatPacket " << (short)formatPacket.ap_X);
	AHNS_DEBUG("UdpRx_DisplayData():: formatPacket " << (short)formatPacket.ap_Y);
	AHNS_DEBUG("UdpRx_DisplayData():: packetInfo formatPacket = udpRxBlock->get_udpDataPacket();");

	AHNS_DEBUG("UdpRx_DisplayData():: Before QString formatting to the first *tempDisp");
	QString qstrTime = currentTimestamp.currentTime().toString("HH:mm:ss");
	AHNS_DEBUG("UdpRx_DisplayData():: After QString formatting to the first *tempDisp");
	tempDisp[0] = QString("%1$  %2").arg(qstrTime).arg((formatPacket.joyX_HI << 8) | formatPacket.joyX_LO);
	AHNS_DEBUG("UdpRx_DisplayData():: tempDisp[0]");
	tempDisp[1] = QString("%1$  %2").arg(qstrTime).arg((formatPacket.joyY_HI << 8) | formatPacket.joyY_LO);
	AHNS_DEBUG("UdpRx_DisplayData():: tempDisp[1]");
	tempDisp[2] = QString("%1$  %2").arg(qstrTime).arg((formatPacket.joyZ_HI << 8) | formatPacket.joyZ_LO);
	AHNS_DEBUG("UdpRx_DisplayData():: tempDisp[2]");
	tempDisp[3] = QString("%1$  CMD: %2").arg(qstrTime).arg(formatPacket.ap_CMD, 0, 16);
	tempDisp[3] = tempDisp[3].toUpper();
	AHNS_DEBUG("UdpRx_DisplayData():: tempDisp[3]");
	tempDisp[4] = QString("%1$  X: %2").arg(qstrTime).arg(formatPacket.ap_X);
	AHNS_DEBUG("UdpRx_DisplayData():: tempDisp[4]");
	tempDisp[5] = QString("%1$  Y: %2").arg(qstrTime).arg(formatPacket.ap_Y);
	AHNS_DEBUG("UdpRx_DisplayData():: After QString formatting to *tempDisp");
	tempDisp[6] = QString("%1$  %2").arg(qstrTime).arg((formatPacket.ap_ALT_HI << 8) | formatPacket.ap_ALT_LO);
	AHNS_DEBUG("UdpRx_DisplayData():: After formatting display for ap_ALT");
	
	for (int ii=0; ii<7; ii++)	db_Disp2->append(tempDisp[ii]);*/
	// ************************************************************************************************************************
	
	
}
