#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19


int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    // spi_init(SPI_PORT, 1000*1000);
    // gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    // gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    // gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    // gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    // gpio_set_dir(PIN_CS, GPIO_OUT);
    // gpio_put(PIN_CS, 1);


    volatile float f1, f2;

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("Enter two floats to use: ");
    scanf("%f %f", &f1, &f2);

    printf("\nFloats: %f, %f \n", f1, f2);

    volatile float f_add, f_sub, f_mult, f_div;
    absolute_time_t start;
    absolute_time_t stop;
    uint64_t startms;
    uint64_t stopms;

    start = get_absolute_time();
    for (int i = 0; i < 1000 ; i++){
        f_add = f1+f2;
    }
    stop = get_absolute_time();

    uint64_t addTime = to_us_since_boot(stop - start);
    printf("%llu \n\r", addTime);
    int addClocks = (int)(addTime / 6.667);
    printf("%d", addClocks);

    start = get_absolute_time();
    for (int i = 0; i < 1000 ; i++){
        f_sub = f1-f2;
    }
    stop = get_absolute_time();

    uint64_t subTime = to_us_since_boot(stop);

    start = get_absolute_time();
    for (int i = 0; i < 1000 ; i++){
        f_mult = f1*f2;
    }
    stop = get_absolute_time();

    uint64_t multTime = to_us_since_boot(stop);

    start = get_absolute_time();
    for (int i = 0; i < 1000 ; i++){
        f_div = f1/f2;
    }
    stop = get_absolute_time();

    uint64_t divTime = to_us_since_boot(stop);


    //printf("Results: \n Addition - %llu\n Subtraction - %llu\n Multiplication - %llu\n Division - %llu\n", addTime, subTime, multTime, divTime);
    printf("Results: \n\r");
    printf("somethign \n\r");
    printf("%d", addClocks);
    //printf("Subtraction - %llu \n\r", addClocks);
}
