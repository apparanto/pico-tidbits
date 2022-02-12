/**
 * @file adc.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Verbose implementation of using the PWM on the PICO
 * @version 0.1
 * @date 2022-02-07
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "adc.h"

#define MAX_RAW 0xfff
#define MIN_RAW 0x012

tb_adc_t tb_adc_setup(uint gpio, uint input, float min, float max) {

    adc_init();
    adc_gpio_init(gpio);
    adc_select_input(input);

    float r2v = (max - min) / ((float)(MAX_RAW - MIN_RAW)) ;

    return (tb_adc_t){
        gpio = gpio,
        input = input,
        r2v = r2v
    };
}

float tb_adc_read(tb_adc_t *adc) {
    uint16_t raw = adc_read();
    adc->raw = raw;
    return (float)(raw - MIN_RAW)* adc->r2v;
}