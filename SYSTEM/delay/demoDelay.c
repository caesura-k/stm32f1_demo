#include "demoDelay.h"

int delay_ms(int ms)
{
	u32  load_ms = 0;
	u32  load_total = 0;
	u32 temp_systick = 0;

	/*ֱ������õ�0x00FF_FFFF�ڵ���ֵ���������ʡ������4����
	*0xFFFFFF=16777215;AHBΪ72M��systickΪ8��Ƶ����ʱԼ1.86�룻
	*1 ����systick��ʱ��ΪAHB/8��
	*2 ÿ������systickʱ��Դ�𶯵Ĵ�����
	*/
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	load_ms = SystemCoreClock/(8*1000);					 
	load_total = ms*load_ms;
	if(load_total > 0xFFFFFF) return 1;
	
	SysTick->LOAD = load_total;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | 
					SysTick_CTRL_TICKINT_Msk |
					(~SysTick_CTRL_CLKSOURCE_Msk);
	do{
		temp_systick = SysTick->CTRL;
	}while(!(temp_systick&SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL = ~SysTick_CTRL_ENABLE_Msk;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
	return 0;
}

int delay_us(int us)
{
	u32  load_us = 0;
	u32  load_total = 0;
	u32 temp_systick = 0;

	/*ֱ������õ�0x00FF_FFFF�ڵ���ֵ���������ʡ������4����
	*0xFFFFFF=16777215;AHBΪ72M��systickΪ8��Ƶ����ʱԼ1.86�룻
	*1 ����systick��ʱ��ΪAHB/8��
	*2 ÿ΢����systickʱ��Դ�𶯵Ĵ�����
	*/
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	load_us = SystemCoreClock/(8*1000*1000);					 
	load_total = us*load_us;
	if(load_total > 0xFFFFFF) return 1;
	
	SysTick->LOAD = load_total;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | 
					SysTick_CTRL_TICKINT_Msk |
					(~SysTick_CTRL_CLKSOURCE_Msk);
	do{
		temp_systick = SysTick->CTRL;
	}while(!(temp_systick&SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL = ~SysTick_CTRL_ENABLE_Msk;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
	return 0;
}


#if 0
/*systick������ΧΪ0��val_load��val_orginalΪ��ʼ����ֵ��val_currentΪ��ǰ����ֵ��
*ͨ���Ա�VAL�Ĵ����п�ʼֵ�͵�ǰֵ�Ĳ�ֵ�����ﵽ��ʱ��Ч����
*����ǰ��Ϊϵͳʱ��72M��AHB/8Ϊʱ��Դ���Ҽ�����Χ������val_load;
*/
int delay_ms(int ms)
{
	u32 val_load = SysTick->LOAD;
	u32 user_count = (SystemCoreClock/(8*1000))*ms;
	u32 val_orginal = 0, val_current = 0;	
	u32 cnt_tempif = 0, cnt_tempelse = 0, cnt_sum = 0 ;
	u32 cnt_flag = 0;

	val_orginal = SysTick->VAL;
	do{
		val_current= SysTick->VAL;
		if(val_orginal >= val_current)
			cnt_tempif = val_orginal-val_current;
		else
			cnt_tempelse = val_load-val_orginal;
		cnt_sum = cnt_tempif + cnt_tempelse;
		cnt_flag++;
		if(cnt_flag > val_load) return 1;
	}while(cnt_sum < user_count);
	return 0;
}
#endif














