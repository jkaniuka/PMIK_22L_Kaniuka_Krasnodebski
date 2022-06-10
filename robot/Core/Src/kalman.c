/**
  ******************************************************************************
  * @file           : kalman.c
  * @brief   This file contains an implementation of the Kalman filter.
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

#include "kalman.h"


/**
  * @brief  Function to initialize Kalman filter.
  * @param[in] *Kalman Pointer to a structure that stores filter parameters
  * @param[in] mea_t Measurement error
  * @param[in] est_e Estimation error
  * @param[in] q Noise in the system
  * @retval None
  */
void KalmanFilter(Kalman_t *Kalman, uint16_t mea_e, uint16_t est_e, uint16_t q)
{
	Kalman->_err_measure=mea_e;
	Kalman->_err_est=est_e;
	Kalman->_q = q;
}

/**
  * @brief  Function for calculating a measurement estimate.
  * @param[in] *Kalman Pointer to a structure that stores filter parameters
  * @param[in] mea Measurement
  * @param[out] _curr_est Measurement estimate
  */
uint16_t estimate(Kalman_t *Kalman, uint16_t mea)
{
	Kalman->_kalman_gain = (Kalman->_err_est)/((Kalman->_err_est) + (Kalman->_err_measure));
	Kalman->_curr_est = Kalman->_last_est + Kalman->_kalman_gain * (mea - Kalman->_last_est);
	Kalman->_err_est =  (1.0 - Kalman->_kalman_gain)*Kalman->_err_est + fabs(Kalman->_last_est-Kalman->_curr_est)*Kalman->_q;
	Kalman->_last_est=Kalman->_curr_est;

  return Kalman->_curr_est;
}
