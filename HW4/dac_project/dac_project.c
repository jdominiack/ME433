#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "math.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define VREF 2.7 // V

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    float v;
    float f = 2; //Hz
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    while (true) {
        float t = 0;

        for (int i = 0; i < 100; i++){
            sleep_ms(10);
            t += 0.01;

            //Square wave (2Hz):
            v = VREF * sin(2*(3.14)*f*t); 

            //Triangle wave (1Hz):
            //v = VREF * t;

            writeDac(0, v);
        }
    }
}

//channel is 1 or 0 that represents channel A or B
void writeDac(int channel, float voltage){
    uint8_t data[2];
    uint16_t v = voltage * 1024 / VREF;
    int len = 2;
    data[0] = 0b01110000;
    data[0] = data[0] | (channel<<7);
    data[0] = data[0] | (v >> 4);

    data[1] = 0b00000000;
    data[1] = data[1] | (v << 4);

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len);
    cs_deselect(PIN_CS);
}


