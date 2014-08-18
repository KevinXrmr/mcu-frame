#include "NRF24L01.h"
#include "spi.h"
#include "debug.h"

#define SPI_BYTE(BYTE) spi_byte(SPI1, BYTE)
#define SPI_BUF_SEND(BUF, SIZE) spi_buf_send(SPI1, BUF, SIZE)
#define SPI_BUF_SWAP(BUF, SIZE) spi_buf_swap(SPI1, BUF, SIZE)

const unsigned char TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const unsigned char RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{
	NRF24L01_CEL(); 	//ʹ��24L01
	NRF24L01_CSH();	//SPIƬѡȡ��		  		 		  
}
//���24L01�Ƿ����
unsigned char NRF24L01_Check(void)
{
#define DUMMY_BYTE 0xA5
	unsigned char buf[5]={DUMMY_BYTE,DUMMY_BYTE,DUMMY_BYTE,DUMMY_BYTE,DUMMY_BYTE};
	unsigned char i;
	
	NRF24L01_Write_Buf(CMD_WRITE_REG+TX_ADDR, buf, 5);
	NRF24L01_Read_Buf(TX_ADDR, buf, 5);
	printlog("NRF24L01 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
		buf[0], buf[1], buf[2], buf[3], buf[4]);
	
	for(i=0; i<5; i++)
	{
		if(buf[i] != DUMMY_BYTE)
			return 0;
	}
	return 1;
}	 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
unsigned char NRF24L01_Write_Reg(unsigned char reg,unsigned char value)
{
	unsigned char status;	
   	NRF24L01_CSL();                 //ʹ��SPI����
  	status = SPI_BYTE(reg);//���ͼĴ����� 
  	SPI_BYTE(value);      //д��Ĵ�����ֵ
  	NRF24L01_CSH();                 //��ֹSPI����	   
  	return(status);       			//����״ֵ̬
}
//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
unsigned char NRF24L01_Read_Reg(unsigned char reg)
{
	unsigned char reg_val;	    
 	NRF24L01_CSL();          //ʹ��SPI����		
  	SPI_BYTE(reg);   //���ͼĴ�����
  	reg_val= SPI_BYTE(0XFF);//��ȡ�Ĵ�������
  	NRF24L01_CSH();          //��ֹSPI����		    
  	return(reg_val);           //����״ֵ̬
}	
//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
unsigned char NRF24L01_Read_Buf(unsigned char reg,unsigned char *pBuf,unsigned char len)
{
	unsigned char status, ctr;	       
  	NRF24L01_CSL();           //ʹ��SPI����
  	status = SPI_BYTE(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(ctr=0; ctr < len; ctr++)
	{
		pBuf[ctr] = SPI_BYTE(0XFF);//��������
	}
  	NRF24L01_CSH();       //�ر�SPI����
  	return status;        //���ض�����״ֵ̬
}
//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
unsigned char NRF24L01_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char len)
{
	unsigned char status, ctr;	    
 	NRF24L01_CSL();          //ʹ��SPI����
  	status = SPI_BYTE(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
  	for(ctr=0; ctr<len; ctr++)
	{
		SPI_BYTE(*pBuf++); //д������	
	}
  	NRF24L01_CSH();       //�ر�SPI����
  	return status;          //���ض�����״ֵ̬
}				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
unsigned char NRF24L01_TxPacket(unsigned char *txbuf)
{
	unsigned char sta;
	NRF24L01_CEL();
  	NRF24L01_Write_Buf(CMD_WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CEH();//��������	   
	while(0 != NRF24L01_IRQ());//�ȴ��������
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(CMD_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(CMD_FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
unsigned char NRF24L01_RxPacket(unsigned char *rxbuf)
{
	unsigned char sta;		    							   
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(CMD_WRITE_REG+STATUS, sta); //���TX_DS��MAX_RT�жϱ�־
	if(sta & RX_OK)//���յ�����
	{
		NRF24L01_Read_Buf(CMD_RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(CMD_FLUSH_RX, 0xFF);//���RX FIFO�Ĵ��� 
		return 1;
	}	   
	return 0;
}					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void RX_Mode(void)
{
	NRF24L01_CEL();	  
  	NRF24L01_Write_Buf(CMD_WRITE_REG+RX_ADDR_P0,(unsigned char*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	  
  	NRF24L01_Write_Reg(CMD_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(CMD_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
  	NRF24L01_Write_Reg(CMD_WRITE_REG+RF_CH,40);	     //����RFͨ��Ƶ��		  
  	NRF24L01_Write_Reg(CMD_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
  	NRF24L01_Write_Reg(CMD_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(CMD_WRITE_REG+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
  	NRF24L01_CEH(); //CEΪ��,�������ģʽ 
}						 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void TX_Mode(void)
{														 
	NRF24L01_CEL();	    
  	NRF24L01_Write_Buf(CMD_WRITE_REG+TX_ADDR,(unsigned char*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
  	NRF24L01_Write_Buf(CMD_WRITE_REG+RX_ADDR_P0,(unsigned char*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

  	NRF24L01_Write_Reg(CMD_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
  	NRF24L01_Write_Reg(CMD_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
  	NRF24L01_Write_Reg(CMD_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
  	NRF24L01_Write_Reg(CMD_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
  	NRF24L01_Write_Reg(CMD_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
  	NRF24L01_Write_Reg(CMD_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CEH();//CEΪ��,10us����������
}		  




