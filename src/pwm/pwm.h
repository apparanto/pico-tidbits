/**
 * @file pwm.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _TIDBIT_PWM_HEADER_
#define _TIDBIT_PWM_HEADER_

/*
 * C HEADERS
 */
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/pwm.h"


typedef struct tb_pwm {
    uint gpio;
    uint slice;
    uint channel;
    uint wrap;
    uint level;
} tb_pwm_t;

/**
 * @brief Setup a PWM and run it immediately
 * 
 * @param gpio 
 * @param wrap 
 * @param chan_level 
 * @return tb_pwm_t 
 */
extern tb_pwm_t tb_pwm_start(uint gpio, uint wrap, uint chan_level);

#endif // _TIDBIT_PWM_HEADER_
