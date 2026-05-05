#include "stm32f407xx.h"
#include "stm32f407xx_i2c_drivers.h"
#include "stm32f407xx_gpio.h"

#define OLED_ADDR    0x3C

extern I2C_Handle_t I2C2Handle; // Points to I2C2

void OLED_WriteCmd(uint8_t cmd) {
    uint8_t tx[2] = {0x00, cmd};
    I2C_MasterSendData(&I2C2Handle, tx, 2, OLED_ADDR, I2C_DISABLE_SR);
}

void OLED_WriteData(uint8_t data) {
    uint8_t tx[2] = {0x40, data};
    I2C_MasterSendData(&I2C2Handle, tx, 2, OLED_ADDR, I2C_DISABLE_SR);
}

void OLED_Init(void) {
    OLED_WriteCmd(0xAE); OLED_WriteCmd(0xD5); OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F); OLED_WriteCmd(0xD3);
    OLED_WriteCmd(0x00); OLED_WriteCmd(0x40); OLED_WriteCmd(0x8D);
    OLED_WriteCmd(0x14); OLED_WriteCmd(0x20); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xA1); OLED_WriteCmd(0xC8); OLED_WriteCmd(0xAF);
}

void OLED_Fill(uint8_t pattern) {
    for(uint16_t i = 0; i < 1024; i++) OLED_WriteData(pattern);
}

void OLED_SetCursor(uint8_t page, uint8_t column) {
    OLED_WriteCmd(0xB0 + (page & 0x07));
    OLED_WriteCmd(0x00 + (column & 0x0F));
    OLED_WriteCmd(0x10 + ((column >> 4) & 0x0F));
}

void OLED_DrawPoint(uint8_t x, uint8_t y) {
    if (x >= 128 || y >= 64) return;
    uint8_t page = y / 8;
    uint8_t bit_pos = 1 << (y % 8);
    OLED_SetCursor(page, x);
    OLED_WriteData(bit_pos);
}
