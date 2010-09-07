/**
 * @file   kf.c
 * @author Liam O'Sullivan
 *
 * $Author: liamosullivan $
 * $Date: 2010-08-28 11:26:11 +1000 (Sat, 28 Aug 2010) $
 * $Rev: 324 $
 * $Id: kf.c 324 2010-08-28 01:26:11Z liamosullivan $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Kalman filter library which implements:
 * - Initialising the Kalman filter
 * - Performs the attitude filtering
 */

#include "kf.h"

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

// initialise the attitude Kalman filter
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

//filtered values
int attitudeFilter(double *rateXd, double *rateYd, double *rateZd, double *accXd, double *accYd, double *accZd, double *rateXf, double *rateYf, double *rateZf, double *phif, double *thetaf, double *psif, double compass, double diffTime)

{
  // signum
  int s = 0;
  // allocated values for (u) -> this is the gyro rates reading
  gsl_matrix_set_zero(state_u);
  gsl_matrix_set(state_u,0,0,*rateXd);
  gsl_matrix_set(state_u,1,0,*rateYd*-1);
  gsl_matrix_set(state_u,2,0,*rateZd*-1);
  // allocate values for (y) -> this is the atan2 reading
  gsl_matrix_set_zero(state_y);
  gsl_matrix_set(state_y,0,0,(atan2(*accYd,sqrt(*accXd * *accXd + *accZd * *accZd)) * 180/M_PI)); // phi measurement
  gsl_matrix_set(state_y,1,0,(atan2(*accXd,sqrt(*accYd* *accYd + *accZd * *accZd)) * 180/M_PI)); // theta measurement
  //gsl_matrix_set(state_y,2,0,(atan2(sqrt(*accXd * *accXd + *accYd * *accYd),*accZd) * 180/M_PI)); // psi measurement
  gsl_matrix_set(state_y,2,0,compass);

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
  // update the euler rates (radians)
  //*rateXf = ((gsl_matrix_get(state_x,0,0) - gsl_matrix_get(state_x_previous,0,0))/diffTime) * M_PI/180;
  //*rateXf = gsl_matrix_get(state_u,0,0) * M_PI/180;
  //*rateYf = ((gsl_matrix_get(state_x,2,0) - gsl_matrix_get(state_x_previous,2,0))/diffTime) * M_PI/180;
  //*rateYf = gsl_matrix_get(state_u,1,0) * M_PI/180;
  //*rateZf = ((gsl_matrix_get(state_x,4,0) - gsl_matrix_get(state_x_previous,4,0))/diffTime) * M_PI/180;
  //*rateZf = gsl_matrix_get(state_u,2,0) * M_PI/180;
  // need to save some variables for the next kalman filter update (state_x and state_p)
  gsl_matrix_memcpy(state_x_previous,state_x);
  gsl_matrix_memcpy(state_p_previous,state_p);
  
  // allocate filtered state values (degrees)
  //*thetaf = gsl_matrix_get(state_x_previous,0,0);//
  //*phif = gsl_matrix_get(state_x_previous,2,0);//
  *psif = gsl_matrix_get(state_x_previous,4,0);//

  // allocate filtered state values (radians)
  *phif = gsl_matrix_get(state_x_previous,0,0) * M_PI/180;
  *thetaf = gsl_matrix_get(state_x_previous,2,0) * M_PI/180;
  //*psif = gsl_matrix_get(state_x_previous,4,0) * M_PI/180;
  return 1;
}








