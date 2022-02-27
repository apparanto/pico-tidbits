/**
 * @file sim7080g_cmd.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-02-26
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "sim7080g_cmd.h"

#define CMD_TOKEN "#"

#define CMD_INIT            "#IN"
#define CMD_TOGGLE_POWER    "#TP"
#define CMD_CONNECT         "#CO"
#define CMD_POWER_DOWN      "#PD"
#define CMD_UE_SYS_INF      "#US"
#define CMD_TEST_HTTP_GET   "#THG"

#define HANDLERS_LEN_INC 10

typedef struct
{
    uint8_t *cmd;
    cmd_handler handler;
    uint8_t *desc;
} handler_entry_t;

void cmd_handler_init(uint8_t *cmd)
{
    sim7080g_init(0);
}

void cmd_handler_tp(uint8_t *cmd)
{
    sim7080g_toggle_power();
}

void cmd_handler_connect(uint8_t *cmd)
{
    sim7080g_connect();
}

void cmd_handler_power_down(uint8_t *cmd)
{
    sim7080g_send_at_expect("AT+CPOWD=1", "NORMAL POWER DOWN");
}

void cmd_handler_ue_sysinfo(uint8_t *cmd)
{
    sim7080g_send_atf_expect_OK("AT+CPSI?");
}

void cmd_handler_test_http(uint8_t *cmd)
{
    sim7080g_test_http_get();
}

static handler_entry_t *handlers = NULL;
static uint handlers_count = 0;
static uint handlers_len = 0;

void add_handler(uint8_t *cmd, cmd_handler handler, uint8_t *description)
{
    handlers_count;
    if (handlers_count >= handlers_len)
    {
        uint new_handlers_len = handlers_len + HANDLERS_LEN_INC;
        handler_entry_t *new_handlers = malloc(new_handlers_len * sizeof(handler_entry_t));
        for (int i = 0; i < handlers_len; i++)
        {
            new_handlers[i] = handlers[i];
        }

        if (handlers_len > 0)
        {
            free(handlers);
        }
        handlers = new_handlers;
        handlers_len = new_handlers_len;
    }
    handlers[handlers_count++] = (handler_entry_t){cmd, handler, description};
}

void setup_handlers()
{
    add_handler(CMD_INIT, cmd_handler_init, "initialize the modem");
    add_handler(CMD_TOGGLE_POWER, cmd_handler_tp, "toggle the modem power");
    add_handler(CMD_CONNECT, cmd_handler_connect, "connect to the network");
    add_handler(CMD_POWER_DOWN, cmd_handler_power_down, "power down the modem");
    add_handler(CMD_UE_SYS_INF, cmd_handler_ue_sysinfo, "show the UE system information");
    add_handler(CMD_TEST_HTTP_GET, cmd_handler_test_http, "test http get using http://httpbin.org/get");
}

void show_handlers()
{
    printf("Available handlers (%u):\n", handlers_count);
    for (int i = 0; i < handlers_count; i++)
    {
        handler_entry_t entry = handlers[i];
        printf("    - %s: %s\n", entry.cmd, entry.desc);
    }
}

cmd_handler cmd_handler_get_for_cmd(uint8_t *cmd)
{
    for (int i = 0; i < handlers_count; i++)
    {
        handler_entry_t entry = handlers[i];
        if (strncasecmp(entry.cmd, cmd, strlen(entry.cmd)) == 0)
        {
            return entry.handler;
        }
    }
    return NULL;
}

void command_handler(uint8_t *cmd)
{
    printf("Handling command: %s\n", cmd);
    cmd_handler handler = cmd_handler_get_for_cmd(cmd);
    if (handler)
    {
        handler(cmd);
    }
    else
    {
        printf("No handler found for command: %s\n", cmd);
    }
}

void sim7080g_command()
{
    setup_handlers();
    show_handlers();
    uart_io_start(sim7080g_io, CMD_TOKEN, command_handler);
}