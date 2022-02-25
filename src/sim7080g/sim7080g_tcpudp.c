/**
 * @file sim7080g_tcpudp.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sim7080g_tcpudp.h"

bool sim7080g_open_udp_connection(uint8_t *server_url, uint port) {
    bool success = false;
    uint try_count = 3;
    do {
        sim7080g_send_atf("AT+CAOPEN=0,0,\"UDP\",\"%s\",%u", server_url, port);
        if (sim7080g_wait_for_response("+CAOPEN: 0,0", 5000)) {
            success = true;
        } else {
            sim7080g_send_atf_expect_OK("AT+CACLOSE=0");
            sleep_ms(2000);
        }
    } while (!success && try_count--);

    return success;
}

bool sim7080g_send_udp_data(uint8_t *data, size_t len) {
    bool success = false;
    sim7080g_send_atf("AT+CASEND=0,%u", len);
    if (sim7080g_wait_for_response(">", 5000)) {
        sim7080g_send_atf(data);
        if (sim7080g_wait_for_response("OK", 5000)) {
            success = true;
        }
    }

    return success;
}

bool sim7080g_test_udp()
{
    bool result = false;
    uint8_t *msg = "Hello world!";

    if (sim7080g_open_udp_connection("52.43.121.77", 10001) 
        && sim7080g_send_udp_data(msg, strlen(msg)))
    {
        sleep_ms(500);
        uint try_count = 3;
        do {
            result = sim7080g_send_at_expect("AT+CARECV=0,100", msg);
        } while (!result && try_count--);
    }
    sim7080g_send_atf_expect_OK("AT+CACLOSE=0");
    return result;
}

bool sim7080g_test_tcp()
{
    bool result = false;
    if (sim7080g_send_atf_expect_OK("AT+CAOPEN=0,0,\"TCP\",\"52.43.121.77\",9002") 
        && sim7080g_send_at_expect("AT+CASEND=0,48", ">"))
    {
        uint8_t d[48];
        for (int i = 0; i < 48; i++)
            d[i] = 0;
        sim7080g_send(d, sizeof(d));
        sleep_ms(500);
        sim7080g_send_atf_expect_OK("AT+CAACK=0");
        for (int i = 0; i < 10; i++)
        {
            if (sim7080g_send_at_expect("AT+CARECV=0,100", "DayTime Server Starts"))
            {
                printf("Buffer contents: %s\n", sim7080g_uartio.rx_buf);

                result = true;
                break;
            }
            sleep_ms(500);
        }
    }
    sim7080g_send_atf_expect_OK("AT+CACLOSE=0");
    return result;
}