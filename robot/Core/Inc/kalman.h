/**
  ******************************************************************************
  * @file           : kalman.h
  * @brief   This file contains all the function prototypes for
  *          the kalman.h file
  ******************************************************************************
  * Authors:
  * Jan Kaniuka
  * Przemysław Krasnodębski
  *
  * Project: Mobile radio-controlled platform.
  *
  * Main functionalities:
  * Execution of movement commands received by radio.
  * Protection against collision with the environment
  * Measurement of movement parameters (current consumption, distance from obstacles)
  * Sending motion parameters to a transmitter.
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>

/// Struct to contain Kalman Filter parameters
volatile typedef struct
{
	float _err_measure;
	float _err_est;
	float _q;
	uint16_t _curr_est;
	uint16_t _last_est;
	float _kalman_gain;

} Kalman_t;

/// Function to initialize Kalman filter.
void KalmanFilter(Kalman_t *Kalman,uint16_t mea_e, uint16_t est_e, uint16_t q);

/// Function for calculating a measurement estimate.
uint16_t estimate(Kalman_t *Kalman, uint16_t mea);
