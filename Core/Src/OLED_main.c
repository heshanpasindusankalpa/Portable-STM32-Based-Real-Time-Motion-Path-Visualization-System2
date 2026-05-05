///*
// * OLED_main.c
// *
// *  Created on: May 2, 2026
// *      Author: ASUS
// */
//#include <stdio.h>
//#include <string.h>
//#include <stdint.h>
//#include "stm32f407xx.h"
//#include "stm32f407xx_gpio.h"
//#include "stm32f407xx_i2c_drivers.h"
//
///* ---------------- OLED (SSD1306) Defines ---------------- */
//#define OLED_ADDR    0x3C
//
//I2C_Handle_t I2C1Handle;
//
///* ---------------- DELAY ---------------- */
//void delay(void)
//{
//    // Approximately 200ms delay
//    for(uint32_t i = 0; i < 500000/2; i++);
//}
//
///* ---------------- I2C GPIO INIT ---------------- */
///* ---------------- I2C GPIO INIT (I2C2) ---------------- */
//void I2C2_GPIOInit(void)
//{
//    GPIO_Handle_t I2CPins;
//
//    // Enable GPIOB clock
//    GPIO_PeriClockControl(GPIOB, ENABLE);
//
//    I2CPins.pGPIOx = GPIOB;
//    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//    I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
//    I2CPins.GPIO_PinConfig.GPIO_PinPulPdControl = GPIO_PU;
//    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4; // AF4 is for I2C1/2/3
//    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//
//    // I2C2 Pins: SCL -> PB10, SDA -> PB11
//    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
//    GPIO_Init(&I2CPins);
//
//    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_11;
//    GPIO_Init(&I2CPins);
//}
//
///* ---------------- I2C PERIPHERAL INIT (I2C2) ---------------- */
//void I2C2_Inits(void)
//{
//    // Use I2C2 clock control
//    I2C_PeriClockControl(I2C2, ENABLE);
//
//    I2C1Handle.pI2Cx = I2C2; // Point handle to I2C2 peripheral
//    I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
//    I2C1Handle.I2C_Config.I2C_DeviceAddress = 0x61;
//    I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
//    I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
//
//    I2C_Init(&I2C1Handle);
//}
//
//void OLED_Init(void) {
//    OLED_WriteCmd(0xAE); OLED_WriteCmd(0xD5); OLED_WriteCmd(0x80);
//    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F); OLED_WriteCmd(0xD3);
//    OLED_WriteCmd(0x00); OLED_WriteCmd(0x40); OLED_WriteCmd(0x8D);
//    OLED_WriteCmd(0x14); OLED_WriteCmd(0x20); OLED_WriteCmd(0x00);
//    OLED_WriteCmd(0xA1); OLED_WriteCmd(0xC8); OLED_WriteCmd(0xAF);
//}
//
//void OLED_Fill(uint8_t pattern) {
//    for(uint16_t i = 0; i < 1024; i++) OLED_WriteData(pattern);
//}
//
//void OLED_WriteCmd(uint8_t cmd) {
//    uint8_t tx[2] = {0x00, cmd};
//    I2C_MasterSendData(&I2C2Handle, tx, 2, OLED_ADDR, I2C_DISABLE_SR);
//}
//
//void OLED_WriteData(uint8_t data) {
//    uint8_t tx[2] = {0x40, data};
//    I2C_MasterSendData(&I2C2Handle, tx, 2, OLED_ADDR, I2C_DISABLE_SR);
//}
//void OLED_Fill(uint8_t pattern) {
//    for(uint16_t i = 0; i < 1024; i++) OLED_WriteData(pattern);
//}
//
//void OLED_SetCursor(uint8_t page, uint8_t column) {
//    OLED_WriteCmd(0xB0 + (page & 0x07));
//    OLED_WriteCmd(0x00 + (column & 0x0F));
//    OLED_WriteCmd(0x10 + ((column >> 4) & 0x0F));
//}
//
//void OLED_DrawPoint(uint8_t x, uint8_t y) {
//    if (x >= 128 || y >= 64) return;
//    uint8_t page = y / 8;
//    uint8_t bit_pos = 1 << (y % 8);
//    OLED_SetCursor(page, x);
//    OLED_WriteData(bit_pos);
//}
//
///* ---------------- MAIN ---------------- */
//int main(void)
//{
//    // 1. Hardware Setup for I2C2
//    I2C2_GPIOInit();
//    I2C2_Inits();
//
//    // Enable I2C2 Peripheral
//    I2C_PeripheralControl(I2C2, ENABLE);
//    I2C_ManageAcking(I2C2, I2C_ACK_ENABLE);
//
//    // 2. Display Setup
//    OLED_Init();
//
//    while(1)
//    {
//        OLED_Fill(0xFF);
//        delay();
//        OLED_Fill(0x00);
//        delay();
//    }
//}


/*
 * OLED_main.c
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_i2c_drivers.h"

/* ---------------- OLED Defines ---------------- */
#define OLED_ADDR  0x78   // 0x3C << 1 (IMPORTANT for your driver)

/* ---------------- I2C Handle ---------------- */
I2C_Handle_t I2C1Handle;

/* ---------------- FUNCTION PROTOTYPES ---------------- */
void delay(void);

void I2C2_GPIOInit(void);
void I2C2_Inits(void);

void OLED_WriteCmd(uint8_t cmd);
void OLED_WriteData(uint8_t data);
void OLED_Init(void);
void OLED_Fill(uint8_t pattern);
void OLED_SetCursor(uint8_t page, uint8_t column);
void OLED_DrawPoint(uint8_t x, uint8_t y);

/* ---------------- DELAY ---------------- */
void delay(void)
{
    for(uint32_t i = 0; i < 250000; i++);
}

/* ---------------- I2C GPIO INIT ---------------- */
void I2C2_GPIOInit(void)
{
    GPIO_Handle_t I2CPins;

    GPIO_PeriClockControl(GPIOB, ENABLE);

    I2CPins.pGPIOx = GPIOB;
    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    I2CPins.GPIO_PinConfig.GPIO_PinPulPdControl = GPIO_PU;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    // PB10 -> SCL
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
    GPIO_Init(&I2CPins);

    // PB11 -> SDA
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_11;
    GPIO_Init(&I2CPins);
}

/* ---------------- I2C INIT ---------------- */
void I2C2_Inits(void)
{
    I2C_PeriClockControl(I2C2, ENABLE);

    I2C1Handle.pI2Cx = I2C2;
    I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C1Handle.I2C_Config.I2C_DeviceAddress = 0x61;
    I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

    I2C_Init(&I2C1Handle);
}

/* ---------------- OLED LOW LEVEL ---------------- */
void OLED_WriteCmd(uint8_t cmd)
{
    uint8_t tx[2] = {0x00, cmd};
    I2C_MasterSendData(&I2C1Handle, tx, 2, OLED_ADDR, I2C_DISABLE_SR);
}

void OLED_WriteData(uint8_t data)
{
    uint8_t tx[2] = {0x40, data};
    I2C_MasterSendData(&I2C1Handle, tx, 2, OLED_ADDR, I2C_DISABLE_SR);
}

/* ---------------- OLED INIT ---------------- */
void OLED_Init(void)
{
    delay();

    OLED_WriteCmd(0xAE); // Display OFF
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14);
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xA1);
    OLED_WriteCmd(0xC8);
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x12);
    OLED_WriteCmd(0x81); OLED_WriteCmd(0xCF);
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x40);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xAF); // Display ON
}

/* ---------------- OLED FUNCTIONS ---------------- */
void OLED_SetCursor(uint8_t page, uint8_t column)
{
    OLED_WriteCmd(0xB0 + page);
    OLED_WriteCmd(0x00 + (column & 0x0F));
    OLED_WriteCmd(0x10 + (column >> 4));
}

void OLED_Fill(uint8_t pattern)
{
    for(uint8_t page = 0; page < 8; page++)
    {
        OLED_SetCursor(page, 0);
        for(uint8_t col = 0; col < 128; col++)
        {
            OLED_WriteData(pattern);
        }
    }
}

void OLED_DrawPoint(uint8_t x, uint8_t y)
{
    if(x >= 128 || y >= 64) return;

    uint8_t page = y / 8;
    uint8_t bit = 1 << (y % 8);

    OLED_SetCursor(page, x);
    OLED_WriteData(bit);
}

/* ---------------- MAIN ---------------- */
int main(void)
{
    I2C2_GPIOInit();
    I2C2_Inits();

    I2C_PeripheralControl(I2C2, ENABLE);
    I2C_ManageAcking(I2C2, I2C_ACK_ENABLE);

    OLED_Init();

    while(1)
    {
        OLED_Fill(0xFF); // White
        delay();

        OLED_Fill(0x00); // Black
        delay();
    }
}
