/**
 * @file dht11.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Verbose implementation of reading data from a DHT11 sensor
 * @version 0.1
 * @date 2022-02-07
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "dht11.h"

#define DATA_LENGTH     40
#define TRACE_DUMP    false

dht11_t dht11_init(uint dht11_pin)
{
    gpio_init(dht11_pin);
    dht11_t dht11 = {
        .pin = dht11_pin,
        .error = 0};
    return dht11;
}

/**
 * @brief Function to wait a maximum of us for a given level
 * 
 * @param pin the pin to poll
 * @param level the level to wait for
 * @param max_wait the max wait time (timeout) in us
 * @return uint32_t the elapsed time
 */
static inline uint32_t wait_for_level(uint pin, bool level, uint32_t max_wait)
{
    uint32_t time_start = time_us_32();
    uint32_t time_now = time_start;
    while (gpio_get(pin) != level && time_now - time_start <= max_wait)
    {
        time_now = time_us_32();
    }
    return time_now - time_start;
}

/**
 * @brief Read the data as described in dht11 spec sheet
 * E.g. https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf
 * I.e. Register the timings of each level duration in an array: If duration >= 70us then '1'  else '0'
 * 
 * @param dht11 
 */
void dht11_read(dht11_t *dht11)
{

    uint dht11_pin = dht11->pin;

    uint idx = 0;
    uint32_t low_times[DATA_LENGTH];
    uint32_t high_times[DATA_LENGTH];

    // 1. Initialize dht11
    //    Set pin to high for 50ms and then low for > 18ms
    gpio_set_dir(dht11_pin, GPIO_OUT);
    gpio_put(dht11_pin, 0);
    sleep_ms(25);
    gpio_put(dht11_pin, 1);

    // 2. Prepare pin to receive data
    gpio_set_dir(dht11_pin, GPIO_IN);

    // 3. Wait for data start

    // 3a. level is initially high, wait for low (20us-40us)
    uint32_t max_wait = 50;
    uint32_t elapsed = wait_for_level(dht11_pin, 0, max_wait);
    if (elapsed > max_wait || elapsed < 10)
    {
        dht11->error = 1;
        dht11->elapsed = elapsed;
        goto error; // This is a friendly goto
    }

    // 3b. level is low, wait for high (80us)
    max_wait = 100;
    elapsed = wait_for_level(dht11_pin, 1, max_wait);
    if (elapsed > max_wait || elapsed < 75)
    {
        dht11->error = 2;
        dht11->elapsed = elapsed;
        goto error;
    }

    // 3c level is high, wait for low again (80us) - data transfer begins after this
    max_wait = 100;
    elapsed = wait_for_level(dht11_pin, 0, max_wait);

    if (elapsed > max_wait || elapsed < 75)
    {
        dht11->error = 3;
        dht11->elapsed = elapsed;
        goto error;
    }

    // 4. Read bits
    while (idx < DATA_LENGTH)
    {
        // Pin is low because of a bit arriving
        // Wait 50us + 10us margin for level high
        max_wait = 60;
        elapsed = wait_for_level(dht11_pin, 1, max_wait);

        // Check if it is low too long:
        // if longer than 50us + margin of 10us then jump to error:
        if (elapsed > max_wait)
        {
            dht11->error = 4;
            dht11->elapsed = elapsed;
            goto error;
        }

        // Pin is now high 
#if TRACE_DUMP
        // Store the time it was low for trace dumping
        low_times[idx] = elapsed;
#endif
        // Now wait for the level to become low again
        // 26us-28us high => '0',
        // 70us high => '1'
        max_wait = 80;
        elapsed = wait_for_level(dht11_pin, 0, max_wait);

        // Check if it was high too long:
        // if longer than 70us + margin of 10us jump to error:
        if (elapsed > max_wait)
        {
            dht11->error = 5;
            dht11->elapsed = elapsed;
            goto error;
        }

        // Pin is now low - store the time it was high
        high_times[idx] = elapsed;

        // Read next bit
        idx++;
    }

#if TRACE_DUMP
    // Dump the timings
    printf("\nTimings:\n");
    for (uint i = 0; i < DATA_LENGTH; i++)
    {
        uint low_time = low_times[i];
        uint high_time = high_times[i];
        printf("%02d : low = %03u, high = %03u\n", i, low_time, high_time);
    }
#endif

    // Convert to 5 bytes
    uint8_t data[5] = { 0, 0, 0, 0, 0};
    for (uint i = 0; i < DATA_LENGTH; i++)
    {
        data[i / 8] |= (high_times[i] >= 70) << (7 - (i % 8));
    }

#if TRACE_DUMP
    printf("\nData:\n");
    for (int i = 0; i < 5; i++)
    {
        printf("%d: %u\n", i, data[i]);
    }
#endif

    // Compute checksum
    uint8_t c = 0;
    for (int i = 0; i < 4; i++)
    {
        c += data[i];
    }

#if TRACE_DUMP
    dht11->checksum = c & 0xff;
    printf("Checksum: %u\n", c & 0xff);
#endif
    // Check checksum
    if (c != data[4]) {
        dht11->error = 99;
        dht11->elapsed = 0;
        goto error;
    }

    // Store the results
    dht11->humidity_int = data[0];
    dht11->humidity_dec = data[1];

    dht11->temp_int = data[2];
    dht11->temp_dec = data[3];

    dht11->error = 0;

    return;

error:
    // Something went wrong
#ifdef DEBUG
    printf("Error [%d] reading DHT11 (elapsed: %u)\n", dht11->error, dht11->elapsed);
#endif
    return;
}
