#include "stm32f407xx.h"
#include "stm32f407xx_i2c_drivers.h"

#define MPU9250_ADDR   0x68
#define PWR_MGMT_1     0x6B
#define WHO_AM_I       0x75
#define ACCEL_XOUT_H   0x3B

extern I2C_Handle_t I2C1Handle; // Points to I2C1

void MPU9250_Init(void) {
    uint8_t tx_buf[2] = {PWR_MGMT_1, 0x00};
    I2C_MasterSendData(&I2C1Handle, tx_buf, 2, MPU9250_ADDR, I2C_DISABLE_SR);
}

void MPU9250_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t reg = ACCEL_XOUT_H;
    uint8_t data[6];
    I2C_MasterSendData(&I2C1Handle, &reg, 1, MPU9250_ADDR, I2C_ENABLE_SR);
    I2C_MasterReceiveData(&I2C1Handle, data, 6, MPU9250_ADDR, I2C_DISABLE_SR);
    *ax = (int16_t)((data[0] << 8) | data[1]);
    *ay = (int16_t)((data[2] << 8) | data[3]);
    *az = (int16_t)((data[4] << 8) | data[5]);
}
