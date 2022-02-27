/**
 * @file sim7080g_sms.c
 * @author Robert Portier(robert.portier@domain.com)
 * @brief Note that the sim7080g_sms can only receive SMS text messages
 * @version 0.1
 * @date 2022-02-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sim7080g_sms.h"

/**
 * @brief Configure for SMS text and passing of the message data in the unsollicited
 * result code (.i.e. +CMT: <oa>,,<timestamp><CR><LF><DATA>)
 * 
 * @return true 
 * @return false 
 */
bool sim7080g_sms_config_receive_text() {
    return sim7080g_send_atf_expect_OK("AT+CMFG=1")        // Set SMS mode to text
           && sim7080g_send_atf_expect_OK("AT+CNMI=2,2");  // Set unsollicited SMS result to contain message
}