/**
 * \file   avrdefines.h
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
 * Header file to provide global definitions of terms not in avr-libc
 */

#ifndef AVRDEFINES_H
#define AVRDEFINES_H

/** Build in debug Mode */
#define DEBUG 

/** Boolian Defines */
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0

/** @name Register Bit Operating Macros */
#define SBR(x) (1<<(x))   /**< Set bit 'x' in register */
#define CBR(x) ~(1<<(x))  /**< Clear bit 'x' in register */
#define BRS(r,x) !!((r) & (1 << (x))) /**< Determine if bit 'x' in register 'r' is set */

/** @name Absolute of integer */
#define abs(x) ((x) < 0 ? -(x) : (x))

#endif // AVRDEFINES_H

