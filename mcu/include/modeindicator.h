/**
 * \file   modeindicator.h
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
 * Declaration of Mode Indicator functions
 */

#include <avr/io.h>

#ifndef MODEINDICATOR_H
#define MODEINDICATOR_H

/** @name Mode LED Indicator */
#define RED_LED PD2   /**< Port Red LED is connected to */
#define GREEN_LED PD3 /**< Port Green LED is connected to */
#define BLUE_LED PD4  /**< Port Blue LED is connected to */

/** @name LED Modes */
#define TOGGLE 3 /**< Toggle mode integer */
#define ON 1 /**< On mode integer */
#define OFF 0 /**< Off mode integer */

/**
 * @brief Initialise the Mode indicator LED Ports
 */
extern uint8_t InitialiseModeIndicator();

/**
 * @brief Toggle the Red LED on or off
 */
extern void ToggleRed(uint8_t mode);

/**
 * @brief Toggle the Green LED on or off
 */
extern void ToggleGreen(uint8_t mode);

/**
 * @brief Toggle the Blue LED on or off
 * @param mode 1 for on, 0 for off, 3 for toggle
 */
extern void ToggleBlue(uint8_t mode);

/**
 * @brief Toggle LEDs to indicate Manual Mode
 * Macro implemented due to excessive functions calls
 */
#define IndicateManual() ToggleRed(ON); \
                         ToggleGreen(OFF);\
                         ToggleBlue(OFF);

/**
 * @brief Toggle LEDs to indicate Augmented Mode
 * Macro implemented due to excessive functions calls
 */
#define IndicateAugmented() ToggleRed(OFF); \
                            ToggleGreen(OFF);\
                            ToggleBlue(ON);

/**
 * @brief Toggle LEDs to indicate Autopilot Mode
 * Macro implemented due to excessive functions calls
 */
#define IndicateAutopilot() ToggleRed(OFF); \
                            ToggleGreen(ON);\
                            ToggleBlue(OFF);

#endif // MODEINDICATOR_H
