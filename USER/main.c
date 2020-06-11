#include "usartDemo.h"

int main(void)
{
    Usart1_Init(460800);
	Usart2_Init(460800);
	while(1)
    {
        if(USART1_REV_FLAG)
        {
        	USART1_REV_FLAG = 0;
            USART1_Send_Data(USART1_REV_BUF,USART1_REV_CNT);
			USART1_REV_CNT = 0;
			printf("print test\r\n");
        }
    
        if(USART2_REV_FLAG)
        {
            USART2_REV_FLAG=0;
            USART2_Send_Data(USART2_REV_BUF,USART2_REV_CNT);//通过串口1将接收到的固定长度字符发送出去
            USART2_REV_CNT=0;
        }
    }	
}



