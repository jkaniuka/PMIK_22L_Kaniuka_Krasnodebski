#include "uart_usb_com.h"
#include "usart.h"



int __io_putchar(int ch)
{
  if (ch == '\n') {
    __io_putchar('\r');
  }

  HAL_UART_Transmit(&UART_HANDLE, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

  return 1;
}
