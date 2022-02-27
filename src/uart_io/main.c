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

#include "uart_io.h"

int main() {

    uart_io_t *uart_io = uart_io_init(uart0, 0, 1, 115200);
    uart_io_start(uart_io, NULL, NULL);

    return 0;
}