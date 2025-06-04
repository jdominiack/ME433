#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"


#define SERVO_PIN 21 // the built in LED on the Pico
#define WRAP 30000

void PWM_init();
void set_angle(float angle);


int main()
{
    stdio_init_all();

    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM); // Set the LED Pin to be PWM

    PWM_init();

    int servoAngle = 0;
    bool incline = true;


    while (true) {
        // pwm_set_gpio_level(SERVO_PIN, (uint16_t)(WRAP * 0.05));
        // set_angle(90);

        // sleep_ms(1000);

        // pwm_set_gpio_level(SERVO_PIN, (uint16_t)(WRAP * 0.06));
        // set_angle(100);

        // sleep_ms(1000);

        if (incline){
            servoAngle += 10;
            if (servoAngle > 180){
                incline = false;
                servoAngle -= 20;
            }
        }
        else {
            servoAngle -= 10;
            if (servoAngle < 0){
                incline = true;
                servoAngle += 20;
            }
        }
        
        set_angle(servoAngle);
        sleep_ms(300);
    }
}

void PWM_init(){
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN); // Get PWM slice number
    float div = 100; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    //uint16_t wrap = 1000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, (uint16_t)WRAP);
    pwm_set_enabled(slice_num, true); // turn on the PWM
    pwm_set_gpio_level(SERVO_PIN, (uint16_t)WRAP * 0.025); // set the duty cycle to 50%
}

void set_angle(float angle){
    if (angle <= 180 && angle >= 0){
        float duty_cycle = ((angle/180) * (0.125 - 0.025)) + 0.025;

        uint16_t wrap = 1000;
        pwm_set_gpio_level(SERVO_PIN, (uint16_t)(WRAP * duty_cycle));
    }

}
