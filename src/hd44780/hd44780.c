/**
 * @file ic2_lcd.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-06
 * 
 * @copyright Copyright (c) 2022
 * @license MIT
 * 
 */

#include "hd44780.h"


static inline int hd44780_write(hd44780_t *hd44780, uint8_t *bytes, uint len) {
    return i2c_write_timeout_us(hd44780->i2c, hd44780->addr, bytes, len, true, hd44780->timeout_us);
}

void hd44780_write_init_nibble(hd44780_t *hd44780, uint8_t nibble) {
    uint8_t byte = ((nibble >> 4) & 0x0f) << HD44780_SHIFT_DATA;
    uint8_t bytes[2] = { byte | HD44780_MASK_E, byte };
    hd44780_write(hd44780, bytes, 2);
}

void hd44780_backlight_on(hd44780_t *hd44780) {
    uint8_t backlight_mask = 1 << HD44780_SHIFT_BACKLIGHT;
    hd44780->backlight_mask = backlight_mask;
    uint8_t bytes[1] = {  backlight_mask };
    hd44780_write(hd44780, bytes, sizeof(bytes));
}

void hd44780_backlight_off(hd44780_t *hd44780) {
    hd44780->backlight_mask = 0;
    uint8_t bytes[1] = { 0 };
    hd44780_write(hd44780, bytes, sizeof(bytes));
}

void hd44780_write_command(hd44780_t *hd44780, uint8_t cmd) {
    uint8_t byte = hd44780->backlight_mask | (((cmd >> 4) & 0x0f) << HD44780_SHIFT_DATA);
    uint8_t bytes[2] = { byte | HD44780_MASK_E, byte };
    hd44780_write(hd44780, bytes, 2);

    byte = hd44780->backlight_mask | ((cmd & 0x0f) << HD44780_SHIFT_DATA);
    bytes[0] = byte | HD44780_MASK_E;
    bytes[1] = byte;
    hd44780_write(hd44780, bytes, 2);
    if (cmd <= 3) {
        sleep_ms(5);
    }
}

void hd44780_write_data(hd44780_t *hd44780, uint8_t data) {
    uint8_t byte =  HD44780_MASK_RS | hd44780->backlight_mask | (((data >> 4) & 0x0f) << HD44780_SHIFT_DATA);
    uint8_t bytes[2] = { byte | HD44780_MASK_E, byte};
    hd44780_write(hd44780, bytes, 2);

    byte =  HD44780_MASK_RS | hd44780->backlight_mask | ((data & 0x0f) << HD44780_SHIFT_DATA);
    bytes[0] = byte | HD44780_MASK_E;
    bytes[1] = byte ;
    hd44780_write(hd44780, bytes, 2);
}

hd44780_t hd44780_init(i2c_inst_t *i2c, uint8_t addr, uint sda_pin, uint scl_pin, uint baudrate) {
    i2c_init(i2c, baudrate);

    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);

    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    hd44780_t hd44780 = {
        .i2c = i2c,
        .addr = addr,
        .backlight_mask = 0,
        .timeout_us = HD44780_WRITE_TIMEOUT_US
    };
   
    // Send reset 3 times, wait at least 4.1 ms the first time
    hd44780_write_init_nibble(&hd44780, HD44780_FUNCTION_RESET);
    sleep_ms(5);
    hd44780_write_init_nibble(&hd44780, HD44780_FUNCTION_RESET);
    sleep_ms(1);
    hd44780_write_init_nibble(&hd44780, HD44780_FUNCTION_RESET);
    sleep_ms(1);
    hd44780_write_init_nibble(&hd44780, HD44780_FUNCTION);
    sleep_ms(1);

    hd44780_write_command(&hd44780, HD44780_FUNCTION | HD44780_FUNCTION_2LINES);

    return hd44780;
}

void hd44780_clear(hd44780_t *hd44780) {
    hd44780_write_command(hd44780, HD44780_CLR);
    hd44780_write_command(hd44780, HD44780_HOME);
}

void hd44780_show_cursor(hd44780_t *hd44780) {
    hd44780_write_command(hd44780, HD44780_ON_CTRL | HD44780_ON_DISPLAY | HD44780_ON_CURSOR);
}

void hd44780_hide_cursor(hd44780_t *hd44780) {
    hd44780_write_command(hd44780, HD44780_ON_CTRL | HD44780_ON_DISPLAY);
}

void hd44780_blink_cursor_on(hd44780_t *hd44780) {
    hd44780_write_command(hd44780, HD44780_ON_CTRL | HD44780_ON_DISPLAY | HD44780_ON_CURSOR | HD44780_ON_BLINK);
}

void hd44780_move_cursor(hd44780_t *hd44780, uint cursor_x, uint cursor_y) {
    uint addr = cursor_x & 0x3f;
    if (cursor_y & 1) {
        addr += 0x40;
    } else if (cursor_y & 2) {
        addr += HD44780_NUM_COLUMNS;
    }
    hd44780_write_command(hd44780, HD44780_DDRAM | addr);
}

void hd44780_cursor_home(hd44780_t *hd44780) {
    hd44780_write_command(hd44780, HD44780_HOME);
    sleep_ms(5);
}

void hd44780_display_on(hd44780_t *hd44780) {
    hd44780_write_command(hd44780, HD44780_ON_CTRL | HD44780_ON_DISPLAY);
}

void hd44780_display_off(hd44780_t *hd44780) {
    hd44780_write_command(hd44780, HD44780_ON_CTRL);
}

void hd44780_put_char(hd44780_t *hd44780, uint8_t c) {
    if (c == '\n') {
        hd44780_move_cursor(hd44780, 0, 1);
    } else {
        hd44780_write_data(hd44780, c);
    }
}

void hd44780_put_str(hd44780_t *hd44780, uint8_t *str) {
    for (int i = 0; i < strlen(str); i++) {
        hd44780_put_char(hd44780, str[i]);
    }
}