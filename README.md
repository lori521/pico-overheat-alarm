# Pico overheat alarm

## Folder hierarchy

```
    pico-overheat-alarm/
    ├── src/
    │   ├── main.cpp
    │   └── pico.hpp
    ├── debugging/
    │   ├── debugging.hpp
    │   └── debugging.cpp 
    ├── utils/
    │   ├── oled/
    │   │     ├── raspberry26x32.hpp
    │   │     ├── ssd1306_font.hpp
    │   │     ├── ssd1306.cpp 
    │   │     └── ssd1306.hpp
    │   └── temp_press_sensor 
    │         ├── bmp280.cpp
    │         └── bmp280.hpp
    ├── .gitignore
    ├── CMakeLists.txt
    ├── pico_sdk_import.cmake
    └── README.md
``` 

## Files and folders 

-> `debugging/` contains functions to test if all components are properly connected on the breadboard:

* `void test_leds()` - lights up each LED to verify if they work 
* `void test_buzzer()` - tests if the buzzer beeps   
* `void test_microphone()` - reads the sound volume from the microphone and prints it to the serial monitor
* `void test_pressure_and_temperature()` - reads the parameters from the sensor and prints them to the serial monitor  
* `void test_oled()` - draws a happy face to verify the OLED display works

-> `utils/` contains libraries downloaded from `pico-examples` to interface with the SSD1306 and BMP280 components:

* `oled/` - contains files that handle the OLED display  
* `temp_press_sensor/` - contains files that read data from the BMP280 sensor  

-> `src/` holds the main logic of the circuit:

* `pico.hpp` - defines the pins used to send and receive information
* `main.cpp` - initializes components, implements the sensor logic, handles inputs, and updates the display  

## How to run

1. **Create the build directory:**
   ```bash
   mkdir build
   cd build
   ```

2. **Generate build files:**
    ```bash
    cmake ..
    ```

3. **Compile the project:**
    ```bash
    make
    ```

4. **Upload to Raspberry Pi Pico:**
    - hold the BOOTSEL button
    - `ls /Volumes` to check if it mounted correctly
    - `cp build/pico_project.uf2 /Volumes/RPI-RP2` to copy the program

5. **Run:**
    - the program will run automatically
    - to get input using the USB open a `serial monitor` and the collected data will be written there
