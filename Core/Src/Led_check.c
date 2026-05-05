#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"

// Simple delay function for cold boot stability
void Simple_Delay(uint32_t count) {
    for(uint32_t i = 0; i < count; i++) {
        __NOP();
    }
}

int main(void) {
    // 1. Safety delay at start (Critical for Run Mode)
    Simple_Delay(1000000);

    // 2. Initialize GPIO Port A for PA2
    GPIO_Handle_t GpioLed;

    // Enable the Clock for GPIOA (Bit 0 of AHB1ENR)
    RCC->AHB1ENR |= (1 << 0);

    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2; // Changed to Pin 2
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPulPdControl = GPIO_NO_PUPD;

    // Initialize the pin
    GPIO_Init(&GpioLed);

    /* ---------------- INFINITE LOOP ---------------- */
    while(1) {
        // Toggle PA2 (XORing bit 2 of the Output Data Register)
        GPIOA->ODR ^= (1 << 2);

        // Delay ~500ms
        Simple_Delay(2000000);
    }
}
