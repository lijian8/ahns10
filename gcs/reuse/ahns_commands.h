/**
 * \file   ahns_commands.h
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
 * Functions to handle the structures from heliconnect10
 * Handing the QByteStream buffer from the telemetry thread to the main GUI is unwise
 * so new memory is set aside to copy the structures into the main program.
 * It is then the main program's responsibility to deallote the structures once they have been used
 */


#ifndef AHNS_COMMANDS_H
#define AHNS_COMMANDS_H

#include "state.h"

/**
  * @brief Copy the Helicopter State
  *
  * @return Pointer to new memory location
  */

inline state_t* copyHeliState(const state_t* const srcState)
{
    state_t* heliState = new state_t;

    *heliState = *srcState;

    return heliState;
}

/**
  * @brief Copy the Flight Computer State
  *
  * @return Pointer to new memory location
  */

inline fc_state_t* copyFCState(const fc_state_t* const srcState)
{
    fc_state_t* FCState = new fc_state_t;

    *FCState = *srcState;

    return FCState;
}

/**
  * @brief Copy the Autopilot Computer State
  *
  * @return Pointer to new memory location
  */

inline ap_state_t* copyAPState(const ap_state_t* const srcState)
{
    ap_state_t* APState = new ap_state_t;

    *APState = *srcState;

    return APState;
}

/**
  * @brief Copy the Gains Structure to new memory location
  *
  * @return Pointer to new memory location
  */

inline gains_t* copyGains(const gains_t* const srcGains)
{
    gains_t* gains = new gains_t;

    *gains = *srcGains;

    return gains;
}

/**
  * @brief Copy the Loop Structure to new memory location
  *
  * @return Pointer to new memory location
  */

inline loop_parameters_t* copyLoopParameters(const loop_parameters_t* const srcLoop)
{
    loop_parameters_t* loop = new loop_parameters_t;

    *loop = *srcLoop;

    return loop;
}

#endif // AHNS_COMMANDS_H
