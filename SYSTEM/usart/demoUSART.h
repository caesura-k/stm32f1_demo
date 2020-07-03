#ifndef _DEMO_USART_H_
#define _DEMO_USART_H_

#include "stdio.h"
#include "stm32f10x.h"
 
extern u8 USART1_REV_BUF[256];	//���ջ���
extern u8 USART1_REV_CNT;		//�����ֽڼ���
extern u8 USART1_REV_FLAG;		//�յ�\r\n
	
extern u8 USART2_REV_BUF[256];
extern u8 USART2_REV_CNT;
extern u8 USART2_REV_FLAG; 


void Usart1_Init(u32 bound);
void USART1_sendbuff(u8 *buf,u16 len);
void USART1_IRQHandler(void);

void Usart2_Init(u32 bound);
void USART2_sendbuff(u8 *buf,u16 len);
void USART2_IRQHandler(void);

#endif

