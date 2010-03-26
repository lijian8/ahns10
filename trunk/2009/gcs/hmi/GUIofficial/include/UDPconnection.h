#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

#include "ui_GUIofficial.h"
#include "ui_UDPconnection.h"
#include "ahns_logger.h"
#include <stdint.h>
#include <QWidget>
#include <QDialog>
#include <QString>

struct buffer_connInfo
{
	QString buf_txIP;
	QString buf_txPort;
	QString buf_rxPort;
};

struct joystickPacket
{
	uint16_t joy_X;
	uint16_t joy_Y;
	uint16_t joy_Z;
};

class ConnectionDialog : public QDialog, private Ui::NetworkConnection
{
	Q_OBJECT
	
public:

	ConnectionDialog();
	~ConnectionDialog(){}
	
	buffer_connInfo get_ConnectionInfo();

public slots:
	/* ******** BUTTON FUNCTIONS ******** */
	void on_btn_Close_clicked();
	void on_btn_Connect_clicked();

signals:
	void signal_resumesMainWidgets();
	void signal_activatesMainWidgets();

private:
	buffer_connInfo connInfo;

protected:
	void closeEvent(QCloseEvent *event);
};


#endif
