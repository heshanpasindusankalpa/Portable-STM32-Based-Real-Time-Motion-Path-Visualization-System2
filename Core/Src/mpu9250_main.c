/*
 * MPU9250_main.c
 *
 *  Created on: May 2, 2026
 *      Author: ASUS
 */


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_i2c_drivers.h"

/* ---------------- MPU9250/6500 Defines ---------------- */
#define MPU9250_ADDR   0x68
#define PWR_MGMT_1     0x6B
#define WHO_AM_I       0x75
#define ACCEL_XOUT_H   0x3B

/* ---------------- Global Handles ---------------- */
I2C_Handle_t I2C1Handle;

/* ---------------- Function Prototypes ---------------- */
void I2C1_GPIOInit(void);
void I2C1_Inits(void);
void MPU9250_Init(void);
void MPU9250_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az);
uint8_t MPU9250_WhoAmI(void);
void delay(void);

/* ---------------- MAIN PROGRAM ---------------- */
int main(void)
{
    int16_t ax, ay, az;
    uint8_t device_id;

    // 1. Configure GPIO Pins for I2C1 (PB6, PB7)
    I2C1_GPIOInit();

    // 2. Configure I2C1 Peripheral settings
    I2C1_Inits();

    // 3. Enable the I2C Peripheral
    I2C_PeripheralControl(I2C1, ENABLE);

    // 4. Enable ACKing (Must be done after PE=1)
    I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

    // 5. Wake up the MPU sensor from sleep mode
    MPU9250_Init();

    // 6. Check communication (Should return 0x71 or 0x73 for MPU9250/6500)
    device_id = MPU9250_WhoAmI();

    while(1)
    {
        // Continuous data acquisition
        MPU9250_ReadAccel(&ax, &ay, &az);

        // Small delay to make the debug 'Expressions' window readable
        delay();
    }
}

/* ---------------- Helper Functions ---------------- */

void I2C1_GPIOInit(void)
{
    GPIO_Handle_t I2CPins;

    // Enable clock for GPIOB
    GPIO_PeriClockControl(GPIOB, ENABLE);

    I2CPins.pGPIOx = GPIOB;
    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    I2CPins.GPIO_PinConfig.GPIO_PinPulPdControl = GPIO_PU;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    // SCL -> PB6
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    GPIO_Init(&I2CPins);

    // SDA -> PB7
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
    GPIO_Init(&I2CPins);
}

void I2C1_Inits(void)
{
    // Enable clock for I2C1
    I2C_PeriClockControl(I2C1, ENABLE);

    I2C1Handle.pI2Cx = I2C1;
    I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C1Handle.I2C_Config.I2C_DeviceAddress = 0x61; // Arbitrary master address
    I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

    I2C_Init(&I2C1Handle);
}

void MPU9250_Init(void)
{
    uint8_t tx_buf[2] = {PWR_MGMT_1, 0x00}; // Resetting PWR_MGMT_1 wakes it up
    I2C_MasterSendData(&I2C1Handle, tx_buf, 2, MPU9250_ADDR, I2C_DISABLE_SR);
}

uint8_t MPU9250_WhoAmI(void)
{
    uint8_t id = 0;
    uint8_t reg = WHO_AM_I;
    I2C_MasterSendData(&I2C1Handle, &reg, 1, MPU9250_ADDR, I2C_ENABLE_SR);
    I2C_MasterReceiveData(&I2C1Handle, &id, 1, MPU9250_ADDR, I2C_DISABLE_SR);
    return id;
}

void MPU9250_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az)
{
    uint8_t reg = ACCEL_XOUT_H;
    uint8_t data[6];

    // Point to the first acceleration register
    I2C_MasterSendData(&I2C1Handle, &reg, 1, MPU9250_ADDR, I2C_ENABLE_SR);

    // Read 6 bytes consecutively (X_H, X_L, Y_H, Y_L, Z_H, Z_L)
    I2C_MasterReceiveData(&I2C1Handle, data, 6, MPU9250_ADDR, I2C_DISABLE_SR);

    *ax = (int16_t)((data[0] << 8) | data[1]);
    *ay = (int16_t)((data[2] << 8) | data[3]);
    *az = (int16_t)((data[4] << 8) | data[5]);
}

void delay(void)
{
    for(uint32_t i = 0; i < 250000; i++);
}
