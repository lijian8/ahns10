/**
 * \file   modeindicator.h
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
 * Declaration of Mode Indicator functions
 */

#include <avr/io.h>

#ifndef MODEINDICATOR_H
#define MODEINDICATOR_H

/** @name Mode LED Indicator */
#define GREEN_LED PD3 /**< Port Green LED is connected to */
#define BLUE_LED PD4  /**< Port Blue LED is connected to */
#define RED_LED PD2   /**< Port Red LED is connected to */

/**
 * @brief Initialise the Mode indicator LED Ports
 */
extern uint8_t InitialiseModeIndicator();

#endif // MODEINDICATOR_H
