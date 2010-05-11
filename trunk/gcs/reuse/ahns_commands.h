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
 * Functions to handle the state structures from heliconnect
 */


#ifndef AHNS_COMMANDS_H
#define AHNS_COMMANDS_H

#include "state.h"

/**
  * @brief Copy the Helicopter State
  *
  * @return Pointer to new memory location
  */

state_t* copyHeliState(const state_t* srcState)
{
    state_t* heliState = new state_t;

    heliState->phi = srcState->phi;
    heliState->theta = srcState->theta;
    heliState->psi = srcState->psi;

    heliState->p = srcState->p;
    heliState->q = srcState->q;
    heliState->r = srcState->r;

    heliState->x = srcState->x;
    heliState->y = srcState->y;
    heliState->z = srcState->z;

    heliState->vx = srcState->vx;
    heliState->vy = srcState->vy;
    heliState->vz = srcState->vz;

    heliState->ax = srcState->ax;
    heliState->ay = srcState->ay;
    heliState->az = srcState->az;

    heliState->trace = srcState->trace;
    heliState->voltage = srcState->voltage;

    return heliState;
}

#endif // AHNS_COMMANDS_H
