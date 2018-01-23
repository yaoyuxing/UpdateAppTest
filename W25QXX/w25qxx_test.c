
#include "w25qxx.h"
#include "delay.h"



const u8 w25qxx_Buffer[]={"Apollo STM32F4 IIC TEST"};
#define SIZE sizeof(w25qxx_Buffer)

void  w25qxx_test(void)
{
	u32 FLASH_SIZE;
	u8 datatemp[SIZE];	
	while(W25QXX_ReadID()!=W25Q128)//检测不到24c02
	{
		printf("w25qxx_test Check Failed!");
		delay_ms(500);
	}
	W25QXX_Write((u8*)w25qxx_Buffer,FLASH_SIZE-100,SIZE);	

	W25QXX_Read(datatemp,FLASH_SIZE-100,SIZE);					//从倒数第100个地址处开始,读出SIZE个字节
	
	printf("%s\r\n",datatemp);
	
}
	