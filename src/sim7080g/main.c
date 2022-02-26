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

#include "sim7080g_cmd.h"


int main() {

    stdio_init_all();
    sleep_ms(1000);
    printf("Initializing. Please wait...\n");
    
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