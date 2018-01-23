#include "bluetooth.h"
#include "AT_bluetooth.h"
#include "delay.h"
#include <stdlib.h>
#include "uart5.h"
#include "rtk_gpio.h"
#include "radio.h"
/*
蓝牙模块测试函数
*/
#define TEST_RTK_BL 1
#define TEST_RADIO 1
void BlTest(void)
{
	char **data_return=0;
	*data_return=0;
	u16 num=0;
	stBlStatus  	status;
	stBlDataHead 	BlDataHead;
	
	stRtkStatus		RtkStatus;
#if TEST_RADIO
	UART7_init(9600);
	RadioInit(&gRadioStatus,&gRadioCommandParameter);	
#endif	
	
#if TEST_RTK_BL
//	RtkStatusInit(&RtkStatus);
	RtkInit(&RtkStatus);
#endif
//	delay_ms(1000);
	BlInit(&status);//初始化蓝牙模块，
	//Rtk初始化
	while(1)
	{
		if(num==50)
		{
#if TEST_RTK_BL
			RtkGpggaStatus(&RtkStatus); //得到GGA数据并且把 状态反馈
#endif	
//			BlRequest_GGA(&BlDataHead,RtkStatus );
			num=0;
		}
		num++;
		//建立状态监控：如果断开连接记录断连并重新连接
		if(BL_CONNECTION_STATUS())//在运行过程中断连了
		{
			gBlRtcmOpenFlag=0;
			BlReset();
			BlInit(&status);
		}
		
		BluetoothAnalysis(data_return,&status);
		if(status.bit.rx_buff_analysis==eBlRxBuffAnalysisY)
		{
			BlReceiveDataUnpacking(data_return,&status,&RtkStatus);
			status.bit.rx_buff_analysis=eBlRxBuffAnalysisN;
		}
		free(*data_return);
		
		delay_ms(1);
	}
	
}
