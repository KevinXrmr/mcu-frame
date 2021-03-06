#include "spi.h"

void spi1_init(u16 rate)
{
	GPIO_InitTypeDef gpioInit;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_StructInit(&gpioInit);
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInit.GPIO_Mode = GPIO_Mode_AF;
		
	gpioInit.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpioInit);
	
	gpioInit.GPIO_Pin = GPIO_Pin_6;
	gpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpioInit);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SPI1EN);
	SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SPI1RST);
	CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SPI1RST);
	
	WRITE_REG(SPI1->CR1, SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | (rate & SPI_CR1_BR));
}

void spi2_init(u16 rate)
{
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_SPI2EN);
	SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_SPI2RST);
	CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_SPI2RST);
	
	WRITE_REG(SPI2->CR1, SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | (rate & SPI_CR1_BR));
}

u8 spi_byte(SPI_TypeDef *spi, u8 byte)
{
	if(READ_BIT(spi->CR1, SPI_CR1_SPE | SPI_CR1_MSTR) == (SPI_CR1_SPE | SPI_CR1_MSTR))
	{
		READ_REG(spi->DR);
		while(!READ_BIT(spi->SR, SPI_SR_TXE));
		WRITE_REG(spi->DR, byte);
		while(!READ_BIT(spi->SR, SPI_SR_RXNE));
	}
	return (u8)READ_REG(spi->DR);
}

void spi_buf_send(SPI_TypeDef *spi, u8 *buf, u16 size)
{
	if(spi == 0 || buf == 0 || size == 0)
	{
		return;
	}
	
	if(READ_BIT(spi->CR1, SPI_CR1_SPE | SPI_CR1_MSTR) == (SPI_CR1_SPE | SPI_CR1_MSTR))
	{
		while(size--)
		{
			while(!READ_BIT(spi->SR, SPI_SR_TXE));
			WRITE_REG(spi->DR, *buf);
			buf++;
		}
	}
}

void spi_buf_swap(SPI_TypeDef *spi, u8 *buf, u16 size)
{
	if(spi == 0 || buf == 0 || size == 0)
	{
		return;
	}
	
	if(READ_BIT(spi->CR1, SPI_CR1_SPE | SPI_CR1_MSTR) == (SPI_CR1_SPE | SPI_CR1_MSTR))
	{
		READ_REG(spi->DR);
		while(size--)
		{
			while(!READ_BIT(spi->SR, SPI_SR_TXE));
			WRITE_REG(spi->DR, *buf);
			while(!READ_BIT(spi->SR, SPI_SR_RXNE));
			*buf = (u8)READ_REG(spi->DR);
			buf++;
		}
	}
}
