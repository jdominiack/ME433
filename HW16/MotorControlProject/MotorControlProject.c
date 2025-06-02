#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"


#define PHASE_PIN 2
#define ENABLE_PIN 3

#define FREQ 20 //kHz

int dutyCycle = 0;
int cycleTime = (float)(1/(FREQ*1000)) * 1000000; //us


int main()
{
    stdio_init_all();

    gpio_init(PHASE_PIN);
    gpio_set_dir(PHASE_PIN, GPIO_OUT);

    gpio_init(ENABLE_PIN);
    gpio_set_dir(ENABLE_PIN, GPIO_OUT);

    while (true) {
        int input = getchar_timeout_us(0);
        if (input != PICO_ERROR_TIMEOUT){
            if((char)input == '+' && dutyCycle < 100){
                //Increase duty cycle
                dutyCycle += 1;
                printf("Duty Cycle: %d", dutyCycle);
            }
            else if ((char)input == '-' && dutyCycle > -100){
                //Decrease duty cycle
                dutyCycle -= 1;
                printf("Duty Cycle: %d", dutyCycle);
            }           
        }

        if (dutyCycle > 0){
            gpio_put(ENABLE_PIN, 1);
            gpio_put(PHASE_PIN, 0);
        }
        else {
            gpio_put(ENABLE_PIN, 1);
            gpio_put(PHASE_PIN, 1);
        }

        sleep_us((int)(dutyCycle*cycleTime/100));

        gpio_put(ENABLE_PIN, 0);
        gpio_put(PHASE_PIN, 0);

        sleep_us((int)((100-dutyCycle)*cycleTime/100));
       
    }
}




