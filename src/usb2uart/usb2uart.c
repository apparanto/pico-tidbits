/**
 * @file usb2uart.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief
 * @version 0.1
 * @date 2022-02-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "usb2uart.h"

#define DEFAULT_BUFFER_SIZE             2048
#define STDIN_CHAR_READ_TIMEOUT_US  100*1000
#define UART_CHAR_READ_TIMEOUT_US   100*1000
#define UART_TX_TIMEOUT                  500
#define UART_RX_TIMEOUT                 5000
#define CHAR_BACKSPACE                     8

usb2uart_t usb2uart_setup(uart_inst_t *uart, uint uart_tx_pin, uint uart_rx_pin, uint baudrate)
{
    uart_init(uart, baudrate);
    gpio_set_function(uart_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(uart_rx_pin, GPIO_FUNC_UART);

    return usb2uart_create_config(uart);
}


usb2uart_t usb2uart_create_config(uart_inst_t *uart)
{
    uint8_t *tx_buf = malloc(DEFAULT_BUFFER_SIZE);
    uint8_t *rx_buf = malloc(DEFAULT_BUFFER_SIZE);

    sleep_ms(3000);

    return (usb2uart_t){
        uart,

        tx_buf,
        (uint)0,
        (uint)DEFAULT_BUFFER_SIZE,
        (uint)UART_TX_TIMEOUT,

        rx_buf,
        (uint)0,
        (uint)DEFAULT_BUFFER_SIZE,
        (uint)UART_RX_TIMEOUT};
}


void usb2uart_clear_buffers(usb2uart_t *usb2uart) {
    usb2uart->tx_buf[0] = 0;
    usb2uart->tx_idx = 0;

    usb2uart->rx_buf[0] = 0;
    usb2uart->rx_idx = 0;
}

void usb2uart_write_tx_buf(usb2uart_t *usb2uart) {
    while (!uart_is_writable(usb2uart->uart)) {
        sleep_ms(100);
    }
    uart_write_blocking(usb2uart->uart, usb2uart->tx_buf, usb2uart->tx_idx);
}

void usb2uart_read_rx_buf(usb2uart_t *usb2uart, uint rx_timeout_ms, bool append) {
    if (!append) {
        usb2uart->rx_idx = 0;
        usb2uart->rx_buf[0] = 0;
    }
    if (uart_is_readable_within_us(usb2uart->uart, rx_timeout_ms * 1000))
    {
        do {
            uint8_t ch = uart_getc(usb2uart->uart);
            if (usb2uart->rx_idx > 0 || (ch && ch != '\n' && ch !='\r')) {
                usb2uart->rx_buf[usb2uart->rx_idx++] = ch;
            }
        } while (uart_is_readable_within_us(usb2uart->uart, UART_CHAR_READ_TIMEOUT_US));
    }
    usb2uart->rx_buf[usb2uart->rx_idx] = 0;
    if (!append) {
        printf("RX[%u]: %s", usb2uart->rx_idx, ((usb2uart->rx_idx) ? (char *)(usb2uart->rx_buf) : "-\r\n"));
    }
}

void usb2uart_send_vfmtln(usb2uart_t *usb2uart, uint8_t *fmt, va_list args)
{
    usb2uart->tx_idx = vsnprintf(usb2uart->tx_buf, usb2uart->tx_buf_size - 3, fmt, args);
    printf("TX: %s\n", usb2uart->tx_buf);

    usb2uart->tx_buf[usb2uart->tx_idx++] = '\r';
    usb2uart->tx_buf[usb2uart->tx_idx++] = '\n';
    usb2uart->tx_buf[usb2uart->tx_idx] = 0;

    usb2uart_write_tx_buf(usb2uart);

}

void usb2uart_start(usb2uart_t *usb2uart)
{
    usb2uart_clear_buffers(usb2uart);

    printf("Session started\n> ");
    bool wait_for_response = false;
    uint rx_timeout_ms = UART_CHAR_READ_TIMEOUT_US;
    while (true)
    {
        int16_t ch = getchar_timeout_us(100);

        while (ch != PICO_ERROR_TIMEOUT)
        {
            if (ch == 13) // send a command
            {
                if (usb2uart->tx_idx > 0)
                {
                    usb2uart->tx_buf[usb2uart->tx_idx++] = '\r';
                    usb2uart->tx_buf[usb2uart->tx_idx++] = '\n';
                    usb2uart->tx_buf[usb2uart->tx_idx] = 0;
                    usb2uart_write_tx_buf(usb2uart);
                    
                    wait_for_response = true;
                    rx_timeout_ms = usb2uart->rx_timeout_ms;
                    usb2uart->tx_idx = 0;
                }
            } 
            else
            {
                putchar(ch);
                if (ch == CHAR_BACKSPACE)
                {
                    usb2uart->tx_idx--;
                }
                else if (usb2uart->tx_idx < usb2uart->tx_buf_size - 3)
                {
                    usb2uart->tx_buf[usb2uart->tx_idx++] = ch & 0xff;
                }
            }
            ch = getchar_timeout_us(STDIN_CHAR_READ_TIMEOUT_US);
        }

        if (uart_is_readable_within_us(usb2uart->uart, rx_timeout_ms))
        {
            do {
                uint8_t ch = uart_getc(usb2uart->uart);
                putchar(ch);
            } while (uart_is_readable_within_us(usb2uart->uart, UART_CHAR_READ_TIMEOUT_US));
        }
        if (wait_for_response)
        {
            printf("\n> ");
            wait_for_response = false;
            rx_timeout_ms = UART_CHAR_READ_TIMEOUT_US;
        }
    }
}