/**
 * @file sim7080g_core.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Functions to setup uart0, buffer and initialize the sim7080g.
 * Also provides functions to send AT commands and retrieve the response.
 *
 * @version 0.1
 * @date 2022-02-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "sim7080g_core.h"

uart_io_t *sim7080g_io = NULL;

void sim7080g_init_uart()
{
    printf("Setting up uart_io\n");
    sim7080g_io = uart_io_init(SIM7080G_UART, SIM7080G_TX_PIN, SIM7080G_RX_PIN, SIM7080G_BAUD);
    uart_set_hw_flow(SIM7080G_UART, false, false);
}

void sim7080g_setup_power()
{
    printf("Setting up modem power\n");
    gpio_init(SIM7080G_PWR_PIN);
    gpio_set_dir(SIM7080G_PWR_PIN, GPIO_OUT);
    gpio_put(SIM7080G_PWR_PIN, false);
    sleep_ms(2000);
}

void sim7080g_toggle_power()
{
    printf("Toggling modem power\n");
    gpio_put(SIM7080G_PWR_PIN, 1);
    sleep_ms(3000);
    gpio_put(SIM7080G_PWR_PIN, 0);
    sleep_ms(2500);
}

/** AT command utility functions */

bool sim7080g_send(uint8_t *data, size_t len)
{
    uart_write_blocking(sim7080g_io->uart, data, len);
}

bool sim7080g_wait_for_response(uint8_t *expected_response, uint rx_timeout_ms)
{
    // printf("Expecting: [%s]\n", expected_response);
    uint64_t timeout_time = time_us_64() + rx_timeout_ms * 1000;
    bool append = false;
    uint8_t *strp = NULL;
    do
    {
        uart_io_read_rx_buf(sim7080g_io, rx_timeout_ms, append);
        append = true;
    } while (((strp = strstr(sim7080g_io->rx_buf, expected_response)) == NULL) && time_us_64() < timeout_time);

    return (strp != NULL);
}

bool sim7080g_send_atf_expect_OK(uint8_t *at_fmt, ...)
{
    va_list args;
    va_start(args, at_fmt);
    uart_io_send_vfmtln(sim7080g_io, at_fmt, args);
    va_end(args);

    return sim7080g_wait_for_response(SIM7080G_RESPONSE_OK, sim7080g_io->rx_timeout_ms);
}

bool sim7080g_send_atf_expect(uint8_t *at_fmt, uint8_t *expect, ...)
{
    va_list args;
    va_start(args, expect);
    uart_io_send_vfmtln(sim7080g_io, at_fmt, args);
    va_end(args);

    return sim7080g_wait_for_response(expect, sim7080g_io->rx_timeout_ms);
}

bool sim7080g_send_at_expect(uint8_t *at_str, uint8_t *expect)
{
    return sim7080g_send_atf_expect(at_str, expect);
}

void sim7080g_send_atf(uint8_t *at_fmt, ...)
{
    va_list args;
    va_start(args, at_fmt);
    uart_io_send_vfmtln(sim7080g_io, at_fmt, args);
    va_end(args);
}

/** END AT command utility functions */

bool sim7080g_check_startup()
{
    bool ready = false;
    for (int i = 0; i < SIM7080G_STARTUP_COUNT; i++)
    {
        if (sim7080g_send_atf_expect_OK("ATE0"))
        {
            printf("SIM7080G Ready\n");
            ready = true;
            break;
        }
        else
        {
            printf("SIM7080G Starting up...\n");
            if (i % (10) == 0)
            {
                sim7080g_toggle_power();
            }
            sleep_ms(2500);
        }
    }
    return ready;
}

bool sim7080g_set_pincode(uint pincode)
{
    if (sim7080g_send_at_expect("AT+CPIN?", "SIM PIN"))
    {
        return sim7080g_send_atf_expect_OK("AT+CPIN=%u", pincode);
    }
    else
    {
        return true;
    }
}

bool sim7080g_init(uint pincode)
{
    sim7080g_init_uart();
    if (sim7080g_send_atf_expect_OK("ATE0"))
    {
        return pincode == 0 || sim7080g_set_pincode(pincode);
    }
    else
    {
        sim7080g_setup_power();
        return sim7080g_check_startup() && (pincode == 0 || sim7080g_set_pincode(pincode));
    }
}

bool sim7080g_disable_uart_irq()
{
    uart_io_disable_irq(sim7080g_io);
}

bool sim7080g_enable_uart_irq()
{
    uart_io_enable_irq(sim7080g_io);
}