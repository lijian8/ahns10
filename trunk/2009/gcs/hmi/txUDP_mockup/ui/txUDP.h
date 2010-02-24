
#ifndef _IRIDIUMTESTCTRL_FORM_H_
#define _IRIDIUMTESTCTRL_FORM_H_

#include "ui_txUDPGUI.h"

#include <QWidget>
#include <QString>
#include <QMap>
#include <QThread>
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

class QCheckBox;

class txUDP : public QMainWindow, private Ui::txSectUDP
{
	Q_OBJECT

public:
	
	txUDP(QWidget* parent = 0);
	~txUDP(){}
		
private slots:

	// GENERAL BUTTON SLOTS
	void on_closeBtn_clicked();
	void on_readDisp_clicked();

	// UDP BUTTON SLOTS
	void on_sendUDP_clicked();
	void on_clearUDP_clicked();
	void on_timedTxUDP_clicked();
	void on_timedTxUDPstop_clicked();
	void on_confirmDetails_clicked();

	// UDP MEMBER FUNCTIONS
	void SendTimedUDP();
	void EnableButtons();

private:

	// UDP PRIVATE DATA MEMBER
	QUdpSocket *udpSocket;
	QByteArray datagram;
	QString label;
	QString text;
	QString textUDP;
	QByteArray rawTextUDP;
	QByteArray UDP_data;
	QString szIP;
	quint16 sPort;

	// TIMER DATA MEMBER
	QTimer *UDP_timer;
	QByteArray timedData;
	short UDP_counter;
};



#endif

