/*
电台的默认的配置参数：
模块型号  	  频率    地址   信道  空中速率  波特率  串口格式  发射功率
E71-433MS30  433MHz  0x0000  0x04  2.5K      9600     8N1      1W 
*/
#include "radio.h"
#include "sys.h"
#include "delay.h"
#include <stdlib.h>
#include <string.h>

stRadioStatus gRadioStatus;//状态的记录
stRadioCommandParameter gRadioCommandParameter;//参数记录
/*
电台的IO相关配置
*/
//C口相关的IO定义
#define	RADIO_M0_PIN	GPIO_PIN_0
#define	RADIO_M1_PIN	GPIO_PIN_1
#define	RADIO_M2_PIN	GPIO_PIN_2
#define	RADIO_AUX_PIN	GPIO_PIN_3
#define	RADIO_GPIO		GPIOC
//B口相关的IO定义
#define	RADIO_PEN_PIN	GPIO_PIN_6
#define	RADIO_PEN_GPIO	GPIOB
//K口相关的IO定义
#define RADIO_RESET_PIN 		GPIO_PIN_7
#define	RADIO_RESET_GPIO		GPIOK


void RadioGpioInit(void )
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOC_CLK_ENABLE();           		//开启GPIOC时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();           		//开启GPIOC时钟
	__HAL_RCC_GPIOK_CLK_ENABLE();           		//开启GPIOC时钟
	//模块电源使能
	GPIO_Initure.Pin=RADIO_PEN_PIN; 			
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          	//上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	
	HAL_GPIO_Init(RADIO_PEN_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(RADIO_PEN_GPIO,RADIO_PEN_PIN,GPIO_PIN_SET); //复位引脚拉高
	
	//电台工作模式配置引脚    
	GPIO_Initure.Pin=RADIO_M0_PIN|RADIO_M1_PIN|RADIO_M2_PIN;
	HAL_GPIO_Init(RADIO_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M0_PIN|RADIO_M1_PIN|RADIO_M2_PIN,GPIO_PIN_SET); //初始化为休眠模式
	
	//电台的发送数据和启动状态的数据反馈
	GPIO_Initure.Pin=RADIO_AUX_PIN; 			
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  		//输入模式
	HAL_GPIO_Init(RADIO_GPIO,&GPIO_Initure);
	//模块复位
	GPIO_Initure.Pin=RADIO_RESET_PIN; 			
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          	//上拉
	HAL_GPIO_Init(RADIO_RESET_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(RADIO_RESET_GPIO,RADIO_RESET_PIN,GPIO_PIN_SET); //复位引脚拉高
	
}
/*
	串口波特率和电台波特率匹配
*/
void RadioUartBoundMatch(eRadioCmdSerialRate SerialRate)
{
	switch(SerialRate)
	{
		case   eRadioSerialRate_1200	: 	RadioUartBound(1200	)	;break;
		case   eRadioSerialRate_2400	: 	RadioUartBound(2400	)	;break;
		case   eRadioSerialRate_4800	: 	RadioUartBound(4800	)	;break;
		case   eRadioSerialRate_9600	: 	RadioUartBound(9600	)	;break;
		case   eRadioSerialRate_19200	: 	RadioUartBound(19200)	;break;
		case   eRadioSerialRate_38400	: 	RadioUartBound(38400)	;break;
		case   eRadioSerialRate_57600	: 	RadioUartBound(57600)	;break;
		case   eRadioSerialRate_115200	:	RadioUartBound(115200)	;break;
		default: break;
	}
}
/*
模式切换函数
*/
void RaModeSwitch(eRadioWorkMode eMode,stRadioStatus *status)
{
//	while(RaReadAux(status)!=eRadioReadyY) //等待设备就绪
//	{
//		delay_ms(1);
//	}
	if(RaReadAux(status)==eRadioReadyY)	//设备就绪切换模式
	{
		delay_ms(2);
		(eMode&0x1)?HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M0_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M0_PIN,GPIO_PIN_RESET);
		(eMode&0x2)?HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M1_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M1_PIN,GPIO_PIN_RESET);
		(eMode&0x4)?HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M2_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(RADIO_GPIO,RADIO_M2_PIN,GPIO_PIN_RESET);
		status->bit.mode=eMode;
	}
	while(RaReadAux(status)!=eRadioReadyY) //等待设备就绪
	{
		delay_ms(1);
	}
}
/*
电台数据状态检测
status:返回电台的设备状态
eRadioReady：
*/
eRadioReady RaReadAux(stRadioStatus *status)
{
	if(HAL_GPIO_ReadPin(RADIO_GPIO,RADIO_AUX_PIN)) //0:表示设备还没有准备就绪  1：设备还没有准备就绪
	{
		status->bit.ready=eRadioReadyY;
		return eRadioReadyY;
	}else{
		status->bit.ready=eRadioReadyN;
		return eRadioReadyN;
	}
}
/*电台配置：命令配置相关函数*******************************/

/*
发送不带参数的命令
*/
void RadioSendCommmandNoParameter(uint32_t data,stRadioStatus *status)
{
	u8	data_buf[4];
	RaModeSwitch(eRaConfigurationMode,status);//切换到配置模式
	data_buf[0]=data&0xff;
	data_buf[1]=(data&0xff)>>8;
	data_buf[2]=(data&0xff)>>16;
	data_buf[3]='\0';
	RadioSendByteS((char*)data_buf);//发送配置的数据
}

/*
	初始化设备参数：后面的电台在这个基础上做配置
*/
void RadioParameterInit(const u8	addH,const u8	addL,stRadioCommandParameter *parameter)
{

	parameter->head=eRadioParameterSaveY;
	parameter->addH=addH;
	parameter->addL=addL;
	//SPED
	parameter->sped.bit.AirRate=eRadioAirRate_2_5k;
	parameter->sped.bit.SerialRate=eRadioSerialRate_9600;
	parameter->sped.bit.SerialCheck=eRadioSerialCheck_8N1;
	//CHAN
	parameter->chan.bit.CommunicationChannel=0x02;
	parameter->chan.bit.PackageLengthSetting=eRadioPackageLength_64;
	//OPTION 
	parameter->option.bit.TransmitPower	=eRadioTransmitPower_30dbm;
	parameter->option.bit.IODriveMode	=eRadioIODriveMode_PushPullUp;	
	parameter->option.bit.FECSwitch		=eRadioFECSwitch_Open;
	parameter->option.bit.WakeUpTime	=eRadioWakeUpTime_1000ms;	
	parameter->option.bit.SentToSent	=eRadioSentToSent_Transparent;	
//	RadioSendByteSizeof((char*)parameter);//发送数据
}
/*
带台的信道设置
最多可以有32个信道
默认 02H（433MHz）通信信道，（431M + CHAN*1M） 默认 02H（433MHz）00H-1FH， 对 应 431~462MHz 
channel  信道值(这个值是0到31 )
parameter 现在已经配置好的参数
status ：状态反馈
*/
void RadioChannelSet(uint8_t channel,stRadioCommandParameter *parameter,stRadioStatus *status)
{
	if(channel<=MAX_RADIO_CHANNEL)
	{
		parameter->chan.bit.CommunicationChannel=channel;
		status->bit.channel=eRadioChannelY;
		RadioParameterSend(parameter,status);//配置相应的通道值
	}else{
		status->bit.channel=eRadioChannelN;
	}
}
/*
读信道
*/
uint8_t RadioChannelRead(stRadioCommandParameter *parameter)
{
	return parameter->chan.bit.CommunicationChannel;
}


/*
设置分包长度
*/
void RadioPackageLengthSettingSet(eRadioPackageLengthSetting ePackageLength,stRadioCommandParameter *parameter,stRadioStatus *status)
{
	parameter->chan.bit.PackageLengthSetting=ePackageLength;
	RadioParameterSend(parameter,status);//配置分包长度
}

/*
	一些定模式的操作
*/
/*
	切换到配置模式（模式 3） :切换到配置模式后开始配置对应的参数命令
baud:返回原有的波特率
*/
void RadioConfigurationMode(stRadioCommandParameter *parameter,stRadioStatus *status,u8 *baud )
{
	*baud=parameter->sped.bit.SerialRate;
	RaModeSwitch(eRaConfigurationMode,status);
	//重新配置串口的波特率
	RadioUartBoundMatch(eRadioSerialRate_9600);
}
/*
	退出配置模式
	判断波特率有没有改变
baud: 发送数据的波特率
*/
void RadioExitConfigurationMode(stRadioCommandParameter *parameter,stRadioStatus *status,u8 *baud)
{
	if(parameter->sped.bit.SerialRate!=eRadioSerialRate_9600)
	{
		//还原原来的波特率
		RadioUartBoundMatch(*baud);
	}
	RaModeSwitch(eRaSubcontractMode,status);//还原到分包模式
}
/*
	参数设置
	拼接配置的参数
*/
void RadioParameterSet(
	stRadioCommandParameter *parameter,//返回电台的参数
	u8 addH,	//高地址
	u8 addL,	//低地址
	eRadioParameterSave	ParameterSave,	//命令头
	eRadioCmdSerialRate SerialRate, 	//串口波特率
	eRadioCmdAirRate 	AirRate,		//空速度
	eRadioSentToSent 	SentToSent		//定点发送使能位

)
{
	parameter->addH=addH;
	parameter->addL=addL;
	parameter->head=ParameterSave;
	parameter->sped.bit.SerialRate=SerialRate;
	parameter->sped.bit.AirRate=AirRate;
	parameter->option.bit.SentToSent=SentToSent;
}
/*
发送电台参数：配置电台
*/

void RadioParameterSend(stRadioCommandParameter *parameter,	stRadioStatus *status)
{
	u8 baud=0;
	u8 len=0;
	len=sizeof(stRadioCommandParameter);
	RadioConfigurationMode(parameter,status,&baud );//切换到配置模式
	RadioSendByteSLen((char*)parameter ,len);
	
//	RadioSendByteSizeof((char*)parameter);					//发送配置信息
	RadioExitConfigurationMode(parameter,status,&baud);//退出配置模式
}
/*
初始化电台
*/
void RadioInit(stRadioStatus *status,stRadioCommandParameter *parameter)
{
	//初始化IO
	RadioGpioInit( );
	//配置初始化参数
	RadioParameterInit(RADIO_BROADCAST_MODE_ADD_H,RADIO_BROADCAST_MODE_ADD_L,parameter);
	//配置需要的参数
	RadioParameterSet(parameter,RADIO_BROADCAST_MODE_ADD_H,RADIO_BROADCAST_MODE_ADD_L,eRadioParameterSaveY,eRadioSerialRate_9600,eRadioAirRate_12k,eRadioSentToSent_FixedPoint);
	RadioParameterSend(parameter,status);//设置配置的参数

}

/*
	电台数据发送：通过电台将将数据发送给下面的其他设备
	：发送的数据有GGA和RTCM数据
*/

void RadioDataSend(stRadioCommandParameter *parameter,char *data)
{	
	char *p;
	uint16_t data_len=0;
	data_len=strlen(data);
#if 0
	p=malloc(data_len+RADIO_DATA_HADE_LEN);
	memset(p,0,data_len+RADIO_DATA_HADE_LEN);
	*p=parameter->addH;
	*(p+1)=parameter->addL;
	*(p+2)=parameter->chan.bit.CommunicationChannel;
	memcpy(p+RADIO_DATA_HADE_LEN,data,data_len);
	RadioSendByteSLen(p ,data_len+RADIO_DATA_HADE_LEN );
	free(p);
#else
	printf("data_len=%d\r\n",data_len);
	p=malloc(data_len);
	
	memset(p,0,data_len);
	memcpy(p,data,data_len);
	RadioSendByteSLen(p ,data_len );
//	RadioSendByteSizeof(data);//发送数据
#endif

}

/*

	电台工作函数
	工作流程：
	1：RTK上传GGA数据到服务器等待rtk配置完成，基站达到固定解，确定基站的位置，再向服务器反馈一个精准的经纬度，用来确定基站的位置和基站的信号辐射范围
	2：等待服务器反馈一个就绪信息，表示服务器已经确定基站位置，等待服务器分配信道，获得信道后配置电台，配置好后切换到连传模式，开始周期性的
传输（GGA和RTCM数据）
	
*/
void RadioMain(void)
{
	stRadioStatus status;//状态的记录
	stRadioCommandParameter parameter;//参数记录
}

