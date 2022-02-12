/**
 * @file main.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <time.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include "adc.h"

#include "pico/stdio.h"

#define ADC_PIN 26
#define ADC_INPUT 0
#define MAX_VALUE 100
#define MIN_VALUE 0

int main() {
    stdio_init_all();

    tb_adc_t adc = tb_adc_setup(ADC_PIN, ADC_INPUT, MIN_VALUE, MAX_VALUE);
    while (true)
    {
        float value = tb_adc_read(&adc);
        printf("read value: %f (raw: %u)\n", value, adc.raw);
        sleep_ms(500);
    }

    return 0;
}