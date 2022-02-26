/**
 * @file uartio.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _UARTIO_HEADER_
#define _UARTIO_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"


typedef struct uartio {
    uart_inst_t *uart;
    
    uint8_t *tx_buf;
    uint tx_idx;
    uint tx_buf_size;
    uint tx_timeout_ms;

    uint8_t *rx_buf;
    uint rx_idx;
    uint rx_buf_size;
    uint rx_timeout_ms;

} uartio_t;

typedef void (*cmd_handler)(uint8_t *cmd);



#define UARTIO_NULL  (uartio_t){ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ISUARTIO_NULL(uartio_obj) ((uartio_obj).uart == 0)

extern uartio_t uartio_setup(uart_inst_t * uart, uint uart_tx_pin, uint uart_rx_pin, uint baudrate);
extern uartio_t uartio_create_config(uart_inst_t *uart);
extern void uartio_clear_buffers(uartio_t *uartio);

/**
 * @brief Write the first tx_idx bytes from the tx_buf to the uart.
 * 
 * @param uartio 
 * @param src 
 * @param len 
 */
extern void uartio_write_tx_buf(uartio_t *uartio);

/**
 * @brief Receive bytes from the uart into the rx_buf, wait for max rx_timeout_ms and optionally append it to
 * the current contents
 * 
 * @param uartio 
 * @param rx_timeout_ms 
 * @param append 
 */
extern void uartio_read_rx_buf(uartio_t *uartio, uint rx_timeout_ms, bool append);

/**
 * @brief Format a string and terminate it with newline (and carriage return) into the tx_buf and write it to the uart.
 * Wait for max rx_timeout_ms to receive bytes into the rx_buf
 * 
 * @param uartio the uartio structure 
 * @param fmt the C string format according to specifications as format in printf 
 * @param args the va_list of args which are handled as the args of vprintf
 */
extern void uartio_send_vfmtln(uartio_t *uartio, uint8_t *fmt, va_list args);

/**
 * @brief bridge between usb stdio and uart
 * 
 * @param uartio specifies which uart to connect to
 * @param cmd_tag the tag for commands to be redirected to the client program instead of the uart
 * @param handler the function to be invoked when a client command is detected
 */
extern void uartio_start(uartio_t *uartio, uint8_t *cmd_tag, cmd_handler handler);

#endif //_UARTIO_HEADER