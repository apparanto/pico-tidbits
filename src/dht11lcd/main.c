/**
 * dht11_lcd for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      Robert Portier
 * @copyright   2022
 * @licence     MIT
 *
 */

#include <stdio.h>


#include "../dht11/dht11.h"
#include "../hd44780/hd44780.h"

#define DHT11_PIN 11

hd44780_t setup_lcd()
{
    hd44780_t hd44780 = hd44780_init(i2c_default,
                                     HD44780_DEFAULT_ADDR,
                                     PICO_DEFAULT_I2C_SDA_PIN,
                                     PICO_DEFAULT_I2C_SCL_PIN,
                                     HD44780_DEFAULT_BAUDRATE);
    hd44780_t *lcd = &hd44780;

    hd44780_backlight_on(lcd);
    hd44780_clear(lcd);
    // hd44780_show_cursor(lcd);
    // hd44780_blink_cursor_on(lcd);
    hd44780_hide_cursor(lcd);

    return hd44780;
}

int main()
{
    // Use for debugging
    stdio_init_all();

    dht11_t dht11 = dht11_init(DHT11_PIN);
    hd44780_t hd44780 = setup_lcd();
    hd44780_put_str(&hd44780, "Initializing...");

    bool initializing = true;
    while (true)
    {
        dht11_read(&dht11);
        if (!dht11.error)
        {
            printf("T: %u.%u°C\n", dht11.temp_int, dht11.temp_dec);
            printf("H: %u.%u%%\n", dht11.humidity_int, dht11.humidity_dec);

            if (initializing) {
                hd44780_clear(&hd44780);
                initializing = false;
            } else {
                hd44780_cursor_home(&hd44780);
            }

            char str[17];
            hd44780_put_str(&hd44780, " T: ");
            hd44780_put_char(&hd44780, 0x30 + (dht11.temp_int / 10)); 
            hd44780_put_char(&hd44780, 0x30 + (dht11.temp_int % 10)); 
            hd44780_put_char(&hd44780, '.');
            hd44780_put_char(&hd44780, 0x30 + (dht11.temp_dec % 10));             
            hd44780_put_char(&hd44780, 0xDF);
            hd44780_put_char(&hd44780, 'C');

            sprintf(str, "\n H: %u.%u ", dht11.humidity_int, dht11.humidity_dec);
            hd44780_put_str(&hd44780, str);
            hd44780_put_char(&hd44780, 0x25);

        }

        sleep_ms(2000);
    }

    return 0;
}
