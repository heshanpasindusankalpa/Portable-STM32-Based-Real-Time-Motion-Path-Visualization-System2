#include "stm32f407xx.h"
#include "stm32f407xx_i2c_drivers.h"
#include "stm32f407xx_gpio.h"

#define OLED_ADDR    0x3C

extern I2C_Handle_t I2C2Handle; // Points to I2C2
void OLED_SetCursor(uint8_t page, uint8_t column);
void OLED_WriteCmd(uint8_t cmd) {
    uint8_t tx[2] = {0x00, cmd};
    I2C_MasterSendData(&I2C2Handle, tx, 2, OLED_ADDR, I2C_DISABLE_SR);
}

void OLED_WriteData(uint8_t data) {
    uint8_t tx[2] = {0x40, data};
    I2C_MasterSendData(&I2C2Handle, tx, 2, OLED_ADDR, I2C_DISABLE_SR);
}

void OLED_Init(void) {
    OLED_WriteCmd(0xAE); // display off
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x00); // horizontal mode
    OLED_WriteCmd(0xB0);
    OLED_WriteCmd(0xC8);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x10);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x81); OLED_WriteCmd(0x7F);
    OLED_WriteCmd(0xA1);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xAF); // display ON
}
void OLED_Fill(uint8_t pattern) {
    for(uint8_t page = 0; page < 8; page++) {
        OLED_SetCursor(page, 0);
        for(uint8_t col = 0; col < 128; col++) {
            OLED_WriteData(pattern);
        }
    }
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
