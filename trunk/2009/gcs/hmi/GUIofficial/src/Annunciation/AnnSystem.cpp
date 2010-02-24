#include "include/GUIofficial.h"
#include "NetworkUDP.h"
#include "AnnSystem.h"
#include "ahns_logger.h"

#include <QString>
#include <QTimer>
#include <QByteArray>
#include <QColor>
#include <QSound>

#include <iostream>
#include <iomanip>
#include <cstring>
#include <unistd.h>

#include <cstdio>
#include <errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

using namespace std;

ANNclass::ANNclass(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);
}

ANNclass::~ANNclass()
{
}

/* ********************************************** */
/* ********** GENERAL ACCESS FUNCTIONS ********** */
/* ********************************************** */
void GUIofficial::Annunciation_Init()
{
	//ann_flashTimer_Telemetry = new QTimer(this);
	//ann_flashTimer_GrdPulse = new QTimer(this);
	//ann_flashTimer_AltMax = new QTimer(this);
	//ann_flashTimer_AltMin = new QTimer(this);
	ann_flashTimer_BattStat = new QTimer(this);
	ann_flashTimer_FltMode = new QTimer(this);
	ann_flashTimer_LatBound = new QTimer(this);
	//ann_flashTimer_SinkRate = new QTimer(this);
	ann_flashTimer_Tmr1Min = new QTimer(this);
	ann_flashTimer_Tmr2Min = new QTimer(this);
	ann_flashTimer_Tmr30Sec = new QTimer(this);
	
	tmr1MinIndx = 0;
	tmr2MinIndx = 0;
	tmr30SecIndx = 0;
	tmrFltModeIndx = 0;
	tmrBattStatIndx = 0;
	
	pbBattLevel = new QPalette(ann_pbBattLevel->palette());
	//lbTel = new QPalette(ann_lbTelemetry->palette());
	//lbGrdPulse = new QPalette(ann_lbGrdPulse->palette());
	btnAltMax = new QPalette(ann_btnAltMax->palette());
	btnAltMin = new QPalette(ann_btnAltMin->palette());
	btnBattStat = new QPalette(ann_btnBattStat->palette());
	btnFltMode = new QPalette(ann_btnFltMode->palette());
	btnLatBound = new QPalette(ann_btnLatBound->palette());
	btnAltSafe = new QPalette(ann_btnAltSafe->palette());
	btnTmr1Min = new QPalette(ann_btnTmr1Min->palette());
	btnTmr2Min = new QPalette(ann_btnTmr2Min->palette());
	btnTmr30Sec = new QPalette(ann_btnTmr30Sec->palette());
	
        //lbTel->setColor(QPalette::WindowText, ANN_RED);
        //lbGrdPulse->setColor(QPalette::WindowText, ANN_RED);
	btnAltMax->setColor(QPalette::ButtonText, ANN_BLACK);
	btnAltMin->setColor(QPalette::ButtonText, ANN_BLACK);
	btnBattStat->setColor(QPalette::ButtonText, ANN_BLACK);
	btnFltMode->setColor(QPalette::ButtonText, ANN_BLACK);
	btnLatBound->setColor(QPalette::ButtonText, ANN_BLACK);
	btnAltSafe->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr1Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr2Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr30Sec->setColor(QPalette::ButtonText, ANN_BLACK);

        //ann_lbTelemetry->setPalette(*lbTel);
        //ann_lbGrdPulse->setPalette(*lbGrdPulse);
	ann_btnAltMax->setPalette(*btnAltMax);
	ann_btnAltMin->setPalette(*btnAltMin);
	ann_btnBattStat->setPalette(*btnBattStat);
	ann_btnFltMode->setPalette(*btnFltMode);
	ann_btnLatBound->setPalette(*btnLatBound);
	ann_btnAltSafe->setPalette(*btnAltSafe);
	ann_btnTmr1Min->setPalette(*btnTmr1Min);
	ann_btnTmr2Min->setPalette(*btnTmr2Min);
	ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
        
	connect(ann_btnTest, SIGNAL(pressed()), this, SLOT(ann_TestON()));
	connect(ann_btnTest, SIGNAL(released()), this, SLOT(ann_TestOFF()));
	//connect(ann_flashTimer_Telemetry, SIGNAL(timeout()), this, SLOT(ann_procTelem()));
	//connect(ann_flashTimer_GrdPulse, SIGNAL(timeout()), this, SLOT(ann_procGrdPulse()));
	//connect(ann_flashTimer_AltMax, SIGNAL(timeout()), this, SLOT(ann_procAltMax()));
	//connect(ann_flashTimer_AltMin, SIGNAL(timeout()), this, SLOT(ann_procAltMin()));
	connect(ann_flashTimer_BattStat, SIGNAL(timeout()), this, SLOT(ann_procBattStat()));
	connect(ann_flashTimer_FltMode, SIGNAL(timeout()), this, SLOT(ann_procFltMode()));
	connect(ann_flashTimer_LatBound, SIGNAL(timeout()), this, SLOT(ann_procLatBound()));
	//connect(ann_flashTimer_SinkRate, SIGNAL(timeout()), this, SLOT(ann_procSinkRate()));
	connect(ann_flashTimer_Tmr1Min, SIGNAL(timeout()), this, SLOT(ann_procTmr1Min()));
	connect(ann_flashTimer_Tmr2Min, SIGNAL(timeout()), this, SLOT(ann_procTmr2Min()));
	connect(ann_flashTimer_Tmr30Sec, SIGNAL(timeout()), this, SLOT(ann_procTmr30Sec()));
	
	/* ******** DETERMINE THE STATUS OF THE BATTERY BAR ******** */
	if ((ann_pbBattLevel->value() <= 100) && (ann_pbBattLevel->value() >= 25))
	{
		ann_flashTimer_BattStat->stop();
		ann_btnBattStat->setText(tr("NORM BAT"));
		ann_flashTimer_BattStat->start(500);
		btnBattStat->setColor(QPalette::ButtonText, ANN_GREEN);
		pbBattLevel->setColor(QPalette::Highlight, ANN_GREEN);
		firstTime = 0;
	}
	else if ((ann_pbBattLevel->value() < 25) && (ann_pbBattLevel->value() > 10))
	{
		ann_flashTimer_BattStat->stop();
		ann_btnBattStat->setText(tr("LOW BAT"));
		ann_flashTimer_BattStat->start(500);
		btnBattStat->setColor(QPalette::ButtonText, ANN_ORANGE);
		pbBattLevel->setColor(QPalette::Highlight, ANN_ORANGE);
		firstTime = 1;
	}
	else
	{
		ann_flashTimer_BattStat->stop();
		ann_btnBattStat->setText(tr("CRIT BAT"));
		ann_flashTimer_BattStat->start(500);
		btnBattStat->setColor(QPalette::ButtonText, ANN_RED);
		pbBattLevel->setColor(QPalette::Highlight, ANN_RED);
		firstTime = 2;
	}
	
	ann_btnBattStat->setPalette(*btnBattStat);
	ann_pbBattLevel->setPalette(*pbBattLevel);
}

void GUIofficial::Annunciation_Exit()
{
	//delete ann_flashTimer_Telemetry;
	//delete ann_flashTimer_GrdPulse;
	//delete ann_flashTimer_AltMax;
	//delete ann_flashTimer_AltMin;
	delete ann_flashTimer_BattStat;
	delete ann_flashTimer_FltMode;
	delete ann_flashTimer_LatBound;
	//delete ann_flashTimer_SinkRate;
	delete ann_flashTimer_Tmr1Min;
	delete ann_flashTimer_Tmr2Min;
	delete ann_flashTimer_Tmr30Sec;
	delete pbBattLevel;
	//delete lbTel;
	//delete lbGrdPulse;
	delete btnAltMax;
	delete btnAltMin;
	delete btnBattStat;
	delete btnFltMode;
	delete btnLatBound;
	//delete btnAltSafe;
	delete btnTmr1Min;
	delete btnTmr2Min;
	delete btnTmr30Sec;
}

void GUIofficial::ann_TestON()
{
        //lbTel->setColor(QPalette::WindowText, ANN_GREEN);
        //lbGrdPulse->setColor(QPalette::WindowText, ANN_GREEN);
	btnAltMax->setColor(QPalette::ButtonText, ANN_RED);
	btnAltMin->setColor(QPalette::ButtonText, ANN_GREEN);
	btnBattStat->setColor(QPalette::ButtonText, ANN_GREEN);
	if ((cmdPos.ap_CMD != AUTO) && (cmdPos.ap_CMD != STAB))	btnFltMode->setColor(QPalette::ButtonText, ANN_GREEN);
	btnLatBound->setColor(QPalette::ButtonText, ANN_GREEN);
	btnAltSafe->setColor(QPalette::ButtonText, ANN_RED);
	btnTmr1Min->setColor(QPalette::ButtonText, ANN_YELLOW);
	btnTmr2Min->setColor(QPalette::ButtonText, ANN_YELLOW);
	btnTmr30Sec->setColor(QPalette::ButtonText, ANN_ORANGE);

        //ann_lbTelemetry->setPalette(*lbTel);
        //ann_lbGrdPulse->setPalette(*lbGrdPulse);
	ann_btnAltMax->setPalette(*btnAltMax);
	ann_btnAltMin->setPalette(*btnAltMin);
	ann_btnBattStat->setPalette(*btnBattStat);
	ann_btnFltMode->setPalette(*btnFltMode);
	ann_btnLatBound->setPalette(*btnLatBound);
	ann_btnAltSafe->setPalette(*btnAltSafe);
	ann_btnTmr1Min->setPalette(*btnTmr1Min);
	ann_btnTmr2Min->setPalette(*btnTmr2Min);
	ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
}

void GUIofficial::ann_TestOFF()
{
        //lbTel->setColor(QPalette::WindowText, ANN_RED);
        //lbGrdPulse->setColor(QPalette::WindowText, ANN_RED);
	btnAltMax->setColor(QPalette::ButtonText, ANN_BLACK);
	btnAltMin->setColor(QPalette::ButtonText, ANN_BLACK);
	btnBattStat->setColor(QPalette::ButtonText, ANN_BLACK);
	//btnFltMode->setColor(QPalette::ButtonText, ANN_BLACK);
	btnLatBound->setColor(QPalette::ButtonText, ANN_BLACK);
	btnAltSafe->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr1Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr2Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr30Sec->setColor(QPalette::ButtonText, ANN_BLACK);

	if ((cmdPos.ap_CMD != AUTO) && (cmdPos.ap_CMD != STAB))	btnFltMode->setColor(QPalette::ButtonText, ANN_BLACK);
	
	/* ******** DETERMINE THE STATUS OF THE BATTERY BAR ******** */
	if ((ann_pbBattLevel->value() <= 100) && (ann_pbBattLevel->value() >= 25))
	{
		ann_btnBattStat->setText(tr("NORM BAT"));
		btnBattStat->setColor(QPalette::ButtonText, ANN_GREEN);
		pbBattLevel->setColor(QPalette::Highlight, ANN_GREEN);
	}
	else if ((ann_pbBattLevel->value() < 25) && (ann_pbBattLevel->value() > 10))
	{
		ann_btnBattStat->setText(tr("LOW BAT"));
		btnBattStat->setColor(QPalette::ButtonText, ANN_ORANGE);
		pbBattLevel->setColor(QPalette::Highlight, ANN_ORANGE);
	}
	else
	{
		ann_btnBattStat->setText(tr("CRIT BAT"));
		btnBattStat->setColor(QPalette::ButtonText, ANN_RED);
		pbBattLevel->setColor(QPalette::Highlight, ANN_RED);
	}

	//ann_lbTelemetry->setPalette(*lbTel);
        //ann_lbGrdPulse->setPalette(*lbGrdPulse);
	ann_btnAltMax->setPalette(*btnAltMax);
	ann_btnAltMin->setPalette(*btnAltMin);
	ann_btnBattStat->setPalette(*btnBattStat);
	ann_btnFltMode->setPalette(*btnFltMode);
	ann_btnLatBound->setPalette(*btnLatBound);
	ann_btnAltSafe->setPalette(*btnAltSafe);
	ann_btnTmr1Min->setPalette(*btnTmr1Min);
	ann_btnTmr2Min->setPalette(*btnTmr2Min);
	ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
}

void GUIofficial::ann_procBatt(int battVal)
{
	//int db_battVal = db_battLevel->value();
	//receivedPacket = udpRxBlock->get_udpDataPacket();
	battVal = 15;
	ann_pbBattLevel->setValue(battVal);
	disp_BattLvl->display(battVal);
	
	if ((ann_pbBattLevel->value() <= 100) && (ann_pbBattLevel->value() >= 25))
	{
		pbBattLevel->setColor(QPalette::Highlight, ANN_GREEN);
		if (firstTime != 0)
		{
			char buffer[8];
			ann_flashTimer_BattStat->start(500);
			sprintf(buffer, "%i", ann_pbBattLevel->value());
			displaySystemMessage_NORMAL(ANNUN_BATT_LVL, buffer);
		}
		firstTime = 0;
	}
	else if ((ann_pbBattLevel->value() < 25) && (ann_pbBattLevel->value() > 10))
	{
		pbBattLevel->setColor(QPalette::Highlight, ANN_ORANGE);
		if (firstTime != 1)
		{
			char buffer[8];
			ann_flashTimer_BattStat->start(500);
			sprintf(buffer, "%i", ann_pbBattLevel->value());
			displaySystemMessage_WARNING(ANNUN_BATT_LOW, buffer);
		}
		firstTime = 1;
	}
	else
	{
		pbBattLevel->setColor(QPalette::Highlight, ANN_RED);
		if (firstTime != 2)
		{
			char buffer[8];
			ann_flashTimer_BattStat->start(500);
			sprintf(buffer, "%i", ann_pbBattLevel->value());
			displaySystemMessage_WARNING(ANNUN_BATT_CRT, buffer);
		}
		firstTime = 2;
	}

	ann_pbBattLevel->setPalette(*pbBattLevel);
}

void GUIofficial::ann_procAltMax(bool flag)
{
	if (flag)
	{
		btnAltMax->setColor(QPalette::ButtonText, ANN_ORANGE);
		//QSound::play("resource/audioSamples/nattend.wav");
	}
	else		btnAltMax->setColor(QPalette::ButtonText, ANN_BLACK);
	ann_btnAltMax->setPalette(*btnAltMax);
}

void GUIofficial::ann_procAltMin(bool flag)
{
	if (flag)
	{
		btnAltMin->setColor(QPalette::ButtonText, ANN_ORANGE);
		//QSound::play("resource/audioSamples/nattend.wav");
	}
	else		btnAltMin->setColor(QPalette::ButtonText, ANN_BLACK);
	ann_btnAltMin->setPalette(*btnAltMin);
}

void GUIofficial::ann_procBattStat()
{
	if (tmrBattStatIndx < 5)
	{
		if ((tmrBattStatIndx % 2) == 0)
		{
			if ((ann_pbBattLevel->value() <= 100) && (ann_pbBattLevel->value() >= 25))
			{
				ann_btnBattStat->setText(tr("NORM BAT"));
				btnBattStat->setColor(QPalette::ButtonText, ANN_GREEN);
				
				//QSound::play("resource/audioSamples/nattend.wav");
			}
			else if ((ann_pbBattLevel->value() < 25) && (ann_pbBattLevel->value() > 10))
			{
				ann_btnBattStat->setText(tr("LOW BAT"));
				btnBattStat->setColor(QPalette::ButtonText, ANN_ORANGE);
				
				//QSound::play("resource/audioSamples/nattend.wav");
			}
			else
			{
				ann_btnBattStat->setText(tr("CRIT BAT"));
				btnBattStat->setColor(QPalette::ButtonText, ANN_RED);
				
				//QSound::play("resource/audioSamples/nattend.wav");
			}	
		}
		else	btnBattStat->setColor(QPalette::ButtonText, ANN_BLACK);
		
		tmrBattStatIndx++;
	}
	else
	{
		ann_flashTimer_BattStat->stop();
		tmrBattStatIndx = 0;
	}
	
	ann_btnBattStat->setPalette(*btnBattStat);
}

void GUIofficial::ann_procFltMode()
{
	if (tmrFltModeIndx < 5)
	{
		if ((tmrFltModeIndx % 2) == 0)
		{
			if (cmdPos.ap_CMD == AUTO)
			{
				ann_btnFltMode->setText(tr("A/P"));
				btnFltMode->setColor(QPalette::ButtonText, ANN_GREEN);
			}
			else if (cmdPos.ap_CMD == STAB)
			{
				ann_btnFltMode->setText(tr("S/AUG"));
				btnFltMode->setColor(QPalette::ButtonText, ANN_ORANGE);
			}
		}
		else	btnFltMode->setColor(QPalette::ButtonText, ANN_BLACK);
		
		tmrFltModeIndx++;
	}
	else
	{
		ann_flashTimer_FltMode->stop();
		tmrFltModeIndx = 0;
	}
	
	ann_btnFltMode->setPalette(*btnFltMode);
}

void GUIofficial::ann_procLatBound()
{
}

void GUIofficial::ann_procAltSafe(bool flag)
{
	if (flag)
	{
		btnAltSafe->setColor(QPalette::ButtonText, ANN_GREEN);
		//QSound::play("resource/audioSamples/nattend.wav");
	}
	else		btnAltSafe->setColor(QPalette::ButtonText, ANN_BLACK);
	
	ann_btnAltSafe->setPalette(*btnAltSafe);
}

void GUIofficial::ann_procTmr1Min()
{
	btnTmr1Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr2Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr30Sec->setColor(QPalette::ButtonText, ANN_BLACK);
	ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
	ann_btnTmr1Min->setPalette(*btnTmr1Min);
	ann_btnTmr2Min->setPalette(*btnTmr2Min);
	
	if ((tmr1MinIndx % 2) == 0)
	{
		btnTmr1Min->setColor(QPalette::ButtonText, ANN_YELLOW);
		ann_btnTmr1Min->setPalette(*btnTmr1Min);
		tmr1MinIndx++;
	}
	else if (tmr1MinIndx >= 5)
	{
		btnTmr1Min->setColor(QPalette::ButtonText, ANN_YELLOW);
		ann_btnTmr1Min->setPalette(*btnTmr1Min);
		tmr1MinIndx = 0;
		ann_flashTimer_Tmr1Min->stop();
	}
	else
	{
		btnTmr1Min->setColor(QPalette::ButtonText, ANN_BLACK);
		ann_btnTmr1Min->setPalette(*btnTmr1Min);
		tmr1MinIndx++;
	}
}

void GUIofficial::ann_procTmr2Min()
{
	btnTmr1Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr2Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr30Sec->setColor(QPalette::ButtonText, ANN_BLACK);
	ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
	ann_btnTmr1Min->setPalette(*btnTmr1Min);
	ann_btnTmr2Min->setPalette(*btnTmr2Min);
	
	if ((tmr2MinIndx % 2) == 0)
	{
		btnTmr2Min->setColor(QPalette::ButtonText, ANN_YELLOW);
		ann_btnTmr2Min->setPalette(*btnTmr2Min);
		tmr2MinIndx++;
	}
	else if (tmr2MinIndx >= 5)
	{
		btnTmr2Min->setColor(QPalette::ButtonText, ANN_YELLOW);
		ann_btnTmr2Min->setPalette(*btnTmr2Min);
		tmr2MinIndx = 0;
		ann_flashTimer_Tmr2Min->stop();
	}
	else
	{
		btnTmr2Min->setColor(QPalette::ButtonText, ANN_BLACK);
		ann_btnTmr2Min->setPalette(*btnTmr2Min);
		tmr2MinIndx++;
	}
}

void GUIofficial::ann_procTmr30Sec()
{
	btnTmr1Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr2Min->setColor(QPalette::ButtonText, ANN_BLACK);
	btnTmr30Sec->setColor(QPalette::ButtonText, ANN_BLACK);
	ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
	ann_btnTmr1Min->setPalette(*btnTmr1Min);
	ann_btnTmr2Min->setPalette(*btnTmr2Min);
	
	if ((tmr30SecIndx % 2) == 0)
	{
		btnTmr30Sec->setColor(QPalette::ButtonText, ANN_ORANGE);
		ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
		tmr30SecIndx++;
	}
	else if (tmr30SecIndx >= 5)
	{
		btnTmr30Sec->setColor(QPalette::ButtonText, ANN_ORANGE);
		ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
		tmr30SecIndx = 0;
		ann_flashTimer_Tmr30Sec->stop();
	}
	else
	{
		btnTmr30Sec->setColor(QPalette::ButtonText, ANN_BLACK);
		ann_btnTmr30Sec->setPalette(*btnTmr30Sec);
		tmr30SecIndx++;
	}
}



ahnsStatus::ahnsStatus()
{
	connectIP = -1;
	portTx = -1;
	portRx = -1;
	rxUDP = -1;
}

void ahnsStatus::setConnectIP(int status)
{
	connectIP = status;
}

void ahnsStatus::setPortTx(int status)
{
	portTx = status;
}

void ahnsStatus::setPortRx(int status)
{
	portRx = status;
}

void ahnsStatus::setRxUDP(int status)
{
	rxUDP = status;
}

int ahnsStatus::printStatus(char* buff, QString apIP, QString txPort, QString rxPort)
{
	char *szConnectIP = new char[16];
	char *szHostIP = new char[64];
	char *szPortTx = new char[16];
	char *szPortRx = new char[16];
	char *szRxUDP = new char[16];
	
	int fd;
	struct if_nameindex *curif, *ifs;
	struct ifreq req;

	if((fd = socket(PF_INET, SOCK_DGRAM, 0)) != -1)
	{
		ifs = if_nameindex();
		if(ifs)
		{
			for(curif = ifs; curif && curif->if_name; curif++)
			{
				strncpy(req.ifr_name, curif->if_name, IFNAMSIZ);
				req.ifr_name[IFNAMSIZ] = 0;
				
				if (ioctl(fd, SIOCGIFADDR, &req) >= 0)
				{
					//printf("%s: [%s]\n", curif->if_name, inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr));
					if (string(curif->if_name) == "eth0")
					{
						sprintf(szHostIP, "%s", inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr));
					}
					else if (string(curif->if_name) == "wlan0")
					{
						sprintf(szHostIP, "%s", inet_ntoa(((struct sockaddr_in*) &req.ifr_addr)->sin_addr));
						//cout << string(szHostIP) << endl;
					}
				}
			}
			if_freenameindex(ifs);
			if(close(fd) != 0)	sprintf(szHostIP, "%s", "Unable to close 'fd' <internal>");
		}
		else	sprintf(szHostIP, "%s", "Could not retrieve IP name index");
	}
	else	sprintf(szHostIP, "%s", "Could not detect network socket");

	switch (connectIP)
	{
		case SET_IP:
			sprintf(szConnectIP, "%s", apIP.toStdString().c_str());
		break;
		case UNSET_IP:
			sprintf(szConnectIP, "NOT SET");
		break;
		default:
			sprintf(szConnectIP, "UnKnown Error");
		break;
	}
	switch (portTx)
	{
		case SET_TX:
			sprintf(szPortTx, "%s", txPort.toStdString().c_str());
		break;
		case UNSET_TX:
			sprintf(szPortTx, "NOT SET");
		break;
		default:
			sprintf(szPortTx, "UnKnown Error");
		break;
	}
	switch (portRx)
	{
		case SET_RX:
			sprintf(szPortRx, "%s", rxPort.toStdString().c_str());
		break;
		case UNSET_RX:
			sprintf(szPortRx, "NOT SET");
		break;
		default:
			sprintf(szPortRx, "Unknown Error");
		break;
	}
	switch (rxUDP)
	{
		case RX_TRUE:
			sprintf(szRxUDP, "BUFFERING...");
		break;
		case RX_FALSE:
			sprintf(szRxUDP, "IDLE");
		break;
		default:
			sprintf(szRxUDP, "Unknown Error");
		break;
	}
	
	sprintf(buff, "Autopilot Computer IP: %s    |    HMI Computer IP: %s    |    Tx Port: %s    |    Rx Port: %s    |    Rx Status: %s",
	szConnectIP, szHostIP, szPortTx, szPortRx, szRxUDP);

	delete [] szConnectIP;
	delete [] szHostIP;
	delete [] szPortTx;
	delete [] szPortRx;
	delete [] szRxUDP;
	
	return 0;
}

void GUIofficial::updateStatus()
{
	char theMsg[256];
	
	if (cb_enableRx->isChecked() && cbFirst)
	{
		displaySystemMessage_NORMAL(UDP_RXSTREAM, "Set to ACCEPT Data");
		cbFirst = false;
	}
	else if (!cb_enableRx->isChecked() && !cbFirst)
	{
		displaySystemMessage_NORMAL(UDP_RXSTREAM, "Set to IDLE");
		cbFirst = true;
	}
	if (connInfo.buf_txIP.isEmpty())		theStatus.setConnectIP(UNSET_IP);
	else						theStatus.setConnectIP(SET_IP);
	if (connInfo.buf_txPort.isEmpty())		theStatus.setPortTx(UNSET_IP);
	else						theStatus.setPortTx(SET_IP);
	if (connInfo.buf_rxPort.isEmpty())		theStatus.setPortRx(UNSET_IP);
	else						theStatus.setPortRx(SET_IP);
	if (cb_enableRx->isChecked())			theStatus.setRxUDP(RX_TRUE);
	else						theStatus.setRxUDP(RX_FALSE);
	
	if(theStatus.printStatus(theMsg, connInfo.buf_txIP, connInfo.buf_txPort, connInfo.buf_rxPort) == 0)
	{
		statusbar->showMessage(tr(theMsg));
	}
}

void GUIofficial::on_ann_btnBattStat_clicked()
{


}
