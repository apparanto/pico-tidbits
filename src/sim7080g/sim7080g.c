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

#include "sim7080g.h"



static usb2uart_t sim7080g_usb2uart = USB2UART_NULL;

void sim7080g_init_uart()
{
    sim7080g_usb2uart = usb2uart_setup(SIM7080G_UART, SIM7080G_TX_PIN, SIM7080G_RX_PIN, SIM7080G_BAUD);
    uart_set_hw_flow(SIM7080G_UART, false, false);
}

void sim7080g_setup_power()
{
    gpio_init(SIM7080G_PWR_PIN);
    gpio_set_dir(SIM7080G_PWR_PIN, GPIO_OUT);
    gpio_put(SIM7080G_PWR_PIN, false);
}

void sim7080g_toggle_power()
{
    printf("Toggling modem power\n");
    gpio_put(SIM7080G_PWR_PIN, 1);
    sleep_ms(3000);
    gpio_put(SIM7080G_PWR_PIN, 0);
}

/** AT command utility functions */

bool sim7080g_send(uint8_t *data, size_t len)
{
    uart_write_blocking(sim7080g_usb2uart.uart, data, len);
}


bool sim7080g_wait_for_response(uint8_t *expected_response, uint rx_timeout_ms)
{
    //printf("Expecting: [%s]\n", expected_response);
    uint64_t timeout_time = time_us_64() + rx_timeout_ms * 1000;
    bool append = false;
    uint8_t *strp = NULL;
    do {
        usb2uart_read_rx_buf(&sim7080g_usb2uart, rx_timeout_ms, append);
        append = true;
    } while (((strp = strstr(sim7080g_usb2uart.rx_buf, expected_response)) == NULL) 
        && time_us_64() < timeout_time);

    return (strp != NULL);
}

bool sim7080g_send_atf_expect_OK(uint8_t *at_fmt, ...)
{
    va_list args;
    va_start(args, at_fmt);
    usb2uart_send_vfmtln(&sim7080g_usb2uart, at_fmt, args);
    va_end(args);

    return sim7080g_wait_for_response(SIM7080G_RESPONSE_OK, sim7080g_usb2uart.rx_timeout_ms);
}

bool sim7080g_send_at_expect(uint8_t *at_str, uint8_t *expect, ...)
{
    va_list args;
    va_start(args, expect);
    usb2uart_send_vfmtln(&sim7080g_usb2uart, at_str, args);
    va_end(args);

    return sim7080g_wait_for_response(expect, sim7080g_usb2uart.rx_timeout_ms);
}

void sim7080g_send_atf(uint8_t *at_fmt, ...)
{
    va_list args;
    va_start(args, at_fmt);
    usb2uart_send_vfmtln(&sim7080g_usb2uart, at_fmt, args);
    va_end(args);
}

/** END AT command utility functions */

bool sim7080g_check_startup()
{
    bool ready = false;
    sim7080g_init_uart();
    for (int i = 1; i <= SIM7080G_STARTUP_COUNT; i++)
    {
        if (sim7080g_send_atf_expect_OK("ATE0"))
        {
            printf("SIM7080G Ready\n");
            ready = true;
            break;
        }
        else
        {
            printf("SIM7080G Starting up...\n");
            if (i % (SIM7080G_STARTUP_COUNT / 5) == 0)
            {
                sim7080g_toggle_power();
            }
            sleep_ms(2000);
        }
    }
    return ready;
}

bool sim7080g_set_pincode(uint pincode)
{
    if (sim7080g_send_at_expect("AT+CPIN?", "SIM PIN"))
    {
        return sim7080g_send_atf_expect_OK("AT+CPIN=%u", pincode);
    }
    else
    {
        return true;
    }
}

bool sim7080g_setup_network()
{
    printf("Setting up network\n");

    if (sim7080g_send_atf_expect_OK("AT+CFUN=6") &&
        sim7080g_send_atf_expect_OK("AT+CNMP=2") &&
        sim7080g_send_atf_expect_OK("AT+CMNB=3") &&
        sim7080g_send_atf_expect_OK("AT+CFUN=1") &&
        sim7080g_send_atf_expect_OK("ATE0"))
    {

        sleep_ms(5000);
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
    for (uint i = 0; i < 15; i++)
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

            // Extract APN from response in usb2uart rx buffer
            uint8_t *apn_str_start = strstr(sim7080g_usb2uart.rx_buf, ",\"") + 2;
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

bool sim7080g_setup_ssl() {
    // close any open connection
    sim7080g_send_atf("AT+CASTATE?");
    if (sim7080g_wait_for_response("+CASTATE: 0,1", 1000)) {
        sim7080g_send_atf_expect_OK("AT+CACLOSE=0");
    }
    sim7080g_send_atf_expect_OK("AT+CSSLCFG=\"SSLVERSION\",0,3") &&
    sim7080g_send_atf_expect_OK("AT+CASSLCFG=0,\"SSL\",1") &&
    sim7080g_send_atf_expect_OK("AT+CASSLCFG=0,\"CRINDEX\",0");
}

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
                printf("Buffer contents: %s\n", sim7080g_usb2uart.rx_buf);

                result = true;
                break;
            }
            sleep_ms(500);
        }
    }
    sim7080g_send_atf_expect_OK("AT+CACLOSE=0");
    return result;
}

bool sim7080g_sync_clock_ntp(uint8_t *ntp_server, uint timezone_offset)
{
    if (sim7080g_send_atf_expect_OK("AT+CNTP=\"%s\",%u,0,2", ntp_server, timezone_offset * 4)) {
        return sim7080g_send_atf_expect_OK("AT+CNTP");
    }
    return false;
}

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
            uint8_t *lenp = strrchr(sim7080g_usb2uart.rx_buf, ',');
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

bool sim7080g_setup(uint pincode)
{
    sim7080g_init_uart();
    sim7080g_setup_power();
    return sim7080g_check_startup() 
            && (pincode == 0 || sim7080g_set_pincode(pincode))
            && sim7080g_setup_network() 
            && sim7080g_connect_network()
            && sim7080g_setup_ssl()
            && sim7080g_sync_clock_ntp("ntp.time.nl", 1)
            && sim7080g_test_udp()
            && sim7080g_test_tcp()
            && sim7080g_test_http_get();
}

void sim7080g_start_interactive()
{
    usb2uart_start(&sim7080g_usb2uart);
}
