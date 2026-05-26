#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "utils/oled/ssd1306.hpp"
#include "utils/temp_press_sensor/bmp280.hpp"

// Definim pinii pentru I2C (folosiți automat de i2c_default în Pico SDK)
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

// Definim pinul pentru microfon (GP26 corespunde canalului ADC 0)
#define MIC_PIN 26

int main() {
    // 1. Inițializare Serială (pentru monitorizare în consolă)
    stdio_init_all();
    sleep_ms(2000); // Un mic delay esențial pentru a apuca să deschizi Serial Monitor
    printf("--- Pornire Sistem Telemetrie Pico ---\n");

    // 2. Inițializare Magistrală I2C
    // Pachetul Pico SDK folosește i2c_default, configurat aici la 400kHz
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // 3. Inițializare Senzor BMP280
    bmp280_init();
    struct bmp280_calib_param calib_params;
    bmp280_get_calib_params(&calib_params);
    printf("[OK] Senzor BMP280 pregătit.\n");

    // 4. Inițializare Ecran OLED SSD1306
    SSD1306_init();
    printf("[OK] Ecran OLED inițializat.\n");

    // 5. Inițializare Port Analogic (ADC) pentru Microfon
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(0); // Selectăm canalul 0 (asociat pinului GP26)
    printf("[OK] Convertor Microfon activat.\n");

    // 6. Pregătirea structurii de randare pentru ecranul OLED
    uint8_t oled_buffer[SSD1306_BUF_LEN];
    
    struct render_area frame_area;
    frame_area.start_col = 0;
    frame_area.end_col = SSD1306_WIDTH - 1;
    frame_area.start_page = 0;
    frame_area.end_page = SSD1306_NUM_PAGES - 1;
    calc_render_area_buflen(&frame_area);

    char text_line[20]; // Buffer temporar pentru formatarea textului

    // Bucla infinită de rulare
    while (true) {
        // --- A. CITIRE ȘI CALCUL DATE SENZOR (BMP280) ---
        int32_t raw_temp, raw_pressure;
        bmp280_read_raw(&raw_temp, &raw_pressure);

        // Funcțiile convertesc datele brute folosind parametrii din fabrică
        int32_t temp_centigrade = bmp280_convert_temp(raw_temp, &calib_params);
        int32_t pressure_pa = bmp280_convert_pressure(raw_pressure, raw_temp, &calib_params);

        // Transformăm valorile întregi în numere cu virgulă (hPa și Celsius)
        float final_temp = temp_centigrade / 100.0f;
        float final_pressure = pressure_pa / 100.0f;

        // --- B. CITIRE VALOARE MICROFON ---
        uint16_t mic_raw_value = adc_read(); // Returnează o valoare între 0 și 4095

        // --- C. AFIȘARE ÎN CONSOLĂ (Prin cablul USB) ---
        printf("Temp: %.2f C | Presiune: %.2f hPa | Mic: %d\n", final_temp, final_pressure, mic_raw_value);

        // --- D. DESENARE ȘI UPDATE ECRA OLED ---
        // Curățăm buffer-ul (ștergem imaginea anterioară umplând totul cu pixeli stinși / 0)
        memset(oled_buffer, 0, SSD1306_BUF_LEN);

        // Linia 1: Temperatura (y = 0)
        snprintf(text_line, sizeof(text_line), "TEMP: %.2f C", final_temp);
        WriteString(oled_buffer, 0, 0, text_line);

        // Linia 2: Presiunea atmosferică (y = 12)
        snprintf(text_line, sizeof(text_line), "PRES: %.1f HPA", final_pressure);
        WriteString(oled_buffer, 0, 12, text_line);

        // Linia 3: Microfonul (y = 24)
        snprintf(text_line, sizeof(text_line), "SUNET: %d", mic_raw_value);
        WriteString(oled_buffer, 0, 24, text_line);

        // Trimitem noul buffer către ecranul fizic
        render(oled_buffer, &frame_area);

        // Citim datele de două ori pe secundă
        sleep_ms(500);
    }

    return 0;
}