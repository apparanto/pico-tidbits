/**
 * @file main.c
 * @author Robert Portier
 * @brief 
 * @version 0.1
 * @date 2022-02-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <time.h>


#include "sim7080g_cmd.h"


int main() {

    stdio_init_all();
    sleep_ms(3000);
    printf("Initializing. Please wait...\n");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf("%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    if (sim7080g_init(0)) {
        printf("Initialization succesful\n");
    } else {
        printf("Initialization failed\n");
    }

    sim7080g_command();

    while (true) {
        tight_loop_contents();
    }
    return 0;
}