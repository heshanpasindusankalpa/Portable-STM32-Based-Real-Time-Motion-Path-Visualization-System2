/*
 * stm32f407xx_i2c_drivers.h
 *
 *  Created on: Apr 4, 2026
 *      Author: ASUS
 */

#ifndef CMSIS_DEVICE_ST_STM32F4XX_INCLUDE_STM32F407XX_I2C_DRIVERS_H_
#define CMSIS_DEVICE_ST_STM32F4XX_INCLUDE_STM32F407XX_I2C_DRIVERS_H_

#include "stm32f407xx.h"

typedef struct
{
    uint32_t I2C_SCLSpeed;
    uint8_t  I2C_DeviceAddress;
    uint8_t  I2C_ACKControl;
    uint16_t I2C_FMDutyCycle;
} I2C_Config_t;

typedef struct
{
    I2C_TypeDef  *pI2Cx;
    I2C_Config_t  I2C_Config;
} I2C_Handle_t;

/*
 * @I2C_SCLSpeed
 */
#define I2C_SCL_SPEED_SM   100000
#define I2C_SCL_SPEED_FM4K 400000
#define I2C_SCL_SPEED_FM2K 200000

/*
 * @I2C_AckControl
 */
#define I2C_ACK_ENABLE  1
#define I2C_ACK_DISABLE 0

/*
 * @I2C_FMDutyCycle
 */
#define I2C_FM_DUTY_2    0
#define I2C_FM_DUTY_16_9 1

/*
 * I2C related status flags definitions
 */
#define I2C_FLAG_TXE     I2C_SR1_TXE
#define I2C_FLAG_RXNE    I2C_SR1_RXNE
#define I2C_FLAG_SB      I2C_SR1_SB
#define I2C_FLAG_OVR     I2C_SR1_OVR
#define I2C_FLAG_AF      I2C_SR1_AF
#define I2C_FLAG_ARLO    I2C_SR1_ARLO
#define I2C_FLAG_BERR    I2C_SR1_BERR
#define I2C_FLAG_STOPF   I2C_SR1_STOPF
#define I2C_FLAG_ADD10   I2C_SR1_ADD10
#define I2C_FLAG_BTF     I2C_SR1_BTF
#define I2C_FLAG_ADDR    I2C_SR1_ADDR
#define I2C_FLAG_TIMEOUT I2C_SR1_TIMEOUT


#define I2C_DISABLE_SR RESET
#define I2C_ENABLE_SR    SET

#define FLAG_SET   1
#define FLAG_RESET 0
/*
 * Peripheral Clock setup
 */
void I2C_PeriClockControl(I2C_TypeDef *pI2Cx, uint8_t EnorDi);

/*
 * Init and De-init
 */
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_TypeDef *pI2Cx);

/*
 * Data Send and Receive
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer,
                        uint32_t Len, uint8_t SlaveAddr,uint8_t Sr);

void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxbuffer,
                        uint32_t Len, uint8_t SlaveAddr,uint8_t Sr);

/*
 * IRQ Configuration and ISR handling
 */
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);

/*
 * Other Peripheral Control APIs
 */
void    I2C_PeripheralControl(I2C_TypeDef *pI2Cx, uint8_t EnOrDi);
uint8_t I2C_GetFlagStatus(I2C_TypeDef *pI2Cx, uint32_t FlagName);
void I2C_ManageAcking(I2C_TypeDef *pI2Cx,uint8_t EnorDi);
/*
 * Application callback
 */
void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);

#endif /* CMSIS_DEVICE_ST_STM32F4XX_INCLUDE_STM32F407XX_I2C_DRIVERS_H_ */
