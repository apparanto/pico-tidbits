/**
 * @file ws2812.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _TIDBIT_WS2812_HEADER
#define _TIDBIT_WS2812_HEADER_

/*
 * C HEADERS
 */

#include "hardware/pio.h"


typedef struct ws2812 {
    PIO pio;
    uint sm;
    uint length;
} ws2812_t;

/**
 * @brief Prepares a PIO for sending pixels to a WS2812 LED string
 * 
 * @param gpio 
 * @param baudrate 
 * @param length 
 * @param rgbw 
 * @return ws2812_t 
 */
extern ws2812_t ws2812_setup(uint gpio, uint32_t baudrate, uint length, bool rgbw);

/**
 * @brief Write 32-bit value to LED string
 * 
 * @param ws2812 
 * @param pixel_grb 
 */
static inline void ws2812_put_pixel(ws2812_t *ws2812, uint32_t pixel_grb) {
    pio_sm_put_blocking(ws2812->pio, ws2812->sm, pixel_grb << 8u);
}

// Constructs a 24 bit grb value from 8-bit r, g and b components
// green indeed comes first
/**
 * @brief Constructs a 24 bit grb value from 8-bit r, g and b components
 * green indeed comes first
 * 
 * @param r 8 bit red value
 * @param g 8 bit green value
 * @param b 8 bit blue value
 * @return uint32_t grb value that can be send to the WS2812 led string
 */
static inline uint32_t urgb2u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

#endif // _TIDBIT_WS2812_HEADER_
