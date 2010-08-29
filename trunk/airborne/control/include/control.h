/**
 * \file   control.c
 * \author Tim Molloy
 *
 * $Author: tlmolloy $
 * $Date: 2010-07-01 14:13:35 +1000 (Thu, 01 Jul 2010) $
 * $Rev: 222 $
 * $Id: flightcontrol.cpp 222 2010-07-01 04:13:35Z tlmolloy $
 *
 * Queensland University of Technology
 *
 * \section DESCRIPTION
 * Declaration of the onboard Control.
 */
#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>

#include "state.h"
#include "MCUCommands.h"

/** @name Control Loop Type */
typedef struct { 
  uint8_t active;      /**< Active Flag */
  double reference;    /**< Commanded Reference */
  double output;       /**< Current Output*/
  
  double referenceDot; /**< Commanded Dot Reference */
  double integralError;          /**< Integral Variable*/        
  
  double Kp;           /**< Proportional Gain */
  double Ki;           /**< Integral Gain */
  double Kd;           /**< Rate Gain */
  
  double maximum;      /**< Maximum Limit */
  double minimum;      /**< Minimum Limit */
  double neutral;      /**< Neutral Value */

  double previousTime; /**< Previous time update */
  double previousReference; /**< Previous reference */
} control_loop_t;

extern volatile enum FlightModes apMode;

/** @name Roll Control Loop */
extern volatile control_loop_t rollLoop;
extern pthread_mutex_t rollLoopMutex;
extern volatile int8_t apRoll;

/** @name Pitch Control Loop */
extern volatile control_loop_t pitchLoop;
extern pthread_mutex_t pitchLoopMutex;
extern volatile int8_t apPitch;

/** @name Yaw Control Loop */
extern volatile control_loop_t yawLoop;
extern pthread_mutex_t yawLoopMutex;
extern volatile int8_t apYaw;

/** @name X Control Loop */
extern volatile control_loop_t xLoop;
extern pthread_mutex_t xLoopMutex;

/** @name Y Control Loop */
extern volatile control_loop_t yLoop;
extern pthread_mutex_t yLoopMutex;

/** @name Z Control Loop */
extern volatile control_loop_t zLoop;
extern pthread_mutex_t zLoopMutex;
extern volatile int8_t apThrottle;

/** @name Mutex for ap values */
extern pthread_mutex_t apMutex;

/** @name Calculate Control Loop */
void updateControlLoop(volatile control_loop_t* controlLoop, double state, double stateDot);

/** @name Telemetry/Control Interface Functions */
uint8_t setAPConfig(const ap_config_t* const srcConfig);
uint8_t setGains(const gains_t* const srcGains);
uint8_t setParameters(const loop_parameters_t* const srcParameters);
uint8_t setAttitude(const attitude_t* const srcAttitude);
uint8_t setPosition(const position_t* const srcPosition);
uint8_t saveConfig();

extern void MutexLockAllLoops();
extern void MutexUnlockAllLoops();
extern void MutexLockAngularLoops();
extern void MutexUnlockAngularLoops();
extern void MutexLockGuidanceLoops();
extern void MutexUnlockGuidanceLoops();

#endif //_CONTROL_H_

