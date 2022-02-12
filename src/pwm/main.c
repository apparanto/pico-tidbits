/**
 * @file main.c
 * @author Robert Portier (robert.portier@apparanto.com)
 * @brief 
 * @version 0.1
 * @date 2022-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "pwm.h"

#define PWM_PIN 15
#define WRAP 1000
#define LEVEL 750

int main() {

    tb_pwm_t tb_pwm = tb_pwm_start(PWM_PIN, WRAP, LEVEL);
    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}