#include "BMI088Middleware.h"

#include "spi.h"


void BMI088_GPIO_init(void)
{

}

void BMI088_com_init(void)
{


}

void BMI088_delay_ms(uint16_t ms)
{
    while(ms--)
    {
        BMI088_delay_us(1000);
    }
}

void BMI088_delay_us(uint16_t us)
{
		/*
    uint32_t ticks = 0;
    uint32_t told = 0;
    uint32_t tnow = 0;
    uint32_t tcnt = 0;
    uint32_t reload = 0;
    reload = SysTick->LOAD;
    ticks = us * 168;
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }*/
		uint16_t i;
    for( i=0;i<us;i++){
        uint16_t a=40;
        while(a--);
    }


}




void BMI088_ACCEL_NS_L(void)
{
	GPIO_WriteBit(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, Bit_RESET);

}
void BMI088_ACCEL_NS_H(void)
{
	GPIO_WriteBit(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, Bit_SET);
}

void BMI088_GYRO_NS_L(void)
{
	GPIO_WriteBit(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, Bit_RESET);
}
void BMI088_GYRO_NS_H(void)
{
	GPIO_WriteBit(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, Bit_SET);
}

uint8_t BMI088_read_write_byte(uint8_t txData)
{
	/*
    uint8_t rx_data;
    HAL_SPI_TransmitReceive(&hspi1, &txdata, &rx_data, 1, 1000);
    return rx_data;
	*/
	

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}

	SPI_I2S_SendData(SPI1, txData); 

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {} 
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}

	return SPI_I2S_ReceiveData(SPI1); 
	
}

