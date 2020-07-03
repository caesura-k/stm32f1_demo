#ifndef __DEMO_SPI_H
#define __DEMO_SPI_H

#include "stm32f10x.h"
 			  	    													  
void SPI2_Init(void);			 //初始化SPI口 
u8 SPI2_sendRevByte(u8 sendbyte);//SPI总线读写一个字节
u8 SPI2_sendBuff(u8 * buff,int size);
u8 SPI2_revBuff(u8 * buff,int size);

		 
#endif
