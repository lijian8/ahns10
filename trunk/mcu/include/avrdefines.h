/**
 * \file   avrdefines.h
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-06-10 23:59:05 +1000 (Thu, 10 Jun 2010) $
 * $Rev: 164 $
 * $Id: main.cpp 164 2010-06-10 13:59:05Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Header file to provide global definitions of terms not in avr-libc
 */

#ifndef AVRDEFINES_H
#define AVRDEFINES_H

/** CPU Clock Speed in Hz */
#define F_CPU 8000000UL // 16MHz scaled to 8 MHz

/** Boolian Defines */
#define TRUE 1
#define true 1
#define FALSE 0
#define false 0

/** @name Register Bit Operating Macros */
#define SBR(x) (1<<(x))   /**< Set bit 'x' in register */
#define CBR(x) ~(1<<(x))  /**< Clear bit 'x' in register */
#define BRS(r,x) !!((r) & (1 << (x))) /**< Determine if bit 'x' in register 'r' is set */

#endif // AVRDEFINES_H

