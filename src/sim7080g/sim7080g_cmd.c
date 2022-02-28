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

#define CMD_TOKEN               "#"

#define CMD_INIT                "#IN"
#define CMD_TOGGLE_POWER        "#TP"
#define CMD_CONNECT             "#CO"
#define CMD_UE_SYS_INF          "#US"
#define CMD_SYNC_NTP            "#ST"
#define CMD_TEST_HTTP_GET       "#THG"
#define CMD_POWER_DOWN          "#PD"

#define CMD_LIST                "#?"

#define HANDLERS_LEN_INC 10

typedef struct
{
    uint8_t *cmd;
    cmd_handler handler;
    uint8_t *desc;
} handler_entry_t;

/**
 * @brief Get the next cmd token
 *
 * @param cmd the command string containing command and args
 * @param prev_token the previous parsed token
 * @return uint8_t* the next token, null if none is available
 */
uint8_t *get_next_cmd_token(uint8_t *cmd, uint8_t *prev_token)
{
    static uint8_t *token_buf = NULL;
    static uint max_token_length = 0;

    uint8_t *cmd_token_ptr = cmd; // Points to a token in the command string
    uint8_t *token_buf_ptr = NULL; // Point to a location in the token buffer

    // If there is a previous token used from the cmd, look it up and skip it.
    if (prev_token)
    {
        cmd_token_ptr = strstr(cmd, prev_token);
        cmd_token_ptr += strlen(prev_token);
    }

    // Advance to the next token in the cmd string
    while (*cmd_token_ptr != ' ' && *cmd_token_ptr != 0)
        cmd_token_ptr++;

    // If there are no more tokens return NULL
    if (*cmd_token_ptr == 0)
    {
        return NULL;
    }

    // Allocate memory for the token in token_buf
    if (max_token_length <= strlen(cmd))
    {
        // release previously allocated memory
        if (max_token_length)
        {
            free(token_buf);
        }
        max_token_length = strlen(cmd) + 1;
        token_buf = malloc(max_token_length);

        // Point to the start of the token buffer
        token_buf_ptr = token_buf;
    }

    // Copy the next token to token_buf
    do {
        *token_buf_ptr++ = *cmd_token_ptr++;
    } while (*cmd_token_ptr != 0);

    // Terminate it
    *token_buf_ptr = 0;
    printf("Parsed token: %s\n", token_buf);

    // The token is in the token_buf;
    return token_buf;
}

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

void cmd_handler_ue_sysinfo(uint8_t *cmd)
{
    sim7080g_send_atf_expect_OK("AT+CPSI?");
}

void cmd_handler_sync_ntp(uint8_t *cmd)
{
    int tz_offset = 0;

    uint8_t *arg = get_next_cmd_token(cmd, NULL );
    if (arg) {
        tz_offset = atoi(arg);
    }
    sim7080g_sync_clock_ntp("ntp.time.nl", tz_offset);
}

void cmd_handler_test_http(uint8_t *cmd)
{
    sim7080g_test_http_get();
}

void cmd_handler_power_down(uint8_t *cmd)
{
    sim7080g_send_at_expect("AT+CPOWD=1", "NORMAL POWER DOWN");
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

void cmd_list_handlers(uint8_t *cmd)
{
    printf("Available handlers (%u):\n", handlers_count);
    for (int i = 0; i < handlers_count; i++)
    {
        handler_entry_t entry = handlers[i];
        printf("    - %s: %s\n", entry.cmd, entry.desc);
    }
}

void setup_handlers()
{
    add_handler(CMD_INIT, cmd_handler_init, "initialize the modem");
    add_handler(CMD_TOGGLE_POWER, cmd_handler_tp, "toggle the modem power");
    add_handler(CMD_CONNECT, cmd_handler_connect, "connect to the network");
    add_handler(CMD_UE_SYS_INF, cmd_handler_ue_sysinfo, "show the UE system information");
    add_handler(CMD_SYNC_NTP, cmd_handler_sync_ntp, "sync the internal modem clock and system clock using ntp\nUsage: #ST [<timezone offset>]");
    add_handler(CMD_TEST_HTTP_GET, cmd_handler_test_http, "test http get using http://httpbin.org/get");
    add_handler(CMD_POWER_DOWN, cmd_handler_power_down, "power down the modem");

    add_handler(CMD_LIST, cmd_list_handlers, "This list");
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
    cmd_list_handlers(NULL);
    uart_io_start(sim7080g_io, CMD_TOKEN, command_handler);
}