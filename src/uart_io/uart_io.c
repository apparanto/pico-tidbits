/**
 * @file uart_io.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief
 * @version 0.1
 * @date 2022-02-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "uart_io.h"

#define DEFAULT_BUFFER_SIZE 2048
#define STDIN_CHAR_READ_TIMEOUT_US 100 * 1000
#define UART_CHAR_READ_TIMEOUT_US 100 * 1000
#define UART_TX_TIMEOUT_MS 500
#define UART_RX_TIMEOUT_MS 500
#define CHAR_BACKSPACE 8

static uart_io_t *uart0_io = NULL;
static uart_io_t *uart1_io = NULL;

void handle_uart0_rx_irq(void)
{
    printf("UART0 IRQ\n");
    uart_io_read_rx_buf(uart0_io, uart0_io->rx_timeout_ms, true);
    printf("UART0 IRQ - handled\n");
}

void handle_uart1_rx_irq(void)
{
    printf("UART1 IRQ\n");
    uart_io_read_rx_buf(uart1_io, uart1_io->rx_timeout_ms, true);
}

void uart_io_setup_irq_handler(uart_io_t *uart_io)
{
    uart_inst_t *uart = uart_io->uart;
    if (uart == uart0)
    {
        irq_clear(UART0_IRQ);
        irq_set_exclusive_handler(UART0_IRQ, handle_uart0_rx_irq);
        irq_set_enabled(UART0_IRQ, true);
    }
    else if (uart == uart1)
    {
        irq_clear(UART1_IRQ);
        irq_set_exclusive_handler(UART1_IRQ, handle_uart1_rx_irq);
        irq_set_enabled(UART1_IRQ, true);
    }
    uart_set_fifo_enabled(uart, false);
    uart_set_irq_enables(uart, true, false);
}

void uart_io_enable_irq(uart_io_t *uart_io)
{
    uart_inst_t *uart = uart_io->uart;
    if (uart == uart0)
    {
        irq_clear(UART0_IRQ);
        irq_set_enabled(UART0_IRQ, true);
    }
    else if (uart == uart1)
    {
        irq_clear(UART1_IRQ);
        irq_set_enabled(UART1_IRQ, true);
    }
    uart_set_irq_enables(uart, true, false);
}

void uart_io_disable_irq(uart_io_t *uart_io)
{
    uart_inst_t *uart = uart_io->uart;
    if (uart == uart0)
    {
        irq_clear(UART0_IRQ);
        irq_set_enabled(UART0_IRQ, false);
    }
    else if (uart == uart1)
    {
        irq_clear(UART1_IRQ);
        irq_set_enabled(UART1_IRQ, false);
    }
    uart_set_irq_enables(uart, false, false);
}

void uart_io_setup_uart(uart_io_t *uart_io, uint uart_tx_pin, uint uart_rx_pin, uint baudrate)
{
    uart_init(uart_io->uart, baudrate);
    gpio_set_function(uart_tx_pin, GPIO_FUNC_UART);
    gpio_set_function(uart_rx_pin, GPIO_FUNC_UART);
}

uart_io_t *uart_io_init(uart_inst_t *uart, uint uart_tx_pin, uint uart_rx_pin, uint baudrate)
{
    uart_io_t *uart_io = uart_io_create(uart);
    uart_io_setup_uart(uart_io, uart_tx_pin, uart_rx_pin, baudrate);
    uart_io_disable_irq(uart_io);

    return uart_io;
}

void uart_io_release(uart_io_t *uart_io)
{
    if (uart_io)
    {
        bool do_release = false;
        if (uart_io == uart0_io)
        {
            uart0_io = NULL;
            do_release = true;
        }
        else if (uart_io == uart1_io)
        {
            uart1_io = NULL;
            do_release = true;
        }
        if (do_release)
        {
            free(uart_io->tx_buf);
            free(uart_io->rx_buf);
            free(uart_io);
        }
    }
}

/**
 * @brief Create an uart_io instance. Free any previously created instances.
 *
 * @param uart
 * @return uart_io_t*
 */
uart_io_t *uart_io_create(uart_inst_t *uart)
{
    // Get the pointer to the uart_io_t instance corresponding to the selected uart (0 or 1)
    uart_io_t **uart_io_selp = uart == uart1 ? &uart1_io : &uart0_io;
    uart_io_t *uart_io = *uart_io_selp;

    // If it refers to an existing instance, release the allocated memory
    uart_io_release(uart_io);

    // allocate memory for uart_io_t structure and its buffers
    uart_io = malloc(sizeof(uart_io_t));

    // Initialize the structure
    uart_io->uart = uart;

    // Setup the tx buffer
    uart_io->tx_buf = malloc(DEFAULT_BUFFER_SIZE);
    uart_io->tx_idx = 0;
    uart_io->tx_buf_size = DEFAULT_BUFFER_SIZE;
    uart_io->tx_timeout_ms = UART_TX_TIMEOUT_MS;

    // Setup the rex buffer
    uart_io->rx_buf = malloc(DEFAULT_BUFFER_SIZE);
    uart_io->rx_idx = 0;
    uart_io->rx_buf_size = DEFAULT_BUFFER_SIZE;
    uart_io->rx_timeout_ms = UART_TX_TIMEOUT_MS;

    // Assign the pointer
    *uart_io_selp = uart_io;

    return uart_io;
}

void uart_io_clear_buffers(uart_io_t *uart_io)
{
    uart_io->tx_buf[0] = 0;
    uart_io->tx_idx = 0;

    uart_io->rx_buf[0] = 0;
    uart_io->rx_idx = 0;
}

void uart_io_write_tx_buf(uart_io_t *uart_io)
{
    while (!uart_is_writable(uart_io->uart))
    {
        sleep_ms(uart_io->tx_timeout_ms);
    }
    uart_write_blocking(uart_io->uart, uart_io->tx_buf, uart_io->tx_idx);
}

void uart_io_read_rx_buf(uart_io_t *uart_io, uint rx_timeout_ms, bool clear)
{
    if (clear)
    {
        uart_io->rx_idx = 0;
        uart_io->rx_buf[0] = 0;
    }
    uint rx_idx_start = uart_io->rx_idx;
    if (uart_is_readable_within_us(uart_io->uart, rx_timeout_ms * 1000))
    {
        do
        {
            uint8_t ch = uart_getc(uart_io->uart);
            uart_io->rx_buf[uart_io->rx_idx++] = ch;
        } while (uart_is_readable_within_us(uart_io->uart, UART_CHAR_READ_TIMEOUT_US));
    }
    uart_io->rx_buf[uart_io->rx_idx] = 0;
    uint received_len = uart_io->rx_idx - rx_idx_start;
    uint8_t *new_line = "\r\n";
    printf("RX[%u]: %s", received_len, (received_len ? (uart_io->rx_buf + rx_idx_start) : new_line));
}

void uart_io_send_vfmtln(uart_io_t *uart_io, uint8_t *fmt, va_list args)
{
    uart_io->tx_idx = vsnprintf(uart_io->tx_buf, uart_io->tx_buf_size - 3, fmt, args);
    printf("TX: %s\n", uart_io->tx_buf);

    uart_io->tx_buf[uart_io->tx_idx++] = '\r';
    uart_io->tx_buf[uart_io->tx_idx++] = '\n';
    uart_io->tx_buf[uart_io->tx_idx] = 0;

    uart_io_write_tx_buf(uart_io);
}

void print_prompt()
{
    if (rtc_running())
    {
        datetime_t t;
        rtc_get_datetime(&t);
        printf("%04u-%02u-%02u %02u:%02u:%02u > ", t.year, t.month, t.day, t.hour, t.min, t.sec);
    }
    else
    {
        printf("> ");
    }
}

/**
 * @brief Start bridging the usb and uart
 *
 * @param uart_io
 * @param cmd_token
 * @param handler
 */
void uart_io_start(uart_io_t *uart_io, uint8_t *cmd_token, cmd_handler handler)
{
    uart_io_clear_buffers(uart_io);
    uart_io_setup_irq_handler(uart_io);

    printf("Session started\n");
    print_prompt();
    bool wait_for_response = false;
    uint rx_timeout_ms = UART_CHAR_READ_TIMEOUT_US;
    uint cmd_token_len = strlen(cmd_token);
    while (true)
    {
        int16_t ch = getchar_timeout_us(100);

        while (ch != PICO_ERROR_TIMEOUT)
        {
            if (ch == 13) // send a command
            {
                if (uart_io->tx_idx > 0)
                {
                    printf("\n");
                    // if it is a client command send it to the client
                    uart_io_disable_irq(uart_io);
                    if (strncmp(uart_io->tx_buf, cmd_token, cmd_token_len) == 0)
                    {
                        uart_io->tx_buf[uart_io->tx_idx] = 0;
                        handler(uart_io->tx_buf);
                    }
                    else
                    {
                        // send the command to the UART
                        uart_io->tx_buf[uart_io->tx_idx++] = '\r';
                        uart_io->tx_buf[uart_io->tx_idx++] = '\n';
                        uart_io->tx_buf[uart_io->tx_idx] = 0;
                        uart_io_write_tx_buf(uart_io);
                        printf("TX[%d]: %s", uart_io->tx_idx, uart_io->tx_buf);

                        // Wait for an answer;
                        uart_io_read_rx_buf(uart_io, uart_io->rx_timeout_ms, true);
                    }
                    uart_io->tx_idx = 0;
                    uart_io_enable_irq(uart_io);

                    print_prompt();
                }
            }
            else
            {
                putchar(ch);
                if (ch == CHAR_BACKSPACE)
                {
                    uart_io->tx_idx--;
                }
                else if (uart_io->tx_idx < uart_io->tx_buf_size - 3)
                {
                    uart_io->tx_buf[uart_io->tx_idx++] = ch & 0xff;
                }
            }
            ch = getchar_timeout_us(STDIN_CHAR_READ_TIMEOUT_US);
        }
    }
}