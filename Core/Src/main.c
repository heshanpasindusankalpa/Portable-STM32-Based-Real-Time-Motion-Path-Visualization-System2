#include "stm32f407xx.h"
#include "stm32f407xx_gpio.h"
#include "stm32f407xx_i2c_drivers.h"

// Handles for separate buses
I2C_Handle_t I2C1Handle; // For MPU9250
I2C_Handle_t I2C2Handle; // For OLED

// External function prototypes
void MPU9250_Init(void);
void MPU9250_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az);
void OLED_Init(void);
void OLED_Fill(uint8_t pattern);
void OLED_SetCursor(uint8_t page, uint8_t column);
void OLED_DrawPoint(uint8_t x, uint8_t y);

/* ---------------- DEAD RECKONING VARIABLES ---------------- */
float ax_f = 0, ay_f = 0;
float vx = 0, vy = 0;
float px = 0, py = 0;
float dt = 0.02f; // ~20 ms loop time

/* ---------------- SIMPLE DELAY ---------------- */
void delay(void) {
    for(uint32_t i = 0; i < 250000; i++);
}

/* ---------------- I2C GPIO INIT (Dual Bus) ---------------- */
void I2C_GPIOInit(void) {
    GPIO_Handle_t I2CPins;

    GPIO_PeriClockControl(GPIOB, ENABLE);

    I2CPins.pGPIOx = GPIOB;
    I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
    I2CPins.GPIO_PinConfig.GPIO_PinPulPdControl = GPIO_PU;
    I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
    I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    // I2C1: PB6 (SCL), PB7 (SDA)
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
    GPIO_Init(&I2CPins);
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
    GPIO_Init(&I2CPins);

    // I2C2: PB10 (SCL), PB11 (SDA)
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
    GPIO_Init(&I2CPins);
    I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_11;
    GPIO_Init(&I2CPins);
}

/* ---------------- I2C PERIPHERAL INIT ---------------- */
void I2C_Inits(void) {

    // I2C1 → MPU9250
    I2C_PeriClockControl(I2C1, ENABLE);
    I2C1Handle.pI2Cx = I2C1;
    I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C1Handle.I2C_Config.I2C_DeviceAddress = 0x61;
    I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
    I2C_Init(&I2C1Handle);

    // I2C2 → OLED
    I2C_PeriClockControl(I2C2, ENABLE);
    I2C2Handle.pI2Cx = I2C2;
    I2C2Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C2Handle.I2C_Config.I2C_DeviceAddress = 0x61;
    I2C2Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
    I2C2Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
    I2C_Init(&I2C2Handle);
}

/* ---------------- MAIN FUNCTION ---------------- */
int main(void) {

    int16_t ax, ay, az;
    int16_t x_bias = -1940; // calibration (adjust manually if needed)

    // Step 1: GPIO Init
    GPIO_PeriClockControl(GPIOB, ENABLE);
    I2C_GPIOInit();

    // Step 2: I2C Init
    I2C_Inits();

    // Step 3: Enable I2C
    I2C_PeripheralControl(I2C1, ENABLE);
    I2C_PeripheralControl(I2C2, ENABLE);

    // Step 4: Enable ACK
    I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);
    I2C_ManageAcking(I2C2, I2C_ACK_ENABLE);

    delay(); delay();

    // Step 5: Init Devices
    MPU9250_Init();
    OLED_Init();

    OLED_Fill(0x00);

    /* ---------------- MAIN LOOP ---------------- */
    while(1) {

        // 1. Read accelerometer
        MPU9250_ReadAccel(&ax, &ay, &az);

        // 2. Convert raw → g → m/s²
        ax_f = (float)(ax - x_bias) / 16384.0f;
        ay_f = (float)(ay) / 16384.0f;

        ax_f *= 9.81f;
        ay_f *= 9.81f;

        // 3. Integrate → velocity
        vx += ax_f * dt;
        vy += ay_f * dt;

        // 4. Integrate → position
        px += vx * dt;
        py += vy * dt;

        // 5. Drift reduction (simple damping)
        vx *= 0.98f;
        vy *= 0.98f;

        // 6. Convert to OLED coordinates
        int16_t x_pixel = 64 + (int16_t)(px * 5);
        int16_t y_pixel = 32 + (int16_t)(py * 5);

        // Clamp
        if(x_pixel < 0) x_pixel = 0;
        if(x_pixel > 127) x_pixel = 127;

        if(y_pixel < 0) y_pixel = 0;
        if(y_pixel > 63) y_pixel = 63;

        // 7. Draw point (path)
        OLED_DrawPoint((uint8_t)x_pixel, (uint8_t)y_pixel);


        delay();
    }
}
