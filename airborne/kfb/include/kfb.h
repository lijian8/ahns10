/**
 * @file   kfb.h
 * @author Liam O'Sullivan
 *
 * $Author: liamosullivan $
 * $Date: 2010-08-28 11:26:11 +1000 (Sat, 28 Aug 2010) $
 * $Rev: 324 $
 * $Id: kf.h 324 2010-08-28 01:26:11Z liamosullivan $
 *
 * Queensland University of Technology
 *
 * @section DESCRIPTION
 * Kalman filter library (revision b)
 *
 */

#ifndef KFB_H
#define KFB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// Kalman filter constants for phi (Q1=0.001,Q2=0.003)
#define PHI_ANGLE_Q 0.057296
#define PHI_GYRO_Q 0.171887
#define PHI_R 1.7
#define PHI_DIRECTION 1.0

// Kalman filter constants for theta
#define THETA_ANGLE_Q 0.057296
#define THETA_GYRO_Q 0.171887
#define THETA_R 1.7
#define THETA_DIRECTION -1.0

// Kalman filter constants for psi
#define PSI_ANGLE_Q 0.057296
#define PSI_GYRO_Q 0.171887
#define PSI_R 5.0
#define PSI_DIRECTION -1.0

// Euler angle calibration cycles
#define CYCLES 1000

// LPF alpha constants for accelerometers
#define ACCX_ALPHA 0.1
#define ACCY_ALPHA 0.1
#define ACCZ_ALPHA 0.1

// LPF alpha constants for gyro rates
#define RATEX_ALPHA 0.1
#define RATEY_ALPHA 0.1
#define RATEZ_ALPHA 0.1

// LPF alpha constant for compass reading
#define COMPASS_ALPHA 0.1

// Data logger flag
#define DATA_LOGGER 0

// struct definition for each axis
typedef struct _axis {
  // x state (tracking angle and gyro bias, 1x2)
  double X[2];
  // p covariance matrix (2x2)
  double P[2][2];
  // s term
  double S;
  // y measurement term
  double Y;
  // error term
  double err;
  // L gain matrix (1x2)
  double L[2];
  // Q process noise term (variance for angle and gyro bias, 1x2)
  double Q[2];
  // R measurement noise term
  double R;
  // direction of axis
  double direction;
  // angle offset
  double offset;
} axis;

// function defintions
int attitudeFilterInitialiseB(double *accXr, double *accYr, double *accZr);
int attitudeFilterB(double *rateXr, double *rateYr, double *rateZr, double *accXr, double *accYr, double *accZr, double *rateXf, double *rateYf, double *rateZf, double *phif, double *thetaf, double *psif, double *compassZr, double dT);
int kFilterTimeUpdate(axis *axis_t, double *gyroRate, double dT);
int kFilterMeasureUpdate (axis *axis_t);
double coarsePitchAngle(double *accXr, double *accYr, double *accZr);
double coarseRollAngle(double *accXr, double *accYr, double *accZr);
int accLPF (double *accXr, double *accYr, double *accZr, double dT);
int rateLPF(double *rateXf, double *rateYf, double *rateZr);
int compassLPF(double *compass_heading);
int calibrateEulerAngles(double *phif, double *thetaf, double *psif);
int printkFilterData(double *rateXr, double *rateYr, double *rateZr, double *accXr, double *accYr, double *accZr, double dT);

#endif
