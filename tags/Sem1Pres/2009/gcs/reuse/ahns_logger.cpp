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
 * The source code for the AHNS logger utility.
 */

#include "ahns_logger.h"

///The fstream for the log file.
std::fstream ahns_logger_file;		

///The time variable for the logging
time_t ahns_logger_time; 	

///The char buffer for the time stamp
char ahns_logger_time_string[80];	

/**
 * The initialisation function for the logger. Opens the file of name specified in argument.
 */
int ahns_logger_init(const char* filename)
{
	ahns_logger_file.open(filename, std::ios::out);
	return 0;
//TODO: return negative if couldn't open file.
}
