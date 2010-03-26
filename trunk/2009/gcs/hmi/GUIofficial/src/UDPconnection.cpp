#include "ui_GUIofficial.h"
#include "ui_UDPconnection.h"
#include "include/GUIofficial.h"
#include "UDPconnection.h"
#include "NetworkUDP.h"

#include <QWidget>
#include <QEvent>
#include <QString>
#include <QDialog>
#include <QCloseEvent>
#include <QTextEdit>

#include <fstream>

void GUIofficial::UDPdialog()
{
	setEnabled(false); // Disables the main window widgets
	int desktop_H = desktopInfo.screenGeometry().height();
	int desktop_W = desktopInfo.screenGeometry().width();
	int udpConnDialog_H = udpConnDialog->size().height();
	int udpConnDialog_W = udpConnDialog->size().width();
	udpConnDialog->move(desktop_W/2 - udpConnDialog_W/2, desktop_H/2 - udpConnDialog_H/2);

	udpConnDialog->exec();
	udpConnDialog->raise();
	udpConnDialog->setFocus();
}

ConnectionDialog::ConnectionDialog()
{
	setupUi(this);
}

/* ********************************************* */
/* ********** PUBLIC ACCESS FUNCTIONS ********** */
/* ********************************************* */
buffer_connInfo ConnectionDialog::get_ConnectionInfo()
{
	AHNS_DEBUG("ConnectionDialog::get_ConnectionInfo(): " << connInfo.buf_txPort.toUShort());
	return connInfo;
}


/* ********************************************** */
/* ********** GENERAL ACCESS FUNCTIONS ********** */
/* ********************************************** */
void ConnectionDialog::on_btn_Close_clicked()
{
	AHNS_DEBUG("ConnectionDialog::get_ConnectionInfo(): " << connInfo.buf_txPort.toUShort());
	emit signal_activatesMainWidgets();
	hide();
}

void ConnectionDialog::on_btn_Connect_clicked()
{
	AHNS_DEBUG("ConnectionDialog::on_btn_Connect_clicked()");
	connInfo.buf_txIP = le_ipAddr->text();
	connInfo.buf_txPort = le_txPortNo->text();
	connInfo.buf_rxPort = le_rxPortNo->text();
	emit signal_resumesMainWidgets();
}

void ConnectionDialog::closeEvent(QCloseEvent *event)
{
	AHNS_DEBUG("ConnectionDialog::closeEvent(QCloseEvent *event)");
	event->accept();
	
	close();
}

