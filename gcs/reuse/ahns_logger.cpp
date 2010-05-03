/**
 * \file
 * \author Alex Wainwright
 *
 * Last Modified by: $Author$
 *
 * $LastChangedDate$
 * $Rev: 118 $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * The source code for the AHNS logger utility.
 */

#include "ahns_logger.h"
#include <stdexcept>

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
    ahns_logger_file.open(filename, std::ios::out | std::ios::app);

    if (ahns_logger_file.good()){
        ahns_logger_file << "--------------------------------------------------------------------------------\n";
    }else{
        throw std::runtime_error("AHNS LOG FILE OPEN FAILED");
    }

    return 0;
}
