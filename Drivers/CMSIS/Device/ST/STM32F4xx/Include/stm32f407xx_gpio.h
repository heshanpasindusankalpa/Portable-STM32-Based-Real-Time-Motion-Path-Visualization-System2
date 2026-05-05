

#ifndef CMSIS_DEVICE_ST_STM32F4XX_INCLUDE_STM32F407XX_GPIO_H_
#define CMSIS_DEVICE_ST_STM32F4XX_INCLUDE_STM32F407XX_GPIO_H_
#include "stm32f407xx.h"

typedef struct
{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;            /*!< possible values from @GPIO_PIN_MODES >*/
	uint8_t GPIO_PinSpeed;           /*!< possible values from @GPIO_PIN_SPEEDS >*/
	uint8_t GPIO_PinPulPdControl;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;


}GPIO_PinConfig_t;

typedef struct
{
	//pointer to hold the base address of the GPIO peripheral
	GPIO_TypeDef *pGPIOx;//This holds the base address of the GPIO port to which the pin belongs
	GPIO_PinConfig_t GPIO_PinConfig;

}GPIO_Handle_t;

//@GPIO_PIN_NUMBERS
//GPIO pin numbers
#define GPIO_PIN_NO_0 0
#define GPIO_PIN_NO_1 1
#define GPIO_PIN_NO_2 2
#define GPIO_PIN_NO_3 3
#define GPIO_PIN_NO_4 4
#define GPIO_PIN_NO_5 5
#define GPIO_PIN_NO_6 6
#define GPIO_PIN_NO_7 7
#define GPIO_PIN_NO_8 8
#define GPIO_PIN_NO_9 9
#define GPIO_PIN_NO_10 10
#define GPIO_PIN_NO_11 11
#define GPIO_PIN_NO_12 12
#define GPIO_PIN_NO_13 13
#define GPIO_PIN_NO_14 14


//@GPIO_PIN_MODES
//GPIO pin possible modes
#define GPIO_MODE_IN  0
#define GPIO_MODE_OUT 1
#define GPIO_MODE_ALTFN 2
#define GPIO_MODE_ANALOG 3
#define GPIO_MODE_IT_FT  4
#define GPIO_MODE_IT_RT  5
#define GPIO_MODE_IT_RFT 6

//@GPIO_PIN_SPEEDS
//GPIO pin possible output types
#define GPIO_OP_TYPE_PP 0
#define GPIO_OP_TYPE_OD 1

//GPIO pin output speeds
#define GPIO_SPEED_LOW 0
#define GPIO_SPEED_MEDIUM 1
#define GPIO_SPEED_FAST 2
#define GPIO_SPEED_HIGH 3

//GPIO pin pull up AND pull down configuration macros
#define GPIO_NO_PUPD  0
#define GPIO_PU       1
#define GPIO_PD       2

//GPIO pin


//Peripheral clock setup
void GPIO_PeriClockControl(GPIO_TypeDef *pGPIOx,uint8_t EnorDi);

void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_TypeDef *pGPIOx);
uint8_t GPIO_ReadFromInputPin(GPIO_TypeDef *pGPIOx,uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_TypeDef *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_TypeDef *pGPIOx,uint8_t PinNumber,uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_TypeDef *pGPIOx,uint8_t Value);
void GPIO_ToggleOutputPin(GPIO_TypeDef *pGPIOx,uint8_t PinNumber);

void GPIO_IRQConfig(uint8_t TRQNumber,uint8_t IRQPriority,uint8_t EnorDi);
void GPIO_IRQHandling(uint8_t PinNumber);


#endif
