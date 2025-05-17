#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#define READ_ADDR 0b01000001
#define WR_ADDR 0b01000000
#define GPIO_REG 0b00001001
#define OLAT_REG 0b00001010
#define IODIR_REG 0b00000000

void setPin(unsigned char address, unsigned char reg, unsigned char value);
unsigned char readPin(unsigned char address, unsigned char reg);
int pico_led_init(void);
void pico_set_led(bool led_on);

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    pico_led_init();
    pico_set_led(true);

    expander_init();

    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    unsigned char IOstates;

    while (true) {

        IOstates = readPin(READ_ADDR, GPIO_REG);

        if (!(IOstates & 0b1)){
            //set GP0 high (turn on LED)
            setPin(WR_ADDR, OLAT_REG, 0b00000001);
        }
        else {
            //set GP0 low (turn off LED)
            setPin(WR_ADDR, OLAT_REG, 0b00000000);
        }

        sleep_ms(100);

        if (gpio_get(PICO_DEFAULT_LED_PIN)){
            pico_set_led(false);
        }
        else {
            pico_set_led(true);
        }
    
    }
}

void expander_init(){
    unsigned char IODirections = 0b01111111;
    setPin(WR_ADDR, IODIR_REG, IODirections);
}

void setPin(unsigned char address, unsigned char reg, unsigned char value) {
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(i2c_default, address, buf, 2, false);

}

unsigned char readPin(unsigned char address, unsigned char reg){
    unsigned char buf;
    i2c_write_blocking(i2c_default, address, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, address, &buf, 1, false);  // false - finished with bus
    return buf;
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
