#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x68

#define NUM_BYTES 14

#define ACCEL_MAX 1

#define DISPLAY_WIDTH 120
#define DISPLAY_HEIGHT 32

void imu_init();
void imu_read();
void i2c_write(unsigned char address, unsigned char reg, unsigned char value);
void i2c_burst_read(unsigned char address, unsigned char reg);
void pixelArrayInit();
void drawAccel();

uint8_t burst_buf[NUM_BYTES];

float Xaccel;
float Yaccel;
float Zaccel;

float Xpos;
float Ypos;
float Zpos;

float xPixels[DISPLAY_WIDTH];
float yPixels[DISPLAY_HEIGHT];

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    ssd1306_setup();
    pixelArrayInit();
    imu_init();
    
    while (true) {
        imu_read();
        printf("X: %f  Y: %f  Z: %f  \n\r", Xaccel, Yaccel, Zaccel);

        drawAccel();
        ssd1306_drawPixel(1,1, 1);
        ssd1306_update();
        
        sleep_ms(10);
    }
}

void imu_init(){
    //Turn the chip on
    i2c_write(WHO_AM_I, PWR_MGMT_1, 0x00);

    //Enable accelerometer
    i2c_write(WHO_AM_I, ACCEL_CONFIG, 0b00000000);

    //Enable gyroscope
    i2c_write(WHO_AM_I, GYRO_CONFIG, 0b00011000);
}

void imu_read(){
    i2c_burst_read(WHO_AM_I, ACCEL_XOUT_H);

    Xaccel = (float)(((uint16_t)burst_buf[0] << 8) | (burst_buf[1]))*0.000061; 
    Yaccel = (float)(((uint16_t)burst_buf[2] << 8) | (burst_buf[3]))*0.000061; 
    Zaccel = (float)(((uint16_t)burst_buf[4] << 8) | (burst_buf[5]))*0.000061; 

    Xpos = (float)(((uint16_t)burst_buf[8] << 8) | (burst_buf[9]))*0.007630;
    Ypos = (float)(((uint16_t)burst_buf[10] << 8) | (burst_buf[11]))*0.007630;
    Zpos = (float)(((uint16_t)burst_buf[12] << 8) | (burst_buf[13]))*0.007630;  

    }

void i2c_write(unsigned char address, unsigned char reg, unsigned char value) {
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(i2c_default, address, buf, 2, false);
}

void i2c_burst_read(unsigned char address, unsigned char reg){
    i2c_write_blocking(i2c_default, address, &reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, address, burst_buf, NUM_BYTES, false);  // false - finished with bus
}

void pixelArrayInit(){
    
    float start = -ACCEL_MAX;
    float end = 0;

    float xStep = (end - start) / (DISPLAY_WIDTH/2);
    float yStep = (end - start) / (DISPLAY_HEIGHT/2);

    for (int i = 0; i < (DISPLAY_WIDTH/2); i++) {
        xPixels[i] = start + i * xStep;
        xPixels[DISPLAY_WIDTH - i - 1] = -start - i * xStep;
    }
    for (int i = 0; i < (DISPLAY_HEIGHT/2); i++) {
        yPixels[i] = -start - i * yStep; 
        yPixels[DISPLAY_HEIGHT - i - 1] = start + i * yStep;
    }
}

void drawAccel(){
    imu_read();

    if (Zaccel > 3) {
        Zaccel = (1-(Zaccel - 3))*-1; 
        for (int i = 0; i < DISPLAY_WIDTH; i++){
            if (i < DISPLAY_WIDTH/2){
                if (Zaccel < xPixels[i]){
                    ssd1306_drawPixel(i,DISPLAY_HEIGHT/2, 1);
                }
                else {
                    ssd1306_drawPixel(i,DISPLAY_HEIGHT/2, 0);
                }
            }
            else {
                ssd1306_drawPixel(i,DISPLAY_HEIGHT/2, 0);
            }
        }
    }
    else if(Zaccel > 0){
        for (int i = 0; i < DISPLAY_WIDTH; i++){
            if (i < DISPLAY_WIDTH/2){
                ssd1306_drawPixel(i,DISPLAY_HEIGHT/2, 0);
            }
            else {
                if (Zaccel > xPixels[i]){
                    ssd1306_drawPixel(i,DISPLAY_HEIGHT/2, 1);
                }
                else {
                    ssd1306_drawPixel(i,DISPLAY_HEIGHT/2, 0);
                }
            }
        }
    }
    
    if (Yaccel > 3) {
        Yaccel = (1 - (Yaccel - 3))*-1;
        for (int i = 0; i < DISPLAY_HEIGHT; i++){
            if (i < DISPLAY_HEIGHT/2){
                ssd1306_drawPixel(DISPLAY_WIDTH/2,i, 0);
            }
            else {
                if (Yaccel < yPixels[i]){
                    ssd1306_drawPixel(DISPLAY_WIDTH/2,i, 1);
                }
                else {
                    ssd1306_drawPixel(DISPLAY_WIDTH/2,i, 0);
                }
            }
        }
    }
    else if(Yaccel > 0){
        for (int i = 0; i < DISPLAY_HEIGHT; i++){
            if (i < DISPLAY_HEIGHT/2){
                if (Yaccel > yPixels[i]){
                    ssd1306_drawPixel(DISPLAY_WIDTH/2,i, 1);
                }
                else {
                    ssd1306_drawPixel(DISPLAY_WIDTH/2,i, 0);
                }
            }
            else {
                ssd1306_drawPixel(DISPLAY_WIDTH/2,i, 0);
            }
        }
    }
    
}
