/**
 * @file sim7080g_ntp.h
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _SIM7080G_NTP_HEADER_
#define _SIM7080G_NTP_HEADER_

#include "sim7080g_network.h"

/**
 * @brief Synchronizes the internal clock with an NTP server
 * 
 * @param ntp_server the ntp server to use, e.g. ntp.time.nl
 * @param timezone_offset_quarts the timezone offset in quarts of an hour (e.g use value 4 for a +01 hour offset)
 * @return true - synchronization succeeded
 * @return false  - synchronization failed
 */
extern bool sim7080g_sync_clock_ntp(uint8_t *ntp_server, uint timezone_offset_quarts);



#endif