
#include "sdram.h"
#include "delay.h"
//要写入到24c02的字符串数组

 u8 dram_Buffer[]={"Apollo STM32F4 IIC TEST"};
#define dramSIZE sizeof(dram_Buffer)

void  sdram_test(void)
{
	u8 datatemp[dramSIZE];	

	FMC_SDRAM_WriteBuffer(dram_Buffer,0,dramSIZE);
	FMC_SDRAM_ReadBuffer(datatemp,0,dramSIZE);
	printf("%s\r\n",datatemp);
	
}
	
