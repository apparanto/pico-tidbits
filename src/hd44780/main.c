/**
 * Tidbit: hd44780
 *
 * @version     1.0.0
 * @author      Robert Portier
 * @copyright   2022
 * @licence     MIT
 *
 */

#include "hd44780.h"

int main()
{
    // Use for debugging
    stdio_init_all();

    hd44780_t hd44780 = hd44780_init(i2c_default, 
                                     HD44780_DEFAULT_ADDR, 
                                     PICO_DEFAULT_I2C_SDA_PIN, 
                                     PICO_DEFAULT_I2C_SCL_PIN, 
                                     HD44780_DEFAULT_BAUDRATE);
    hd44780_t *lcd = &hd44780;

    hd44780_backlight_on(lcd);
    hd44780_clear(lcd);
    hd44780_show_cursor(lcd);
    hd44780_blink_cursor_on(lcd);
    char str[] = "Hello world!\nWe meet again!";
    hd44780_put_str(lcd, str);

    // Do nothing
    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}
