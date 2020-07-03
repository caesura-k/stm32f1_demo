#include "w25qxx.h" 
#include "demoSPI.h"		//��Ϊw25qxx���շ������õ���spi2���շ�������
#include "demoUSART.h"
#include <string.h>
													 
//��ʼ��FLASH��SPI2���裬PB12Ƭѡ��ΪGPIO��ʹ�ã�
void w25qxx_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ��

	//���w25qxxָ����ҪCS���ߺ��ִ�У���Ϊͨ��GPIO��ʹ�÷�����ƣ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
	SPI_InitStructure.SPI_CRCPolynomial = 7;//��λֵҲ��7��ûʲô�ã�	
	SPI_Init(SPI2, &SPI_InitStructure);  
	SPI_Cmd(SPI2, ENABLE); 
}  
		   
//0XEF14-0XEF17��ΪW25Q16-W25Q128 
u16 w25qxx_readID(void)
{
	printf("read id...");
	u16 Temp = 0;	  
	SPI2_CS(0);					    
	SPI2_sendRevByte(0x90);		    
	SPI2_sendRevByte(0x00); 	    
	SPI2_sendRevByte(0x00); 	    
	SPI2_sendRevByte(0x00); 	 			   
	Temp|=SPI2_sendRevByte(0xFF)<<8;  
	Temp|=SPI2_sendRevByte(0xFF);	 
	SPI2_CS(1);			 
	printf("done. %2x\r\n",Temp);
	return Temp;
}  

//W25Q128��4096��Sector��ÿ��sector�洢4096�ֽڣ���0x000-0xfff,һ��12bit��Ѱַ��С��λ���ֽڣ�		 
int w25qxx_write(u8* pbuff,u32 len,u16 sectorNum)   
{
	u16 sectorWriteBytes = 4096 ;			//ÿ��дsectorд����ֽ���
	u16 sectorEndBytes = len%4096;			//���һ��дsecotrд����ֽ���
	u32 restSpace = (4096-sectorNum)*4096;	//оƬ��д���ֽ���
	if( len>restSpace)
	{
		printf("space not enough...\r\n");
		return 0x46;
	}
	do
	{
		if(len==sectorEndBytes)
			sectorWriteBytes=sectorEndBytes;
		w25qxx_eraseSector(sectorNum*4096);
		w25qxx_writeSector(pbuff,sectorWriteBytes,sectorNum*4096);
		pbuff = pbuff+sectorWriteBytes;
		len = len-sectorWriteBytes;
		sectorNum = sectorNum+1;
	}while(sectorWriteBytes!=sectorEndBytes);
	printf("write done......\r\n");
	return 0x54;
}

void w25qxx_writeSector(u8* pbuff,u16 len,u32 addr)   
{ 
	printf("write sector ...\r\n");
	u16 pageBytes = 256;		//ÿ��д����ָ����ֽ������ֵΪ256���ɽ�������������ģ� 
	u16 endBytes = len%256;		//���һ��д����ֽ�����
	do
	{
		if(endBytes == len)		//�ж������Ƿ����д�ꣻ		
			pageBytes=endBytes;	//����д�ꣻ
		w25qxx_writePage(pbuff,pageBytes,addr);							 
		pbuff += pageBytes;
		len -= pageBytes;
		addr += pageBytes;	
	}while(pageBytes!=endBytes);//���ǵĻ�����ѭ����
	printf("write sector done...\r\n");
} 

//SPI�Ľ��뻺����256�ֽڣ�����ִ��һ��д������ʱ��д�����ݵĴ�С������256�ֽڣ�
void w25qxx_writePage(u8* pbuff,u16 len,u32 addr)
{
	printf("write page......\r\n");
	u16 i;  
	w25qxx_writeEnable();				
	SPI2_CS(0);	   
	SPI2_sendRevByte(W25X_PageProgram);	   
	SPI2_sendRevByte(addr>>16);    
	SPI2_sendRevByte(addr>>8);   
	SPI2_sendRevByte(addr);   
	for(i=0;i<len;i++)
		SPI2_sendRevByte(pbuff[i]);	 
	SPI2_CS(1);	 
	w25qxx_waitBusy();	
	printf("write page done......\r\n");
} 


//���ã�����readBytes���ֽ����ݴ浽pBuffer�У�һ����ָ�����ɶ���flash��
void w25qxx_read(u8* pBuffer,u32 len,u16 sectorNum)
{
	printf("read ......\r\n");
 	u32 i; 
	u32 addr = sectorNum*4096;
	SPI2_CS(0);		  
    SPI2_sendRevByte(W25X_ReadData);		
    SPI2_sendRevByte(addr>>16);  	    
    SPI2_sendRevByte(addr>>8);   
    SPI2_sendRevByte(addr);   
    for(i=0;i<len;i++) 
        pBuffer[i]=SPI2_sendRevByte(0XFF);	
	SPI2_CS(1);  				    	      
}  

//��������оƬ,���1�룻
void w25qxx_eraseChip(void)   
{                                   
    w25qxx_writeEnable();                 	 	
    w25qxx_waitBusy();   
  	SPI2_CS(0);                            	   
    SPI2_sendRevByte(W25X_ChipErase);        	
	SPI2_CS(1);                          	    	      
	w25qxx_waitBusy();
	printf("erase chip done...\r\n");
}   

void w25qxx_eraseSector(u32 addr)   
{   
 	printf("erase sector : %4x \r\n",addr);	  
    w25qxx_writeEnable();                  	
    w25qxx_waitBusy();   
  	SPI2_CS(0); 			 
    SPI2_sendRevByte(W25X_SectorErase);			
    SPI2_sendRevByte(addr>>16);		 
    SPI2_sendRevByte(addr>>8);   
    SPI2_sendRevByte(addr);  
	SPI2_CS(1);			   	      
    w25qxx_waitBusy();							
    printf("erase sector done...\r\n");
}  
  
void w25qxx_writeEnable(void)   
{
	SPI2_CS(0);	 
    SPI2_sendRevByte(W25X_WriteEnable);
	SPI2_CS(1);	     	      
} 
  
void w25qxx_writeDisable(void)   
{  
	SPI2_CS(0);	 
    SPI2_sendRevByte(W25X_WriteDisable);   
	SPI2_CS(1);	   	      
} 

void w25qxx_waitBusy(void)   
{
	printf("wait busy...   ");
	u8 sr1=0;   
	SPI2_CS(0);	
	do
	{
		SPI2_sendRevByte(W25X_ReadStatusReg);
		sr1=SPI2_sendRevByte(0Xff);
	}while(sr1&0x01);		
	SPI2_CS(1);	
	printf("done.\r\n");
}  

void w25qxx_test(void)
{
	u16 id;
	u8 sendbuff[512];				//�ֲ�������ջΪ1K�ֽڣ�����Ϊ512�ֽڣ�
	u8 revbuff[512];
	u16 len = 512;
	u16 sectorNum = 0;
	memset(sendbuff,0x06,len);		//���д�����ݣ�����0x06��
	memset(revbuff,0x00,len);		//��Ŷ������ݣ���ʼ��Ϊ0x00��
	//printf("check memset %2x",sendbuff[2]);

	w25qxx_init();			//��ʼ��spi2��IO��
	id=w25qxx_readID();		//������ID
	printf("w25qxx id :%2x \r\n",id);
	//w25qxx_eraseChip();	//Լ1���ӣ��е�ã�
	w25qxx_write(sendbuff, len, sectorNum);		//sectorNum����:0-4095;
	w25qxx_read(revbuff,len,sectorNum);
	for(int i=0;i<len;i++)
		printf("%2x ",revbuff[i]);				//��ӡ���������ݣ�0x06��
	printf("\r\n");
}





















