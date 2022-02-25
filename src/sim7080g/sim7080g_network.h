/**
 * @file sim7080g_network.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Functions to start and configure a data connection using LTE NB-IOT
 * @version 0.1
 * @date 2022-02-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef _SIM7080G_NETWORK_HEADER_
#define _SIM7080G_NETWORK_HEADER_

#include "sim7080g_core.h"

/**
 * @brief Initializes the data network connections using LTE NB-IOT, retrieves the APN and uses to create a PDP context. 
 * 
 * @return true 
 * @return false 
 */
extern bool sim7080g_connect();

#endif