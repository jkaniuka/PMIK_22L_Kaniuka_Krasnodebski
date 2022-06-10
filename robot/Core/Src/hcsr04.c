/**
  ******************************************************************************
  * @file           : hcsr04.c
  * @brief   This file contains the distance measurement implementation
  *			for the HCSR04 distance sensor.
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

#include "main.h"
#include "hcsr04.h"

///Timer for distance measurement
TIM_HandleTypeDef *htim_hcsr04;

/// Volatile variable to store distance measurement
volatile uint16_t  hcsr04_Distance;


/**
  * @brief  Function to get current measurement value.
  * @param[in] *Result Pointer on the variable receiving the measurement
  * @retval None
  */
void HCSR04_Read(uint16_t *Result)
{
	*Result =  hcsr04_Distance;

}

/**
  * @brief  Function to determine the value of the measured distance.
  * @param[in] *htim Pointer on the timer that triggers the measurement
  * @retval None
  */
void HCSR04_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == htim_hcsr04)
	{
		uint16_t time;

		time = (uint16_t)((uint16_t)__HAL_TIM_GetCompare(htim_hcsr04, HCSR04_STOP_CHANNEL) - (uint16_t)__HAL_TIM_GetCompare(htim_hcsr04, HCSR04_START_CHANNEL));

		  /** Saving the distance to a volatile variable
		  */
		hcsr04_Distance = time / 58;


		HAL_TIM_IC_Start_IT(htim_hcsr04, HCSR04_STOP_CHANNEL);
	}
}

/**
  * @brief  Init HCSR04 timers.
  * @retval None
  */
void HCSR04_Init(TIM_HandleTypeDef *htim)
{
	htim_hcsr04 = htim;

	HAL_TIM_Base_Start(htim_hcsr04);
	HAL_TIM_PWM_Start(htim_hcsr04, HCSR04_PWM_CHANNEL);
	HAL_TIM_IC_Start(htim_hcsr04, HCSR04_START_CHANNEL);
	HAL_TIM_IC_Start_IT(htim_hcsr04, HCSR04_STOP_CHANNEL);

}
