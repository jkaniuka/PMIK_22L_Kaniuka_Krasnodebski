/**
  ******************************************************************************
  * @file    gui.c
  * @brief   This file provides code to display digits, chars and letters on TFT display. 
  ******************************************************************************
  */

#include "gui.h"
#include "transmitter.h"
#include "standard_font.h"
#include "stm32l4xx_hal.h"
 

/**
 *  Plots blue frame around display for design purposes only.
 */
void plot_frame(){
    for (int i = 0; i < 8; i++) {
      hagl_draw_rounded_rectangle(2+i, 2+i, 158-i, 126-i, 8-i, rgb565(0, 0, i*16));
    }
}

/**
 * Displays float (signed/unsigned) with 2 digits after decimal point.
 */
void display_float(float val, int x0, int y0){
	int index = x0;

	char array[7];
	sprintf(array, "%.2f", val);

	for(int i = 0; i < 7; i++){
		int unicode = array[i];
		hagl_put_char(unicode, index, y0, YELLOW, std_font);
		index += 6;
	}

}

/**
 *  Displays int (signed/unsigned) on display
 */
void display_int(int val, int x0, int y0){
	int digits_num = count_digits(val);
	int index = x0;

	char array[digits_num];
	sprintf(array, "%d", val);

	for(int i = 0; i < digits_num; i++){
		int unicode = array[i];
		hagl_put_char(unicode, index, y0, YELLOW, std_font);
		index += 6;
	}

}

/**
 * Converts ADC voltage value from analog ASC711EX current sensor to mA current value.
@param[in] acs711ex_voltage
@param[out] current
*/
float calc_current(int acs711ex_voltage){
	float voltage = 3.3f * acs711ex_voltage / 4096.0f;
	float current_mA = 36.7*voltage/3.3 - 18.3;
	return current_mA;
}

/**
 *  Refresh the screen by painting the entire screen black. 
 */
void render_screen(){
	  while (lcd_is_busy()) {}
	  lcd_copy();


	  uint32_t tickstart = HAL_GetTick();
	  while ((HAL_GetTick() - tickstart) < 25)
	  {
	  }

	 hagl_fill_rectangle(15, 15, 130, 110, 0x0000);

	  while (lcd_is_busy()) {}
	  lcd_copy();
}

/**
 *  Plot "SIGNAL LOST" alarm when robot is out of range.
 */
void signal_lost(){
	 hagl_put_char(83, 20, 100, RED, std_font);
	 hagl_put_char(73, 26, 100, RED, std_font);
	 hagl_put_char(71, 32, 100, RED, std_font);
	 hagl_put_char(78, 38, 100, RED, std_font);
	 hagl_put_char(65, 44, 100, RED, std_font);
	 hagl_put_char(76, 50, 100, RED, std_font);

	 hagl_put_char(76, 62, 100, RED, std_font);
	 hagl_put_char(79, 68, 100, RED, std_font);
	 hagl_put_char(83, 74, 100, RED, std_font);
	 hagl_put_char(84, 80, 100, RED, std_font);
}

/**
 *  Dispaly robot params in form of "variable_name = variable_value"
 */
void display_params(uint16_t joystick_x, uint16_t joystick_y, int distance, float current){
	 PWM_duty_cycle result;
	 result = calc_PWM(joystick_x, joystick_y);

	 hagl_put_char(76, 20, 20, YELLOW, std_font);
	 hagl_put_char(61, 27, 20, YELLOW, std_font);
     display_float(result.vel_L, 40, 20);


	 hagl_put_char(82, 20, 40, YELLOW, std_font);
	 hagl_put_char(61, 27, 40, YELLOW, std_font);
     display_float(result.vel_R, 40, 40);


	 hagl_put_char(100, 20, 60, YELLOW, std_font);
	 hagl_put_char(61, 27, 60, YELLOW, std_font);
	 display_int(distance, 40, 60);


	 hagl_put_char(73, 20, 80, YELLOW, std_font);
	 hagl_put_char(61, 27, 80, YELLOW, std_font);
	 display_float(current, 40, 80);

}




