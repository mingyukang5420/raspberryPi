#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <sys/ioctl.h>

static const char* I2C_DEV = "/dev/i2c-1";
static const int I2C_SLAVE = 0x0703;

static const int LPS25H = 0x5c;

static const int CTRL_REG1 = 0x20;
static const int CTRL_REG2 = 0x21;

static const int PRESS_OUT_XL = 0x28;
static const int PRESS_OUT_L = 0x29;
static const int PRESS_OUT_H = 0x2A;
static const int PTEMP_OUT_L = 0x2B;
static const int PTEMP_OUT_H = 0x2C;

void getPressure(int fd, double *temperature, double *pressure);

int main (){
    int i2c_fd;
    double t_c = 0.0;
    double pressure = 0.0;

    if((i2c_fd = open(I2C_DEV, O_RDWR)) < 0){
        perror("Unable to open I2C device");
        return 1;
    }

    if(ioctl(i2c_fd, I2C_SLAVE, LPS25H) < 0){
        perror("Unable to configure I2C slave device");
        close(i2c_fd);
        return 1;
    }

    for(int i = 0; i < 10; i++){
        wiringPiI2CWriteReg8(i2c_fd, CTRL_REG1, 0x00);
        wiringPiI2CWriteReg8(i2c_fd, CTRL_REG1, 0x84);
        wiringPiI2CWriteReg8(i2c_fd, CTRL_REG2, 0x01);

        getPressure(i2c_fd, &t_c, &pressure);

        printf("Temperature(from LPS25H) = %.2f°C\n", t_c);
        printf("Pressure = %.0f hPa\n", pressure);

        delay(1000);
    }

    wiringPiI2CWriteReg8(i2c_fd, CTRL_REG1, 0x00);
    close(i2c_fd);
    return 0;
}

void getPressure(int fd, double *temperature, double *pressure){
    int result;

    unsigned char temp_out_l = 0, temp_out_h = 0, press_out_xl = 0, press_out_l = 0, press_out_h = 0;

    short temp_out = 0;
    int press_out = 0;

    do{
        delay(25);
        result = wiringPiI2CReadReg8(fd, CTRL_REG2);
    }while(result != 0);

        temp_out_l = wiringPiI2CReadReg8(fd, PTEMP_OUT_L);
        temp_out_h = wiringPiI2CReadReg8(fd, PTEMP_OUT_H);

        press_out_xl = wiringPiI2CReadReg8(fd, PRESS_OUT_XL);
        press_out_l = wiringPiI2CReadReg8(fd, PRESS_OUT_L);
        press_out_h = wiringPiI2CReadReg8(fd, PRESS_OUT_H);

        temp_out = (temp_out_h << 8) | temp_out_l;
        press_out = (press_out_h << 16) | (press_out_l << 8) | press_out_xl;

        *temperature = 42.5 + ((double)temp_out / 480.0);
        *pressure = (double)press_out / 4096.0;
}
