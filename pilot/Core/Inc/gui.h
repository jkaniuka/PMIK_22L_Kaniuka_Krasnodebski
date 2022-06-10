#pragma once

#include "hagl.h"
#include "rgb565.h"



void plot_frame();

void display_float(float val, int x0, int y0);

void display_int(int val, int x0, int y0);

float calc_current(int acs711ex_voltage);

void render_screen();

void signal_lost();

void display_params(uint16_t joystick_x, uint16_t joystick_y, int distance, float current);




