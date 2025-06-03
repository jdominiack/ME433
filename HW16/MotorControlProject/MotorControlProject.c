#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PHASE_PIN 16
#define ENABLE_PIN 17

#define FREQ 75 //kHz

int dutyCycle = 0;
int cycleTime = (float)(1/(FREQ*1000)) * 1000000; //us


int main()
{
    stdio_init_all();

    gpio_init(PHASE_PIN);
    gpio_set_dir(PHASE_PIN, GPIO_OUT);

    //gpio_init(ENABLE_PIN);
    gpio_set_function(ENABLE_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(ENABLE_PIN); // Get PWM slice number
    float div = 2; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 1000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(ENABLE_PIN, 0); // set the duty cycle to 0%

    while (true) {
        int input = getchar_timeout_us(0);
        if (input != PICO_ERROR_TIMEOUT){
            if((char)input == '+' && dutyCycle < 100){
                //Increase duty cycle
                dutyCycle += 1;
                printf("Duty Cycle: %d\n\r", dutyCycle);
            }
            else if ((char)input == '-' && dutyCycle > -100){
                //Decrease duty cycle
                dutyCycle -= 1;
                printf("Duty Cycle: %d\n\r", dutyCycle);
            }
            pwm_set_gpio_level(ENABLE_PIN, wrap*abs(dutyCycle)/100);           
        }
        if (dutyCycle > 0){
            gpio_put(PHASE_PIN, 0);
        }
        else {
            gpio_put(PHASE_PIN, 1);
        }
       
    }
}




