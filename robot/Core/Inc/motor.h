/**
  ******************************************************************************
  * @file           : motor.h
  * @brief   This file contains all the function prototypes for
  *          the motor.h file
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
#include <stdlib.h>

/// Timer Channel for M1 motor
#define MOTOR_M1_CHANNEL TIM_CHANNEL_1
/// Timer Channel for M2 motor
#define MOTOR_M2_CHANNEL TIM_CHANNEL_2

/// Actuators
enum {
  MOTOR_LEFT,
  MOTOR_RIGHT,
};


/// Struct to contain PWM values of both motors
typedef struct
{
	int16_t pwmMotorRight;
	int16_t pwmMotorLeft;
} Motors_t;

/// Top-level function for PWM control of motors.
void setMotorPWM(int16_t pwmLeft, int16_t pwmRight);


/// Selection and operation of the actuator.
void setActuator(char type, int16_t value);


/// Control L298N motor driver.
void setL298N(GPIO_TypeDef * portPinDir, int pinDir, int pinPWM, int16_t speed);

/// Set PWM Pulse on Timer Channels.
void setPwmDrivenByTimer(uint8_t pwm, int16_t duty);

/// Init motors timers.
void MOTORS_Init(TIM_HandleTypeDef *htim);
