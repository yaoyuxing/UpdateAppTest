#ifndef __ME3630_H__
#define __ME3630_H__
#include "sys.h"
/*
配置命令列表
*/
#define AT				"AT\r"
#define	CALL_OPEN		"AT+ZIPCALL=1\r" 							//打开协议
//#define	TCP_OPEN		"AT+ZIPOPEN=1,0,120.76.224.208,60000\r"  	//链接tcp协议
#define	TCP_OPEN		"AT+ZIPOPEN=1,0,47.92.131.173,8096\r"  	//链接tcp协议
#define	CALL_CLOSE		"AT+ZIPCALL=0\r"   							//关闭协议
#define	TCP_CLOSE		"AT+ZIPCLOSE=1\r"  							//关闭TCP
#define	CSQ				"AT+CSQ\r"  								//信号强度读取

#define	SEND_ORIGINAL_DATA	"AT+ZIPSENDRAW=1,"						//发送原始数据
#define	SEND_DATA		"AT+ZIPSEND=1,"								//发送命令

#define COMMAND_END		"\r\0"										//结束符
/*
配置命令列表  end
*/
#define MAX_SENDCOMMAND_NUM  20			//重复发送命令的次数
#define	MAX_SENDDATA_NUM     200		//最大发送的字节数
#define	SENDTIME_MS     	 200		//发送的间隔时间
#define	MAX_EXTRACTION_DATA_LEN     	 800		//拆包后的最大数据长度 char //600
/*
命令状态列表
*/
//字符状态
#define	COMMAND_OK		"OK"					//命令发送正确
#define	COMMAND_ERR		"ERROR"					//命令发送错误
/*
	AT+CSQ 信号强度查询
	反馈的数据对应的
	该指令用于查询接收信号强度（rssi）和信道误码率(ber) 
*/



//标记命令发送成功
enum  eMe3630CommandStatus
{
	eSendCommandOk=0,
	eSendCommandErr,
	eSendCommandIng,
};
//标记数据发送
enum  eMe3630DataStatus
{
	eSendDataOk=0,//发送成功
	eSendDataErr,//发送失败
	eSendDataIng,//正在发送
};
//标记命令发送超时
enum  eMe3630CTimeoutStatus
{
	eTimeoutY=0,
	eTimeoutN,
};
//TCP	0链接成功 1没有链接成功
enum  eMe3630TcpLinkStatus
{
	eLinkY=0,
	eLinkN,
	eLinkIng,
};
//0表示模块开机准备就绪   1表示还没有就绪
enum  eMe3630OnStatus
{
	eOn=0,
	eOff,
};


//标记是否不断循环发送 1不断循环 0表示不一直循环
enum  eMe3630SendCtrCycleAlways
{
	eAlwaysN=0,
	eAlwaysY,
};
/*
发送命令和数据时的控制
*/
typedef  struct 
{
	uint32_t	cycle_nunm:8;//标记最大的循环次数
	uint32_t	time_len:16;//每次命令的延时时间单位ms
	uint32_t	cycle_always:1;//标记是否不断循环发送 1不断循环 0表示不一直循环
	uint32_t	:7;
}stMe3630CtrBit;
typedef union{
	uint32_t   data;
	stMe3630CtrBit bit;
}stMe3630Ctr;
/*
发送命令和数据时的控制end
*/

/*
记录4G模块发送的相关状态
*/
//标记是否不断循环发送 1不断循环 0表示不一直循环
enum  eMe3630SendRaw
{
	eSendRawY=0,
	eSendRawN,
};

typedef  struct 
{
	uint32_t send:1;//标记命令发送成功 0Y  1N  收到OK
	uint32_t send_raw:1;//标记发送原始数据命令发送成功  0Y  1N  收到NO
	uint32_t :30;
}stMe3630SendStatusBit;
typedef union{
	uint32_t data;
	stMe3630SendStatusBit bit;
}stMe3630SendStatus;

/*
记录4G模块发送的相关状态
*/

/*
记录4G模块的相关状态
*/
//标记是否不断循环发送 1不断循环 0表示不一直循环
enum  eMe3630Receive
{
	eReceiveFullY=0,
	eReceiveFullN,
};
//获取RTCM数据有没有成功  0成功  1失败
enum  eMe3630GetRtcm
{
	eGetRtcmY=0,
	eGetRtcmN,
};
#define MAXTCPPAKNUM  1024
typedef  struct 
{

	uint32_t	call_timeout:1;  //打开协议0超时 1没有超时 eMe3630CTimeoutStatus
	uint32_t	tcp_timeout:1;  //tcp链接协议0超时 1没有超时 eMe3630CTimeoutStatus
	uint32_t	tcp_link:2;  //0链接成功 1没有链接成功 2正在链接 eMe3630TcpLinkStatus
	uint32_t	send_commd:2; //0成功  1失败   2正在发送  eMe3630CommandStatus
	uint32_t	on:1; //0表示模块开机准备就绪   1表示还没有就绪
	uint32_t	receive:1;// 0接收数据满 1 表示数据没有接收满    
	uint32_t	read:1;// 0 表示读数据没有读完    1表示读到接收到的最大值
	uint32_t	send_data:2;//数据发送状态记录  0成功  1失败   2正在发送  eMe3630DataStatus
	uint32_t	get_rtcm:1;//获取RTCM数据有没有成功  0成功  1失败
	uint32_t	:(32-12);

}stMe3630StatusBit;

typedef union{
	uint32_t   data;
	stMe3630StatusBit bit;
}stMe3630Status;
extern stMe3630Status  Me3630Status;
/*
记录4G模块的相关状态 end
*/

/*
命令状态列表end
**************************************************************/
/*
api标识码
编码	接口		注释
1	接受请求响应	用于接受到数据后，反馈给数据发送方的确认收到消息包
11	GGA解析			本地向服务器发送GGA数据:解析gga字符串，千寻服务
12	基站信息上传	上传基站的状态信息：上传基站相关数据
*/
typedef enum  eMe3630ApiLogo
{
	eApiLogoRequestResponse=1,//用于接受到数据后，反馈给数据发送方的确认收到消息包
	eApiLogoGGA=5,//本地向服务器发送GGA数据:解析gga字符串，千寻服务
	eApiLogoSysStatus=12,//上传基站的状态信息：上传基站相关数据
}eMe3630ApiLogo;
typedef enum  eMe3630DataOutWay//数据序列化方式
{
	eDataOutWayMsgpack=1,
	eDataOutWayJson=2,
}eMe3630DataOutWay;
/*GGA反馈的数据状态*/
typedef enum  eMe3630ReGgaStatus//向服务器发送数据后得到的GGA的状态
{
	ReGgaStatus_Y=1014,//正确
	ReGgaStatusL_N=0,//本地错误
	ReGgaStatusQX_N=1002,//千寻服务器的错误代码
}eMe3630ReGgaStatus;

/*
GGA的数据结构定义
*/

#pragma pack(1) 
typedef  struct{
	char data[400];
	int length;
	int ret;
	
}stMe3630reGgaData;
#pragma pack()
/*
定义发送的数数据结构
*/

#pragma pack(1) 
typedef  struct 
{
	uint32_t	 ApiLogo;			//api标识码：（由服务器给出API接口列表）
	uint32_t	RequestID;			//请求ID，（基站请求和发送个服务器的数据ID号：使用这个ID号来作为我本地数据的请求的标示）
	uint32_t 	DataLen;			//数据长度(不包含数据头)
	uint32_t	DataOutWay;			//数据序列化方式 1 msgpack 序列化 2Json
	uint32_t 	PackageInsideNum;	//数据在分包内的序列，从0开始
	uint32_t	PackageNum;			//数据的分包数量，最小为1
}stMe3630SendHead;
#pragma pack()
/*
定义发送的数数据结构end
*/
/*
me3630数据发送
data:发送数据
len：发送的数据长度
status：数据发送的状态
*/
extern void me3630_SendData(char *data,uint32_t len,stMe3630Status *status);
/*
发送指定长度的数据
*/
extern void me3630_SendDataBytes(char*data ,u16 len);
/*
外部函数接口
*/
/*
登录TCP服务端
*/
extern void openTCP(stMe3630Status *status);
/*
接收到的数据（解包）
*/
extern void  me3630RxUnpacking(stMe3630Status *Status);
/*
包数据提取（拆包）
\r\n+ZIPRECV: 1,120.76.224.208,60000,180,
返回数据长度
*/
extern uint32_t  me3630RxDataExtraction(u8 *src,u8 **des);
/*
	发送me_3630的配置命令
*/
extern void me3630_SendBytes(char *command);
//对发送的循环配置
extern void me3630SendCtr(stMe3630Ctr *ctr,uint8_t cycle_nunm, uint16_t time_len ,uint8_t cycle_always);
/*
获取4G信号强度
data :反馈的信号强度数据
status:状态反馈
命令反馈的数据
AT+CSQ 
+CSQ: 17,99 
 
OK 

*/

extern void Me3630GetSignal(char *data,stMe3630Status *status );

//初始化测试函数
extern void me3630TestDataInit(stMe3630SendHead *data);
//初始状态表示
extern void Me3630StatusInit(stMe3630Status *Me3630Status);
/*
4G模块测试韩式
*/
extern void me3630_test(void);
#endif
