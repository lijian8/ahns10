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

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

#include "control.h"
#include "state.h"


volatile enum FlightModes apMode;

/** @name Roll Control Loop */
volatile control_loop_t rollLoop;
pthread_mutex_t rollLoopMutex = PTHREAD_MUTEX_INITIALIZER;
volatile int8_t apRoll;

/** @name Pitch Control Loop */
volatile control_loop_t pitchLoop;
pthread_mutex_t pitchLoopMutex = PTHREAD_MUTEX_INITIALIZER;
volatile int8_t apPitch;

/** @name Yaw Control Loop */
volatile control_loop_t yawLoop;
pthread_mutex_t yawLoopMutex = PTHREAD_MUTEX_INITIALIZER;
volatile int8_t apYaw;

/** @name X Control Loop */
volatile control_loop_t xLoop;
pthread_mutex_t xLoopMutex = PTHREAD_MUTEX_INITIALIZER;

/** @name Y Control Loop */
volatile control_loop_t yLoop;
pthread_mutex_t yLoopMutex = PTHREAD_MUTEX_INITIALIZER;

/** @name Z Control Loop */
volatile control_loop_t zLoop;
pthread_mutex_t zLoopMutex = PTHREAD_MUTEX_INITIALIZER;
volatile int8_t apThrottle;

/** @name Mutex for ap values */
pthread_mutex_t apMutex;

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
  uint8_t bRet = 1;

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

  MutexLockAllLoops();

  if(!(srcParameters->rollMaximum >= srcParameters->rollNeutral) || !(srcParameters->rollNeutral >= srcParameters->rollMinimum))
  {
    bRet = 0;
  }
  else if (!(srcParameters->pitchMaximum >= srcParameters->pitchNeutral) || !(srcParameters->pitchNeutral >= srcParameters->pitchMinimum))
  {
    bRet = 0;
  }
  else if (!(srcParameters->yawMaximum >= srcParameters->yawNeutral) || !(srcParameters->yawNeutral >= srcParameters->yawMinimum))
  {
    bRet = 0;
  }
  else if (!(srcParameters->xMaximum >= srcParameters->xNeutral) || !(srcParameters->xNeutral >= srcParameters->xMinimum))
  {
    bRet = 0;
  }
  else if (!(srcParameters->yMaximum >= srcParameters->yNeutral) || !(srcParameters->yNeutral >= srcParameters->yMinimum))
  {
    bRet = 0;
  }
  else if(!(srcParameters->zMaximum >= srcParameters->zNeutral) || !(srcParameters->zNeutral >= srcParameters->zMinimum))
  {
    bRet = 0;
  }
  else
  {
    bRet = 1;
    rollLoop.maximum = srcParameters->rollMaximum;
    rollLoop.minimum = srcParameters->rollMinimum;
    rollLoop.neutral = srcParameters->pitchNeutral;
    
    pitchLoop.maximum = srcParameters->pitchMaximum;
    pitchLoop.minimum = srcParameters->pitchMinimum;
    pitchLoop.neutral = srcParameters->pitchNeutral;

    yawLoop.maximum = srcParameters->yawMaximum;
    yawLoop.minimum = srcParameters->yawMinimum;
    yawLoop.neutral = srcParameters->yawNeutral;

    xLoop.maximum = srcParameters->xMaximum;
    xLoop.minimum = srcParameters->xMinimum;
    xLoop.neutral = srcParameters->xNeutral;
    
    yLoop.maximum = srcParameters->yMaximum;
    yLoop.minimum = srcParameters->yMinimum;
    yLoop.neutral = srcParameters->yNeutral;
    
    zLoop.maximum = srcParameters->zMaximum;
    zLoop.minimum = srcParameters->zMinimum;
    zLoop.neutral = srcParameters->zNeutral;
    
  }


  MutexUnlockAllLoops();  

  return bRet;
}

uint8_t setAttitude(const attitude_t* const srcAttitude)
{
  uint8_t bRet = 1;

  MutexLockAngularLoops();
  
  rollLoop.reference = srcAttitude->phi;
  pitchLoop.reference = srcAttitude->theta;
  yawLoop.reference = srcAttitude->psi;

  MutexUnlockAngularLoops();
  return bRet;
}

uint8_t setPosition(const position_t* const srcPosition)
{
  uint8_t bRet = 1;
  
  MutexLockGuidanceLoops();

  xLoop.reference = srcPosition->x;
  yLoop.reference = srcPosition->y;
  zLoop.reference = srcPosition->z;

  MutexUnlockGuidanceLoops();
  
  return bRet;
}

uint8_t saveConfig()
{
  uint8_t bRet = 0;

   // initialise gains and parameters from files
  FILE *gainsfd = fopen("gains.ahnsgains","w");
  FILE *parametersfd =  fopen("parameters.ahnsparameters","w");
  
  if (gainsfd && parametersfd)
  {
    fprintf(gainsfd,"%lf\t%lf\t%lf\n",rollLoop.Kp,rollLoop.Ki,rollLoop.Kd);
    fprintf(gainsfd,"%lf\t%lf\t%lf\n",pitchLoop.Kp,pitchLoop.Ki,pitchLoop.Kd);
    fprintf(gainsfd,"%lf\t%lf\t%lf\n",yawLoop.Kp,yawLoop.Ki,yawLoop.Kd);
    fprintf(gainsfd,"%lf\t%lf\t%lf\n",xLoop.Kp,xLoop.Ki,xLoop.Kd);
    fprintf(gainsfd,"%lf\t%lf\t%lf\n",yLoop.Kp,yLoop.Ki,yLoop.Kd);
    fprintf(gainsfd,"%lf\t%lf\t%lf\n",zLoop.Kp,zLoop.Ki,zLoop.Kd);

    fprintf(parametersfd,"%lf\t%lf\t%lf\n",rollLoop.maximum,rollLoop.neutral,rollLoop.minimum);
    fprintf(parametersfd,"%lf\t%lf\t%lf\n",pitchLoop.maximum,pitchLoop.neutral,pitchLoop.minimum);
    fprintf(parametersfd,"%lf\t%lf\t%lf\n",yawLoop.maximum,yawLoop.neutral,yawLoop.minimum);
    fprintf(parametersfd,"%lf\t%lf\t%lf\n",xLoop.maximum,xLoop.neutral,xLoop.minimum);
    fprintf(parametersfd,"%lf\t%lf\t%lf\n",yLoop.maximum,yLoop.neutral,yLoop.minimum);
    fprintf(parametersfd,"%lf\t%lf\t%lf\n",zLoop.maximum,zLoop.neutral,zLoop.minimum);

    bRet = 1;
    fclose(gainsfd);
    fclose(parametersfd);
  }

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


void MutexLockAngularLoops()
{
  pthread_mutex_lock(&rollLoopMutex);
  pthread_mutex_lock(&pitchLoopMutex);
  pthread_mutex_lock(&yawLoopMutex);
  
  return;
}

void MutexUnlockAngularLoops()
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

/**
 * @brief Function to Update the control loops
 * @param controlLoop Control Loop to be activated
 * @param state Current state variable to be controlled by the loop
 * @param stateDot Current Derivative of the state being controlled by the loop
 *
 * @TODO Implement integral anti-windup and control
 */
inline void updateControlLoop(volatile control_loop_t* controlLoop, double state, double stateDot)
{
  double tempOutput = 0.0;
  double tempError = 0.0;
  struct timeval currentTimeStruct;
  gettimeofday(&currentTimeStruct,NULL);
  double currentTime = currentTimeStruct.tv_sec + currentTimeStruct.tv_usec / 1e6;
  double dt = currentTime - controlLoop->previousTime;
 
  // reset integrators if reference changed 
  if (controlLoop->previousReference != controlLoop->reference)
  {
    controlLoop->integralError = 0.0;
  }

  if (controlLoop->active)
  {
    tempError = controlLoop->reference - state;
    tempOutput = controlLoop->Kp*(tempError) + controlLoop->Kd*(controlLoop->referenceDot - stateDot) + controlLoop->Ki*(controlLoop->integralError) + controlLoop->neutral;

    // bound the output
    // only integrate if not in saturation
    if (tempOutput > controlLoop->maximum)
    {
      controlLoop->output = controlLoop->maximum;
    }
    else if (tempOutput < controlLoop->minimum)
    {
      controlLoop->output = controlLoop->minimum;
    }
    else
    {
      controlLoop->output = tempOutput; 
      controlLoop->integralError += dt*tempError;
    }
    
    // Store previous time
    controlLoop->previousTime = currentTime;
    controlLoop->previousReference = controlLoop->reference; 
  }
  else
  {
    controlLoop->output = 0.0;
  }
  
  return;
}
