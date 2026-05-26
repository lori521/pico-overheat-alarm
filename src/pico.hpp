#ifndef PICO_HPP
#define PICO_HPP

#pragma once

// all includes
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "../utils/oled/ssd1306.hpp"
#include "../utils/temp_press_sensor/bmp280.hpp"

// pins definition 
// pins for oled (ssd1306)
#define SSD_1306_SCL 5
#define SSD_1306_SDA 4

// pins for temerature and preassure (bmp280)
#define BMP_280_SCL 5
#define BMP_280_SDA 4

// pin for microphone
#define MAX4466_OUT 26

// pin for buzzer
#define BUZZER_POS 16

// pin for red LED
#define LED_RED 17
// pin for yellow LED
#define LED_YELLOW 18
// pin for green LED
#define LED_GREEN 19

#endif // END_PICO_HPP