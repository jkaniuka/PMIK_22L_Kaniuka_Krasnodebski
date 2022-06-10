/**
  ******************************************************************************
  * @file    transmitter.c
  * @brief   The file contains the functions responsible for controlling the robot.
  ******************************************************************************
  */
#include "transmitter.h"

/// Joystick position whe stable (x,y), and two offset values for better control.
struct joystick_params joystick = {1917.0f, 1955.0f, 5.0f, 40.0f};

/**
 * Converts ADC voltages (joystick position) to PWM duty cycle (value from 0 to 100)
*/
PWM_duty_cycle calc_PWM(uint16_t x, uint16_t y)
{
	PWM_duty_cycle robot_vel;


	// first quarter
	if( (x >= joystick.ADC_X_center) && (y <= joystick.ADC_Y_center ) ){
		robot_vel.vel_L = sqrt((pow (x - joystick.ADC_X_center, 2.0f) + pow (y - joystick.ADC_Y_center, 2.0f)))/(joystick.ADC_Y_center)*100.0f;
		if (robot_vel.vel_L > 100.0f){
			robot_vel.vel_L = 100.0f;
		}
		robot_vel.vel_R = robot_vel.vel_L * atan((joystick.ADC_Y_center - y)/(x - joystick.ADC_X_center)) * 2.0f/PI ;
	}


	// second quarter
	if( (x < joystick.ADC_X_center) && (y <= joystick.ADC_Y_center ) ){
		robot_vel.vel_R = sqrt((pow (x - joystick.ADC_X_center, 2.0f) + pow (y - joystick.ADC_Y_center, 2.0f)))/(joystick.ADC_Y_center)*100.0f;
		if (robot_vel.vel_R > 100.0f){
			robot_vel.vel_R = 100.0f;
		}
		robot_vel.vel_L = robot_vel.vel_R * atan((joystick.ADC_Y_center - y)/(joystick.ADC_X_center - x)) * 2.0f/PI ;
	}


	// back
	if( ((x > joystick.ADC_X_center - joystick.radius_back) && (x < joystick.ADC_X_center + joystick.radius_back)) &&  (y > joystick.ADC_Y_center + joystick.radius_back) ){
				robot_vel.vel_L = (-1.0f) * (y- joystick.ADC_Y_center - joystick.stable_radius)/(4095.0f- joystick.ADC_Y_center - joystick.stable_radius ) * 100.0f;
				robot_vel.vel_R = (-1.0f) * (y- joystick.ADC_Y_center - joystick.stable_radius)/(4095.0f- joystick.ADC_Y_center - joystick.stable_radius ) * 100.0f;
	}

	// deadband at center
	if( ((x > joystick.ADC_X_center - joystick.stable_radius) && (x < joystick.ADC_X_center + joystick.stable_radius)) && ((y > joystick.ADC_Y_center - joystick.stable_radius) && (y < joystick.ADC_Y_center + joystick.stable_radius)) ){
		robot_vel.vel_L = 0.0f;
		robot_vel.vel_R = 0.0f;
	}

    return robot_vel;
}

/**
 * Truncates PWM value to 2 digits to ensure constant message length.
*/
PWM_duty_cycle truncate_digits_to_send(float vel_L, float vel_R){
	PWM_duty_cycle result;

	  if (vel_L > 99){
	  	result.vel_L = 99;
	  }
	  if (vel_L < -99){
	  	result.vel_L = -99;
	  }

	  if (vel_R > 99){
	  	result.vel_R = 99;
	  }

	  if (vel_R < -99){
	  	result.vel_R = -99;
	  }

	  return result;
}

/**
 * Function determines rotation direction for motor controller with H-bridge.
*/
Wheels_directions calc_dirs(int velocity_left, int velocity_right){

	Wheels_directions robot_dirs;

	if(velocity_left > 0){
		robot_dirs.dirL = 0;
	}
	else {
		robot_dirs.dirL = 1;
	}

	if(velocity_right > 0){
		robot_dirs.dirR = 0;
	}
	else {
		robot_dirs.dirR = 1;
	}

	return robot_dirs;
}


/**
 * Function checks how long buffer should be created (depreceated)
*/
int count_digits(int number){
	int digits = 0;
	if (number == 0) return 1;
	if (number < 0) digits = 1;
	while (number) {
		number /= 10;
		digits++;
	}
	return digits;
}

/**
 * Function builds message with PWM values and appropriate identifier (SV = speed value) (depreceated)
*/
char* speed_SV_msg(float vel_L, float vel_R){
	static char msg[20];
	memset(msg, 0, sizeof msg);
	msg[0]='S';
	msg[1]='V';
	msg[2]='$';

	int velL_int = vel_L;
	int velR_int = vel_R;

	char bufferL[count_digits(velL_int)], bufferR[count_digits(velR_int)];

	sprintf(bufferL, "%d\r\n", velL_int);
	for(int i=0; i <count_digits(velL_int);i++ ){
		msg[3+i] = bufferL[i];
	}
	msg[2+count_digits(velL_int)+1]='$';

	sprintf(bufferR, "%d\r\n", velR_int);
	for(int i=0; i <count_digits(velR_int);i++ ){
		msg[2+count_digits(velL_int)+2+i] = bufferR[i];
	}

	return msg;

}
