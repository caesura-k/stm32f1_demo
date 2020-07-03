#include "demoDelay.h"

int delay_ms(int ms)
{
	u32  load_ms = 0;
	u32  load_total = 0;
	u32 temp_systick = 0;

	/*直接用算好的0x00FF_FFFF内的数值加载则可以省略以下4步；
	*0xFFFFFF=16777215;AHB为72M，systick为8分频，则定时约1.86秒；
	*1 设置systick的时钟为AHB/8；
	*2 每毫秒内systick时钟源震动的次数；
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

	/*直接用算好的0x00FF_FFFF内的数值加载则可以省略以下4步；
	*0xFFFFFF=16777215;AHB为72M，systick为8分频，则定时约1.86秒；
	*1 设置systick的时钟为AHB/8；
	*2 每微秒内systick时钟源震动的次数；
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
/*systick计数范围为0到val_load；val_orginal为开始计数值，val_current为当前计数值；
*通过对比VAL寄存器中开始值和当前值的差值，来达到计时的效果；
*本例前提为系统时钟72M，AHB/8为时钟源；且计数范围不超过val_load;
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














