#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

int pico_led_init(void);
void pico_set_led(bool led_on);
void drawChar(int xO, int yO, char character);
void drawString(int xO, int yO, char str[]);

int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    const float conversion_factor = 3.3f / (1 << 12);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    pico_led_init();
    pico_set_led(true);

    ssd1306_setup();

    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    bool pixelOn = false;

    //Increase x -> pixel moves left
    //Increase y -> pixel moves up

    int frameCounter = 0;
    float fps = 0;
    uint64_t start = to_us_since_boot(get_absolute_time());
    uint64_t stop;

    while (true) {
        sleep_ms(1);

        uint16_t adcVolt = adc_read();
        char vReport[50];
        sprintf(vReport, "Voltage: %.2fV", adcVolt*conversion_factor);

        char fpsReport[50];
        sprintf(fpsReport, "FPS: %.2f", fps);

        drawString(120,24,vReport);
        drawString(65, 0, fpsReport);

        ssd1306_update();
        frameCounter += 1;

        if (frameCounter%10 == 0){
            stop = to_us_since_boot(get_absolute_time());
            fps = (float)(frameCounter/((float)(stop-start)/1000000));

            start = to_us_since_boot(get_absolute_time());
            frameCounter = 0;
        }

        if (gpio_get(PICO_DEFAULT_LED_PIN)){
            pico_set_led(false);
        }
        else {
            pico_set_led(true);
        }
    }
}

int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

void drawChar(int xO, int yO, char character){
    char col;
    for (int i = 0; i < 5; i++){
        col = ASCII[(int)character-32][i];
        for (int j = 0; j < 8; j++){
            if ((col >> j) & 0b1){
                ssd1306_drawPixel(xO-i,yO+7-j, 1);
            }
            else{
                ssd1306_drawPixel(xO-i,yO+7-j, 0);
            }
        }
    }
}

void drawString(int xO, int yO, char str[]){
    int y = yO;
    int x = xO;
    for (int i = 0; str[i] != '\0'; i++){
        if ((x-7*i - 5) < 0){
            y -= 8;
            x = 120;
        }
        drawChar(x-7*i, y, str[i]);
    }
}

