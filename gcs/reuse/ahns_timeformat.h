/**
 * @file ahns_timeformat.h
 * @author Tim Molloy
 *
 * $Author$
 * $LastChangedDate$
 * $Rev$
 * $Id$
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * A time format utility for AHNS programs, based on inline functions
 */

#ifndef AHNS_TIMEFORMAT_H
#define AHNS_TIMEFORMAT_H

#include <cstdio>

extern char ahnsTime[20];

inline char* AHNS_HMS(int hr, int min, int sec)
{
    sprintf(ahnsTime,"%02i:%02i:%02i%c",hr,min,sec,'\0');
    return ahnsTime;
}

inline char* AHNS_MSD(int min, int sec, int dec)
{
    sprintf(ahnsTime,"%02i:%02i.%02i%c",min,sec,dec,'\0');
    return ahnsTime;
}

#endif // AHNS_TIMEFORMAT_H
