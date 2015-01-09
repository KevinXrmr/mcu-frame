#include "FT5406.h"
#include "public.h"
/**************************************************************
IO����
**************************************************************/
#define sda_get()		READ_IO(GPIOB, BIT(7))
#define sda_set()		SET_IO(GPIOB, BIT(7))
#define sda_clr()		CLEAR_IO(GPIOB, BIT(7))
#define scl_set()		SET_IO(GPIOB, BIT(6))
#define scl_clr()		CLEAR_IO(GPIOB, BIT(6))
/**************************************************************
SDA��Ϊ����
**************************************************************/
#define sda_input() \
do{ \
	MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE7 | GPIO_CRL_CNF7, GPIO_CRL_CNF7_1); \
	SET_BIT(GPIOB->ODR, GPIO_ODR_ODR7); \
} while(0)
/**************************************************************
SDA��Ϊ���
**************************************************************/
#define sda_output() \
do{ \
	CLEAR_BIT(GPIOB->ODR, GPIO_ODR_ODR7); \
	MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE7 | GPIO_CRL_CNF7, GPIO_CRL_MODE7); \
} while(0)
/**************************************************************
SCL��Ϊ���
**************************************************************/
#define scl_output() \
do{ \
	MODIFY_REG(GPIOB->CRL, GPIO_CRL_MODE6 | GPIO_CRL_CNF6, GPIO_CRL_MODE6); \
} while(0)
/**************************************************************
EEPROM����͹���
**************************************************************/
#define sleep() \
do { \
} while(0)
/**************************************************************
IO��ʼ��
**************************************************************/
#define init() \
do { \
	scl_output(); \
	sda_output(); \
	stop(); \
} while(0)
/**************************************************************
I2C���߿�ʼ
**************************************************************/
#define start() \
do { \
	stop(); \
	sda_clr(); \
	delay(); \
	scl_clr(); \
	delay(); \
} while(0)
/**************************************************************
I2C����ֹͣ
**************************************************************/
#define stop() \
do { \
	delay(); \
	scl_set(); \
	delay(); \
	sda_set(); \
	delay(); \
} while(0)
/**************************************************************
��ʱ������10us
**************************************************************/
static void delay()
{
	unsigned long i = 1;
	while(i--)
	{
		nop();
	}
}
/**************************************************************
����Ӧ��λ
**************************************************************/
static void set_ack()
{
	delay();
	sda_clr();
	delay();
	scl_set();
	delay();
	scl_clr();
	delay();
	sda_set();
	delay();
}
/**************************************************************
��ȡӦ��λ
**************************************************************/
static char get_ack()
{
	char sdaState;

	sda_input();
	delay();
	scl_set();
	delay();
	sdaState = !sda_get();
	scl_clr();
	sda_output();
	return sdaState;
}
/**********************************************************
��ȡ���ֽ�
**********************************************************/
static unsigned char read_byte()
{
	unsigned char i, buf = 0;
	
	sda_input();
	delay();
	
	for(i = 0; i < 8; i++)
	{
		scl_set();
		delay();
		buf <<= 1;
		
		if(sda_get())
			 buf |= 0x01;

		scl_clr();
		delay();
	}
	sda_output();
	return buf;
}
/**************************************************************
д�뵥�ֽ�
**************************************************************/
static char write_byte(unsigned char buf)
{
	unsigned char i;
	
	for(i=0; i<8; i++)
	{
		if(buf & 0x80)
			sda_set();
		else
			sda_clr();
			
		buf <<= 1;
		delay();
		scl_set();
		delay();
		scl_clr();
		delay();
	}
	sda_clr();
	return get_ack();
}
/**************************************************************
д���ַ
**************************************************************/
static char write_address(unsigned short address)
{
	unsigned char i, j;
	
	for(i = 0; i < 50; i++)
	{
		start();
		do {
#if _EEPROM_SIZE > 2048
			if(!write_byte(0xA0))
				break;
			if(!write_byte((unsigned char)(address >> 8)))
				break;
#else
			if(!write_byte((unsigned char)(address >> 7) & 0x0E | 0xA0))
				break;
#endif
			if(!write_byte((unsigned char)(address & 0xFF)))
				break;

			return 1;
		} while(0);

		for(j = 0; j < 20; j++)
			delay();
	}
	return 0;
}
/**************************************************************
��EEPROM
**************************************************************/
char eeprom_read(unsigned short address, unsigned char *buf, unsigned short size)
{
	init();

	if(!write_address(address))
		return 0;

	start();

	if(!write_byte(0xA1))
		return 0;
		
	while(size--)
	{
		*buf++ = read_byte();
		if(size > 0)
			set_ack();
	}
	stop();
	sleep();
	return 1;
}
/**************************************************************
дEEPROM
**************************************************************/
char eeprom_write(unsigned short address, unsigned char *buf, unsigned short size)
{
	init();

	if(!write_address(address))
		return 0;

	while(size--)
	{
		if(!write_byte(*buf++))
			return 0;
		if(!(++address % 16))
			if(!write_address(address))
				return 0;
	}
	stop();
	sleep();
	return 1;
}
/**************************************************************
EEPROM����͹���
**************************************************************/
void eeprom_sleep()
{
	sleep();
}
