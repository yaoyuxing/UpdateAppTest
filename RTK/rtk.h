#ifndef __RTK_H__
#define __RTK_H__
#include "sys.h"
#include "uart4.h"
#include "rtk.h"
#include "me3630.h"
//strcmp()
//   <0               第一个字符串小于第二个字符串
//     0               两个字符串相等    ·
//    >0               第一个字符串大于第二个字符串
#define ToString(x) #x  //个X变量中的数据加上""
#define RTK_COMMAND_COM  "com1"
#define RTK_DATA_COM  "com2"

#define RTK_DATA_COM_BAUD_INIT  "9600"//默认波特率
#define RTK_DATA_COM_BAUD_SET  	"57600"//需要设置的波特率

#define MAX_RTK_COMMAND_BUF  200

/*
串口的相关定义
*/
//rtcm数据端口

#define RtkSendRtcmUartInit(bound)  uart4_init(bound)
#define RtkSendRtcmToRtk(data,len)	UART4_SendNumBytes(data,len)//用来发送RCM数据到RTK
#define RtkSendRtcmUartByte(data) 	UART4_SendByte(data)

//命令端口
#define RtkSendCommandUartInit(bound)  UART3_Bound(bound)
#define RtkSendCommandUartChar(data)  rtkSendByte(data)
#define RtkSendCommandUartChars(data,len) UART3_SendDataBytes(data,len)

/*
串口的相关定义 end
*/
extern char gCommandBuf[MAX_RTK_COMMAND_BUF];

#define   POSITION(X)    (0x1<<(X))

/*
RTK状态控制
*/
//判断RTK的命令是否发送成功
enum  eRtkSendCommand
{
	eSendCommandY=0,
	eSendCommandN,
};
//判断RTK的命令是否发送成功
enum  eRtkGgaStatus
{
	eGpggaStatus1=0,
	eGpggaStatus4,  //GGA的状态为4
	eGpggaStatus7,
	eGpggaStatusN,
};
//判断BESTPOS的数据是否获取成功
enum  eRtkBestposStatus
{
	eBestposStatusY=0,  
	eBestposStatusN,
};
//判断是否进入了基站模式
enum  eRtkBaseStation
{
	eBaseStationY=0,  
	eBaseStationN,
};
//gga的获得情况 
enum  eRtkGetGga
{
	eGetGgaY=0,  
	eGetGgaN,
};
typedef  struct 
{
	uint32_t	send_command:1;//判断发送命令有没有配置成功eRtkSendCommand
	uint32_t	gpgga_status:2;//表示状态有没有到达状态4
	uint32_t	bestpos_status:1;//eRtkBestposStatus
	uint32_t	base_station:1; //eRtkBaseStation
	uint32_t	get_gga:1;//gga的获得情况  o：获得成功  1：获得失败
	uint32_t	:32-6;

}stRtkStatusBit;
typedef union{
	uint32_t   data;
	stRtkStatusBit bit;
}stRtkStatus;
/*
RTK经度 纬度 高度
纬度：latitude
经度：Accuracy
高度：heigh
*/
#define MAX_BESTPOS_DATA_LEN 20
typedef  struct 
{
	char	latitude[MAX_BESTPOS_DATA_LEN];//纬度：
	char	accuracy[MAX_BESTPOS_DATA_LEN];//经度：
	char	heigh[MAX_BESTPOS_DATA_LEN];//高度：
}stRtkBestposData;
/*
RTK状态控制end
*/
/*
外部函数接口
*/
extern void rtk_SendBytes(char *command);
/*
发送指定长度的数据到RTK
*/
extern void rtk_SendNumByte(char *data ,uint16_t len);
/*
发送RTK GGA的状态解析
[COM2]$GPGGA,100625.00,3030.5032,N,10407.1052,E,7,08,2.0,502.33,M,-43.20,M,,*4F
*/
extern void RtkGpggaStatus(stRtkStatus *RtkStatus); 
/*
外部函数接口 end
*/
extern void RtkBestposAnalysis(stRtkBestposData *BestposData,stRtkStatus *RtkStatus);
/*
拼接LOG命令
GGA
bestpos
log com2 gpgga ontime 1
*/
extern void LogCommand(char *command,char *comx,char *delaytime ,char* return_command);
/*
设置进入基准站模式的命令拼接
fix position 纬度 经度 高度（这个经纬度高度是bestpos的）
纬度：latitude
经度：Accuracy
高度：height
*/
extern void FixPositionCommand(char *latitude,char *accuracy ,char* height,char* return_command);
/*
设置进入基准站模式的命令拼接
fix none
*/
extern void FixNoneCommand(char* return_command);
/*
com com2 115200
设置COM的波特率
*/
extern void RtkComBaudCommand(char * comx,char *baud,char* return_command);
/*
配置com口的收发协议
interfacemode com2 rtcmv3 novatel on
INTERFACEMODE [port] rxtype txtype [responses]
*/
extern void RtkInterfaceCommand(char * comx,char *rxtype,char *txtype,char* return_command);

/*
RTK复位到出厂设置
freset
*/
extern void RtkFreset(char* return_command);
/*
关闭对应COM口的对应命令输出
unlog com1 gpgga 
*/
extern void UnLogCommand(char *comx,char *command,char* return_command);
/*
关闭所有COM口的数据输出
unlogall
*/
extern void UnLogAllCommand(char* return_command);
/*
发送清除命令COM的数据输出
*/
extern void RtkSendUnlogallcom(char *com,stRtkStatus *RtkStatus);
/*
配置命令后判断数据有没有配置成功
RtkStatus：RTK的发送状态
*/
extern void RtkSendCommandStatus(stRtkStatus *RtkStatus);
/*
发送RTK配置命令
command：发送的命令
RtkStatus：RTK的发送状态
*/
extern void RtkSendCommand(char *command,stRtkStatus *RtkStatus);
/*
发送RTK GGA的状态解析
[COM2]$GPGGA,100625.00,3030.5032,N,10407.1052,E,7,08,2.0,502.33,M,-43.20,M,,*4F
*/
extern void RtkGpggaStatus(stRtkStatus *RtkStatus);
/*
解析Bestpos数据的
*/
extern void RtkBestposAnalysis(stRtkBestposData *BestposData,stRtkStatus *RtkStatus);
/*
发送GGA命令并读取状态
log com2 gpgga ontime 1
*/
extern void RtkSendGpgga(stRtkStatus *RtkStatus,char *time);
/*
发送GGA命令并读取状态:指定一个端口
comx:rtk端口号
time：发送频率
log com2 gpgga ontime 1
*/
extern void RtkSendGpggaComx(char * comx,char *time,stRtkStatus *RtkStatus);
/*
RTK初始化：
开始状态数据初始化
*/
extern void RtkInit(stRtkStatus *RtkStatus);
/*
RTCM数据发送,判断gga的状态有没有到达4
*/
extern void RtkSendRtcmDataStatus(stRtkStatus *RtkStatus,char *rtcm_data);
/*
函数功能：
1：配置RTK输出LOG GGA的数据
2：提起GGA的数据，判断GGA的状态
3：将GGA数据发给服务器
同时在另一个任务判断有没有收到rtcm数据
如果有收到将数据发送给RTK（函数me3630RxAnalysis）
*/
extern void RtkGetGga(stRtkStatus *RtkStatus,stMe3630Status *Me3630Status);
/*
RTK:自校准
	校准流程：用基站自己的GGA数据
*/

extern void RtkSelfCalibration(stRtkStatus *RtkStatus);
/*
设置进入基准站模式
fix position 纬度 经度 高度
*/

extern void RtkSendPosition(stRtkBestposData *BestposData,stRtkStatus *RtkStatus);
/*
自校准基站模式命令
state = on off
maxtime:等待时间
maxhstd:水平误差
maxvstd：高度误差
*/

extern void RtkCommandPosave(char *state,char *maxtime,char *maxhstd ,char *maxvstd ,char* return_command);

/*
RTK的状态标记初始化
*/

extern void RtkStatusInit(stRtkStatus *RtkStatus);

extern void rtk_test(void);



extern stRtkStatus	gRtkStatus;
#endif
