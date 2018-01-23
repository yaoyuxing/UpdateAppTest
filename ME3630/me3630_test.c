
#include "me3630.h"
#include "delay.h"
#include "json_test.h"
#include "string.h"
stMe3630SendHead test_data;

char *GGA={"$GPGGA,000001,3112.518576,N,12127.901251,E,1,8,1,0,M,-32,M,3,0*4B"};
//初始化测试函数
void me3630TestDataInit(stMe3630SendHead *data)
{

	data->ApiLogo=eApiLogoGGA;
	data->RequestID=1;
	data->DataLen=0;
	data->DataOutWay=1;
	data->PackageInsideNum=0;
	data->PackageNum=1;		
}
//模块测试函数
void me3630_test(void)
{
	int num ;
	uint16_t data_len;
	char jsonGgaData[200];
	char DataBuf[300];
	me3630TestDataInit(&test_data);
	openTCP(&Me3630Status);
	while(1)
	{
		if(num==100)
		{
			cJSON_GgaToJson(GGA);
			while(1);
//			cJSON_GgaToJson(GGA,jsonGgaData);
//			printf("%s",jsonGgaData);
//			data_len=strlen(jsonGgaData)+sizeof(stMe3630SendHead);
//			strncpy(DataBuf,(char *)(&test_data),sizeof(stMe3630SendHead));
//			strncpy(DataBuf+sizeof(stMe3630SendHead),jsonGgaData,strlen(jsonGgaData));
//			
//			me3630_SendData(DataBuf,data_len,&Me3630Status);
			num=0;
		}
		num++;
		me3630RxUnpacking(&Me3630Status);
		delay_ms(10);
	}
}
