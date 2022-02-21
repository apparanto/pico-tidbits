/**
 * @file usb2uart.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _USB2UART_HEADER_
#define _USB2UART_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"


typedef struct usb2uart {
    uart_inst_t *uart;
    
    uint8_t *tx_buf;
    uint tx_idx;
    uint tx_buf_size;
    uint tx_timeout_ms;

    uint8_t *rx_buf;
    uint rx_idx;
    uint rx_buf_size;
    uint rx_timeout_ms;

} usb2uart_t;

typedef void (*HandleCommand)(uint8_t *command);

#define USB2UART_NULL  (usb2uart_t){ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#define ISUSB2UART_NULL(usb2uart_obj) ((usb2uart_obj).uart == 0)

extern usb2uart_t usb2uart_setup(uart_inst_t * uart, uint uart_tx_pin, uint uart_rx_pin, uint baudrate);
extern usb2uart_t usb2uart_create_config(uart_inst_t *uart);
extern void usb2uart_clear_buffers(usb2uart_t *usb2uart);

/**
 * @brief Write the first tx_idx bytes from the tx_buf to the uart.
 * 
 * @param usb2uart 
 * @param src 
 * @param len 
 */
extern void usb2uart_write_tx_buf(usb2uart_t *usb2uart);

/**
 * @brief Receive bytes from the uart into the rx_buf, wait for max rx_timeout_ms and optionally append it to
 * the current contents
 * 
 * @param usb2uart 
 * @param rx_timeout_ms 
 * @param append 
 */
extern void usb2uart_read_rx_buf(usb2uart_t *usb2uart, uint rx_timeout_ms, bool append);

/**
 * @brief Format a string and terminate it with newline (and carriage return) into the tx_buf and write it to the uart.
 * Wait for max rx_timeout_ms to receive bytes into the rx_buf
 * 
 * @param usb2uart the usb2uart structure 
 * @param fmt the C string format according to specifications as format in printf 
 * @param args the va_list of args which are handled as the args of vprintf
 */
extern void usb2uart_send_vfmtln(usb2uart_t *usb2uart, uint8_t *fmt, va_list args);

/**
 * @brief Start interactive session passing data back and forth between usb and uart
 * 
 * @param usb2uart 
 */
extern void usb2uart_start(usb2uart_t *usb2uart);

#endif //_USB2UART_HEADER