/**
 * \file   control.h
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
 * Implementation of the onboard Control.
 */

#include "control.h"
#include "state.h"
#include "stdio.h"
#include "stdint.h"

/** @name Roll Control Loop */
control_loop_t rollLoop;
pthread_mutex_t rollLoopMutex;

/** @name Pitch Control Loop */
control_loop_t pitchLoop;
pthread_mutex_t pitchLoopMutex;

/** @name Yaw Control Loop */
control_loop_t yawLoop;
pthread_mutex_t yawLoopMutex;

/** @name X Control Loop */
control_loop_t xLoop;
pthread_mutex_t xLoopMutex;

/** @name Y Control Loop */
control_loop_t yLoop;
pthread_mutex_t yLoopMutex;

/** @name Z Control Loop */
control_loop_t zLoop;
pthread_mutex_t zLoopMutex;

uint8_t setAPConfig(const ap_config_t* const srcConfig)
{
  uint8_t bRet = 0;

  if ((srcConfig->xActive) && (srcConfig->yActive) && (srcConfig->zActive)) // Guidance On
  {
    if ((srcConfig->phiActive) && (srcConfig->thetaActive) && (srcConfig->psiActive)) // G and A
    {
      bRet = 1;
    } // else fail since no A
  }
  else if ((!srcConfig->xActive) && (!srcConfig->yActive) && (srcConfig->zActive)) // z only
  {
    bRet = 1;
  }
  else if ((!srcConfig->xActive) && (!srcConfig->yActive) && (!srcConfig->zActive)) // A only
  {
    bRet = 1;
  } // else fail

  MutexLockAllLoops();

  rollLoop.active = srcConfig->phiActive;
  pitchLoop.active = srcConfig->thetaActive;
  yawLoop.active = srcConfig->psiActive;
  xLoop.active = srcConfig->xActive;
  yLoop.active = srcConfig->yActive;
  zLoop.active = srcConfig->zActive;

  MutexUnlockAllLoops();

  return bRet;
}

uint8_t setGains(const gains_t* const srcGains)
{
  uint8_t bRet = 0;

  MutexLockAllLoops();

  rollLoop.Kp = srcGains->rollKp;
  rollLoop.Ki = srcGains->rollKi;
  rollLoop.Kd = srcGains->rollKd;

  pitchLoop.Kp = srcGains->pitchKp;
  pitchLoop.Ki = srcGains->pitchKi;
  pitchLoop.Kd = srcGains->pitchKd;

  yawLoop.Kp = srcGains->yawKp;
  yawLoop.Ki = srcGains->yawKi;
  yawLoop.Kd = srcGains->yawKd;

  xLoop.Kp = srcGains->xKp;
  xLoop.Ki = srcGains->xKi;
  xLoop.Kd = srcGains->xKd;

  yLoop.Kp = srcGains->yKp;
  yLoop.Ki = srcGains->yKi;
  yLoop.Kd = srcGains->yKd;

  zLoop.Kp = srcGains->zKp;
  zLoop.Ki = srcGains->zKi;
  zLoop.Kd = srcGains->zKd;
  

  MutexUnlockAllLoops();
  
  return bRet;
}

uint8_t setParameters(const loop_parameters_t* const srcParameters)
{
  uint8_t bRet = 1;
  return bRet;
}

uint8_t setAttitude(const attitude_t* const srcAttitude)
{
  uint8_t bRet = 1;
  return bRet;
}

uint8_t setPosition(const position_t* const srcPosition)
{
  uint8_t bRet = 1;
  return bRet;
}

uint8_t saveConfig()
{
  uint8_t bRet = 1;
  return bRet;
}

void MutexLockAllLoops()
{
  pthread_mutex_lock(&rollLoopMutex);
  pthread_mutex_lock(&pitchLoopMutex);
  pthread_mutex_lock(&yawLoopMutex);
  pthread_mutex_lock(&xLoopMutex);
  pthread_mutex_lock(&yLoopMutex);
  pthread_mutex_lock(&zLoopMutex);
  return;
}

void MutexUnlockAllLoops()
{
  pthread_mutex_unlock(&zLoopMutex);
  pthread_mutex_unlock(&yLoopMutex);
  pthread_mutex_unlock(&xLoopMutex);
  pthread_mutex_unlock(&yawLoopMutex);
  pthread_mutex_unlock(&pitchLoopMutex);
  pthread_mutex_unlock(&rollLoopMutex);
  return;
}


void MutexLockAnglularLoops()
{
  pthread_mutex_lock(&rollLoopMutex);
  pthread_mutex_lock(&pitchLoopMutex);
  pthread_mutex_lock(&yawLoopMutex);
  
  return;
}

void MutexUnlockAnglularLoops()
{
  pthread_mutex_unlock(&yawLoopMutex);
  pthread_mutex_unlock(&pitchLoopMutex);
  pthread_mutex_unlock(&rollLoopMutex);
  
  return;
}


void MutexLockGuidanceLoops()
{
  pthread_mutex_lock(&xLoopMutex);
  pthread_mutex_lock(&yLoopMutex);
  pthread_mutex_lock(&zLoopMutex);
  
  return;
}

void MutexUnlockGuidanceLoops()
{
  pthread_mutex_unlock(&zLoopMutex);
  pthread_mutex_unlock(&yLoopMutex);
  pthread_mutex_unlock(&xLoopMutex);
  
  return;
}

