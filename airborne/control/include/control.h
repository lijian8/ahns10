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
  uint8_t vicon;       /**< Flag to Choose Vicon */
  
  double reference;    /**< Commanded Reference */
  double referenceDot; /**< Commanded Dot Reference */
  
  double output;       /**< Current Output*/
  
  double integralError;          /**< Integral Variable*/
  double windup;        
  
  double Kp;           /**< Proportional Gain */
  double Ki;           /**< Integral Gain */
  double Kd;           /**< Rate Gain */
  
  double maximum;      /**< Maximum Limit */
  double minimum;      /**< Minimum Limit */
  double neutral;      /**< Neutral Value */

  double previousTime;      /**< Previous time update */
  double previousReference; /**< Previous reference */
  double previousState;     /**< Previous Control State */
} control_loop_t;

extern volatile enum FlightModes apMode;

/** @name RC Commands */
extern pthread_mutex_t rcMutex;
extern uint8_t rcMode;
extern int8_t rcThrottle;
extern int8_t rcRoll, rollTrim;
extern int8_t rcPitch, pitchTrim;
extern int8_t rcYaw, yawTrim;

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

/** @name Calculate Guidance Loop */
void updateGuidanceLoop(volatile control_loop_t* controlLoop, double tempError, double state, double stateDot);

/** @name Calculate Yaw Loop */
void updateYawLoop(volatile control_loop_t* controlLoop, double state, double stateDot);

/** @name Telemetry/Control Interface Functions */
uint8_t setAPConfig(const ap_config_t* const srcConfig);
uint8_t setGains(const gains_t* const srcGains);
uint8_t setParameters(const loop_parameters_t* const srcParameters);
uint8_t setAttitude(const attitude_t* const srcAttitude);
uint8_t setPosition(const position_t* const srcPosition);
uint8_t saveConfig();

/** @name Vicon State */
extern volatile vicon_state_t viconState;
extern pthread_mutex_t viconMutex;

/** @name Scale number in range [max1,min1] to [max2,min2] */
extern double MapCommands(double x, double max1, double min1, double max2, double min2);

/** @name Information for a Stable Flight Mode */
#define LEVEL_ROLL 0
#define LEVEL_PITCH 1
extern double levelAdjust[2];
extern double rollError, pitchError;
extern const double levelLimit, levelOff;
extern control_loop_t levelRollLoop, levelPitchLoop;


extern void MutexLockAllLoops();
extern void MutexUnlockAllLoops();
extern void MutexLockAngularLoops();
extern void MutexUnlockAngularLoops();
extern void MutexLockGuidanceLoops();
extern void MutexUnlockGuidanceLoops();

#endif //_CONTROL_H_

