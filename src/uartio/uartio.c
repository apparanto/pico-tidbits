/**
 * @file uartio.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief
 * @version 0.1
 * @date 2022-02-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "uartio.h"

#define DEFAULT_BUFFER_SIZE             2048
#define STDIN_CHAR_READ_TIMEOUT_US  100*1000
#define UART_CHAR_READ_TIMEOUT_US   100*1000
#define UART_TX_TIMEOUT                  500
#define UART_RX_TIMEOUT                 5000
#define CHAR_BACKSPACE                     8

uartio_t uartio_setup(uart_inst_t *uart, uint uart_tx_pin, uint uart_rx_pin, uint baudrate)
{
    uart_init(uart, baudrate);
    gpio_set_function(uart_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(uart_rx_pin, GPIO_FUNC_UART);

    return uartio_create_config(uart);
}


uartio_t uartio_create_config(uart_inst_t *uart)
{
    uint8_t *tx_buf = malloc(DEFAULT_BUFFER_SIZE);
    uint8_t *rx_buf = malloc(DEFAULT_BUFFER_SIZE);

    sleep_ms(3000);

    return (uartio_t){
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


void uartio_clear_buffers(uartio_t *uartio) {
    uartio->tx_buf[0] = 0;
    uartio->tx_idx = 0;

    uartio->rx_buf[0] = 0;
    uartio->rx_idx = 0;
}

void uartio_write_tx_buf(uartio_t *uartio) {
    while (!uart_is_writable(uartio->uart)) {
        sleep_ms(100);
    }
    uart_write_blocking(uartio->uart, uartio->tx_buf, uartio->tx_idx);
}

void uartio_read_rx_buf(uartio_t *uartio, uint rx_timeout_ms, bool append) {
    if (!append) {
        uartio->rx_idx = 0;
        uartio->rx_buf[0] = 0;
    }
    if (uart_is_readable_within_us(uartio->uart, rx_timeout_ms * 1000))
    {
        do {
            uint8_t ch = uart_getc(uartio->uart);
            if (uartio->rx_idx > 0 || (ch && ch != '\n' && ch !='\r')) {
                uartio->rx_buf[uartio->rx_idx++] = ch;
            }
        } while (uart_is_readable_within_us(uartio->uart, UART_CHAR_READ_TIMEOUT_US));
    }
    uartio->rx_buf[uartio->rx_idx] = 0;
    if (!append) {
        printf("RX[%u]: %s", uartio->rx_idx, ((uartio->rx_idx) ? (char *)(uartio->rx_buf) : "-\r\n"));
    }
}

void uartio_send_vfmtln(uartio_t *uartio, uint8_t *fmt, va_list args)
{
    uartio->tx_idx = vsnprintf(uartio->tx_buf, uartio->tx_buf_size - 3, fmt, args);
    printf("TX: %s\n", uartio->tx_buf);

    uartio->tx_buf[uartio->tx_idx++] = '\r';
    uartio->tx_buf[uartio->tx_idx++] = '\n';
    uartio->tx_buf[uartio->tx_idx] = 0;

    uartio_write_tx_buf(uartio);

}

void uartio_start(uartio_t *uartio)
{
    uartio_clear_buffers(uartio);

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
                if (uartio->tx_idx > 0)
                {
                    uartio->tx_buf[uartio->tx_idx++] = '\r';
                    uartio->tx_buf[uartio->tx_idx++] = '\n';
                    uartio->tx_buf[uartio->tx_idx] = 0;
                    uartio_write_tx_buf(uartio);
                    
                    wait_for_response = true;
                    rx_timeout_ms = uartio->rx_timeout_ms;
                    uartio->tx_idx = 0;
                }
            } 
            else
            {
                putchar(ch);
                if (ch == CHAR_BACKSPACE)
                {
                    uartio->tx_idx--;
                }
                else if (uartio->tx_idx < uartio->tx_buf_size - 3)
                {
                    uartio->tx_buf[uartio->tx_idx++] = ch & 0xff;
                }
            }
            ch = getchar_timeout_us(STDIN_CHAR_READ_TIMEOUT_US);
        }

        if (uart_is_readable_within_us(uartio->uart, rx_timeout_ms))
        {
            do {
                uint8_t ch = uart_getc(uartio->uart);
                putchar(ch);
            } while (uart_is_readable_within_us(uartio->uart, UART_CHAR_READ_TIMEOUT_US));
        }
        if (wait_for_response)
        {
            printf("\n> ");
            wait_for_response = false;
            rx_timeout_ms = UART_CHAR_READ_TIMEOUT_US;
        }
    }
}