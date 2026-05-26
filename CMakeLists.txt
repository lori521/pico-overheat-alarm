cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(pico_project C CXX ASM)

pico_sdk_init()

add_executable(pico_project
    main.cpp
    utils/oled/ssd1306.cpp
    utils/temp_press_sensor/bmp280.cpp
    debugging/debugging.cpp
)

# usb enable
pico_enable_stdio_usb(pico_project 1)
pico_enable_stdio_uart(pico_project 0)

target_link_libraries(pico_project pico_stdlib hardware_i2c hardware_adc hardware_gpio)
pico_add_extra_outputs(pico_project)