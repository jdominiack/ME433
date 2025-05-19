/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

#define FLAG_VALUE 123
#define LED_PIN 15

int command;
const float conversion_factor = 3.3f / (1 << 12);
float voltage;

void core1_entry() {

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    multicore_fifo_push_blocking(FLAG_VALUE);

    while (1){
        uint32_t com = multicore_fifo_pop_blocking();

        if (com == 0) {
            //read voltage on A0
            uint16_t adcVal = adc_read();
            voltage = (float)(adcVal * conversion_factor);
        }
        else if (com == 1) {
            //turn on GP15 LED
            gpio_put(LED_PIN, 1);
        }
        else if (com == 2) {
            //turn off GP15 LED
            gpio_put(LED_PIN, 0);
        }

        multicore_fifo_push_blocking(FLAG_VALUE);
    }
}

int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 0!\n");
    else {

        while (1) {
            printf("Enter a command: \n\r");
            scanf("%d", &command);
            printf("Command: %d\n\r", command);
            multicore_fifo_push_blocking(command);

            g = multicore_fifo_pop_blocking();

            if (g != FLAG_VALUE){
                printf("Hmm, that's not right on core 0!\n");
            }
            else if (command == 0){
                printf("Voltage: %f\n\n\r", voltage);
            }
            else if (command == 1){
                printf("LED on!\n\n\r");
            }
            else if (command == 2){
                printf("LED off!\n\n\r");
            }
            sleep_ms(500);
        }

    }
    

    /// \end::setup_multicore[]
}
