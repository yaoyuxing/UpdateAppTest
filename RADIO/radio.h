#ifndef __RADIO_H_
#define __RADIO_H_
#include "sys.h"
#include "uart7.h"

#define	RadioSendByteS(X)				UART7_SendBytes(X)	//发送多个字节的数据  以'\0'结束
#define	RadioSendByteSizeof(X) 			UART7_SendBytesSizeof(X)//发送一段内存
#define	RadioSendByte(X)				UART7_SendByte(X)	//发送一个字节
#define	RadioSendByteSLen(data ,len )	UART7_SendBytesLen(data ,len)	//发送一个字节

#define	RadioUartBound(X)				UART7_Bound(X)		//设置电台对应的串口的波特率
#define	RADIO_RETURN_PARAMETER_CMD	0xc1c1c1//返回已保存的参数  
#define	RADIO_RETURN_VERSION_CMD	0xc3c3c3//模块返回版本信息
#define	RADIO_RETURN_RESET_CMD		0xc4c4c4//模块将产生一次复位 
//广播模式的地址设置
#define	RADIO_BROADCAST_MODE_ADD_L	0xFF
#define	RADIO_BROADCAST_MODE_ADD_H	0xFF

#define	RADIO_DATA_HADE_LEN	3


/*
命令参数定义
*/
//参数是否掉电保存
typedef enum	eRadioParameterSave
{
	eRadioParameterSaveY=0xc0,// C0：所设置的参数会掉电保存
	eRadioParameterSaveN=0xc2,//	C2：所设置的参数不会掉电保存。
}eRadioParameterSave ;
//串口校验位 
typedef enum	eRadioCmdSerialCheck
{
	eRadioSerialCheck_8N1=0,//默认
	eRadioSerialCheck_8O1,
	eRadioSerialCheck_8E1,
}eRadioCmdSerialCheck;
//串口波特率
typedef enum	eRadioCmdSerialRate
{
	eRadioSerialRate_1200 =0,
	eRadioSerialRate_2400,
	eRadioSerialRate_4800,
	eRadioSerialRate_9600,//默认
	eRadioSerialRate_19200 ,
	eRadioSerialRate_38400 ,
	eRadioSerialRate_57600 ,
	eRadioSerialRate_115200,

}eRadioCmdSerialRate;
//空速度
typedef enum	eRadioCmdAirRate
{
	eRadioAirRate_2_5k=0,//默认
	eRadioAirRate_5k,
	eRadioAirRate_12k,
	eRadioAirRate_28k ,
	eRadioAirRate_64k ,
	eRadioAirRate_168k,
}eRadioCmdAirRate;
//通信信道
#define  MAX_RADIO_CHANNEL 0x1f//(最大的跨度)：默认 02H（433MHz）通信信道，（431M + CHAN*1M） 默认 02H（433MHz）00H-1FH， 对 应 431~462MHz  
//包长设定 （单位字节）
typedef enum	eRadioPackageLengthSetting 
{
	eRadioPackageLength_16=0,//
	eRadioPackageLength_32,
	eRadioPackageLength_64,//默认
	eRadioPackageLength_128,
	eRadioPackageLength_256,
	eRadioPackageLength_512,
	eRadioPackageLength_1024,
	eRadioPackageLength_2048,
}eRadioPackageLengthSetting;
//发射功率(大约值) 
typedef enum	eRadioTransmitPower 
{
	eRadioTransmitPower_30dbm=0,//默认
	eRadioTransmitPower_27dbm,
	eRadioTransmitPower_24dbm,
	eRadioTransmitPower_17dbm,
}eRadioTransmitPower;
// 驱动方式（默认1） 
typedef enum	eRadioIODriveMode 
{
	eRadioIODriveMode_OpenCircuit=0,// 0：TXD、AUX开路输出，RXD开路输入
	eRadioIODriveMode_PushPullUp,// 1：TXD、AUX推挽输出，RXD上拉输入 （默认1） 

}eRadioIODriveMode;
// FEC 开关  
typedef enum	eRadioFECSwitch 
{
	eRadioFECSwitch_Close=0,// 关闭 FEC 纠错
	eRadioFECSwitch_Open,// 打开 FEC 纠错（默认） 

}eRadioFECSwitch;
// 唤醒时间  
typedef enum	eRadioWakeUpTime 
{
	eRadioWakeUpTime_500ms=0,
	eRadioWakeUpTime_1000ms,//默认
	eRadioWakeUpTime_1500ms,
	eRadioWakeUpTime_2000ms,
	eRadioWakeUpTime_2500ms,
	eRadioWakeUpTime_3000ms,
	eRadioWakeUpTime_3500ms,
	eRadioWakeUpTime_4000ms,
}eRadioWakeUpTime;
// 定点发送使能位  
typedef enum	eRadioSentToSent
{
	eRadioSentToSent_Transparent=0,//透明传输模式（默认）
	eRadioSentToSent_FixedPoint,// 定点传输模式 

}eRadioSentToSent;
#pragma pack(1)  
/*
速率参数，包括串口速率和空中速率 
*/
typedef	union{
	struct{
		u8	AirRate:3;//空中速率
		u8	SerialRate:3;//串口速率
		u8	SerialCheck :2;//串口校验位 
	}bit;
	u8 byte;
}stRadioSped;
/*
包长设定（仅用于分包模式） 
通信信道，（431M + CHAN*1M） 
*/
typedef	union{
	struct{
		u8	CommunicationChannel :5;//通信信道  00H-1FH
		u8	PackageLengthSetting :3;//包长设定
	}bit;
	u8 byte;
}stRadioChan ;
/*
7：定点发送使能位（类 MODBUS） 
6 , 5 , 4   唤醒时间 
3,        FEC 开关       
2       IO 驱动方式（默认1）
1, 0     发射功率(大约值) 
*/
typedef	union{
	struct{
		u8	TransmitPower	:2;//发射功率(大约值) 
		u8	IODriveMode		:1;// 驱动方式（默认1）
		u8	FECSwitch		:1;// FEC 开关 
		u8	WakeUpTime		:3;//唤醒时间 
		u8	SentToSent		:1;//定点发送使能位
	}bit;
	u8 byte;
}stRadioOption  ;
/*
电台参数定义
*/
typedef	struct{
	u8	head;//eRadioCmdHead C0：所设置的参数会掉电保存。 C2：所设置的参数不会掉电保存。
	u8	addH;//模块地址高字节（默认00H）
	u8	addL;//模块地址低字节（默认00H） 
	//SPED 
//	u8	AirRate:3;			//空中速率
//	u8	SerialRate:3;		//串口速率
//	u8	SerialCheck :2;		//串口校验位 
	stRadioSped sped;
	//CHAN 
//	u8	CommunicationChannel :5;	//通信信道  00H-1FH
//	u8	PackageLengthSetting :3;	//包长设定
	stRadioChan chan;
	//OPTION 
//	u8	TransmitPower:2;	//发射功率(大约值) 
//	u8	IODriveMode:1;		// 驱动方式（默认1）
//	u8	FECSwitch:1;		// FEC 开关 
//	u8	WakeUpTime:3;		//唤醒时间 
//	u8	SentToSent:1;		//定点发送使能位
	stRadioOption option; 
}stRadioCommandParameter;
#pragma pack()  
/*
电台的工作模式
*/
typedef enum  eRadioWorkMode
{
	eRaRssiMode=0,					//0 RSSI 模式	用于监听环境噪声 
	eRaContinuousTransmissionMode,	//1 连传模式	双方的串口参数设定必须一致
	eRaSubcontractMode,				//2 分包模式	该方式下，串口波特率和空速是独立的，收发双方可以具有不同的串口波特率，但必须具有相同的空速设定。 
	eRaConfigurationMode,			//3 配置模式	可以用于模块参数设置，使用串口 9600、8N1，通过特定指令格式设置模块工作参数 
	eRaWakeMode,					//4 唤醒模式 
	eRaPowerSavingMode=6,			//6 省电模式 
	eRaSleepMode,					//7 休眠模式 
}eRadioWorkMode;
/*电台的数据状态*/
typedef enum	eRadioReady
{
	eRadioReadyY=0, //设备准备就绪
	eRadioReadyN,	
}eRadioReady ;
/*电台信道的设置状态记录*/
typedef enum	eRadioChannel
{
	eRadioChannelY=0, //电台信道的设置状态记录成功
	eRadioChannelN,	
}eRadioChannel;
/*电台的状态信息 */
typedef  struct 
{
	uint32_t 	ready:1;//电台状态准备状态  eRadioReady
	uint32_t	mode:3;//记录当前的工作模式	eRadioWorkMode
	uint32_t	channel:1;//设置信道的状态记录 
	uint32_t	:32-5;
}stRadioStatusBit;
typedef union{
	uint32_t			data;
	
	stRadioStatusBit	bit;
}stRadioStatus;

/*电台的数据状态 end*/


/*
电台数据状态检测
status:返回电台的设备状态
eRadioReady：
*/
extern eRadioReady RaReadAux(stRadioStatus *status);
/*
模式切换函数
*/
extern void RaModeSwitch(eRadioWorkMode eMode,stRadioStatus *status);
/*
	串口波特率和电台波特率匹配
*/
extern void RadioUartBoundMatch(eRadioCmdSerialRate SerialRate);
/*
切换到配置模式（模式 3） :切换到配置模式后开始配置对应的参数命令
*/
extern void RadioConfigurationMode(stRadioCommandParameter *parameter,stRadioStatus *status,u8 *baud );
/*
	退出配置模式
	判断波特率有没有改变
baud: 发送数据的波特率
*/
extern void RadioExitConfigurationMode(stRadioCommandParameter *parameter,stRadioStatus *status,u8 *baud);

/*
发送电台参数：配置电台
*/
extern void RadioParameterSend(stRadioCommandParameter *parameter,	stRadioStatus *status);

/*
初始化电台
*/

extern void RadioInit(stRadioStatus *status,stRadioCommandParameter *parameter);
/*
读信道
*/
uint8_t RadioChannelRead(stRadioCommandParameter *parameter);
/*
	电台数据发送：通过电台将将数据发送给下面的其他设备
	：发送的数据有GGA和RTCM数据
*/
extern void RadioDataSend(stRadioCommandParameter *parameter,char *data);
/*
带台的信道设置
最多可以有32个信道
默认 02H（433MHz）通信信道，（431M + CHAN*1M） 默认 02H（433MHz）00H-1FH， 对 应 431~462MHz 
channel  信道值(这个值是0到31 )
parameter 现在已经配置好的参数
status ：状态反馈
*/
extern void RadioChannelSet(uint8_t channel,stRadioCommandParameter *parameter,stRadioStatus *status);
/*
测试代码
*/
extern void radio_test(void);




extern stRadioStatus gRadioStatus;//状态的记录
extern stRadioCommandParameter gRadioCommandParameter;//参数记录

#endif








