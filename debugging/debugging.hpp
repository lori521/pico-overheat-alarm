#ifndef DEBUGGING_HPP
#define DEBUGGING_HPP

#pragma once 

#include "../src/pico.hpp"


void test_leds();
void test_buzzer();
void test_microphone();
void test_pressure_and_temperature(struct bmp280_calib_param calib_params);
void test_oled();

#endif