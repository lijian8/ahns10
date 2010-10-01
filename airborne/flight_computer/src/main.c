/**
 * $Author: liamosullivan $
 * $Date: 2010-06-21 15:25:25 +1000 (Mon, 21 Jun 2010) $
 * $Rev: 193 $
 * $Id: imuServerTester.c 193 2010-06-21 05:25:25Z liamosullivan $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Flight computer main program
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "main.h"
#include "server.h"
#include "state.h"
#include "imuserial.h"
#include "arduserial.h"
#include "kfb.h"
#include "control.h"
#include "mcuserial.h"
#include "MCUCommands.h"

// udp server
static Server server;
// state variable
state_t state;
// sensor data variable
sensor_data_t raw_IMU;
// compass heading
double compass_heading = 0.0;
// flight computer + engine state
fc_state_t fcState;
// autopilot state
ap_state_t apState;

pthread_mutex_t fcMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t apStateMut = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// flight computer functions
int sensorInit();
int mcuInit();
// flight computer thread functions
void * sendUDPData(void *pointer);
void * updateIMUdata(void *pointer);
void * updateCompassHeading(void *pointer);
void * updateArduinoData(void *pointer);
void * updateMCU(void *pointer);
void * updateControl(void *pointer);

int main(int argc, char *argv[])
{
  // boolean variables to check serial connections
  int sensorReady = 0;
  int mcuReady = 0;
  // init the server
  server_init(&server, SERVER_PORT);
  // initialise the sensors
  sensorReady = sensorInit();
  // initialise the MCU
  mcuReady = mcuInit();
  // initialise the Kalman filter
  //attitudeFilterInitialise();
  // all systems operational - begin the flight computer
  if (sensorReady && mcuReady)
  {
    // create the flight computer threads
    pthread_t udpThread;
    pthread_t imuThread;
    pthread_t arduThread;
    pthread_t mcuThread;
    pthread_t controlThread;

    // create the server thread
    pthread_create(&udpThread, NULL, sendUDPData, (int*) 1);
    // create the imu thread
    pthread_create(&imuThread, NULL, updateIMUdata, (int*) 2);
    // create the arduino thread
    //pthread_create(&arduThread, NULL, updateCompassHeading, (int*) 3);i
    pthread_create(&arduThread, NULL, updateArduinoData, (int*) 3);
    //create the mcu thread
    pthread_create(&mcuThread, NULL, updateMCU, (int*) 4);
    //create the control thread
    pthread_create(&controlThread, NULL, updateControl, (int*) 5);

    // execute the udp server thread
    pthread_join(udpThread,NULL);
    // execute the state thread
    pthread_join(imuThread,NULL);
    // execute the compass thread
    pthread_join(arduThread,NULL);
    // execute the mcu thread
    pthread_join(mcuThread,NULL);
    // execute the control thread
    pthread_join(controlThread,NULL);
   
  }
  return 0;
}

// update the compass heading from the arduino
void * updateCompassHeading(void *pointer)
{
  double previous_compass_heading = 0.0;
  while(1)
  {
    // sleep the thread for updating the compass
    usleep(COMPASS_DELAY*1e3);
    // get the compass heading from the arduino
    pthread_mutex_lock(&mut);
    previous_compass_heading = compass_heading;
    if (!getCompassHeading(&compass_heading))
    {
      compass_heading = previous_compass_heading;
    }
    raw_IMU.psi = compass_heading;
    pthread_mutex_unlock(&mut);
  }
  return NULL;
}

// update the arduino data
void * updateArduinoData(void *pointer)
{
  double compass_heading_old = 0.0;
  double voltage_old = 0.0;
  double z_old = 0.0;
  while(1)
  {
    // sleep the thread for updating the arduino
    usleep(COMPASS_DELAY*1e3);
    // get the data arduino
    pthread_mutex_lock(&mut);
    compass_heading_old = compass_heading;
    voltage_old = state.voltage;
    z_old = raw_IMU.z;
    if (!getArduinoDataCan(&compass_heading, &state.voltage, &raw_IMU.z))
    {
      compass_heading = compass_heading_old;
      state.voltage = voltage_old;
      raw_IMU.z = z_old;
    }
    //printf(">> Ard: %f %f %f\n",compass_heading,state.voltage,raw_IMU.z);
    raw_IMU.psi = compass_heading;
    pthread_mutex_unlock(&mut);
  }
  return NULL;
}

// update IMU data
void * updateIMUdata(void *pointer)
{
  // time stamp
  struct timeval timestamp;
  // time between filter updates
  double startFilterTime, endFilterTime, diffFilterTime;
  // get IMU data to calculate initial state
  getImuSensorData(&raw_IMU.p, &raw_IMU.q, &raw_IMU.r, &raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az);
  // initialise the Kalman filter
  attitudeFilterInitialiseB(&raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az);
  // calculate filter start time
  gettimeofday(&timestamp, NULL); 
  startFilterTime = timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
  while(1)
  {
    // sleep the thread for updating the IMU
    usleep(IMU_DELAY*1e3);
    pthread_mutex_lock(&mut);
    // get the IMU sensor data
    getImuSensorData(&raw_IMU.p, &raw_IMU.q, &raw_IMU.r, &raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az);
    // calculate the time elapsed since last update
    gettimeofday(&timestamp, NULL); 
    endFilterTime = timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
    diffFilterTime = endFilterTime - startFilterTime;
    // calculate filter start time
    gettimeofday(&timestamp, NULL); 
    startFilterTime = timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
    // perform the attitude filtering using the imu data
    //attitudeFilter(&raw_IMU.p, &raw_IMU.q, &raw_IMU.r, &raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az, &state.p, &state.q, &state.r, &state.phi, &state.theta, &state.psi, compass_heading, diffFilterTime);
    attitudeFilterB(&raw_IMU.p, &raw_IMU.q, &raw_IMU.r, &raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az, &state.p, &state.q, &state.r, &state.phi, &state.theta, &state.psi, &compass_heading, diffFilterTime);
    //printf(">> kf update : %f\n",1/diffFilterTime);
    state.trace = (1/diffFilterTime);

    pthread_mutex_unlock(&mut);
  }
  return NULL;
}

// initialise the sensors connected to the flight computer
// 1. IMU
// 2. Arduino (compass, camera, ultrasonic)
int sensorInit()
{
  int retSerial = 0;
  int retValue =0;
  // connect to the IMU
  retSerial = openIMUSerial(IMU_SERIAL_PORT, IMU_BAUD_RATE);
  if(retSerial)
  {
    retValue = 1;
    printf(">> IMU connected\n");
  } else
    {
      printf(">> Error: Cannot connect to IMU on serial port: %s\n",IMU_SERIAL_PORT);
    }
  // connect to the arduino
  retSerial = openArduSerial(ARDU_SERIAL_PORT, ARDU_BAUD_RATE);
  if(retSerial)
  {
    retValue = 1;
    printf(">> Arduino connected\n");
  } else
    {
      printf(">> Error: Cannot connect to Arduino on serial port: %s\n", ARDU_SERIAL_PORT);
    }

  return retValue;
}

// initialise connection to the MCU
int mcuInit()
{
  int retValue = 0;
  retValue = mcuOpenSerial(MCU_SERIAL_PORT,MCU_BAUD_RATE); 
  if (retValue)
  {
    printf(">> MCU connected \n");
  } else
    {
      printf(">> Error: Cannot connect to MCU on serial port: %s\n",MCU_SERIAL_PORT);
    }
  return retValue;
}

// send the UDP data packets to clients
void * sendUDPData(void *pointer)
{
  // udp packet type
  int udpType = 0;
  int init = 1;
  int count=0;
  struct timeval timestamp;
  double startServerTime = 0;
  double endServerTime = 0;
  double diffServerTime = 0;
  int dataLength;
  unsigned char buffer[512];
  // initialise startTime
  gettimeofday(&timestamp, NULL); 
  startServerTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);

  while(1)
  {
    int type=0;
    if(server_poll(&server,1))
    {
      udpType = server_get_packet(&server);
    }    
    if(udpType < 0)
    {
      fprintf(stderr,"Read error from server");
      //return -1;
    }
    count++;
    usleep(SERVER_DELAY*1e3);
    //send data to the socket
    if(count%1 == 0)
    {
      init = 1;
    }
    if(init)
    {
      // calculate server delay time
      gettimeofday(&timestamp, NULL); 
      endServerTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
      diffServerTime = endServerTime - startServerTime;
      // output the sent states  
      //printf(">> state : %f %f %f %f | %f\n",raw_IMU.p,raw_IMU.q,raw_IMU.r,state.psi,(1/diffServerTime));
      // send the state packet
      server_send_packet(&server, HELI_STATE, &state, sizeof(state_t));
      // send the raw sensor data packet
      dataLength = PackSensorData(buffer, &raw_IMU);
      server_send_packet(&server, SENSOR_DATA, buffer, dataLength);
      // send the fc state packet
      pthread_mutex_lock(&fcMut);
      dataLength = PackFCState(buffer,&fcState);
      pthread_mutex_unlock(&fcMut);
      server_send_packet(&server, FC_STATE, buffer, dataLength);
      // send ap state packet
      pthread_mutex_lock(&apStateMut);
      dataLength = PackAPState(buffer,&apState);
      server_send_packet(&server, AUTOPILOT_STATE, buffer, dataLength);
      pthread_mutex_unlock(&apStateMut);
      init = 0;
      // reinitialise startTime
      gettimeofday(&timestamp, NULL); 
      startServerTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
    }   
  }
  return NULL;
}

void* updateMCU(void *pointer)
{
  uint8_t mcuMode = 0;
  uint16_t readEngine[4];
  static int mcuDelayCount = 0;
  while (1)
  {
    //fprintf(stderr,"void* updateMCU()");
    
    // Send Data
    pthread_mutex_lock(&apMutex);
    sendMCUCommands(&apMode, &apThrottle, &apRoll, &apPitch, &apYaw);
    pthread_mutex_unlock(&apMutex);

    // Get RC Data
    getMCUPeriodic(&mcuMode,readEngine);
    
    // RC Commands
    pthread_mutex_lock(&rcMutex);
    rcMode = mcuMode;
    rcThrottle = PWMToCounter(readEngine[0] - zeroThrottle);
    rcRoll = PWMToCounter(readEngine[1] - zeroRoll);
    rcPitch = PWMToCounter(readEngine[2] - zeroPitch);
    rcYaw = PWMToCounter(-readEngine[3] + zeroYaw);

    if (rcThrottle <= 0)
    {
      rollTrim = rcRoll;
      pitchTrim = rcPitch;
      yawTrim = rcYaw;
      fprintf(stderr,">> RC Trims Set...");
    }
    pthread_mutex_unlock(&rcMutex);
    
    // Report to GCS
    pthread_mutex_lock(&fcMut);
    if (mcuMode == FAIL_SAFE)
    {
      fcState.rclinkActive = 0;
    }
    else
    {
        fcState.rclinkActive = 1;
    }
    fcState.commandedEngine1 = readEngine[0];
    fcState.commandedEngine2 = readEngine[1];
    fcState.commandedEngine3 = readEngine[2];
    fcState.commandedEngine4 = readEngine[3];
    pthread_mutex_unlock(&fcMut);
    
    usleep(MCU_UPDATE_DELAY*1e3);
  }
  
  return NULL;
}

void* updateControl(void *pointer)
{ 
  // initialise gains and parameters from files
  int i = 0, j = 0;
  FILE *gainsfd = NULL;
  double gains[6][3];
  FILE *parametersfd = NULL; 
  double parameters[6][3];
  
  gainsfd = fopen("gains.ahnsgains","r");
  parametersfd = fopen("parameters.ahnsparameters","r"); 
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
    fclose(gainsfd);
    fclose(parametersfd); 
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
    

  // control thread states
  double x = 0, y = 0, z = 0;
  double vx = 0, vy = 0, vz = 0;
  double phi = 0, theta = 0, psi = 0;
  double p = 0, q = 0, r = 0;
  // time stamp
  struct timeval timestamp1;
  // time between updates
  double startControlTime, endControlTime, diffControlTime;
  // calculate filter start time
  gettimeofday(&timestamp1, NULL);
  startControlTime = timestamp1.tv_sec+(timestamp1.tv_usec/1000000.0);
  while(1)
  {
    //fprintf(stderr,"void* updateControl()");
   
    //Sort Vicon or State Data 
    pthread_mutex_lock(&mut);
    pthread_mutex_lock(&viconMutex);
    if (xLoop.vicon)
    {
      x = viconState.x;
      vx = viconState.vx;
    }
    else
    {
      x = state.x;
      vx = state.vx;
    }
 
    if (yLoop.vicon)
    {
      y = viconState.y;
      vy = viconState.vy;
    }
    else
    {
      y = state.y;
      vy = state.vy;
    }
 
    if (zLoop.vicon)
    {
      z = viconState.z;
      vz = viconState.vz;
    }
    else
    {
      z = state.z;
      vz = state.vz;
    }
    
    if (rollLoop.vicon)
    {
      phi = viconState.phi;
      p = state.p;
    }
    else
    {
      phi = state.phi;
      p = state.p;
    }
    
    if (pitchLoop.vicon)
    {
      theta = viconState.theta;
      q = state.q;
    }
    else
    {
      theta = state.theta;
      q = state.q;
    }
   
    if (yawLoop.vicon)
    {
      psi = viconState.psi;
      r = state.r;
    }
    else
    { 
      psi = state.psi;
      r = state.r;
    }
    pthread_mutex_unlock(&viconMutex);
    pthread_mutex_unlock(&mut); 
    
    // Update Guidance Loops
    pthread_mutex_lock(&xLoopMutex);
    pthread_mutex_lock(&yLoopMutex);
    // Calculate Position Error in Vicon Frame
    double xError_v = xLoop.reference - x;
    double yError_v = yLoop.reference - y;
    // Rotate Position Error Displacement Vector into Body Frame
    double xError_b = xError_v * cos(psi) + yError_v * sin(psi);
    double yError_b = xError_v * cos(psi) - yError_v * sin(psi);
    // Rotate Velocity Vector into Body Frame
    double vx_b = vx * cos(psi) + vy * sin(psi);
    double vy_b = vy * cos(psi) - vy * sin(psi);
    // Feed these errors to the PID Guidance Loops
    // x Loop
    updateGuidanceLoop(&xLoop,xError_b,x,vx_b); 
    if (xLoop.active)
    {
      pitchLoop.reference = xLoop.output;
    }
    // y Loop
    updateGuidanceLoop(&yLoop,yError_b,y,vy_b);
    if (yLoop.active)
    {
      rollLoop.reference = yLoop.output;
    }
    pthread_mutex_unlock(&yLoopMutex);
    pthread_mutex_unlock(&xLoopMutex);

    // Altitude Loop
    pthread_mutex_lock(&zLoopMutex);
    // Assume the quad is horizontally level
    double zError = zLoop.reference - z;
    fprintf(stderr,"zError = %lf\n",zError);
    updateGuidanceLoop(&zLoop,zError,z,vz);
    pthread_mutex_unlock(&zLoopMutex);

#ifdef _GYRO_
    // RC Gyro will do the mixing so roll, pitch and yaw loops will not be active   
    // Yaw Loop as Heading Hold
    pthread_mutex_lock(&yawLoopMutex);
    updateYawLoop(&yawLoop,phi,r);
    pthread_mutex_unlock(&yawLoopMutex);

    // Generate Commands for the MCU 
    MutexLockAllLoops();
    pthread_mutex_lock(&apMutex);

    if (apMode == FAIL_SAFE)
    {
      apMode = FAIL_SAFE;
      //printf("CONTROL >> COMMANDED FAILSAFE");
    }
    else if (zLoop.active && xLoop.active && yLoop.active && yawLoop.active)
    {
      apMode = RC_NONE;
    }
    else if (!zLoop.active && xLoop.active && yLoop.active && yawLoop.active)
    {
      apMode = RC_THROTTLE;
    }
    else if (zLoop.active && !yLoop.active && xLoop.active && yawLoop.active)
    {
      apMode = RC_ROLL;
    }
    else if (zLoop.active && yLoop.active && !xLoop.active && yawLoop.active)
    {
      apMode = RC_PITCH;
    }
    else if (zLoop.active && yLoop.active && xLoop.active && !yawLoop.active)
    {
      apMode = RC_YAW;
    }
    else if (!zLoop.active && !yLoop.active && xLoop.active && yawLoop.active)
    {
      apMode = RC_THROTTLE_ROLL;
    }
    else if (!zLoop.active && yLoop.active && !xLoop.active && yawLoop.active)
    {
      apMode = RC_THROTTLE_PITCH;
    }
    else if (!zLoop.active && yLoop.active && xLoop.active && !yawLoop.active)
    {
      apMode = RC_THROTTLE_YAW;
    }
    else if (!zLoop.active && !yLoop.active && !xLoop.active && yawLoop.active)
    {
      apMode = RC_THROTTLE_ROLL_PITCH;
    }
    else if (!zLoop.active && !yLoop.active && xLoop.active && !yawLoop.active)
    {
      apMode = RC_THROTTLE_ROLL_YAW;
    }
    else if (!zLoop.active && yLoop.active && !xLoop.active && !yawLoop.active)
    {
      apMode = RC_THROTTLE_PITCH_YAW;
    }
    else if (zLoop.active && !yLoop.active && !xLoop.active && yawLoop.active)
    {
      apMode = RC_ROLL_PITCH;
    }
    else if (zLoop.active && !yLoop.active && xLoop.active && !yawLoop.active)
    {
      apMode = RC_ROLL_YAW;
    }
    else if (zLoop.active && yLoop.active && !xLoop.active && !yawLoop.active)
    {
      apMode = RC_PITCH_YAW;
    }
    else if (zLoop.active && !yLoop.active && !xLoop.active && !yawLoop.active)
    {
      apMode = RC_ROLL_PITCH_YAW;
    }

    // Gyro will handle the inner loops
    apThrottle = zLoop.output;
    apRoll = yLoop.output;
    apPitch = xLoop.output;
    apYaw = yawLoop.output;

#else
    // Update Attitude Loops
    // No RC Gyro thus roll, pitch and yaw control loops in use
    pthread_mutex_lock(&rcMutex);

    double rcFactor = 1.0;
    if (rcMode == AUGMENTED) // use the gyro rates
    {
      // Roll Rate Control
      pthread_mutex_lock(&rollLoopMutex);
      rollLoop.reference = CounterToPWM(rcFactor*rcRoll) + zeroRoll;
      rollLoop.referenceDot = rollLoop.previousState; // Roll Loop D term is now Kd*(pNew - pOld);
      // Scale Gyro Rates to 1000 to 2000 us based on +-300 deg/s IMU
      p = MapCommands(p*180.0/M_PI,300.0,-300.0,2000.0,1000.0);
      updateControlLoop(&rollLoop,p,p);
      // Scale Output to Timer Value
      rollLoop.output = PWMToCounter(rollLoop.output);
      pthread_mutex_unlock(&rollLoopMutex);

      // Pitch Rate Control
      pthread_mutex_lock(&pitchLoopMutex);
      pitchLoop.reference = CounterToPWM(rcFactor*rcPitch) + zeroPitch;
      pitchLoop.referenceDot = pitchLoop.previousState; // Pitch Loop D term is now Kd*(qNew - qOld);
      // Scale Gyro Rates to 1000 to 2000 us based on +-300 deg/s IMU
      q = MapCommands(q*180.0/M_PI,300.0,-300.0,2000.0,1000.0);
      updateControlLoop(&pitchLoop,q,q);
      // Scale Output to Timer Value
      pitchLoop.output = PWMToCounter(pitchLoop.output);
      pthread_mutex_unlock(&pitchLoopMutex);
    
    }
    else if (rcMode == AUTOPILOT) // use the filtered angles
    {
      pthread_mutex_lock(&rollLoopMutex);
      pthread_mutex_lock(&pitchLoopMutex);

      // TODO actually give these dedicated packets
      // at the moment these are coming from the angular loops
      levelRollLoop.Kp = rollLoop.maximum;
      levelRollLoop.Ki = rollLoop.minimum;
      levelPitchLoop.Kp = pitchLoop.maximum;
      levelPitchLoop.Ki = pitchLoop.minimum;

      // Level Control
      rollError = MapCommands(rcRoll,PWMToCounter(500),PWMToCounter(-500),M_PI/4.0,-M_PI/4.0) - phi;
      pitchError = MapCommands(rcPitch,PWMToCounter(500),PWMToCounter(-500),M_PI/4.0,-M_PI/4.0) + theta;
      levelAdjust[LEVEL_ROLL] = rollError*levelRollLoop.Kp;
      levelAdjust[LEVEL_PITCH] = pitchError*levelPitchLoop.Kp;
      // Check if the controller should try and return to hover
      if ((abs(rcRoll - rollTrim) > levelOff) || (abs(rcPitch - pitchTrim) > levelOff))
      {
        levelRollLoop.integralError = 0.0;
        levelPitchLoop.integralError = 0.0;
      }
      else
      {
        levelRollLoop.integralError += (rollError * diffControlTime) * levelRollLoop.Ki;
	if (abs(levelRollLoop.integralError) > levelLimit)
	{
	  levelRollLoop.integralError = levelLimit;
	}

        levelPitchLoop.integralError = (pitchError * diffControlTime) * levelPitchLoop.Ki;
	if (abs(levelPitchLoop.integralError) > levelLimit)
	{
	  levelPitchLoop.integralError = levelLimit;
	}
      }

      // Roll Control
      rollLoop.reference = CounterToPWM(rcFactor*rcRoll) + zeroRoll + levelAdjust[LEVEL_ROLL];
      rollLoop.referenceDot = rollLoop.previousState; // Roll Loop D term is now Kd*(pNew - pOld);
      // Scale Gyro Rates to 1000 to 2000 us based on +-300 deg/s IMU
      p = MapCommands(p*180.0/M_PI,300.0,-300.0,2000.0,1000.0);
      updateControlLoop(&rollLoop,p,p);
      // Scale Output to Timer Value
      rollLoop.output = PWMToCounter(rollLoop.output + levelRollLoop.integralError);

      // Pitch Control   
      pitchLoop.reference = CounterToPWM(rcFactor*rcPitch) + zeroPitch + levelAdjust[LEVEL_PITCH];
      pitchLoop.referenceDot = pitchLoop.previousState; // Pitch Loop D term is now Kd*(qNew - qOld);
      // Scale Gyro Rates to 1000 to 2000 us based on +-300 deg/s IMU
      q = MapCommands(q*180.0/M_PI,300.0,-300.0,2000.0,1000.0);
      updateControlLoop(&pitchLoop,q,q);
      // Scale Output to Timer Value
      pitchLoop.output = PWMToCounter(pitchLoop.output + levelPitchLoop.integralError);
      
      pthread_mutex_unlock(&pitchLoopMutex);
      pthread_mutex_unlock(&rollLoopMutex);
    }
    
    // Yaw Control
    pthread_mutex_lock(&yawLoopMutex);
    yawLoop.reference = CounterToPWM(rcFactor*rcYaw) + zeroYaw;
    yawLoop.referenceDot = yawLoop.previousState; // Yaw Loop D term is now Kd*(rNew - rOld);
    // Scale Gyro Rates to 1000 to 2000 us based on +-300 deg/s IMU
    r = MapCommands(r*180.0/M_PI,300.0,-300.0,2000.0,1000.0);
    updateControlLoop(&yawLoop,r,r);
    // Scale Output to Timer Value
    yawLoop.output = PWMToCounter(yawLoop.output);
    pthread_mutex_unlock(&yawLoopMutex);
    
    pthread_mutex_unlock(&rcMutex);
    
    // Generate Commands for the MCU 
    MutexLockAllLoops();
    pthread_mutex_lock(&apMutex);
    
    if (apMode == FAIL_SAFE)
    {
      apMode = FAIL_SAFE;
      //printf("CONTROL >> COMMANDED FAILSAFE");
    }
    else if (zLoop.active && xLoop.active && yLoop.active && yawLoop.active)
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
    
    // AP Handles the Inner Loop
    apThrottle = zLoop.output;
    apRoll = rollLoop.output;
    apPitch = pitchLoop.output;
    apYaw = yawLoop.output;
#endif
    pthread_mutex_unlock(&apMutex);
   
    // Update AP State
    pthread_mutex_lock(&apStateMut); 
    apState.referencePhi = rollLoop.reference;
    apState.referenceTheta = pitchLoop.reference;
    apState.referencePsi = yawLoop.reference;
    apState.referenceX = xLoop.reference;
    apState.referenceY = yLoop.reference;
    apState.referenceZ = zLoop.reference;
    
    apState.phiActive = rollLoop.active;
    apState.thetaActive = pitchLoop.active;
    apState.psiActive = yawLoop.active;
    apState.xActive = xLoop.active;
    apState.yActive = yLoop.active;
    apState.zActive = zLoop.active;
    pthread_mutex_unlock(&apStateMut); 
    
    MutexUnlockAllLoops();
    // calculate the control thread update time
    gettimeofday(&timestamp1, NULL);
    endControlTime = timestamp1.tv_sec+(timestamp1.tv_usec/1000000.0);
    diffControlTime = endControlTime - startControlTime;
    startControlTime = timestamp1.tv_sec+(timestamp1.tv_usec/1000000.0);
    printf(">> Control update: %lf\n",1/diffControlTime);
    
    usleep(CONTROL_DELAY*1e3); 
  }
  return NULL;
}
