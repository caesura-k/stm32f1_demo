#include"demoGPIO.h"
//ͨ��GPIO��Ҫ������������CRL��CRH֮�󣬾Ϳ���ͨ��������д�����ˣ�
//CRL��CRH��ͨ��GPIO_Init()�����ã�
//IDR��ODR���Ǽ���GPIO�Ķ�д������
//BSRR��BRR���Ǽ���GPIO��λ����������

void gpio_Demo(void)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitTypeDef GPIO_Struct;

	//KEY0 PE4 �����������	
	GPIO_Struct.GPIO_Pin = GPIO_Pin_4;			
	GPIO_Struct.GPIO_Mode =GPIO_Mode_IPU;//�������룬Ĭ��ֵΪ1��	
	GPIO_Init(GPIOE, &GPIO_Struct); 

	//LED0 PB5 ��ô�������						
	GPIO_Struct.GPIO_Pin = GPIO_Pin_5; 
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_Struct);

	int ret = 1 ;
	while(1)
	{
		ret = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4);
		if(!ret)
			delay_ms(5);
		if(!ret)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);//����
			delay_ms(300);
			GPIO_SetBits(GPIOB,GPIO_Pin_5);//���
		}	
	}
}
