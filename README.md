# pico-tidbits

A repository for some cleaned-up basic pieces of C/C++ code that can be re-used for my IOT projects with a raspberry pico. Code is verbose and size and efficiency is traded for clarity.

## Prequisites

- picoprobe
- openocd
- picotool
- visual studio code
- cmake

Visual studio code extensions:

- C/C++
- Cortex-Debug
- CMake
- CMake Tools

See instructions here: https://www.digikey.nl/en/maker/projects/raspberry-pi-pico-and-rp2040-cc-part-2-debugging-with-vs-code/470abc7efb07432b82c95f6f67f184c0

This code is developed and tested on a Mac.

## Available tidbits

- i2c_lcd: control a HD44780 LCD driven by a PCF8574 I2C adapter module
Based on spec sheet: https://www.futurlec.com/Datasheet/LED/HD44780.pdf

- dht11: Read data from a dht11 temperature and humidity sensor
Based on spec sheet: https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf

-... [More to come]



