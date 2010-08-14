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

static inline void updateControlLoop(volatile control_loop_t* controlLoop, double state, double stateDot);

volatile enum FlightModes apMode;

/** @name Roll Control Loop */
volatile control_loop_t rollLoop;
pthread_mutex_t rollLoopMutex;
volatile int8_t apRoll;

/** @name Pitch Control Loop */
volatile control_loop_t pitchLoop;
pthread_mutex_t pitchLoopMutex;
volatile int8_t apPitch;

/** @name Yaw Control Loop */
volatile control_loop_t yawLoop;
pthread_mutex_t yawLoopMutex;
volatile int8_t apYaw;

/** @name X Control Loop */
volatile control_loop_t xLoop;
pthread_mutex_t xLoopMutex;

/** @name Y Control Loop */
volatile control_loop_t yLoop;
pthread_mutex_t yLoopMutex;

/** @name Z Control Loop */
volatile control_loop_t zLoop;
pthread_mutex_t zLoopMutex;
volatile int8_t apThrottle;

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

void* controlThread(void *pointer)
{ 
  // initialise gains and parameters from files
  int i = 0, j = 0;
  FILE *gainsfd = fopen("gains.ahnsgains","r");
  double gains[6][3];
  FILE *parametersfd =  fopen("parameters.ahnsparameters","r");
  double parameters[6][3];
  
  if (gainsfd && parametersfd)
  { 
    for (i = 0; i < 6; ++i)
    {
      for (j = 0; j < 3; ++j)
      {
        fscanf(gainsfd, "%lf", &gains[i][j]);
        fscanf(parametersfd, "%lf", &parameters[i][j]);
      }
    }
  }
  else
  {
    fprintf(stderr,"void* controlThread(void *pointer) :: FILE OPEN FAILED");
    for (i = 0; i < 6; ++i)
    {
      for (j = 0; j < 3; ++j)
      {
        gains[i][j] = 0.0;
        parameters[i][j] = 0.0;
      }
    }
  }
  
  rollLoop.maximum = parameters[0][0];
  rollLoop.neutral = parameters[0][1];
  rollLoop.minimum = parameters[0][2];

  pitchLoop.maximum = parameters[1][0];
  pitchLoop.neutral = parameters[1][1];
  pitchLoop.minimum = parameters[1][2];

  yawLoop.maximum = parameters[2][0];
  yawLoop.neutral = parameters[2][1];
  yawLoop.minimum = parameters[2][2];

  xLoop.maximum = parameters[3][0];
  xLoop.neutral = parameters[3][1];
  xLoop.minimum = parameters[3][2];

  yLoop.maximum = parameters[4][0];
  yLoop.neutral = parameters[4][1];
  yLoop.minimum = parameters[4][2];

  zLoop.maximum = parameters[5][0];
  zLoop.neutral = parameters[5][1];
  zLoop.minimum = parameters[5][2];

  rollLoop.Kp = gains[0][0];
  rollLoop.Ki = gains[0][1];
  rollLoop.Kd = gains[0][2];

  pitchLoop.Kp = gains[1][0];
  pitchLoop.Ki = gains[1][1];
  pitchLoop.Kd = gains[1][2];

  yawLoop.Kp = gains[2][0];
  yawLoop.Ki = gains[2][1];
  yawLoop.Kd = gains[2][2];

  xLoop.Kp = gains[3][0];
  xLoop.Ki = gains[3][1];
  xLoop.Kd = gains[3][2];

  yLoop.Kp = gains[4][0];
  yLoop.Ki = gains[4][1];
  yLoop.Kd = gains[4][2];

  zLoop.Kp = gains[5][0];
  zLoop.Ki = gains[5][1];
  zLoop.Kd = gains[5][2];
    
  fclose(gainsfd);
  fclose(parametersfd); 

  // control thread
  while(1)
  {
    /** @TODO input actual states*/
    // Update Guidance Loops
    updateControlLoop(&xLoop,0,0);
    if (xLoop.active)
    {
      pitchLoop.reference = xLoop.output;
    }
    
    updateControlLoop(&yLoop,0,0);
    if (yLoop.active)
    {
      rollLoop.reference = yLoop.output;
    }

    updateControlLoop(&zLoop,0,0);
    apThrottle = zLoop.output;

    // Update Control Loops
    updateControlLoop(&rollLoop,0,0);
    apRoll = rollLoop.output;

    updateControlLoop(&pitchLoop,0,0);
    apPitch = pitchLoop.output;

    updateControlLoop(&yawLoop,0,0);
    apYaw = yawLoop.output;
    
    // Determine AP Mode for MCU usage
    if (zLoop.active && rollLoop.active && pitchLoop.active && yawLoop.active)
    {
      apMode = RC_NONE;
    }
    else if (!zLoop.active && rollLoop.active && pitchLoop.active && yawLoop.active)
    {
      apMode = RC_THROTTLE;
    }
    else if (zLoop.active && !rollLoop.active && pitchLoop.active && yawLoop.active)
    {
      apMode = RC_ROLL;
    }
    else if (zLoop.active && rollLoop.active && !pitchLoop.active && yawLoop.active)
    {
      apMode = RC_PITCH;
    }
    else if (zLoop.active && rollLoop.active && pitchLoop.active && !yawLoop.active)
    {
      apMode = RC_YAW;
    }
    else if (!zLoop.active && !rollLoop.active && pitchLoop.active && yawLoop.active)
    {
      apMode = RC_THROTTLE_ROLL;
    }
    else if (!zLoop.active && rollLoop.active && !pitchLoop.active && yawLoop.active)
    {
      apMode = RC_THROTTLE_PITCH;
    }
    else if (!zLoop.active && rollLoop.active && pitchLoop.active && !yawLoop.active)
    {
      apMode = RC_THROTTLE_YAW;
    }
    else if (!zLoop.active && !rollLoop.active && !pitchLoop.active && yawLoop.active)
    {
      apMode = RC_THROTTLE_ROLL_PITCH;
    }
    else if (!zLoop.active && !rollLoop.active && pitchLoop.active && !yawLoop.active)
    {
      apMode = RC_THROTTLE_ROLL_YAW;
    }
    else if (!zLoop.active && rollLoop.active && !pitchLoop.active && !yawLoop.active)
    {
      apMode = RC_THROTTLE_PITCH_YAW;
    }
    else if (zLoop.active && !rollLoop.active && !pitchLoop.active && yawLoop.active)
    {
      apMode = RC_ROLL_PITCH;
    }
    else if (zLoop.active && !rollLoop.active && pitchLoop.active && !yawLoop.active)
    {
      apMode = RC_ROLL_YAW;
    }
    else if (zLoop.active && rollLoop.active && !pitchLoop.active && !yawLoop.active)
    {
      apMode = RC_PITCH_YAW;
    }
    else if (zLoop.active && !rollLoop.active && !pitchLoop.active && !yawLoop.active)
    {
      apMode = RC_ROLL_PITCH_YAW;
    }

    // Send to MCU
    usleep(20e3); 
  }
  return NULL;
}

/**
 * @brief Function to Update the control loops
 * @param controlLoop Control Loop to be activated
 * @param state Current state variable to be controlled by the loop
 * @param stateDot Current Derivative of the state being controlled by the loop
 *
 * @TODO Implement integral anti-windup and control
 */
static inline void updateControlLoop(volatile control_loop_t* controlLoop, double state, double stateDot)
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
