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

#include "uartio.h"

int main() {

    uartio_t uartio = uartio_setup(uart0, 0, 1, 115200);
    uartio_start(&uartio);

    return 0;
}