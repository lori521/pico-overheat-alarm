#include "debugging.hpp"

// test components functions to make sure they work
void test_leds() {
    gpio_put(LED_RED, true);
    gpio_put(LED_YELLOW, true);
    gpio_put(LED_GREEN, true);

}

void test_buzzer() {
    // DEBUG
    gpio_put(BUZZER_POS, true);
}

void test_microphone() {
    while (1) {
        // read current sound value using api function 
        uint16_t current_microphone_value = adc_read();

        printf("current microphone value is: %d\n", current_microphone_value);

        sleep_ms(100);
    }
}

void test_pressure_and_temperature(struct bmp280_calib_param calib_params) {
    while (1) {
        // read current parameters
        int32_t raw_temperature;
        int32_t raw_pressure;

        // extract numbers 
        bmp280_read_raw(&raw_temperature, &raw_pressure);

        // convert raw data with constrains in calib_params
        int32_t converted_temperature = bmp280_convert_temp(raw_temperature, &calib_params);
        int32_t converted_pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &calib_params);

        // conversions
        float temp_celsius = converted_temperature / 100.0f;
        float pressure_hpa = converted_pressure / 100.0f;

        printf("temperature: %.2f, pressure: %.2f\n", temp_celsius, pressure_hpa);

        sleep_ms(500);
    }
}

void test_oled() {
    // initialise frame
    struct render_area frame_area;
    frame_area.start_col = 0;
    frame_area.end_col = SSD1306_WIDTH - 1;
    frame_area.start_page = 0;
    frame_area.end_page = SSD1306_NUM_PAGES - 1;
    calc_render_area_buflen(&frame_area);

    // intermediate send buffer
    uint8_t send_buffer[SSD1306_BUF_LEN];

    while (1) {
        memset(send_buffer, SSD1306_BUF_LEN,sizeof(send_buffer));

        // write :) in buffer
        // eyes
        SetPixel(send_buffer, 60, 22, true); // Ochiul stâng
        SetPixel(send_buffer, 68, 22, true); // Ochiul drept

        // Gura (o linie orizontală pentru bază)
        DrawLine(send_buffer, 61, 28, 67, 28, true);
        
        // Colțurile gurii (pentru a crea efectul de zâmbet curbat)
        SetPixel(send_buffer, 60, 27, true); // Colțul stâng
        SetPixel(send_buffer, 68, 27, true); // Colțul drept

        // send buffer to frame
        render(send_buffer, &frame_area);

        sleep_ms(1000);
    }
}