#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_i2c_drivers.h"

/* ---------------- PROTOTYPES ---------------- */
void PA2_Heartbeat_Init(void);
void I2C_GPIOInit(void);
void I2C_Inits(void);
void MPU9250_Init(void);
void MPU9250_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az);
void OLED_Init(void);
void OLED_Fill(uint8_t pattern);
void OLED_DrawPoint(uint8_t x, uint8_t y);

/* ---------------- GLOBAL HANDLES ---------------- */
I2C_Handle_t I2C1Handle; // MPU9250
I2C_Handle_t I2C2Handle; // OLED

/* ---------------- DEAD RECKONING VARIABLES ---------------- */
float ax_f = 0, ay_f = 0;
float vx = 0, vy = 0;
float px = 0, py = 0;
float dt = 0.02f;

/* ---------------- MAIN FUNCTION ---------------- */
int main(void) {
    int16_t ax, ay, az;
    int16_t x_bias = -1940;

    // 1. CRITICAL: Safety Startup Delay
    // Gives sensors and power rails time to stabilize before I2C starts.
  //  for(uint32_t i = 0; i < 1000000; i++);

    // 2. Initialize Heartbeat LED (PA2)
    PA2_Heartbeat_Init();

    // 3. Initialize I2C GPIOs and Peripherals
    I2C_GPIOInit();
    I2C_Inits();

    // 4. Enable I2C Peripherals
    I2C_PeripheralControl(I2C1, ENABLE);
    I2C_PeripheralControl(I2C2, ENABLE);
    I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);
    I2C_ManageAcking(I2C2, I2C_ACK_ENABLE);

    // 5. Hardware Init (If this hangs, PA2 will stop blinking)
    MPU9250_Init();
    OLED_Init();
    OLED_Fill(0x00);

    /* ---------------- MAIN LOOP ---------------- */
    while(1) {
        // Toggle PA2: If this stops blinking, the code is stuck in an I2C loop
        GPIOA->ODR ^= (1 << 2);

        // Read accelerometer
        MPU9250_ReadAccel(&ax, &ay, &az);

        // Math: Raw to m/s^2
        ax_f = ((float)(ax - x_bias) / 16384.0f) * 9.81f;
        ay_f = ((float)ay / 16384.0f) * 9.81f;

        // Integration
        vx += ax_f * dt;
        vy += ay_f * dt;
        px += vx * dt;
        py += vy * dt;

        // Simple Damping
        vx *= 0.98f;
        vy *= 0.98f;

        // OLED Mapping
        int16_t x_pix = 64 + (int16_t)(px * 5);
        int16_t y_pix = 32 + (int16_t)(py * 5);

        // Clamping
        if(x_pix < 0) x_pix = 0;   if(x_pix > 127) x_pix = 127;
        if(y_pix < 0) y_pix = 0;   if(y_pix > 63)  y_pix = 63;

        OLED_DrawPoint((uint8_t)x_pix, (uint8_t)y_pix);

        // Loop Delay
        for(uint32_t i = 0; i < 250000; i++);
    }
}

/* ---------------- INITIALIZATION HELPERS ---------------- */

void PA2_Heartbeat_Init(void) {
    GPIO_Handle_t GpioLed;
    RCC->AHB1ENR |= (1 << 0); // Enable GPIOA Clock
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPulPdControl = GPIO_NO_PUPD;
    GPIO_Init(&GpioLed);
}

void I2C_GPIOInit(void) {
    GPIO_Handle_t I2CPins;
    RCC->AHB1ENR |= (1 << 1); // Enable GPIOB Clock

    I2CPins.pGPIOx = GPIOB;
    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    I2CPins.GPIO_PinConfig.GPIO_PinPulPdControl = GPIO_PU;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    // I2C1: PB6, PB7
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    GPIO_Init(&I2CPins);
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
    GPIO_Init(&I2CPins);

    // I2C2: PB10, PB11
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
    GPIO_Init(&I2CPins);
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_11;
    GPIO_Init(&I2CPins);
}

void I2C_Inits(void) {
    I2C1Handle.pI2Cx = I2C1;
    I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C1Handle.I2C_Config.I2C_DeviceAddress = 0x61;
    I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
    I2C_Init(&I2C1Handle);

    I2C2Handle.pI2Cx = I2C2;
    I2C2Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C2Handle.I2C_Config.I2C_DeviceAddress = 0x61;
    I2C2Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C2Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
    I2C_Init(&I2C2Handle);
}/*
 * led_i2c.c
 *
 *  Created on: May 5, 2026
 *      Author: ASUS
 */


