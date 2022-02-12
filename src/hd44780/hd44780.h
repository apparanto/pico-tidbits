/**
 * @file hd44780.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-06
 * 
 *
 * @copyright Copyright (c) 2022
 * @license MIT
 * 
 */

#ifndef _HD44780_
#define _HD44780_

#include "pico/stdlib.h"
#include "hardware/i2c.h"


// HD44780 LCD controller command set
#define HD44780_CLR                     0x01  // DB0: clear display
#define HD44780_HOME                    0x02  // DB1: return to home position

#define HD44780_ENTRY_MODE              0x04  // DB2: set entry mode
#define HD44780_ENTRY_INC               0x02  // DB1: increment
#define HD44780_ENTRY_SHIFT             0x01  // DB0: shift

#define HD44780_ON_CTRL                 0x08  // DB3: turn lcd/cursor on
#define HD44780_ON_DISPLAY              0x04  // DB2: turn display on
#define HD44780_ON_CURSOR               0x02  // DB1: turn cursor on
#define HD44780_ON_BLINK                0x01  // DB0: blinking cursor

#define HD44780_MOVE                    0x10  // DB4: move cursor/display
#define HD44780_MOVE_DISP               0x08  // DB3: move display (0-> move cursor)
#define HD44780_MOVE_RIGHT              0x04  // DB2: move right (0-> left)

#define HD44780_FUNCTION                0x20  // DB5: function set
#define HD44780_FUNCTION_8BIT           0x10  // DB4: set 8BIT mode (0->4BIT mode)
#define HD44780_FUNCTION_2LINES         0x08  // DB3: two lines (0->one line)
#define HD44780_FUNCTION_10DOTS         0x04  // DB2: 5x10 font (0->5x7 font)
#define HD44780_FUNCTION_RESET          0x30  // See "Initializing by Instruction" section

#define HD44780_CGRAM                   0x40  // DB6: set CG RAM address
#define HD44780_DDRAM                   0x80  // DB7: set DD RAM address

#define HD44780_RS_CMD                  0
#define HD44780_RS_DATA                 1

#define HD44780_RW_WRITE                0
#define HD44780_RW_READ                 1

#define HD44780_MASK_RS                 0x01  // P0
#define HD44780_MASK_RW                 0x02  // P1
#define HD44780_MASK_E                  0x04  // P2

#define HD44780_SHIFT_BACKLIGHT          3    // P3
#define HD44780_SHIFT_DATA               4    // P4-P7

#define HD44780_DEFAULT_ADDR            0x27
#define HD44780_DEFAULT_BAUDRATE        100000
#define HD44780_WRITE_TIMEOUT_US        5000  // 5MS

#define HD44780_NUM_COLUMNS             16
#define HD44780_NUM_ROWS                2


typedef struct hd44780 {
    i2c_inst_t *i2c;
    uint8_t addr;

    uint8_t backlight_mask;
    uint timeout_us;
} hd44780_t;

/**
 * @brief Initialize LCD
 * 
 * @param i2c 
 * @param addr 
 * @param baudrate 
 * @return hd44780_t 
 */
extern hd44780_t hd44780_init(i2c_inst_t *i2c, uint8_t addr, uint sda_pin, uint scl_pin, uint baudrate);

extern void hd44780_backlight_on(hd44780_t *hd44780);
extern void hd44780_backlight_off(hd44780_t *hd44780);

extern void hd44780_clear(hd44780_t *hd44780);
extern void hd44780_display_on(hd44780_t *hd44780);
extern void hd44780_display_off(hd44780_t *hd44780);

extern void hd44780_show_cursor(hd44780_t *hd44780);
extern void hd44780_hide_cursor(hd44780_t *hd44780);
extern void hd44780_blink_cursor_on(hd44780_t *hd44780);
extern void hd44780_move_cursor(hd44780_t *hd44780, uint cursor_x, uint cursor_y);
extern void hd44780_cursor_home(hd44780_t *hd44780);

extern void hd44780_put_char(hd44780_t *hd44780, uint8_t c);
extern void hd44780_put_str(hd44780_t *hd44780, uint8_t *str, uint len);

extern void hd44780_write_cgram(hd44780_t *hd44780);

#endif