/**
 * @file main.c
 * @author Robert Portier
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "usb2uart.h"

int main() {

    usb2uart_t usb2uart = usb2uart_setup(uart0, 0, 1, 115200);
    usb2uart_start(&usb2uart);

    return 0;
}