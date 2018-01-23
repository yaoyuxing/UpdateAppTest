#include "socket.h"
#include "qxwz_rtcm.h"
#include "prefs.h"
#include "rtcm.h"
#include "debug.h"

/*
ǧѰ�������������
*/
void QxSdkSetUp(char *appKey,char *secret,char *device_ID)
{
	qxwz_config config; 
	config.appkey = appKey;       
	config.appsecret= secret;       
	config.device_ID = device_ID; //�豸��Ψһ ID������ʹ���豸�����к�   
	config.device_Type = "Test_device"; 
	qxwz_setting(&config); 
	qxwz_start(QxRtcmResponse,QxStatusResponse); 
}
/*
����RTCM����typedef void(*qxwz_rtcm_response)(char* rtcm, size_t length);
*/
void  QxRtcmResponse(char* rtcm, size_t length)
{
	char *rtcm_data_buf=0;
	if(length!=0)
	{
		rtcm_data_buf =malloc(length*sizeof(char));
		if(rtcm_data_buf!=NULL)
		{
			
		
		}
	}
	
	
	

}
/*
״̬��������
typedef void(*qxwz_status_response)(qxwz_rtcm_status_code status);
*/
void  QxStatusResponse(qxwz_rtcm_status_code status)
{
#if DEBUG_RTCM
	printf("rtcm status  %d ",status);
#endif

}




