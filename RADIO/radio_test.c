#include "radio.h"
#include "delay.h"
#include "uart7.h"
void radio_test(void)
{
	stRadioStatus status;//״̬�ļ�¼
	stRadioCommandParameter parameter;//������¼
	UART7_init(9600);
	RadioInit(&status,&parameter);
	while(1)
	{
//		RadioSendByte(0x55);
		RadioDataSend(&parameter,"0123456789");
		delay_ms(100);
	
	}
}