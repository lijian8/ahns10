#ifndef ANNUNCIATION_SYSTEM_H
#define ANNUNCIATION_SYSTEM_H

#include "ui_GUIofficial.h"

#define ANN_YELLOW QColor(226, 226, 29)
#define ANN_ORANGE QColor(255, 165, 20)
#define ANN_RED QColor(255, 0, 0)
#define ANN_BLACK QColor(0, 0, 0)
#define ANN_GREEN QColor(0, 255, 0)

/* **** DEFINE STATUS BAR MACROS **** */
#define SET_IP		1
#define UNSET_IP	0
#define SET_TX		1
#define UNSET_TX	0
#define SET_RX		1
#define UNSET_RX	0
#define RX_TRUE		1
#define RX_FALSE	0

/**************************************************/
/* ********** START OF THE HEADER FILE ********** */
/**************************************************/
class ANNclass : public QMainWindow, private Ui::GUIofficial_MASTER
{
	Q_OBJECT
	
public:

	ANNclass(QWidget* parent = 0);
	~ANNclass();


public slots:

};

class ahnsStatus
{
	public:		
		ahnsStatus();

		int printStatus(char* buff, QString apIP, QString txPort, QString rxPort);
		void setConnectIP(int status);
		void setPortTx(int status);
		void setPortRx(int status);
		void setRxUDP(int status);


	private:
		int connectIP;
		int portTx;
		int portRx;
		int rxUDP;
};

#endif
