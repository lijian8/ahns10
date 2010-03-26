
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <signal.h>
#include "ui/rxUDP.h"
#include <QApplication>


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
	
	rxUDP mainwindow;
	
	mainwindow.show();

	return app.exec();
}
