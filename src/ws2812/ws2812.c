/**
 * @file ws2812.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Verbose implementation of using the PWM on the PICO
 * @version 0.1
 * @date 2022-02-07
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ws2812.h"

#include "../../build/src/ws2812/ws2812.pio.h"

ws2812_t ws2812_setup(uint gpio, uint32_t baudrate, uint length, bool rgbw) {
    PIO pio = pio0;
    uint sm = pio_claim_unused_sm(pio, true);

    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, gpio, baudrate, rgbw);

    return (ws2812_t) {
        pio,
        sm,
        length
    };
}


