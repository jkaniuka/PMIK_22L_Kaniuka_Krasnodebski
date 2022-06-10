/**
  ******************************************************************************
  * @file           : motor.c
  * @brief   This file contains code to configure and operate the motors.

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
#include "motor.h"

///Timer for PWM
TIM_HandleTypeDef *htim_motors;


/**
  * @brief  Top-level function for PWM control of motors.
  * @param[in] pwmLeft Left motor PWM
  * @param[in] pwmRight Right motor PWM
  * @retval None
  */
void setMotorPWM(int16_t pwmLeft, int16_t pwmRight){

	setActuator(MOTOR_LEFT, pwmLeft);
	setActuator(MOTOR_RIGHT, pwmRight);
}

/**
  * @brief  Selection and operation of the actuator.
  * @retval None
  */
void setActuator(char type, int16_t value){

  switch (type){
    case MOTOR_RIGHT:setL298N(M1_DIR_GPIO_Port, M1_DIR_Pin, 15, value); break;
    case MOTOR_LEFT:setL298N(M2_DIR_GPIO_Port, M2_DIR_Pin, 16, value); break;
  }
}

/**
  * @brief  Control L298N motor driver.
  * @retval None
  */
void setL298N(GPIO_TypeDef * portPinDir, int pinDir, int pinPWM, int16_t speed){
  if (speed < 0){
	HAL_GPIO_WritePin(portPinDir, pinDir, GPIO_PIN_SET);
	setPwmDrivenByTimer(pinPWM, 10000 - abs(speed));
  } else {
	HAL_GPIO_WritePin(portPinDir, pinDir, GPIO_PIN_RESET);
	  setPwmDrivenByTimer(pinPWM, speed);
  }
}

/**
  * @brief  Set PWM Pulse on Timer Channels.
  * @retval None
  */
void setPwmDrivenByTimer(uint8_t pwm, int16_t duty) {
    switch (pwm) {
        case 15: TIM3->CCR1 = duty; break;
        case 16: TIM3->CCR2 = duty; break;

        default: break;
    }
}

/**
  * @brief  Init motors timers.
  * @retval None
  */
void MOTORS_Init(TIM_HandleTypeDef *htim) {
	htim_motors = htim;

	HAL_TIM_Base_Start_IT(htim_motors);
	HAL_TIM_PWM_Start(htim_motors, MOTOR_M1_CHANNEL);
	HAL_TIM_PWM_Start(htim_motors, MOTOR_M2_CHANNEL);

}




