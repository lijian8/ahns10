
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <signal.h>
#include "include/GUIofficial.h"
#include <QApplication>

using namespace std;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);	
	GUIofficial mainwindow;	
	mainwindow.show();
	return app.exec();
}
