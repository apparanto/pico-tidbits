/**
 * @file pwm.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Verbose implementation of using the PWM on the PICO
 * @version 0.1
 * @date 2022-02-07
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "pwm.h"

tb_pwm_t tb_pwm_start(uint gpio, uint wrap, uint level) {

    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, channel, level);

    pwm_set_enabled(slice, true);

    return (tb_pwm_t){
        gpio = gpio,
        slice = slice,
        channel = channel,
        wrap = wrap,
        level = level
    };

}