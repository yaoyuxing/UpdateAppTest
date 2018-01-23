#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__
#include "uart5.h"
#include "rtk.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#define BLE_BOUND 		"9600"
#define BLE_UART_BOUND	9600

#define BLE_DEVELOPMENT_BOARD	0  //==1是开发板

/*
蓝牙接收的串口相关全全局变量引用 
*/
#define BLUETOOTH_RX_STATUS 	gUSART5_RX_STATUS
#define BLUETOOTH_UART_NUM 		gUART5_NUM
#define BLUETOOTH_RX_BUF 		gUSART5_RX
#define BLUETOOTH_REC_LEN		USART5_REC_LEN
#define MAX_BL_COMMAND_ECHO		100  //命令回显最大的回显数据大小 char

#define BlSendCommand(command)			  UART5_SendBytes(command) //用来发送命令
#define BlSendData(data)			UART5_SendBytesSizeof(data)//发送数据结构
#define BlSendDataLen(data,len)	 	UART5_SendDataBytes(data,len)//发送指定长度的内存
#define BlSendByte(ch)	 			UART5_SendByte(ch)//发送一个字符


/*
蓝牙的相关引脚控制和状态监控的引脚初始化
*/
#if BLE_DEVELOPMENT_BOARD
	/*蓝牙DIO_6引脚控制的相关配置*/
	#define	BL_MRDY_PIN				GPIO_PIN_0
	#define	BL_MRDY_GPIO			GPIOB
	#define BL_MRDY_RCC()			__HAL_RCC_GPIOB_CLK_ENABLE()
	//DIO_6:状态相关设置
	#define	BL_MRDY_PIN_SET()		HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_SET)
	#define	BL_MRDY_PIN_RESET()		HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_RESET)

	/*连接指示，当模块建立连接后输出低电平 蓝牙DIO_8引脚控制的相关配置*/
	#define	BL_CONNECTION_PIN			GPIO_PIN_1
	#define	BL_CONNECTION_GPIO			GPIOB
	#define BL_CONNECTION_RCC()			__HAL_RCC_GPIOB_CLK_ENABLE()
	//DIO_8:状态读取
	#define BL_CONNECTION_STATUS()		HAL_GPIO_ReadPin(BL_CONNECTION_GPIO,BL_CONNECTION_PIN) //0表示连接上了 1断开
	/*
	复位引脚相关信息
	*/
	#define	BL_RESET_PIN			GPIO_PIN_5
	#define	BL_RESET_GPIO			GPIOB
	#define BL_RESET_RCC()			__HAL_RCC_GPIOB_CLK_ENABLE()
	//复位引脚电设置
	#define	BL_RESET_PIN_SET()		HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_SET)
	#define	BL_RESET_PIN_RESET()	HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_RESET)
#else
	/*蓝牙DIO_6引脚控制的相关配置  M_BL_MRDY*/
	#define	BL_MRDY_PIN				GPIO_PIN_4
	#define	BL_MRDY_GPIO			GPIOK
	#define BL_MRDY_RCC()			__HAL_RCC_GPIOK_CLK_ENABLE()
	//DIO_6:状态相关设置
	#define	BL_MRDY_PIN_SET()		HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_SET)
	#define	BL_MRDY_PIN_RESET()		HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_RESET)

	/*连接指示，当模块建立连接后输出低电平 蓝牙DIO_8引脚控制的相关配置  M_BL_LINK*/
	#define	BL_CONNECTION_PIN			GPIO_PIN_6
	#define	BL_CONNECTION_GPIO			GPIOK
	#define BL_CONNECTION_RCC()			__HAL_RCC_GPIOK_CLK_ENABLE()
	//DIO_8:状态读取
	#define BL_CONNECTION_STATUS()		HAL_GPIO_ReadPin(BL_CONNECTION_GPIO,BL_CONNECTION_PIN) //0表示连接上了 1断开
	/*
	复位引脚相关信息
	*/
	#define	BL_RESET_PIN			GPIO_PIN_5
	#define	BL_RESET_GPIO			GPIOB
	#define BL_RESET_RCC()			__HAL_RCC_GPIOB_CLK_ENABLE()
	//复位引脚电设置
	#define	BL_RESET_PIN_SET()		HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_SET)
	#define	BL_RESET_PIN_RESET()	HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_RESET)
#endif

/*
蓝牙的数据格式
数据头：$
数据长度：u16：表示数据的长度
数据类型：u8：表示是什么数据：（预留）
数据：数据
数据尾@

*/
extern char gBlRtcmOpenFlag;
#define BLUETOOTH_DATA_HEAD  '#'
#define BLUETOOTH_DATA_END   '@'
#define  ESCAPE_CHARACTER	0x7d //转义字符
#define  XOR_VALUE			0x20	//异或值
/*
蓝牙传输的数据定义
*/
/*
data_types;	//数据类型

*/
#pragma pack(1)  

typedef  struct 
{
	u16	data_len;//数据长度
	u8	data_types;	//数据类型
	u8  *data;//数据

}stBluetoothData;
#pragma pack()
/*
蓝牙状态记录
*/
//蓝牙的连接状态记录
enum  eBlConnection
{
	eBlConnectionY=0,  //已经链接  
	eBlConnectionN,
};
//蓝牙的数据模式记录 （蓝牙串口数据模式）
//模式回传显示字符
#define COMMAND_MODE_RETURN		"CMD IN"
#define PENETRATE_MODE_RETURN	"CMD OUT"
enum  eBlMode
{
	eBlUCommandMode=0,  //已经命令模式 
	eBlUPenetrateMode,	//穿透
	eBlUModeNC,		
	eBlUModeN,	       //没有确定现在是在什么模式下
};
//获得发送命令的回显
enum  eBlCommandEcho
{
	eBlCommandEchoY=0,  //获取命令回显成功
	eBlCommandEchoN,

};
//标记缓存区中的数据有没有解析完
enum  eBlRxBuffAnalysis
{
	eBlRxBuffAnalysisY=0,  //数据解析完成
	eBlRxBuffAnalysisIng,
	eBlRxBuffAnalysisN,
	
};
//命令是否发送成功
enum  eBlCommandSendStatus
{
	eBlCommandSendY=0,  //成功
	eBlCommandSendN,
	
};
//蓝牙状态数据定义
typedef  struct 
{
	uint32_t connection:1;//蓝牙的连接状态记录	eBlConnection
	uint32_t mode:2;	//蓝牙的数据模式记录	eBlMode
	uint32_t command_echo:1;//发送命令的回显	eBlCommandEcho
	uint32_t rx_buff_analysis:2;//标记缓存区中的数据有没有解析完	eBlRxBuffAnalysis
	uint32_t command_send_status:1;//命令是否发送成功
	uint32_t	:25;
}stBluetoothStatusBit;

typedef union{
	uint32_t				data;
	stBluetoothStatusBit	bit;
}stBlStatus;
/*
蓝牙状态记录end
*/
/*
蓝牙给手机的数据（(读数据)请求模式：数据类型）
1：设备类型（手持、基站）设备的类型（这台设备是什么设备），设备的编号（这台设备是第	几个同类型的设备）
2：GGA数据：（给手机作为RTCM数据的反馈）
3：电台的相关配置：（信道的读取）
4：系统的工作状态：（作为系统的检测:在手机端监控(信道、请求设备ID号)）

手机给蓝牙的数据（（写数据）设置模式：数据类型）
1：电台的相关配置：（信道设置）
2：基站手持杖模式切换
3：RTCM数据
*/
//(读数据)请求模式：数据类型
typedef enum  eBlRequestDataTypes//请求的数据类型
{
	eBlDataRequestTypes_DeviceId=0,	//请求设备ID号
	eBlDataRequestTypes_GGA,//gga数据
	eBlDataRequestTypes_Channel,//信道
	eBlDataRequestTypes_SysStatus,//系统状态
	eBlDataRequestTypes_BleSetStatus,//蓝牙设置数据设置是否成功
}eBlRequestDataTypes;
//（写数据）设置模式：数据类型
typedef enum  eBSetUpDataTypes//设置的数据类型
{

	eBlDataSetUpTypes_Channel=0,//电台的相关配置：（信道设置）
	eBlDataSetUpTypes_ModeSwitch,//基站手持杖模式切换
	eBlDataSetUpTypes_Rtcm,	//RTCM数据
	eBlDataSetUpTypes_SelfCalibration,	//RTK自校准
	eBlDataSetUpTypes_SetPosition,		//手动设置基站相关信息
}eBlSetUpDataTypes;
#define BlDataTypes(x)	(0x1<<x)//配合 eBSetUpDataTypes   eBlRequestDataTypes定义对应的不同的位对应不同的数据

typedef enum  eBlDataMode
{
	eBlDataRequestMode=0,	//请求模式
	eBlDataSetUpMode,		//设置模式
	eBlDataSetStatusUpMode,		//设置状态反馈
	
}eBlDataMode;
/*
蓝牙发送接收数据类型定义
*/

#pragma pack(1)
typedef  struct 
{
	uint32_t  DataMode:2;	//数据的模式（0:请求数据，1:设置数据,2:主动反馈表示有没有接收到蓝牙的设置信息）
	uint32_t  DataTypes:30;	//数据的类型（每一个bit位对应一种数据类型）
}stBlDataTypesBit;
typedef union{
	uint32_t			data;
	stBlDataTypesBit	bit;
}stBlDataTypes;
/*
蓝牙发送接收数据类型定义end
*/
typedef  struct 
{
	stBlDataTypes	DataTypes;	//数据的类型
	uint32_t		DataLen;	//数据的长度(原始数据长度)
	uint32_t		DataEscapedLen;	//数据的长度(原始数据长度)
}stBlDataHead;
#pragma pack() 

/*
蓝牙测试代码
*/
extern  void test_bluetooth(void);
//蓝牙数据解析
extern  void BluetoothAnalysis(char **data_return,stBlStatus *BlStatus);
//初始化蓝牙相关的IO
extern  void BluetoothGPIO(void );
//清空数据缓存空间
extern  void BlRxDatabuffClear(void);
/*
命令模式下的命令回传数据获取
*/
extern  void BlCommandReturnAnalysis(char *return_data,stBlStatus *BlStatus);
/*
清空蓝牙的回显数据缓存空间
*/
extern	void  BlCommandEchoBuffClear(void);
/*
切换到透传模式(串口数据模式)
*/
extern	void BlUPenetrateMode(stBlStatus  *status);
/*
切换到命令模式(串口数据模式)
*/
extern	void BlUCommandMode(stBlStatus  *status);
/*
	初始化蓝牙等待连接蓝牙
	等待设备连接
	切换到数据透传模式
*/
extern	void BlInit(stBlStatus  *status);
/*
在运行过程中断连了，等待重新建立连接
*/
extern  void BlReconnection(stBlStatus  *status);
/*
硬件复位
*/
extern void BlReset(void);

/*
设置RTCM数据
*/

extern void BlRTCMSet(char *src_data,stBlDataHead *head,stBlStatus *BlStatus);
extern void BlReceiveDataUnpacking( char** data,stBlStatus *status,stRtkStatus *RtkStatus);
extern void BlRequest_GGA(stBlDataHead *BlDataHead,stRtkStatus	*RtkStatus );
/*
全局变量定义
*/
extern	char		gCommandEchoBuff[MAX_BL_COMMAND_ECHO];//存储命令回显的buff
extern	stBlStatus  gBlStatus;//蓝牙的状态
/*
测试代码
*/
extern void BlTest(void);
/*
蓝牙模块测试函数
*/
void BlTest(void);
#endif

