#include "pico.hpp"
#include "debugging/debugging.hpp"


// struct for calibrating temperature and preassure sensor
struct bmp280_calib_param calib_params;

// initialise components
void initialise_components() {
    // LEDs pins initialisation
    gpio_init(LED_RED);
    gpio_init(LED_YELLOW);
    gpio_init(LED_GREEN);

    // pin current transfer
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    // buzzer pin initialization
    gpio_init(BUZZER_POS);
    gpio_set_dir(BUZZER_POS, GPIO_OUT);

    // microphone pin initialization
    // open hardware
    adc_init();
    // set pin for microphone
    adc_gpio_init(MAX4466_OUT);
    // activate read
    adc_select_input(0);

    // initialise oled
    // standard 400 khz speed
    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);

    // pin management because of overlapping with sensor
    gpio_set_function(SSD_1306_SDA, GPIO_FUNC_I2C);
    gpio_set_function(SSD_1306_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(SSD_1306_SDA);
    gpio_pull_up(SSD_1306_SCL);

    // temperature and pressure sensor initialisation using utils libraries api
    bmp280_init();
    bmp280_get_calib_params(&calib_params);

    SSD1306_init();
}



int main() {
    // for serial monitor
    stdio_init_all();

    // intialise pins for input and output data 
    initialise_components();

    // define oled frame
    struct render_area frame_area;
    frame_area.start_col = 0;
    frame_area.end_col = SSD1306_WIDTH - 1;
    frame_area.start_page = 0;
    frame_area.end_page = SSD1306_NUM_PAGES - 1;
    calc_render_area_buflen(&frame_area);

    // create buffer
    uint8_t send_buffer[SSD1306_BUF_LEN];

    // line buffer pentru scrierea fiecarui rand
    char line_buffer[32];

    while (1) {
        // read current sound value using api function 
        uint16_t current_microphone_value = adc_read();

        // read current parameters for temperature and pressure
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

        // for tempreture the intervals are:
        // < 25 -> green led all fine
        // [25, 27] -> yellow led warning
        // > 27 -> red led + buzzer overheating
        // for sound the intervals are:
        // < 2000 -> green led all fine
        // [2000, 3000] -> yellow led warning
        // > 3000 -> red led + buzzer overheating
        if (temp_celsius > 30.0f || current_microphone_value > 3000) {
            printf("critical!!!\n");
            gpio_put(LED_RED, true);
            gpio_put(LED_YELLOW, false);
            gpio_put(LED_GREEN, false);
            gpio_put(BUZZER_POS, true);
        } else if (temp_celsius >= 29.0f || current_microphone_value >= 2500) {
            printf("worried\n");
            gpio_put(LED_RED, false);
            gpio_put(LED_YELLOW, true);
            gpio_put(LED_GREEN, false);
            gpio_put(BUZZER_POS, false);
        } else {
            printf("ok\n");
            gpio_put(LED_RED, false);
            gpio_put(LED_YELLOW, false);
            gpio_put(LED_GREEN, true);
            gpio_put(BUZZER_POS, false);
        }

        printf("temperature: %.2f, pressure: %.2f\nsound volume: %d\n", temp_celsius, pressure_hpa, current_microphone_value);
        
        // clean buffer
        memset(send_buffer, 0, SSD1306_BUF_LEN);
        
        // write temperature line 
        snprintf(line_buffer, sizeof(line_buffer), "temp: %.2f C", temp_celsius);
        WriteString(send_buffer, 0, 0, line_buffer);

        // write pressure line 
        snprintf(line_buffer, sizeof(line_buffer), "press: %.2f hpa", pressure_hpa);
        WriteString(send_buffer, 0, 8, line_buffer);

        // write sound line 
        snprintf(line_buffer, sizeof(line_buffer), "vol: %d", current_microphone_value);
        WriteString(send_buffer, 0, 16, line_buffer);

        // write state on oled
        if (temp_celsius > 30.0f || current_microphone_value > 3000) {
            WriteString(send_buffer, 0, 24, (char*)"STATE: CRITICAL!");
        } else if (temp_celsius >= 29.0f || current_microphone_value >= 2500) {
            WriteString(send_buffer, 0, 24, (char*)"STATE: WARNING");
        } else {
            WriteString(send_buffer, 0, 24, (char*)"STATE: NORMAL");
        }

        // send to oled
        render(send_buffer, &frame_area);

        sleep_ms(100);
    }



    return 0;
}
