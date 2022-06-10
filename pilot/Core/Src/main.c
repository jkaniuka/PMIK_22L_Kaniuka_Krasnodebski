/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          Main program body
  ******************************************************************************
  * Autorzy:
  * Jan Kaniuka
  * Przemysław Krasnodębski
  *
  * Projekt: Pilot RC.
  *
  * Główne funckjonalności:
  * Generowanie poleceń ruchu zgodnie z ruchem joysticka,
  * Przesył poleceń ruchu drogą radiową do odbiornika,
  * Odbiór parametrów ruchu z platformy mobilnej,
  * Wyświetlanie informacji diagnostycznych oraz parametrów ruchu na ekranie,
  * Powiadomienie użytkownika o wejściu w kolizję z otoczeniem.
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "nRF24/nRF24_Defs.h"
#include "nRF24/nRF24.h"
#include "ring_buffer.h"
#include "uart_usb_com.h"
#include "transmitter.h"
#include "font6x9.h"
#include "gui.h"
#include "stdlib.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MESSAGE_BUFF_LEN 200 ///< message buffer length
#define USE_RADIO ///< flag if radio should be turned on 
#undef SHOW_DUTY_CYCLE 
#define USE_LCD ///< flag if you want to use TFT LCD 160x128px display
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t nrf24_rx_flag, nrf24_tx_flag, nrf24_mr_flag; ///< transmission flags for transreceiver
uint8_t Nrf24_Message[MESSAGE_BUFF_LEN];
uint8_t Message[MESSAGE_BUFF_LEN];
uint8_t MessageLength; 

uint8_t tMessage[MESSAGE_BUFF_LEN]; ///< buffer with message to send
uint8_t tMessageLength; ///< TX buffer length

uint8_t Message_RX[MESSAGE_BUFF_LEN];  ///< buffer for message to receive
uint8_t MessageLength_RX; ///<RX buffer length

volatile static uint16_t joystick[2]; ///< raw ADC value from joystick potentiometers
volatile bool print_alarm = 0; ///< if equals 1, display ALARM message on the screen
volatile int distance; ///< distance value from HC-SR04 in cm unit
volatile int acs711ex_voltage = 2048; ///< raw ADC voltage from analog current sensor
volatile bool send_msg = 0; ///< if equals 1, messages will be send
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * Parse incomming message from robot 
*/
void MessegeHandle(uint8_t Message[]){
    switch(Message[0]){
    case('d'):
        {
    	if(Message[1] == 'd')
    	{
    		break;
    	}
        char received_dist[] = {Message[1], Message[2], Message[3]};
        distance = atoi(received_dist);
        char received_current[] = {Message[7], Message[8], Message[9], Message[10]};
        acs711ex_voltage = atoi(received_current);
        break;
        }

    default:
        break;
    }
}

/**
 * Print message which is beeing send at the moment
*/
void nRF24_EventTxCallback(void)
{
      MessageLength = sprintf((char*)Message, "TX\n\r");
      HAL_UART_Transmit_IT(&huart2, Message, MessageLength);
}


void nRF24_EventMrCallback(void)
{
      MessageLength = sprintf((char*)Message, "MX\n\r");
      HAL_UART_Transmit_IT(&huart2, Message, MessageLength);
}

/**
 * Print received message on serial port terminal emulator 
*/
void nRF24_EventRxCallback(void){

    nRF24_ReadData(Message_RX, &MessageLength_RX);
    if(MessageLength_RX > 0){

    HAL_UART_Transmit_IT(&huart2, Message_RX, MessageLength_RX);
    MessegeHandle(Message_RX);
    }
      nRF24_FlushRX();

}

/**
 * Check if SPI transmission to TFT LCD display is finished.
*/
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi2)
	{
		lcd_transfer_done();
	}
}

/**
 * Send controls in regular time intervals.
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim3) {
	  send_msg = !send_msg;
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
  MX_SPI1_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  nRF24_Init(&hspi1);
  nRF24_SetRXAddress(0, (uint8_t*)"Nad");
  nRF24_SetTXAddress((uint8_t*)"Odb");
  nRF24_RX_Mode();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)joystick, 2);
  HAL_TIM_Base_Start_IT(&htim3);
  lcd_init();

  while (1)
  {

#ifdef SHOW_DUTY_CYCLE
	  printf("VRx=%u, VRy=%u\n", joystick[0], joystick[1]);
	  PWM_duty_cycle result;
	  result = calc_PWM(joystick[0], joystick[1]);
	  printf("PWM : lewe=%f, prawe=%f\n", result.vel_L, result.vel_R);
#endif


	  if (send_msg == 1){
		  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		  PWM_duty_cycle result;
		  result = calc_PWM(joystick[0], joystick[1]);
		  Wheels_directions directions;
		  directions = calc_dirs( (int)result.vel_L, (int)result.vel_R );
		  result = truncate_digits_to_send(result.vel_L, result.vel_R);

		  MessageLength = sprintf((char*)Message, "s%d%02d%d%02d\n\r", directions.dirL, abs((int)result.vel_L), directions.dirR, abs((int)result.vel_R) );
		  nRF24_SendData(Message, MessageLength);
	  }



#ifdef USE_RADIO
#if (NRF24_USE_INTERRUPT == 1)
	  nRF24_Event();
#endif
#endif


#ifdef USE_LCD
	  	 display_params(joystick[0], joystick[1], distance, calc_current(acs711ex_voltage));
	  	 if(print_alarm == 1){
		 signal_lost();
	  	 }
 		 render_screen();
#endif


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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * Handle interrupt from nRF24L01+ module and from user button on STM32 Nucleo board.
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == NRF24_IRQ_Pin)
	{
#if (NRF24_USE_INTERRUPT == 1)

		nRF24_IRQ_Handler();
#endif
	}

	if (GPIO_Pin == USER_BUTTON_Pin) {
		print_alarm = !print_alarm;
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
