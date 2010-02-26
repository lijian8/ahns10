/**
 * @file
 * @author Alex Wainwright
 *
 * Last Modified by: $Author$ 
 *
 * $LastChangedDate: 2009-10-07 11:53:50 +1000 (Wed, 07 Oct 2009) $
 * $Rev: 771 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 *
 * This file contains main(), and is just responsible for starting off the application implemented in /trunk/autopilot/autopilot/ui/groundstationform.cpp
 *
 */

#include "ui/groundstationform.h"
#include <QApplication>

using namespace std;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
		
	//signal(SIGINT,signalhandlers);
	
	GroundStationForm mainwindow;
	
	mainwindow.show();

	return app.exec();
}

