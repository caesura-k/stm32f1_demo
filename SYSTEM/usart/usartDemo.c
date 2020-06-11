#include "usartDemo.h"  

u8 USART1_REV_BUF[256];	//接收缓存
u8 USART1_REV_CNT = 0;	//接收字节计数
u8 USART1_REV_FLAG = 0;	//收到\r\n

u8 USART2_REV_BUF[256];
u8 USART2_REV_CNT = 0;
u8 USART2_REV_FLAG = 0; 

//usart1初始化之后，便可以通过串口读写了；
void Usart1_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //设置NVIC中断分组2:2位抢占优先级，2位响应优先级   0-3;

    //USART1外设中断配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;	//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);  
	
    //GPIO初始化 USART1_TX	PA9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//输出需要配置速率，输入不需要配置速率；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出,<中文..手册>8.1.11外设的GPIO配置
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //GPIO初始化    USART1_RX	PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

   //USART1初始化
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;   //CR1中的TE,RE  
    USART_Init(USART1, &USART_InitStructure); 
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//CR1中的RXNEIE中断
    USART_Cmd(USART1, ENABLE);                    //CR1中的UE
}

void USART1_Send_Data(u8 *buf,u16 len)
{
    u16 t;
    for(t=0;t<len;t++)        
    {
        USART_SendData(USART1,buf[t]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
		//发送字节完成后，TC硬件置1；
    }	//先读SR，后写DR清除TC位；	
}

void USART1_IRQHandler(void)                    
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
     {
     	USART1_REV_BUF[USART1_REV_CNT] =USART_ReceiveData(USART1);		//读DR，硬件清0 RXNE位；
        USART1_REV_CNT++; 
        if(USART1_REV_BUF[USART1_REV_CNT-2]==0x0d&&USART1_REV_BUF[USART1_REV_CNT-1]==0x0a)
			USART1_REV_FLAG = 1;			          
     }
	//RXNE为1，读数据的同时又来了数据，那么新的数据丢失；产生溢出错误，读完数据后RXNE为0，但ORE标志还在；
	//RXNE为1，又来了数据，产生接收溢出错误，置位ORE；
	//详见<中文参考手册>25.3.3 字符接收小节
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(USART1);
    //    USART_ClearFlag(USART1,USART_FLAG_ORE);//先读SR,后读DR，可以复位ORE位；应该不用软件清除了；
    }
    // USART_ClearFlag(USART1,USART_IT_RXNE); //读DR可以清除RXNE，应该不用软件清除了；
}

void Usart2_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//PA2 USART2_TX

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//PA3 USART2_RX

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//设置NVIC中断分组2:2位抢占优先级，2位响应优先级   0-3;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//使能串口2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//先占优先级2级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//从优先级2级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;							//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//8位数据长度
    USART_InitStructure.USART_StopBits = USART_StopBits_1;				//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;					//奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//收发模式
    USART_Init(USART2, &USART_InitStructure);			
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);                    

}

void USART2_Send_Data(u8 *buf,u16 len)
{
    u16 t;
      for(t=0;t<len;t++)        
    {        
        USART_SendData(USART2,buf[t]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    }
}

void USART2_IRQHandler(void)
{
     if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET) 
    {
    	USART2_REV_BUF[USART2_REV_CNT] =USART_ReceiveData(USART2);     
		USART2_REV_CNT++;
		if(USART2_REV_BUF[USART2_REV_CNT-2]==0x0d&&USART2_REV_BUF[USART2_REV_CNT-1]==0x0a)
			USART2_REV_FLAG=1;                        
    }
}

//加入以下代码,串口1支持printf函数;  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

