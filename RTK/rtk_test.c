#include "rtk.h"
#include "delay.h"
#include "my_task.h"
#include "string.h"
#include "me3630.h"
void rtk_test(void)
{
	stMe3630Status Me3630Status;
	stRtkStatus		RtkStatus;
	RtkInit(&RtkStatus);
	RtkSelfCalibration(&RtkStatus);
	while(1)
	{
		RtkGpggaStatus(&RtkStatus); 
		
//		RtkGetGga(&RtkStatus,&Me3630Status);
		//RtkSendRtcmDataStatus(&RtkStatus,rtcm_data);
        delay_ms(1000);                           //延时1s，也就是1000个时钟节拍	
	}

}
