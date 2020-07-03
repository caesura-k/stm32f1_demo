#ifndef __FLASH_H
#define __FLASH_H			    
#include "stm32f10x.h" 
#include <string.h>
#include <stdio.h>


  
#define W25Q128	0XEF17
#define SPI2_CS(x)	( x ? GPIO_SetBits(GPIOB,GPIO_Pin_12):GPIO_ResetBits(GPIOB,GPIO_Pin_12))
extern u16 W25QXX_TYPE;		
 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

void w25qxx_init(void);
u16 w25qxx_readID(void);
int w25qxx_write(u8* pbuff,u32 len,u16 sectorNum) ;
void w25qxx_writeSector(u8* pbuff,u16 len,u32 addr)  ;
void w25qxx_writePage(u8* pbuff,u16 len,u32 addr);
void w25qxx_read(u8* pBuffer,u32 len,u16 sectorNum);
void w25qxx_eraseChip(void)   ;
void w25qxx_eraseSector(u32 addr) ;


void w25qxx_writeEnable(void)  ;
void w25qxx_writeDisable(void)  ;
void w25qxx_waitBusy(void)   ;

void w25qxx_test(void);


#endif










