/**
 * @file sim7080g_core.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Header file for the sim7080g core functionality
 * @version 0.1
 * @date 2022-02-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef _SIM7080G_CORE_HEADER_
#define _SIM7080G_CORE_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "hardware/uart.h"
#include "hardware/gpio.h"

#include "../uartio/uartio.h"


#define SIM7080G_UART           uart0
#define SIM7080G_BAUD           115200
#define SIM7080G_TX_PIN         0
#define SIM7080G_RX_PIN         1
#define SIM7080G_PWR_PIN        14
#define SIM7080G_WKU_PIN        17
#define SIM7080G_STARTUP_COUNT  50
#define SIM7080G_RESPONSE_OK    "OK"


extern uartio_t sim7080g_uartio;

/**
 * @brief Initialize the uart, buffer and simcom modem.
 * 
 * @param pincode The pincode for the sim, 0 if to be ommitted
 * @return true initialisation succeeded
 * @return false initialisation failed
 */
extern bool sim7080g_init(uint pincode);

/**
 * @brief This pulls the power pin hig for 3 seconds and then low for 2.5 seconds
 * 
 */
extern void sim7080g_toggle_power();

/**
 * @brief Send byte array directly to the sim7080g modem via uart0
 * 
 * @param data pointer to byte array
 * @param len length of byte array
 * @return true write succeeds within timeout configured in uartio
 * @return false write fails within timeout
 */
extern bool sim7080g_send(uint8_t *data, size_t len);

/**
 * @brief Formats a string into output buffer, appends it with '\r\n' and transmits the result to the sim7080g.
 * To be used to send AT commands to the modem.
 * 
 * @param at_fmt the string format
 * @param ... parameters that are interpreted in the same way as by printf
 */
extern void sim7080g_send_atf(uint8_t *at_fmt, ...);

/**
 * @brief Waits for a specific response to arrive in the uartio's rx buffer 
 * 
 * @param expected_response the response that is expected
 * @param rx_timeout_ms the max time in milliseconds to wait for the expected response to arrive
 * @return true if the expected response arrived in time
 * @return false if the expected response did not arrive within the specified timeout
 */
extern bool sim7080g_wait_for_response(uint8_t *expected_response, uint rx_timeout_ms);

/**
 * @brief Send an AT command and wait for a specific response
 * 
 * @param at_str the AT command to send to the modem
 * @param expect the expected response
 * @return true if the expected response arrived within the tx timeout of the uartio
 * @return false if the expected response did not arrive within the tx timeout of the uartio
 */
extern bool sim7080g_send_at_expect(uint8_t *at_str, uint8_t *expect);

/**
 * @brief Send a formatted AT command and wait for the response 'OK' from the modem.
 * 
 * @param at_fmt the AT command format according to the same string format as used by printf
 * @param ... the parameters to the AT command format working in the same way as for printf
 * @return true 
 * @return false 
 */
extern bool sim7080g_send_atf_expect_OK(uint8_t *at_fmt, ...);


#endif