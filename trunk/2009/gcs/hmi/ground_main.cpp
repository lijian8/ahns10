
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <signal.h>
#include "ui/groundstationform.h"
#include <QApplication>
#include <QThread>
extern "C" {
#include "ui/driver/joystick.h"
}

using namespace std;


/*void signalhandlers(int signaltype)
{
	switch (signaltype)
	{ 
	case SIGINT:
	{
		cout << endl << "Caught Ctrl+C" << endl;
		//iridiumtestctrl.IridiumStop();
	}

	default:
		break;
	}
}*/

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	//signal(SIGINT,signalhandlers);
	
	GroundStationForm mainwindow;
	
	mainwindow.show();

	return app.exec();
}
