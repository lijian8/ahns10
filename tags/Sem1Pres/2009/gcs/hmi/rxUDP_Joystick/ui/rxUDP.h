
#ifndef _IRIDIUMTESTCTRL_FORM_H_
#define _IRIDIUMTESTCTRL_FORM_H_

#include "ui_rxUDPGUI.h"

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

class rxUDP : public QMainWindow, private Ui::rxSectUDP
{
	Q_OBJECT

public:
	
	rxUDP(QWidget* parent = 0);
	~rxUDP(){}
		
private slots:

	// BUTTON SLOTS
	void on_closeBtn_clicked();
	void processTxtPendingDatagrams();
	void processJsPendingDatagrams();
	void on_dispClear_clicked();

private:

	QUdpSocket *udpTxtSocket;
	QUdpSocket *udpJsSocket;
	QByteArray datagram;
};



#endif

