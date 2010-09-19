/**
 * @file   kf.h
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
 * Kalman filter library
 *
 */

#ifndef KF_H
#define KF_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

// function defintions
int attitudeFilterInitialise();
int attitudeFilter(double *rateXd, double *rateYd, double *rateZd, double *accXd, double *accYd, double *accZd, double *rateXf, double *rateYf, double *rateZf, double *phif, double *thetaf, double *psif, double compass, double diffTime);

#endif
