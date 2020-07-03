#include"demoGPIO.h"
//通用GPIO主要就是配置完了CRL和CRH之后，就可以通过函数读写操作了；
//CRL和CRH：通过GPIO_Init()来配置；
//IDR和ODR：那几个GPIO的读写函数；
//BSRR和BRR：那几个GPIO的位操作函数；

void gpio_Demo(void)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitTypeDef GPIO_Struct;

	//KEY0 PE4 如果按键输入	
	GPIO_Struct.GPIO_Pin = GPIO_Pin_4;			
	GPIO_Struct.GPIO_Mode =GPIO_Mode_IPU;//上拉输入，默认值为1；	
	GPIO_Init(GPIOE, &GPIO_Struct); 

	//LED0 PB5 那么亮灯输出						
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
			GPIO_ResetBits(GPIOB,GPIO_Pin_5);//亮灯
			delay_ms(300);
			GPIO_SetBits(GPIOB,GPIO_Pin_5);//灭灯
		}	
	}
}
