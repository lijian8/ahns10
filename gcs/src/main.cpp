/**
 * \file   main.cpp
 * \author Tim Molloy
 *
 * $Author$
 * $Date$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Main file used to excecute the AHNS gcsmainwindow class, i.e. to run the gcs application.
 */

#include <QtGui/QApplication>
#include <stdexcept>
#include "gcsmainwindow.h"
#include "ahns_logger.h"


int main(int argc, char *argv[])
{
    try{
        ahns_logger_init("gcs_log.txt");
    }catch(const std::exception &e){
        std::cerr << e.what();
        exit(-1); // not worth continuing with no logging
    }
    AHNS_DEBUG("Logging Begun");


    QApplication a(argc, argv);

    //Main Window
    AHNS_DEBUG("Create Main Window");
    gcsMainWindow w;
    w.show();

    AHNS_DEBUG("Run application");
    return a.exec();
}
