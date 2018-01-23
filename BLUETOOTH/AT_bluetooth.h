#ifndef __AT_BLUETOOTH_H__
#define __AT_BLUETOOTH_H__
#include "bluetooth.h"
//命令回显结束符
/*
0a------换行符号－－－－－－"\n"
0d------回车符号－－－－－－"\r"
0D 0A 
*/
#define BL_SEND_COMMAND_ECHO_1	'\r'
#define BL_SEND_COMMAND_ECHO_2	'\n'

//命令符号 equal
#define	BL_EQUAL	"="
#define	BL_COMMAND_END	"\0"
//模式切换命令
#define	BL_MODE		"+++"			//模式切换指令
#define	BL_ADVON	"AT+ADVON"		//打开模块广播功能
#define	BL_ADVOFF	"AT+ADVOFF"		//关闭模块广播功能
#define	BL_DISCONN	"AT+DISCONN"	//断开已连接的设备
#define	BL_RESET	"AT+RESET"		//复位指令
#define	BL_FACTORY	"AT+FACTORY"	//恢复出厂设置
//只做查询的命令
#define	BL_VER		"AT+VER"		//查询版本号
#define	BL_MAC		"AT+MAC"		//查询模块 MAC地址
#define	BL_SCAN		"AT+SCAN"		//扫描当前在广播的设备
//查询和设置命令
#define	BL_NAME		"AT+NAME"		//查询设置模块 NAME
#define	BL_UART		"AT+UART"		//查询设置串口波特率
#define	BL_ROLE		"AT+ROLE"		//查询设置模块角色
#define	BL_TXPWR	"AT+TXPWR"		//查询设置模块发射功率
#define	BL_UUID		"AT+UUID"		//查询设置设备 UUID
#define	BL_ADVDATA	"AT+ADVDATA"	//查询设置广播数据


#define	BL_ADVIN	"AT+ADVIN"		//查询设置模块广播间隙
#define	BL_CONIN	"AT+CONIN"		//查询设置模块连接间隙

//不常用命令
#define	BL_CONNECT	"AT+CONNECT"	//指定连接设备
#define	BL_IBEACON	"AT+IBEACON"	//查询设置 IBEACON 数据
#define	BL_RSSI		"AT+RSSI"		//获取已连接设备 RSSI
//用于命令的拼接时拼接成查询模式还是设置模式的命令
typedef enum  eBlCommandMode
{
	eBlCommandInquire=0, //查询模式
	eBlCommandSetUp,	//设置模式
}eBlCommandMode;

//查询设置模块角色
 enum  eBlCommandRole
{
	eBlCommandOrdinary=0, //设置当前模块角色为普通模式
	eBlCommandMulti,//设置当前模块角色为多角色模式
};
#define   CMD_LEN_MAX         10
#define   COMP_LEN_MAX         20   //
#define   COMP_TYPE_MAX        2 //查询和设置两个类型需要校验
#define   CMD_NUM_MAX              20
typedef  void (*SendCommadFunType)(char * ,char* ,eBlCommandMode );
typedef struct  cmd_ctrl_struct_base
{
	SendCommadFunType  pFunc;
	char     CmdStr[CMD_LEN_MAX];          //命令
	char     CompareStr[2][COMP_LEN_MAX];  //校验的字符串
	unsigned short DelayTime;           //接收延时时间
}stCmdCtrlBaseType;

typedef struct  cmd_ctrl_struct
{
	 
 	stCmdCtrlBaseType stCmdCtrlBase[CMD_NUM_MAX] ;
	unsigned char     CmdNum;
}stCmdCtrlType;

/*函数接口*/
/*查询设置模块 NAME
执行指 令 AT+NAME=CdEbyte_MultiRole 设置当 前模块名 字，不超 过 20 个字符 。成功后 返回 PARA SET:
CdEbyte_MultiRole。
*/
extern void AtBlueNameCommand(char *name ,char* return_command,eBlCommandMode CommandMode);
/*查询设置串口波特率
执行指令 AT+UART=115200,8,1,0 设置当前模块名字，成功后返回 PARA SET: 115200,8,1,0。
第一个数据代表波特率：1200，2400，4800，9600，19200，38400，57600，115200，128000，230400，256000，
460800，921600；
第二个数据代表数据位：5,6,7,8；
第三个数据代表停止位：0,1；
第四个数据代表校验位：0(none)，1(even)，2(odd))。
*/
extern void AtBlueUartCommand(char * baud,char* return_command,eBlCommandMode CommandMode);
/*查询设置模块角色
执行指令 AT+ROLE=0 设置当前模块角色为普通模式，成功后返回 PARA SET: Normal；
执行指令 AT+ROLE=1 设置当前模块角色为多角色模式，成功后返回 PARA SET: MultiRole。
*/
extern void AtBlueRoleCommand(char * role,char* return_command,eBlCommandMode CommandMode);
/*查询设置模块发射功率
执行指令 AT+TXPWR=2dbm 设置当前模块发射功率，成功后返回 PARA SET: AT+TXPWR=2dbm。
其中，设置的功率范围为：2dbm，1dbm，0dbm，-3dbm，-6dbm，-9dbm，-12dbm，-15dbm，-18dbm，-21dbm。
*/
extern void AtBlueRoleTxpwr(char * txpwr,char* return_command,eBlCommandMode CommandMode);
/*查询设置设备 UUID
执行指令 AT+UUID=0,FFF0 设置当前模块名字，成功后返回 PARA SET: AT+UUID=0,FFF0。
第一个参数设置第几个 UUID，其范围 0 ~ 3；param =num+uuid 例如"0,FFF0"*/

extern void AtBlueRoleUuid(char *param,char* return_command,eBlCommandMode CommandMode);
/*查询设置广播数据 ADVDATA
执行 AT+ADVDATA=0A00010203040506070809 后，模块将返回 PARA SET: 0x0A00010203040506070809。
其中上面的参数 0A 代表要输入的数据长度，00010203040506070809 代表徐要输入的数据。输入数据按十六进制格式输
入，如 00 代表 0x00，01 代表 0x01，02 代表 0x02。数据长度不超过 23 字节。
*/
extern void AtBlueAdvdataUuid(char * advdata,char* return_command,eBlCommandMode CommandMode);



/*
发送蓝牙设置命令
uuid的配置需要单独的函数
void(*StringCommand)(char * ,char* ,eBlCommandMode ) //调用打包命令的函数  

data：要发送的数据命令
*/
extern void  BlSendSetUpCommand(void(*StringCommand)(char * ,char* ,eBlCommandMode ),char * data);
/*
发送UUID配置命令
num_uuid:第几个uuid
uuid:uuid号
*/
extern void  BlSendSetUpUuid(char * num_uuid,char *uuid);
/*
发送蓝牙查询命令
*/
extern void BlSendInquireCommand(void(*StringCommand)(char * ,char* ,eBlCommandMode ));
/*
一些没有参数的配置命令：一些开关命令
*/
extern void BlSendSwitchCommand(char *command);
/*
发送一个命令等待状态正确
void(*StringCommand)(char * ,char* ,eBlCommandMode ):命令函数
data：命令的数据
CompareEcho：传入比较的真正的数据
status：比较的状态
*/
extern void BlSendSetUpCommandEcho(void(*StringCommand)(char * ,char* ,eBlCommandMode ),eBlCommandMode eMode,char * data ,stBlStatus  *status);

 


#endif

