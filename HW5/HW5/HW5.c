#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define DAC_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define RAM_CS   13
#define VREF 3.3


static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);

union FloatInt {
    float f;
    uint32_t i;
};

int main()
{
    stdio_init_all();
    spi_init();

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
    //printf("%llu \n\r", addTime);
    int addClocks = (int)(addTime / 6.667);
    printf("Addition Cycles: %d \n\r", addClocks);

    start = get_absolute_time();
    for (int i = 0; i < 1000 ; i++){
        f_sub = f1-f2;
    }
    stop = get_absolute_time();

    uint64_t subTime = to_us_since_boot(stop - start);
    int subClocks = (int)(subTime / 6.667);
    printf("Subtraction Cycles: %d \n\r", subClocks);

    start = get_absolute_time();
    for (int i = 0; i < 1000 ; i++){
        f_mult = f1*f2;
    }
    stop = get_absolute_time();

    uint64_t multTime = to_us_since_boot(stop - start);
    int multClocks = (int)(multTime / 6.667);
    printf("Multiplication Cycles %d \n\r", multClocks);

    start = get_absolute_time();
    for (int i = 0; i < 1000 ; i++){
        f_div = f1/f2;
    }
    stop = get_absolute_time();

    uint64_t divTime = to_us_since_boot(stop - start);
    int divClocks = (int)(divTime / 6.667);
    printf("Division Cycles: %d \n\r", divClocks);

    float v;

    //RAM Stuff
    for (int i = 0; i < 1000; i++){
        //calculate sine
        v = VREF/2 * sin(4*(3.14)*i) + VREF/2;

        //write v to ram
        spi_ram_write(i, v);
    }

    int addrCount = 0;
    while (true) {
        v = spi_ram_read(addrCount);
        writeDac(0, v);

        addrCount += 1;

        if (addrCount > 999){
            addrCount = 0;
        }

        sleep_ms(1);
    }

}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void spi_init(){
    //SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(RAM_CS,   GPIO_FUNC_SIO);
    gpio_set_function(DAC_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    //Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(RAM_CS, GPIO_OUT);
    gpio_put(RAM_CS, 1);

    gpio_set_dir(DAC_CS, GPIO_OUT);
    gpio_put(DAC_CS, 1);
}

void spi_ram_init(){
    uint8_t buf[2];
    buf[0] = 0b1;
    buf[1] = 0b01000000; // seq mode

    cs_select(RAM_CS);
    spi_write_blocking(spi_default, buf, 2); 
    cs_deselect(RAM_CS);   
}

void spi_ram_write(uint16_t addr, float v){
    uint8_t buf[7];
    buf[0] = 0b00000010;
    buf[1] = addr>>8;
    buf[2] = addr&0xFF;

    union FloatInt num;
    num.f = v;

    buf[3] = (num.i>>24)&0xFF;
    buf[4] = (num.i>>16)&0xFF;
    buf[5] = (num.i>>8)&0xFF;
    buf[6] = num.i&0xFF;
    
    cs_select(RAM_CS);
    spi_write_blocking(spi_default, buf, 7);
    cs_deselect(RAM_CS);

}

float spi_ram_read(uint16_t address){
    uint8_t write[7], read[7];
    write[0] = 0b00000011;
    write[1] = (address>>8) & 0xFF;
    write[2] = address & 0xFF;

    cs_select(RAM_CS);
    spi_write_read_blocking(spi_default, write, read, 7);
    cs_deselect(RAM_CS);

    union FloatInt num;
    num.i = ((uint32_t)read[3] << 24) |
            ((uint32_t)read[4] << 16) |
            ((uint32_t)read[5] << 8)  |
            ((uint32_t)read[6]);

    return num.f;
}

void writeDac(int channel, float voltage){
    uint8_t data[2];
    uint16_t v = (uint16_t)(voltage * 1024.0 / VREF);
    int len = 2;
    data[0] = 0b01110000;
    data[0] = data[0] | (channel << 7);
    data[0] = data[0] | (uint8_t)((v >> 6) & 0xF); 

    data[1] = 0b00000000;
    data[1] = data[1] | (uint8_t)((v << 2) & 0x00FF); 

    cs_select(DAC_CS);
    spi_write_blocking(SPI_PORT, data, len);
    cs_deselect(DAC_CS);
}