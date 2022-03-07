/**
 * @file uart_io.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _UART_IO_HEADER_
#define _UART_IO_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/uart.h"
#include "hardware/rtc.h"
#include "hardware/timer.h"


typedef struct uart_io {
    uart_inst_t *uart;
    
    uint8_t *tx_buf;
    uint tx_idx;
    uint tx_buf_size;
    uint tx_timeout_ms;

    uint8_t *rx_buf;
    uint rx_idx;
    uint rx_buf_size;
    uint rx_timeout_ms;

} uart_io_t;

typedef void (*cmd_handler)(uint8_t *cmd);


extern uart_io_t *uart_io_init(uart_inst_t * uart, uint uart_tx_pin, uint uart_rx_pin, uint baudrate);
extern uart_io_t *uart_io_create(uart_inst_t *uart);
extern void uart_io_clear_buffers(uart_io_t *uart_io);

/**
 * @brief Write the first tx_idx bytes from the tx_buf to the uart.
 * 
 * @param uart_io 
 * @param src 
 * @param len 
 */
extern void uart_io_write_tx_buf(uart_io_t *uart_io);

/**
 * @brief Receive bytes from the uart into the rx_buf, wait for max rx_timeout_ms and optionally append it to
 * the current contents
 * 
 * @param uart_io 
 * @param rx_timeout_ms 
 * @param append 
 */
extern void uart_io_read_rx_buf(uart_io_t *uart_io, uint rx_timeout_ms, bool append);

/**
 * @brief Format a string and terminate it with newline (and carriage return) into the tx_buf and write it to the uart.
 * Wait for max rx_timeout_ms to receive bytes into the rx_buf
 * 
 * @param uart_io the uart_io structure 
 * @param fmt the C string format according to specifications as format in printf 
 * @param args the va_list of args which are handled as the args of vprintf
 */
extern void uart_io_send_vfmtln(uart_io_t *uart_io, uint8_t *fmt, va_list args);

/**
 * @brief bridge between usb stdio and uart
 * 
 * @param uart_io specifies which uart to connect to
 * @param cmd_tag the tag for commands to be redirected to the client program instead of the uart
 * @param handler the function to be invoked when a client command is detected
 */
extern void uart_io_start(uart_io_t *uart_io, uint8_t *cmd_tag, cmd_handler handler);

extern void uart_io_enable_irq(uart_io_t *uart_io);
extern void uart_io_disable_irq(uart_io_t *uart_io);

#endif //_UARTIO_HEADER