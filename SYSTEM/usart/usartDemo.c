#include "usartDemo.h"  

u8 USART1_REV_BUF[256];	//���ջ���
u8 USART1_REV_CNT = 0;	//�����ֽڼ���
u8 USART1_REV_FLAG = 0;	//�յ�\r\n

u8 USART2_REV_BUF[256];
u8 USART2_REV_CNT = 0;
u8 USART2_REV_FLAG = 0; 

//usart1��ʼ��֮�󣬱����ͨ�����ڶ�д�ˣ�
void Usart1_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�   0-3;

    //USART1�����ж�����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;	//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);  
	
    //GPIO��ʼ�� USART1_TX	PA9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�����Ҫ�������ʣ����벻��Ҫ�������ʣ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//�����������,<����..�ֲ�>8.1.11�����GPIO����
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //GPIO��ʼ��    USART1_RX	PA10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

   //USART1��ʼ��
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;   //CR1�е�TE,RE  
    USART_Init(USART1, &USART_InitStructure); 
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//CR1�е�RXNEIE�ж�
    USART_Cmd(USART1, ENABLE);                    //CR1�е�UE
}

void USART1_Send_Data(u8 *buf,u16 len)
{
    u16 t;
    for(t=0;t<len;t++)        
    {
        USART_SendData(USART1,buf[t]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
		//�����ֽ���ɺ�TCӲ����1��
    }	//�ȶ�SR����дDR���TCλ��	
}

void USART1_IRQHandler(void)                    
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
     {
     	USART1_REV_BUF[USART1_REV_CNT] =USART_ReceiveData(USART1);		//��DR��Ӳ����0 RXNEλ��
        USART1_REV_CNT++; 
        if(USART1_REV_BUF[USART1_REV_CNT-2]==0x0d&&USART1_REV_BUF[USART1_REV_CNT-1]==0x0a)
			USART1_REV_FLAG = 1;			          
     }
	//RXNEΪ1�������ݵ�ͬʱ���������ݣ���ô�µ����ݶ�ʧ������������󣬶������ݺ�RXNEΪ0����ORE��־���ڣ�
	//RXNEΪ1�����������ݣ������������������λORE��
	//���<���Ĳο��ֲ�>25.3.3 �ַ�����С��
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(USART1);
    //    USART_ClearFlag(USART1,USART_FLAG_ORE);//�ȶ�SR,���DR�����Ը�λOREλ��Ӧ�ò����������ˣ�
    }
    // USART_ClearFlag(USART1,USART_IT_RXNE); //��DR�������RXNE��Ӧ�ò����������ˣ�
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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�   0-3;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//ʹ�ܴ���2�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//��ռ���ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�����ȼ�2��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;							//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//8λ���ݳ���
    USART_InitStructure.USART_StopBits = USART_StopBits_1;				//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;					//��żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//�շ�ģʽ
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

//�������´���,����1֧��printf����;  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

