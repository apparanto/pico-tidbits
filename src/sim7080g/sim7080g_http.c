/**
 * @file sim7080g_http.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sim7080g_http.h"

void sim7080g_set_http_size(uint body_size, uint header_size)
{
    sim7080g_send_atf_expect_OK("AT+SHCONF=\"BODYLEN\",%u", body_size);
    sim7080g_send_atf_expect_OK("AT+SHCONF=\"HEADERLEN\",%u", header_size);
}

void sim7080g_set_http_headers()
{
    sim7080g_send_atf_expect_OK("AT+SHCHEAD");
    sim7080g_send_atf_expect_OK("AT+SHAHEAD=\"Content-Type\",\"application/x-www-form-urlencoded\"");
    sim7080g_send_atf_expect_OK("AT+SHAHEAD=\"User-Agent\",\"pico_sim7080g\"");
    sim7080g_send_atf_expect_OK("AT+SHAHEAD=\"Cache-control\",\"no-cache\"");
    sim7080g_send_atf_expect_OK("AT+SHAHEAD=\"Connection\",\"keep-alive\"");
    sim7080g_send_atf_expect_OK("AT+SHAHEAD=\"Accept\",\"*/*\"");
    sim7080g_send_atf_expect_OK("AT+SHCHEAD");
}

bool sim7080g_http_get(uint8_t *host_url, uint8_t *path)
{
    sim7080g_send_at_expect("AT+SHDISC", "ERROR");

    sim7080g_send_atf_expect_OK("AT+SHCONF=\"URL\",\"%s\"", host_url);
    sim7080g_set_http_size(1024, 350);

    uint try_count = 3;
    bool connected = false;

    do {
        sim7080g_send_atf_expect_OK("AT+SHCONN");
        sim7080g_send_atf("AT+SHSTATE?");
        connected = sim7080g_wait_for_response("+SHSTATE: 1", 5000);
    } while (!connected && try_count--);

    if (connected)
    {
        sim7080g_set_http_headers();

        // Send the request
        sim7080g_send_atf_expect_OK("AT+SHREQ=\"%s\",1", path);
        // Wait for the HTTP response in an unsollicited message from the modem
        // (timeout 30 seconds)
        if (sim7080g_wait_for_response("+SHREQ:", 30000))
        {
            // Extract the length of the response
            uint8_t *lenp = strrchr(sim7080g_uartio.rx_buf, ',');
            lenp++;

            // Read the response
            sim7080g_send_atf_expect_OK("AT+SHREAD=0,%s", lenp);

            // Disconnect
            sim7080g_send_atf_expect_OK("AT+SHDISC");

            return true;
        }
        return false;
    }
    else
    {
        printf("HTTP connection disconnected, please check and try again\n");
        return false;
    }
}

bool sim7080g_test_http_get()
{
    return sim7080g_http_get("http://api.seniverse.com", "/v3/weather/now.json?key=SwwwfskBjB6fHVRon&location=shenzhen&language=en&unit=c");
}