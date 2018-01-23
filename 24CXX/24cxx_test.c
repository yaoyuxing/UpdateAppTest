#include "24cxx.h"
#include "delay.h"
//要写入到24c02的字符串数组

const u8 TEXT_Buffer[]={"Apollo STM32F4 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)

void  eeprom_test(void)
{
	u8 datatemp[SIZE];	
	while(AT24CXX_Check())//检测不到24c02
	{
		printf("24C02 Check Failed!");
		delay_ms(500);
	}
	
	AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);
	
	AT24CXX_Read(0,datatemp,SIZE);
	printf("%s\r\n",datatemp);
	
}
	