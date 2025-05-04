#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LED_PIN 19
#define BUTTON_PIN 20
#define ADC_RESOLUTION 4095
#define BUTTON_SLEEP 200


int main()
{
    stdio_init_all();

    gpio_init(BUTTON_PIN);
    gpio_init(LED_PIN);

    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    gpio_put(LED_PIN, 1);

    while (gpio_get(BUTTON_PIN)){
        sleep_ms(100);
    }

    gpio_put(LED_PIN, 0);

    char message[100];

    while(1){

        printf("Please enter a number of analog samples to read (1 - 100): \n");
        scanf("%s", message);
        printf("Reading %s samples...\r\n",message);
        sleep_ms(50);
    
        int samples;
        sscanf(message, "%d", &samples);

        float voltage;
    
        for (int i = 0; i < samples; i++){
            voltage = 3.3 * (float) adc_read()/ADC_RESOLUTION;

            printf("%f\r\n", voltage);
            sleep_ms(10);
    
        }
    
    }


}
