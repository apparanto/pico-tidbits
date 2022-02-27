/**
 * @file sim7080g.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief Waveshare SIM7080G PICO HAT utility functions
 * @version 0.1
 * @date 2022-02-14
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "sim7080g_network.h"


bool sim7080g_setup_network()
{
    printf("Setting up network\n");

    if (sim7080g_send_atf_expect_OK("AT+CFUN=0") &&
        sim7080g_send_atf_expect_OK("AT+CNMP=2") &&
        sim7080g_send_atf_expect_OK("AT+CMNB=3") &&
        sim7080g_send_atf_expect_OK("AT+CFUN=1") &&
        sim7080g_send_atf_expect_OK("ATE0"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sim7080g_connect_network()
{
    printf("Connecting to network\n");
    bool online = false;

    // Response time of CGATT is up to 75 seconds!
    for (uint i = 0; i < 20; i++)
    {
        sim7080g_send_atf("AT+CGATT?");

        if (sim7080g_wait_for_response("+CGATT: 1", 5000))
        {
            printf("SIM7080G is online\n");
            sim7080g_send_atf_expect_OK("ATE0");
            online = true;
            break;
        }
        else
        {
            printf("SIM7080G is connecting. Please wait...\n");
        }
    }
    if (!online)
        return false;

    // Check connection
    if (sim7080g_send_atf_expect_OK("AT+CSQ") &&
        sim7080g_send_atf_expect_OK("AT+CPSI?") &&
        sim7080g_send_atf_expect_OK("AT+COPS?"))
    {
        // Retrieve APN
        if (sim7080g_send_atf_expect_OK("AT+CGNAPN"))
        {

            // Extract APN from response in uart_io rx buffer
            uint8_t *apn_str_start = strstr(sim7080g_io->rx_buf, ",\"") + 2;
            uint8_t *apn_str_end = strstr(apn_str_start, "\"");
            *apn_str_end = 0;
            printf("APN: %s\n", apn_str_start);

            // Set APN
            if (!sim7080g_send_atf_expect_OK("AT+CNCFG=0,1,\"%s\"", apn_str_start)) return false;
    
            
            // Connect
            bool connected = false;
            uint try_count = 3;
            do {
                sim7080g_send_atf_expect_OK("AT+CNACT=0,1");
                sim7080g_send_atf("AT+CNACT?");
                connected = sim7080g_wait_for_response("+CNACT: 0,1", 3000);
            } while (!connected && try_count--);
        }
    }
    else
    {
        return false;
    }
}

bool sim7080g_connect() {
    sim7080g_send_atf("AT+CNACT?");
    if (sim7080g_wait_for_response("+CNACT: 0,1", 3000)) {
        return true;
    }
    return sim7080g_setup_network() && sim7080g_connect_network();
}
