/**
 * @file sim7080g_cmd.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Interact with sim7080g modem via USB tty 
 * @version 0.1
 * @date 2022-02-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sim7080g_core.h"
#include "sim7080g_network.h"
#include "sim7080g_tcpudp.h"
#include "sim7080g_ntp.h"
#include "sim7080g_http.h"

/**
 * @brief Start an interactive session where the uart_io connected the modem is working as a bridge
 * between the usb stdio and the uart connected to the modem. 
 * When the cmd_token is detected in the USB stdin it is sent to the client program by invoking the cmd_handler
 * 
 * 
 * @param cmd_token the token that marks the beginning of a client command.
 * @param handler cmd_handler invoked when the cmd_token is the prefix of the string read from the USB stdin
 */
extern void sim7080g_command();
