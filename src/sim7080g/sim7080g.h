/**
 * @file sim7080g.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef _SIM7080G_HEADER_
#define _SIM7080G_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "hardware/uart.h"
#include "hardware/gpio.h"

#include "../usb2uart/usb2uart.h"

#define SIM7080G_UART           uart0
#define SIM7080G_BAUD           115200
#define SIM7080G_TX_PIN         0
#define SIM7080G_RX_PIN         1
#define SIM7080G_PWR_PIN        14
#define SIM7080G_STARTUP_COUNT  30
#define SIM7080G_RESPONSE_OK    "OK"


extern bool sim7080g_setup(uint pincode);
extern void sim7080g_toggle_power();

extern bool sim7080g_send_at(uint8_t *at_str, uint8_t *expected_response);
extern void sim7080g_start_interactive();
#endif