#include "stm32f407xx_i2c_drivers.h"

uint16_t AHB_PreScaler[8] = {2, 4, 8, 16, 64, 128, 256, 512};
uint16_t APB_PreScaler[8] = {2, 4, 8, 16};

static void I2C_GenerateStartCondition(I2C_TypeDef *pI2Cx);
static void I2C_ExecueAdressPhaseWrite(I2C_TypeDef *pI2Cx, uint8_t SlaveAddr);
static void I2C_ExecueAdressPhaseRead(I2C_TypeDef *pI2Cx, uint8_t SlaveAddr);
static void I2C_ClearADDRFlag(I2C_TypeDef *pI2Cx);
static void I2C_GenerateStopCondition(I2C_TypeDef *pI2Cx);

static void I2C_GenerateStartCondition(I2C_TypeDef *pI2Cx)
{
    pI2Cx->CR1 |= I2C_CR1_START;
}

static void I2C_ExecueAdressPhaseWrite(I2C_TypeDef *pI2Cx, uint8_t SlaveAddr)
{
    SlaveAddr = SlaveAddr << 1;
    SlaveAddr &= ~(1);
    pI2Cx->DR = SlaveAddr;
}

static void I2C_ExecueAdressPhaseRead(I2C_TypeDef *pI2Cx, uint8_t SlaveAddr)
{
    SlaveAddr = SlaveAddr << 1;
    SlaveAddr |= 1;
    pI2Cx->DR = SlaveAddr;
}


static void I2C_ClearADDRFlag(I2C_TypeDef *pI2Cx)
{
    uint32_t dummyRead = pI2Cx->SR1;
    dummyRead = pI2Cx->SR2;
    (void)dummyRead;
}

static void I2C_GenerateStopCondition(I2C_TypeDef *pI2Cx)
{
    pI2Cx->CR1 |= I2C_CR1_STOP;
}

uint32_t RCC_GetPLLOutputclock()
{
    return 0;
}

uint32_t RCC_GetPCLK1Value(void)
{
    uint32_t pclk1, SystemClk;
    uint8_t clksrc, temp, ahbp, apb1p;

    clksrc = ((RCC->CFGR >> 2) & 0x3);
    if (clksrc == 0)
        SystemClk = 16000000;
    else if (clksrc == 1)
        SystemClk = 8000000;
    else if (clksrc == 2)
        SystemClk = RCC_GetPLLOutputclock();

    temp = ((RCC->CFGR >> 4) & 0xF);
    if (temp < 8)
        ahbp = 1;
    else
        ahbp = AHB_PreScaler[temp - 8];

    temp = ((RCC->CFGR >> 10) & 0x7);
    if (temp < 4)
        apb1p = 1;
    else
        apb1p = APB_PreScaler[temp - 4];

    pclk1 = (SystemClk / ahbp) / apb1p;
    return pclk1;
}

void I2C_PeriClockControl(I2C_TypeDef *pI2Cx, uint8_t EnorDi)
{
    if (EnorDi == ENABLE)
    {
        if (pI2Cx == I2C1)       I2C1_PCLK_EN();
        else if (pI2Cx == I2C2)  I2C2_PCLK_EN();
        else if (pI2Cx == I2C3)  I2C3_PCLK_EN();
    }
    else
    {
        if (pI2Cx == I2C1)       I2C1_PCLK_DI();
        else if (pI2Cx == I2C2)  I2C2_PCLK_DI();
        else if (pI2Cx == I2C3)  I2C3_PCLK_DI();
    }
}

void I2C_PeripheralControl(I2C_TypeDef *pI2Cx, uint8_t EnorDi)
{
    if (EnorDi == ENABLE)
        pI2Cx->CR1 |= I2C_CR1_PE;
    else
        pI2Cx->CR1 &= ~I2C_CR1_PE;
}

void I2C_Init(I2C_Handle_t *pI2CHandle)
{
    uint32_t tempreg = 0;

    tempreg |= pI2CHandle->I2C_Config.I2C_ACKControl << 10;
    pI2CHandle->pI2Cx->CR1 |= tempreg;

    tempreg = 0;
    tempreg |= RCC_GetPCLK1Value() / 1000000U;
    pI2CHandle->pI2Cx->CR2 = (tempreg & 0x3F);

    tempreg |= pI2CHandle->I2C_Config.I2C_DeviceAddress << 1;
    tempreg |= (1 << 14);
    pI2CHandle->pI2Cx->OAR1 = tempreg;

    uint16_t ccr_value = 0;
    tempreg = 0;
    if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
    {
        ccr_value = (RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
        tempreg |= (ccr_value & 0xFFF);
    }
    else
    {
        tempreg |= (1 << 15);
        tempreg |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << 14);
        if (pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2)
            ccr_value = (RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
        else
            ccr_value = (RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed));
        tempreg |= (ccr_value & 0xFFF);
    }
    pI2CHandle->pI2Cx->CCR = tempreg;

    if (pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
        tempreg = (RCC_GetPCLK1Value() / 1000000U) + 1;
    else
        tempreg = (RCC_GetPCLK1Value() * 300 / 1000000000U) + 1;

    pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F);
}

void I2C_DeInit(I2C_TypeDef *pI2Cx)
{
}

uint8_t I2C_GetFlagStatus(I2C_TypeDef *pI2Cx, uint32_t FlagName)
{
    if (pI2Cx->SR1 & FlagName)
        return FLAG_SET;
    return FLAG_RESET;
}

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxbuffer,
                        uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
    // 1. Generate START condition
    I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

    // 2. Wait for SB flag
    while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

    // 3. Send slave address with Write bit
    I2C_ExecueAdressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

    // 4. Wait for ADDR flag
    while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

    // 5. Clear ADDR flag
    I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

    // 6. Send data
    while (Len > 0)
    {
        while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
        pI2CHandle->pI2Cx->DR = *pTxbuffer;
        pTxbuffer++;
        Len--;
    }

    // 7. Wait for TXE and BTF
    while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
    while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

    // 8. Generate STOP condition
    if(Sr==I2C_DISABLE_SR)
    I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
}


void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxbuffer,
                        uint32_t Len, uint8_t SlaveAddr,uint8_t Sr)
{
    // 1. Generate START condition
    I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

    // 2. Wait for SB flag
    while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB));

    // 3. Send slave address with read bit
    I2C_ExecueAdressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

    // 4. Wait for ADDR flag
    while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

    //procedure to read only 1 byte from slave
    if(Len==1)
    {
    	//Disable Acking
    	I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);


    	//clear the ADDR flag
    	I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

    	//wait until RXNE become 1
    	 while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

    	 //generate STOP condition
    	 if(Sr==I2C_DISABLE_SR)
    	    I2C_GenerateStopCondition(pI2CHandle->pI2Cx);

    	 //read data in to buffer
    	 *pRxbuffer=pI2CHandle->pI2Cx->DR;



    }

    //procedure to read data from slave when Len>1
    if(Len>1)
    {
    	//clear the ADDR flag
    	I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

    	//read he data until Len becomes 1
    	for(uint32_t i=Len;i>0;i--)
    	{
    		//wait until RXNE becomes 1
    		while (!I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE));

    		if(i==2)//if last 2 bytes are remaining
    		{
    			//clear the ack bit
    			I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_DISABLE);


    			//generate STOP condition
    		    if(Sr==I2C_DISABLE_SR)
    			   I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
    		}
    		//read the data from data register in to byffer
    		*pRxbuffer=pI2CHandle->pI2Cx->DR;
    		//increment the buffer address
    		pRxbuffer++;
    	}
    }

    //re-enable ACKing
    if(pI2CHandle->I2C_Config.I2C_ACKControl==I2C_ACK_ENABLE)
    {
    I2C_ManageAcking(pI2CHandle->pI2Cx,I2C_ACK_ENABLE);
    }


}

void I2C_ManageAcking(I2C_TypeDef *pI2Cx,uint8_t EnorDi)
{
	if(EnorDi==I2C_ACK_ENABLE)
	{
		//enable
		pI2Cx->CR1|=I2C_CR1_ACK;

	}else
	{
		//disable the  ack
		pI2Cx->CR1 &=~I2C_CR1_ACK;
	}
}





