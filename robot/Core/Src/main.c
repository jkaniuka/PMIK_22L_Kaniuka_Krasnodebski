/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "nRF24/nRF24_Defs.h"
#include "nRF24/nRF24.h"
#include "mpu6050.h"
#include "hcsr04.h"
#include "kalman.h"
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#undef DEBUG
#define USE_RADIO
#define USE_RADIO_SENT
#define USE_HCSR04
#define USE_ADC
#undef USE_IMU

#define MESSAGE_BUFF_LEN 400 // 32 payload + \0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

Motors_t Car1;

#ifdef USE_RADIO
	volatile uint8_t nrf24_rx_flag, nrf24_tx_flag, nrf24_mr_flag;
	uint8_t Message[MESSAGE_BUFF_LEN];
	uint8_t MessageLength;

	uint8_t Message_RX[MESSAGE_BUFF_LEN];
	uint8_t MessageLength_RX;
#endif

#ifdef USE_HCSR04
	/// raw distance from sensor
	uint16_t Distance;
	/// struct with Kalman filter parameter
	volatile Kalman_t HCSR04;
#endif

#ifdef USE_ADC
	/// current measurement
	volatile static uint16_t current[1];
#endif

#ifdef USE_IMU
	MPU6050_t MPU6050;
#endif

/// actual time
uint32_t delayTimer = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  Function to handle the message received by the radio module.
  * @param[in] Message[] Char table
  * @param[in] *DataStruct Pointer on the struct with PWM value
  * @retval None
  */
void MessegeHandle(uint8_t Message[], Motors_t *DataStruct){
	switch(Message[0]){
	case('s'):
		{
		if((Message[4] != '0' && Message[4] != '1' )){
			break;
		}
		char pwmRm[] = {Message[5], Message[6]};
		DataStruct->pwmMotorRight = atoi(pwmRm)*100;
		if(Message[4] == '1'){
			DataStruct->pwmMotorRight = DataStruct->pwmMotorRight*-1;
		}
		char pwmLm[] = {Message[2], Message[3]};
		DataStruct->pwmMotorLeft = atoi(pwmLm)*100;
		if(Message[1] == '1'){
			DataStruct->pwmMotorLeft = DataStruct->pwmMotorLeft*-1;
		}
		if((HCSR04._last_est<20) && ((DataStruct->pwmMotorLeft) > 0 || (DataStruct->pwmMotorRight) > 0)){
			setMotorPWM(0, 0);
		}else{
			setMotorPWM(DataStruct->pwmMotorLeft, DataStruct->pwmMotorRight);
		}

		break;
		}

	default:
		break;
	}
}


/**
  * @brief  Callback to handle received messages.
  * @retval None
  */
void nRF24_EventRxCallback(void){
	delayTimer = HAL_GetTick();
	nRF24_ReadData(Message_RX, &MessageLength_RX);
	if(MessageLength_RX > 0){
		MessegeHandle(Message_RX, &Car1);
#ifdef DEBUG
	HAL_UART_Transmit_IT(&huart2, Message_RX, MessageLength_RX);
#endif
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_I2C2_Init();
  MX_DMA_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
#ifdef USE_HCSR04
  HCSR04_Init(&htim2);
  KalmanFilter(&HCSR04, 2, 2, 3);
  Distance = 50;
  estimate(&HCSR04, Distance);
#endif

  MOTORS_Init(&htim3);
  HAL_TIM_Base_Start_IT(&htim6);

#ifdef USE_RADIO
  nRF24_Init(&hspi1);
  nRF24_SetRXAddress(0, (uint8_t*)"Odb");
  nRF24_SetTXAddress((uint8_t*)"Nad");
  nRF24_RX_Mode();
#endif



  /// time to stop
  uint32_t stopTime = 4000;



#ifdef USE_IMU
  while (MPU6050_Init(&hi2c2) == 1);
#endif

#ifdef USE_ADC
  HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc, (uint32_t*)current, 2);
#endif


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	#ifdef USE_RADIO
	  nRF24_Event();
	#endif


	  if(((HAL_GetTick() - delayTimer) > stopTime) && ((Car1.pwmMotorLeft) == 0 && (Car1.pwmMotorRight) == 0)) {
		  setMotorPWM(0, 0);
	  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == NRF24_IRQ_Pin)
	{
#if (NRF24_USE_INTERRUPT == 1)
		nRF24_IRQ_Handler();

#endif
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	HCSR04_TIM_IC_CaptureCallback(htim);
	HCSR04_Read(&Distance);
	estimate(&HCSR04, Distance);
	if((HCSR04._last_est<20) && ((Car1.pwmMotorLeft) > 0 || (Car1.pwmMotorRight) > 0)){
		setMotorPWM(0, 0);
	}

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim6) {

#ifdef USE_HCSR04
	  HCSR04_Read(&Distance);
	  MessageLength = sprintf((char *)Message, "d%.3d\n\r",HCSR04._last_est);

#ifdef USE_RADIO_SENT
	  nRF24_SendData(Message, MessageLength);
#endif

#endif

#ifdef USE_ADC
#ifdef USE_RADIO_SENT
	  MessageLength = sprintf((char *)Message, "v%.4d\n\r", current[0]);
	  nRF24_SendData(Message, MessageLength);
#endif
#endif

#ifdef USE_IMU
	MPU6050_Read_All(&hi2c2, &MPU6050);
#endif
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
