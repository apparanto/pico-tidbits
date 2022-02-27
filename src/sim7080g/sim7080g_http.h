/**
 * @file sim7080g_http.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _SIM7080G_HTTP_HEADER_
#define _SIM7080G_HTTP_HEADER_

#include "sim7080g_network.h"

/**
 * @brief HTTP get
 * 
 * @param host_url 
 * @param path 
 * @return true 
 * @return false 
 */
extern bool sim7080g_http_get(uint8_t *host_url, uint8_t *path);

/**
 * @brief Test HTTP get using http://httpbin.org/get
 * 
 * @return true 
 * @return false 
 */
extern bool sim7080g_test_http_get();

#endif