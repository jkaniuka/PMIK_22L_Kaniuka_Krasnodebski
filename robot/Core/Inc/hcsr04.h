/**
  ******************************************************************************
  * @file           : hcsr04.h
  * @brief   This file contains all the function prototypes for
  *          the hcsr04.h file
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
/// Timer channel to generate output PWM
#define HCSR04_PWM_CHANNEL TIM_CHANNEL_3
/// Timer Channel to store the measurement start time
#define HCSR04_START_CHANNEL TIM_CHANNEL_2
/// Timer Channel to store the measurement stop time
#define HCSR04_STOP_CHANNEL TIM_CHANNEL_1

/// Function to get current measurement value.
void HCSR04_Read(uint16_t *Result);

/// Function to determine the value of the measured distance.
void HCSR04_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

/// Init HCSR04 timers.
void HCSR04_Init(TIM_HandleTypeDef *htim);
