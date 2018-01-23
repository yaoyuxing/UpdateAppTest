#ifndef __HX_DU1018D_AT_H_
#define __HX_DU1018D_AT_H_

//命令的通用配置
#define	HX_COMMAND_END "\r\n"
#define	HX_COMMAND_MAX_LEN  50


//第一类命令：特殊AT命令 

#define AT_W "AT&W"//参数保存命令  发送命令：AT&W+回车换行 设置成功返回”OK” 
#define  FREQUENCY_TABLE_MAX_NUM 7
extern char pDU1018DFrequencyTable[FREQUENCY_TABLE_MAX_NUM][sizeof("ATP0=00 450.125 450.125\r\n")];//400M定频  工作频率表配置 
#define AT_V "AT&V"//当前电台工作参数显示 发送命令：AT&V+回车换行  设置成功返回”OK” 
//第二类命令：普通AT命令配置 
/*
默认配置
ATS102=1-115200;
ATS103=2-9600
ATS108=NO
ATS131=NO
ATS132=NO
*/
#define AT_SerialBaud 		"ATS102="	//串口波特率修改 发送命令：ATS102=value+回车换行 1-115200；2-57600；3-38400；5-19200；7-9600； 
#define AT_AeriallBaud 		"ATS103="	//空中波特率修改 发送命令：ATS103=value+回车换行 窄带模式：0-4800;1-8000;2-9600;3-16000;4-19200;
#define AT_TransmitPower 	"ATS108="	//发射功率修改 发送命令：ATS108=value+回车换行 H:高功率；L：低功率；
#define AT_TXChannelSettings "ATS131="	//当前电台工作发送通道设置  value取值范围 0-63 
#define AT_RXChannelSettings "ATS132="	//当前电台工作接收通道设置  value取值范围 0-63 
#define AT_Communication	 "ATS186="	//通信协议选择 1-TRIMTALK; 2-TRIMMK3; 4-TT450S; 5-TRANSEOT 

//串口波特率修改
typedef enum  eUsartBaud102
{
	eSerialBaud_115200 = '1',
	eSerialBaud_57600,
	eSerialBaud_38400,
	eSerialBaud_28800,
	eSerialBaud_19200,
	eSerialBaud_14400,
	eSerialBaud_9600,

}eUsartBaud102;
//空中波特率修改
typedef enum  eAeriallBaud103
{
	eAeriallBaud_4800 = '0',
	eAeriallBaud_8000,
	eAeriallBaud_9600,
	eAeriallBaud_16000,
	eAeriallBaud_19200,
}eAeriallBaud103;

//发射功率修改 发送命令
typedef enum  eTransmitPower108
{
	eHighPower = 'H',
	eLowPower = 'L',
}eTransmitPower108;


//通信协议选择
typedef enum  eCommunication186
{
	eTRIMTALK='1',
	eTRIMMK3,
	eTT450S='4',
	eTRANSEOT,
}eCommunication186;
//命令有没有参数typedef
typedef enum  eAtDu1018dCommandParameter
{
	eAtDu1018dCommandParameterY = 0,//有参数
	eAtDu1018dCommandParameterN ,
}eAtDu1018dCommandParameter;
/*
命令拼接：
command:拼接的命令
parameter：命令的参数
parameterY_N：有没有参数的标记
return_command：命令返回
*/
extern void AtDu1018dCommand(char * command ,char *parameter,eAtDu1018dCommandParameter parameterY_N,char *return_command);
#endif 

