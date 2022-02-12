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

#include "ws2812.h"

#include "pico/stdio.h"

#define WS2812_PIN 28
#define WS2812_PIO pio0
#define WS2812_BAUD 800000
#define WS2812_ISRGBW false

#define WS2812_LENGTH 30

void hilo(ws2812_t *ws2812) {
    uint32_t grb = 0;
    uint skip = 0;
    uint light = 0;
    int dir = 1;

    while (true)
    {
        if (grb > 0xffffff)
        {
            grb = 0;
        }
        // uint32_t grb = urgb2u32(c, c, c);
        int l = 0;
        for (;l < skip; l++) {
            ws2812_put_pixel(ws2812, 0);
        }
        for (; l < skip + light; l++) {
            ws2812_put_pixel(ws2812, grb);
        }
        for (; l < ws2812->length; l++) {
            ws2812_put_pixel(ws2812,0);
        }
        sleep_ms(50);
        grb += 2;
        light += dir;
        if (light == ws2812->length + 1) {
            light = ws2812->length;
            dir = -1;
        } else if (light == 0) {
            dir = 1;
        }
    }
}

int main() {
    stdio_init_all();

    ws2812_t ws2812 = ws2812_setup(WS2812_PIN, WS2812_BAUD, WS2812_LENGTH, WS2812_ISRGBW);
    hilo(&ws2812);

    return 0;
}