/**
 * @file
 * @author Alex Wainwright
 *
 * Last Modified by: $Author: wainwright.alex $
 *
 * $LastChangedDate: 2009-10-17 16:10:27 +1000 (Sat, 17 Oct 2009) $
 * $Rev: 854 $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * A logger utility for AHNS programs, based on macros
 */

#ifndef AHNS_LOGGER
#define AHNS_LOGGER

#include <fstream>
#include <iomanip>
#include <iostream>
#include <time.h>
#include <ctime>
//#include <sys/fcntl.h>


/**
 * For general logging and debugging. Use it to log just about everything that happens. Goes to file.
 */
#define AHNS_DEBUG(out) std::time(&ahns_logger_time);\
        		strftime (ahns_logger_time_string, 80, "%H-%M-%S::", localtime(&ahns_logger_time));\
			ahns_logger_file << ahns_logger_time_string <<  "DEBUG: " << out << std::endl;

/**
 * For reporting errors. Goes to file.
 */
#define AHNS_ERROR(out) ahns_logger_file << "ERROR: " << out << std::endl

/** 
 * For important events which the operator should be notified of. Goes to file and screen (stderr)
 */
#define AHNS_ALERT(out) ahns_logger_file << "ALERT: " << out << std::endl;\
			cerr << "ALERT: " << out << std::endl

extern std::fstream ahns_logger_file;  		
extern time_t ahns_logger_time;			
extern char ahns_logger_time_string[80];


int ahns_logger_init(const char* filename);

#endif
