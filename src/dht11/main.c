/**
 * dht for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      apparanto
 * @copyright   2022
 * @licence     MIT
 *
 */

#include <stdio.h>
#include "dht11.h"

#define DHT11_PIN 11

int main()
{
    // Use for debugging
    stdio_init_all();

    dht11_t dht11 = dht11_init(DHT11_PIN);

    while (true)
    {
        dht11_read(&dht11);
        if (!dht11.error) {
            printf("T: %u.%u°C\n", dht11.temp_int, dht11.temp_dec);
            printf("H: %u.%u%%\n", dht11.humidity_int, dht11.humidity_dec);
        }

        sleep_ms(2000);
    }

    return 0;
}
