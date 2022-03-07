/**
 * @file sim7080g_ntp.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sim7080g_ntp.h"


bool sim7080g_sync_clock_ntp(uint8_t *ntp_server, uint timezone_offset)
{
    if (sim7080g_send_atf_expect_OK("AT+CNTP=\"%s\",%u,0,2", ntp_server, timezone_offset * 4)) {
        return sim7080g_send_at_expect("AT+CNTP", "+CNTP");
    }
    return false;
}
