/*
 * OLED.h
 *
 *  Created on: May 6, 2026
 *      Author: ASUS
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

#include <stdint.h>

void OLED_Init(void);
void OLED_Clear(void);
void OLED_UpdateScreen(void);
void OLED_DrawPixel(uint8_t x, uint8_t y);
void OLED_ClearPixel(uint8_t x, uint8_t y);


#endif /* INC_OLED_H_ */
