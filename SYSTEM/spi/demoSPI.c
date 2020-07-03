#include "demoSPI.h"
#include "demoUSART.h"
//为了使用printf包含的"demoUSART.h"

//hard和soft模式的NSS；hard下是一直被拉低吗？
void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//使能GPIO时钟 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//使能外设时钟

	//PB12_FCS,  PB13_SCK,  PB14_MISO,  PB15_MOSI; ***ques:这14 15都设置成了输出***
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//bit15：0
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//bit8和bit2:1	
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//bit11:0
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	//bit1:1	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//bit0:1
	SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;//bti9:0 soft模式由软件控制NSS，hard模式由硬件控制NSS模式；
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//bit[5:3]定义波特率2分频，即18MHz；
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//bit7:0	
	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRCPR寄存器中的校验项，复位值也是7，bit13使能CRC校验；这里没使能CRC校验；	
	SPI_Init(SPI2, &SPI_InitStructure); 
 
	SPI_Cmd(SPI2, ENABLE); //bit6:1 
}   

//全双工通信，所以发送一个字节的时候接收一个字节；
u8 SPI2_sendRevByte(u8 sendbyte)
{		
	u8 fail=0;	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
	{
		printf("SPI_I2S_GetFlagStatus TXE RESET... ... \r\n ");
		fail++;
		if(fail>1000)
			return 0x46;
	}
	SPI_I2S_SendData(SPI2, sendbyte); 
	fail=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
	{
		fail++;
		//简单收发数据不会进入while循坏，使用FLASH的时候有时候会进入；
		if(fail>1000)
		{
			printf("SPI_I2S_GetFlagStatus RXNE RESET... ... \r\n ");
			return 0x46;
		}
	}	  						    
	return SPI_I2S_ReceiveData(SPI2); 					    
}

u8 SPI2_sendBuff(u8 * buff,int size)
{
	int i=0;
	u8 fail=0;
	for(i=0;i<size;i++)
	{	//当数据由TX数据寄存器转移到移位寄存器后，TXE置1，向DR写数据可清零SR的TXE；
		fail=0;
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
		{
			printf("SPI_I2S_GetFlagStatus TXE RESET... ...\r\n");
			fail++;
			if(fail>1000)	
				return 0x46;
		}
		SPI_I2S_SendData(SPI2, buff[i]);//库函数发送一个字节;
	}
	return 0x54;
}

u8 SPI2_revBuff(u8 * buff,int size)
{
	int i=0;
	u8 fail=0;
	for(i=0;i<size;i++)
	{	//当数据由移位寄存器转移到RX数据寄存器后，RXNE置1，向DR读数据可清零SR的RXNE；
		fail=0;
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) 
		{
			printf("SPI_I2S_GetFlagStatus RXNE RESET... ...\r\n");
			fail++;
			if(fail>1000)
				return 0x46;
		}
		buff[i] = SPI_I2S_ReceiveData(SPI2);//使用库函数接收一个字节；
	}
	return 0x54;	
}























