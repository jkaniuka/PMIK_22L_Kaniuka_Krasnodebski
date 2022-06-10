/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    uart_usb_com.c
  * @brief   The file contains a function for simply sending messages from the microcontroller via UART/USART to a computer. 
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "uart_usb_com.h"
#include "usart.h"


/** The printf() function writes data to standard output. 
 * In embedded C we do not have a console directly available to display e.g. the values of variables. 
 * The printf() function calls the _write() function, and this function calls the __io_putchar() function for each character. 
 * It has the weak attribute, so you can override it and display characters on your computer using a serial port emulator. 
  */
int __io_putchar(int ch)
{
  if (ch == '\n') {
    __io_putchar('\r');
  }

  HAL_UART_Transmit(&UART_HANDLE, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

  return 1;
}
