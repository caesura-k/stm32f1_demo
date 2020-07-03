#include "demoUSART.h"
#include "demoSPI.h"
#include "w25qxx.h"

int main(void)
{
	Usart1_Init(115200);
	
	while(1)
    {
        if(USART1_REV_FLAG)
        {
        	USART1_REV_FLAG = 0;
			w25qxx_test();
			USART1_REV_CNT = 0;
        }
    }	
}
