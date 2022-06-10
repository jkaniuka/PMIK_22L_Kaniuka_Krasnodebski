#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


#define PI 3.14159265

struct joystick_params {
	float ADC_X_center;
	float ADC_Y_center;
	float stable_radius;
	float radius_back;
};


struct velocities {
	float vel_L;
	float vel_R;
};

typedef struct velocities PWM_duty_cycle;

PWM_duty_cycle calc_PWM(uint16_t x, uint16_t y);

PWM_duty_cycle truncate_digits_to_send(float vel_L, float vel_R);

struct directions {
	int dirL;
	int dirR;
};

typedef struct directions Wheels_directions;

Wheels_directions calc_dirs(int velocity_left, int velocity_right);

int count_digits(int number);

char* speed_SV_msg(float vel_L, float vel_R);


