/**
 * @file   filterTester.c
 * @author Liam O'Sullivan
 *
 * $Author: liamosullivan $
 * $Date: 2010-06-21 15:25:25 +1000 (Mon, 21 Jun 2010) $
 * $Rev: 193 $
 * $Id: imuServerTester.c 193 2010-06-21 05:25:25Z liamosullivan $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Driver file to test the IMU serial library and transmit UDP packets with
 * the UDP server
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include "state.h"
#include "server.h"
#include "commands.h"
#include "udp.h"
#include "imuserial.h"

#define PI 3.14159265

int verbose = 0;
int server_port	= 2002;
// gravity
const double gravity = 9.80665;
double gravity_offset = 0;

char *server_host;
char *imu_serial_port;
static Server server;
unsigned char init = 1;
struct timeval timestamp,local_time;
char *file;
char text[128];
int temp;
int dataLength;
unsigned char buffer[512];

// IMU timing
double startTime, endTime, diffTime;

// Kalman filter matrices
// formulate the state matrix (x) (t-1)
gsl_matrix *state_x_previous;
// formulate the one step ahead prediction matrix (x) (t-)
gsl_matrix *state_x_one_step;
// formulate the state transition matrix (a)
gsl_matrix *state_a;
// formulate the state transmition matrix (a') 
gsl_matrix *state_a_t;
// formulate the state transmition matrix (b) 
gsl_matrix *state_b;
// formulate the state transmition matrix (b) 
gsl_matrix *state_b1;
// formulate the state transmition matrix (u) 
gsl_matrix *state_u;
// formulate the state covariance matrix (Q)
gsl_matrix *state_Q;
// formulate the error covariance matrix (p) (t-1)
gsl_matrix *state_p_previous;
// formulate the error covariance matrix (p) one step
gsl_matrix *state_p_one_step;
// formulate the selection matrix (H)
gsl_matrix *state_H;
// formulate the selection matrix (H')
gsl_matrix *state_H_t;
// formulate the covariance matrix (R)
gsl_matrix *state_R;
// formulate the Kalman gain matrix (K)
gsl_matrix *state_K;
// formulate the intermediate Kalman gain matrices (K1)
gsl_matrix *state_K1;
// formulate the intermediate Kalman gain matrices (K2)
gsl_matrix *state_K2;
// formulate the intermediate Kalman gain matrices (K3)
gsl_matrix *state_K3;
// formulate the intermediate Kalman gain matrices (K4)
gsl_matrix *state_K4;
// formulate the intermediate Kalman gain matrices (K5)
gsl_matrix *state_K5;
// formulate the intermediate Kalman gain matrices (K6)
gsl_matrix *state_K6;
// formulate the permutation matrix p for inversion
gsl_permutation *p;
// formulate the measurements matrix (y)
gsl_matrix *state_y;
// formulate the predicated matrix (x)
gsl_matrix *state_x;
// formulate the intermediate Kalman gain matrices (K7)
gsl_matrix *state_K7;
// formulate the error covariance matrix (p)
gsl_matrix *state_p;

// function defintions
int attitudeFilterInitialise();
int attitudeFilter(double *rateXd, double *rateYd, double *rateZd, double *accXd, double *accYd, double *accZd, double *phif, double *thetaf, double *psif);

int main(int argc, char *argv[])
{
  const char *program = argv[0];
  int c, errflag, temp = 0;

  server_host = (char *) malloc(100*sizeof(char));
  imu_serial_port = (char *) malloc(100*sizeof(char));
  file = (char *) malloc(100*sizeof(char));

  if(server_host == NULL)
  {
    fprintf(stderr, "Cannot allocate mem\n");
    exit(-1);
  }

  // default server host
  server_host = "127.0.0.1";
  // default imu serial port
  imu_serial_port = "/dev/ttyUSB0";

  while((c = getopt(argc, argv, "i:s:p:v")) != -1)
  {
    switch(c)
    {
      case 'p':
        server_port = atoi(optarg);
        fprintf(stderr, "port: %d\n", server_port);
        break;
      case 's':
        server_host = optarg;
        fprintf(stderr, "server: %s\n", server_host);
        break;
      case 'v':
        verbose = atoi(optarg);
        fprintf(stderr, "verbose %d\n", verbose);
        break;
      case 'i':
        imu_serial_port = optarg;
        fprintf(stderr, "IMU serial port: %s\n", imu_serial_port);
        break;
      case '?':
        ++errflag;
        break;
    }
  }
  // connect to the IMU
  if (!openSerial(imu_serial_port, BAUD_RATE_DEFAULT))
  {
    fprintf(stderr,"Cannot connect to IMU\n");
    return -1;
  }
  //gravity_offset = gravity*0.99;
  // save the startTime
  gettimeofday(&timestamp, NULL); 
  startTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);

  msg_camera_t cam_msg;
  // initialise the state variables
  state_t state;
  state_t state_old;
  // test for the sensor data
  sensor_data_t raw_IMU;

  
  int count=0;
  int type;
    
  server_init(&server, 2002); 
  server_handle(&server, CAM_STATE, server_cam_state, (void*) &cam_msg);
  memset((void *)(&cam_msg), 0, sizeof(msg_camera_t));
  memset((void *)(&state), 0, sizeof(state_t));
  memset((void *)(&state_old), 0, sizeof(state_t));
  memset((void *)(&raw_IMU), 0, sizeof(sensor_data_t));

  attitudeFilterInitialise();

  while(1)
  {
    //gettimeofday(&local_time,0);
    int type=0;
    if(server_poll(&server,1))
    {
      type = server_get_packet(&server);
    }    
    if(type < 0)
    {
      fprintf(stderr,"Read error from server");
      return -1;
    }
    count++;
    //usleep(1000000); // 100hz
    //send data to the socket
    if(count%1 == 0)
    {
      init = 1;
    }

    if(init)
    {
      //memset((void *)(&state), 0, sizeof(state_t));
      // get the IMU sensor data
      //getImuSensorData(&state.p, &state.q, &state.r, &state.ax, &state.ay, &state.az);
      getImuSensorData(&raw_IMU.p, &raw_IMU.q, &raw_IMU.r, &raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az);
      // calculate the time elapsed since last update
      gettimeofday(&timestamp, NULL); 
      endTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
      diffTime = endTime - startTime;
      // calculate attitude filter values
      //attitudeFilter(&state.p, &state.q, &state.r, &state.ax, &state.ay, &state.az, &state.vx, &state.vy, &state.vz);
      attitudeFilter(&raw_IMU.p, &raw_IMU.q, &raw_IMU.r, &raw_IMU.ax, &raw_IMU.ay, &raw_IMU.az, &state.phi, &state.theta, &state.psi);
      //state.trace = state_filter.phi;
      //state.ay = state_filter.theta;
      //state.az = state_filter.psi;
      // calculate the displacement in x,y and z
      //state.x = state_old.x + diffTime*((state_old.vx+state.vx)/2);
      //state.y = state_old.y + diffTime*((state_old.vy+state.vy)/2);
      //state.z = state_old.z + diffTime*((state_old.vz+state.vz)/2);
      // calculate the velocity in x,y and z
      //state.vx = state_old.vx + diffTime*((state_old.ax*gravity+state.ax*gravity)/2);
      //state.vy = state_old.vy + diffTime*((state_old.ay*gravity+state.ay*gravity)/2);
      //state.vz = state_old.vz + diffTime*(((state_old.az*gravity+state.az*gravity)/2)-gravity_offset);
      
      // calculate the filtered Euler rates
      state.p = ((state.phi - state_old.phi)/diffTime);//*PI/180;
      state.q = ((state.theta - state_old.theta)/diffTime);//*PI/180;
      state.r = ((state.psi - state_old.psi)/diffTime);//*PI/180;

      // calculate the Euler angles
      //state.phi = state_old.phi + diffTime*(state.p);
      //state.theta = state_old.theta + diffTime*(state.q);
      //state.psi = state_old.psi + diffTime*(state.r);
      state.vy = state_old.vy + diffTime*(raw_IMU.p);
      state.vx = state_old.vx + diffTime*(raw_IMU.q);
      state.vz = state_old.vz + diffTime*(raw_IMU.r);
      // approx Euler angles estimate
      state.y = atan2(raw_IMU.ay,raw_IMU.az) * 180/PI;
      state.x = atan2(raw_IMU.ax,raw_IMU.az) * 180/PI;
      state.z = atan2(raw_IMU.ax,raw_IMU.ay) * 180/PI;
      // calculate the frequency (save in voltage)
      state.voltage = 1/(diffTime);
      // save the old state values
      state_old = state;
      // restart the clock TODO: this should be moved 
      gettimeofday(&timestamp, NULL); 
      startTime=timestamp.tv_sec+(timestamp.tv_usec/1000000.0);
      // output the sent states  
      printf( "\n\nSending state \n"),
        fprintf(stderr,"state : %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
		state.p ,
 		state.q ,
 		state.r ,
                state.phi ,
                state.theta ,
                state.psi ,
		state.ax,
                state.ay, 
                state.az,
		state.vx,
                state.vy, 
                state.vz,
		state.x,
                state.y, 
                state.z,
                state.voltage);
      //gettimeofday(&local_time,0);   
      //fprintf(stderr,"time: %d : %d", local_time.tv_sec,local_time.tv_usec);
      // send the server packet
      server_send_packet(&server, HELI_STATE, &state, sizeof(state_t));
      
      // pack the data
     
      dataLength = PackSensorData(buffer, &raw_IMU);
      server_send_packet(&server, SENSOR_DATA, buffer, dataLength);
      //server_send_packet(&server, SENSOR_DATA, &raw_IMU, sizeof(sensor_data_t)); 
      init = 0;
    }   
  }
  return 0;
}

int attitudeFilterInitialise()
{
  // number of states
  int n = 6;
  // number of measurements
  int m = 3;
  // formulate the state matrix (x) (t-1)
  state_x_previous = gsl_matrix_calloc(n,1);
  // formulate the one step ahead prediction matrix (x) (t-)
  state_x_one_step = gsl_matrix_calloc(n,1);
  // formulate the state transition matrix (a)
  state_a = gsl_matrix_calloc(n,n);
  // formulate the state transmition matrix (a')
  state_a_t = gsl_matrix_calloc(n,n);
  // formulate the B matrix (b)
  state_b = gsl_matrix_calloc(n,m);
  // formulate the intermediate B matrix (B1)
  state_b1 = gsl_matrix_calloc(n,1);
  // formulate the measurement matrix (u)
  state_u = gsl_matrix_calloc(m,1);
  // formulate the state covariance matrix (Q)
  state_Q = gsl_matrix_calloc(n,n);
  // formulate the error covariance matrix (p) (t-1) 
  state_p_previous = gsl_matrix_calloc(n,n);
  // formulate the error covariance matrix (p) one step
  state_p_one_step = gsl_matrix_calloc(n,n);
  // formulate the selection matrix (H)
  state_H = gsl_matrix_calloc(m,n);
  // formulate the selection matrix (H')
  state_H_t = gsl_matrix_calloc(n,m);
  // formulate the covariance matrix (R)
  state_R = gsl_matrix_calloc(m,m);
  // formulate the Kalman gain matrix (K)
  state_K = gsl_matrix_calloc(n,m);
  // formulate the intermediate Kalman gain matrices (K1) -> mxn
  state_K1 = gsl_matrix_calloc(m,n);
  // formulate the intermediate Kalman gain matrices (K2) -> mxm
  state_K2 = gsl_matrix_calloc(m,m);
  // formulate the intermediate Kalman gain matrices (K3) -> mxm
  state_K3 = gsl_matrix_calloc(m,m);
  // formulate the intermediate Kalman gain matrices (K4) -> nxm
  state_K4 = gsl_matrix_calloc(n,m);
  // formulate the intermediate Kalman gain matrices (K5) -> mx1
  state_K5 = gsl_matrix_calloc(m,1);
  // formulate the intermediate Kalman gain matrices (K6) -> nx1
  state_K6 = gsl_matrix_calloc(n,1);
  // formulate the permutation matrix p for inversion -> m
  p = gsl_permutation_calloc (m);
  // formulate the measurements matrix (y)
  state_y = gsl_matrix_calloc(m,1);
  // formulate the predicated matrix (x)
  state_x = gsl_matrix_calloc(n,1);
  // formulate the intermediate Kalman gain matrices (K7) -> nxn
  state_K7 = gsl_matrix_calloc(n,n);
  // formulate the error covariance matrix (p)
  state_p = gsl_matrix_calloc(n,n);

  // allocate the state transition matrix (a) values // not requited
  gsl_matrix_set_identity(state_a);
  gsl_matrix_set(state_a,0,1,-1*diffTime);
  gsl_matrix_set(state_a,2,3,-1*diffTime);
  gsl_matrix_set(state_a,4,5,-1*diffTime);
  // allocate the transpose of state transition
  gsl_matrix_memcpy(state_a_t, state_a);
  gsl_matrix_transpose(state_a_t);
  // allocate values for (H)
  gsl_matrix_set(state_H,0,0,1);
  gsl_matrix_set(state_H,1,2,1);
  gsl_matrix_set(state_H,2,4,1);
  // allocate values for (H')
  gsl_matrix_set(state_H_t,0,0,1);
  gsl_matrix_set(state_H_t,2,1,1);
  gsl_matrix_set(state_H_t,4,2,1);
  // allocate values for (Q)
  gsl_matrix_set(state_Q,0,0,0.001); // expected value for phi
  gsl_matrix_set(state_Q,1,1,0.003); // expected value for phi-bias
  gsl_matrix_set(state_Q,2,2,0.001); // expected value for theta
  gsl_matrix_set(state_Q,3,3,0.003); // expected value for theta-bias
  gsl_matrix_set(state_Q,4,4,0.001); // expected value for psi
  gsl_matrix_set(state_Q,5,5,0.003); // expected value for psi-bias
  // allocate values for (R)
  gsl_matrix_set(state_R,0,0,1); // expected measurement value for phi
  gsl_matrix_set(state_R,1,1,1); // expected measurement value for theta
  gsl_matrix_set(state_R,2,2,1); // expected measurement value for psi
  // initial x conditions
  gsl_matrix_set(state_x_previous,0,0,0);
  gsl_matrix_set(state_x_previous,1,0,0);
  gsl_matrix_set(state_x_previous,2,0,0);
  gsl_matrix_set(state_x_previous,3,0,0);
  gsl_matrix_set(state_x_previous,4,0,0);
  gsl_matrix_set(state_x_previous,5,0,0);
  // initial p conditions
  gsl_matrix_set(state_p_previous,0,0,1);
  gsl_matrix_set(state_p_previous,1,1,1);
  gsl_matrix_set(state_p_previous,2,2,1);
  gsl_matrix_set(state_p_previous,3,3,1);
  gsl_matrix_set(state_p_previous,4,4,1);
  gsl_matrix_set(state_p_previous,5,5,1);
 
  return 1;
}

//fitered values
int attitudeFilter(double *rateXd, double *rateYd, double *rateZd, double *accXd, double *accYd, double *accZd, double *phif, double *thetaf, double *psif)
{
  // signum
  int s = 0;
  // allocated values for (u) -> this is the gyro rates reading
  gsl_matrix_set_zero(state_u);
  gsl_matrix_set(state_u,0,0,*rateXd);
  gsl_matrix_set(state_u,1,0,*rateYd);
  gsl_matrix_set(state_u,2,0,*rateZd);
  // allocate values for (y) -> this is the atan2 reading
  gsl_matrix_set_zero(state_y);
  gsl_matrix_set(state_y,0,0,(atan2(*accYd,*accZd)*180/PI)); // phi measurement
  gsl_matrix_set(state_y,1,0,(atan2(*accXd,*accZd)*180/PI)); // theta measurement
  gsl_matrix_set(state_y,2,0,0); // psi measurement
  // allocate values for (a) -> change diff time
  gsl_matrix_set_identity(state_a);
  gsl_matrix_set(state_a,0,1,-1*diffTime);
  gsl_matrix_set(state_a,2,3,-1*diffTime);
  gsl_matrix_set(state_a,4,5,-1*diffTime);
  // allocate the transpose of state transition
  gsl_matrix_memcpy(state_a_t, state_a);
  gsl_matrix_transpose(state_a_t);
  // allocate values for (b) -> change diff time
  gsl_matrix_set_zero(state_b);
  gsl_matrix_set(state_b,0,0,diffTime);
  gsl_matrix_set(state_b,2,1,diffTime);
  gsl_matrix_set(state_b,4,2,diffTime);
  // allocate values for (p)
  gsl_matrix_set_identity(state_p);

  // Time update 
  // 1. calculate one step ahead prediction of x: (x) (t-) = A * (x) (t-1) + B * u
  // multiply A with (x) (t-1)
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_a, state_x_previous, 0.0, state_x_one_step);
  // multiply B with u -> store in B1
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_b, state_u, 0.0, state_b1);
  // A*x + B*u -> store in state_x_one_step
  gsl_matrix_add(state_x_one_step,state_b1);
  // 2. calculate the error covariance matrix P :  A * P (t-1) * A' + Q
  gsl_matrix_memcpy(state_p_one_step,state_a);
  gsl_matrix_mul_elements(state_p_one_step,state_p_previous);
  gsl_matrix_mul_elements(state_p_one_step, state_a_t);
  gsl_matrix_add(state_p_one_step, state_Q);
  // Measurement update
  // 1. calculate the Kalman gain K : P*H'(H*P*H'+R)^-1
  // 1a : H*P -> K1 TODO: could optimise this part by doing P*H'
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_H, state_p_one_step, 0.0, state_K1);
  // 1b: H*P*H' -> K1*H' -> K2
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_K1, state_H_t, 0.0, state_K2);
  // 1c: H*P*H'+R -> K2+R -> stored in K2
  gsl_matrix_add(state_K2, state_R);
  // 1d: (H*P*H'+R)^-1 -> K3
  // LU decomp
  gsl_linalg_LU_decomp (state_K2,p,&s);
  // compute the inverse and store in K3
  gsl_linalg_LU_invert(state_K2,p,state_K3);
  // 1e: P*H' -> K4
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_p_one_step, state_H_t, 0.0, state_K4);
  // 1f:  P*H'(H*P*H'+R)^-1 -> K4*K3 -> K
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_K4, state_K3, 0.0, state_K);
  // 2. calculate the estimate at time t, x : x_one_step + K*(y - H*x_one_step)
  // 2a: H*x_one_step -> K5
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_H, state_x_one_step, 0.0, state_K5);
  // 2b: y - H*x_one_step -> y-K5 -> store in y
  gsl_matrix_sub(state_y,state_K5);
  // 2c:  K*(y - H*x_one_step) -> K*y -> store in K6
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_K, state_y, 0.0, state_K6);
  // 2d:  x_one_step + K*(y - H*x_one_step -> x_one_step + K6 -> store in x
  gsl_matrix_add(state_x_one_step, state_K6);
  gsl_matrix_memcpy(state_x,state_x_one_step);
  // 3. calculate the error covariance estimate state_p : (I-K*H)*P_one_step
  // 3a: K*H -> K7
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, state_K, state_H, 0.0, state_K7);
  // 3b: (I-K*H) -> state_p - state_K7 -> store in state_p
  gsl_matrix_sub(state_p,state_K7);
  // 3c: (I-K*H)*P_one_step -> state_p * p_one_step -> store in state_p
  gsl_matrix_mul_elements(state_p, state_p_one_step);
  
  // done with time and measurement update
  // need to save some variables for the next kalman filter update (state_x and state_p)
  gsl_matrix_memcpy(state_x_previous,state_x);
  gsl_matrix_memcpy(state_p_previous,state_p);
  // allocate filtered state values 
  //*phif = gsl_matrix_get(state_x_previous,0,0)*PI/180;
  //*thetaf = gsl_matrix_get(state_x_previous,2,0)*PI/180;
  *thetaf = gsl_matrix_get(state_x_previous,0,0);//*PI/180;
  *phif = gsl_matrix_get(state_x_previous,2,0);//*PI/180;
  *psif = gsl_matrix_get(state_x_previous,4,0);//*PI/180;
  return 1;
}



















