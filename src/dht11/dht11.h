/**
 * @file dht11.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __DHT11_HEADER__
#define __DHT11_HEADER__

#include <stdbool.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"

typedef struct dht11 {
    uint pin;

    uint8_t humidity_int;
    uint8_t humidity_dec;

    uint8_t temp_int;
    uint8_t temp_dec;

    uint8_t checksum;
    uint error;
    uint32_t elapsed;
} dht11_t;

extern dht11_t dht11_init(uint pin);
extern void dht11_read(dht11_t *dht11);

#endif