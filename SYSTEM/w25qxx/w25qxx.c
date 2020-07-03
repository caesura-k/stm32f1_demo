#include "w25qxx.h" 
#include "demoSPI.h"		//因为w25qxx的收发数据用到了spi2的收发函数；
#include "demoUSART.h"
#include <string.h>
													 
//初始化FLASH的SPI2外设，PB12片选作为GPIO来使用；
void w25qxx_init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能

	//许多w25qxx指令需要CS拉高后才执行，作为通用GPIO来使用方便控制；
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
	SPI_InitStructure.SPI_CRCPolynomial = 7;//复位值也是7；没什么用；	
	SPI_Init(SPI2, &SPI_InitStructure);  
	SPI_Cmd(SPI2, ENABLE); 
}  
		   
//0XEF14-0XEF17：为W25Q16-W25Q128 
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

//W25Q128有4096个Sector，每个sector存储4096字节，即0x000-0xfff,一共12bit；寻址最小单位是字节；		 
int w25qxx_write(u8* pbuff,u32 len,u16 sectorNum)   
{
	u16 sectorWriteBytes = 4096 ;			//每次写sector写入的字节数
	u16 sectorEndBytes = len%4096;			//最后一次写secotr写入的字节数
	u32 restSpace = (4096-sectorNum)*4096;	//芯片可写入字节数
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
	u16 pageBytes = 256;		//每次写操作指令的字节数最大值为256；由解码器缓存决定的； 
	u16 endBytes = len%256;		//最后一次写入的字节数；
	do
	{
		if(endBytes == len)		//判断数据是否可以写完；		
			pageBytes=endBytes;	//可以写完；
		w25qxx_writePage(pbuff,pageBytes,addr);							 
		pbuff += pageBytes;
		len -= pageBytes;
		addr += pageBytes;	
	}while(pageBytes!=endBytes);//不是的话继续循坏；
	printf("write sector done...\r\n");
} 

//SPI的解码缓存是256字节，所以执行一次写操作的时候，写入数据的大小不大于256字节；
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


//作用：读出readBytes个字节数据存到pBuffer中；一条读指令最多可读完flash；
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

//擦除整个芯片,差不多1秒；
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
	u8 sendbuff[512];				//局部变量堆栈为1K字节，设置为512字节；
	u8 revbuff[512];
	u16 len = 512;
	u16 sectorNum = 0;
	memset(sendbuff,0x06,len);		//存放写入数据，都是0x06；
	memset(revbuff,0x00,len);		//存放读出数据，初始化为0x00；
	//printf("check memset %2x",sendbuff[2]);

	w25qxx_init();			//初始化spi2的IO口
	id=w25qxx_readID();		//读厂家ID
	printf("w25qxx id :%2x \r\n",id);
	//w25qxx_eraseChip();	//约1分钟，有点久；
	w25qxx_write(sendbuff, len, sectorNum);		//sectorNum参数:0-4095;
	w25qxx_read(revbuff,len,sectorNum);
	for(int i=0;i<len;i++)
		printf("%2x ",revbuff[i]);				//打印读出的数据，0x06；
	printf("\r\n");
}





















