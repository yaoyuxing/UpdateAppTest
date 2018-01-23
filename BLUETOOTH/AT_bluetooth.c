//蓝牙的AT指令设置

#include "AT_bluetooth.h"
#include "bluetooth.h"
#include "string.h"
#include "sys.h"
#include "delay.h"
#include "debug.h"
#include "task.h"

#define    CMD_INQUIRE_STR_COMP_LEN  (2)
#define    CMD_SET_STR_COMP_LEN      (9)
 stCmdCtrlType  gstCmdCtrl ={{ 
	                      NULL,BL_MODE,{"OK","PARA SET:"},200, //初始化命令控制结构
                        NULL,BL_ADVON,{"OK","PARA SET:"},1000,
	                      NULL,BL_ADVOFF,{"OK","PARA SET:"},1000,
	                      NULL,BL_DISCONN,{"OK","PARA SET:"},1000,
	                      NULL,BL_RESET,{"OK","PARA SET:"},1000,
	                      NULL,BL_FACTORY,{"OK","PARA SET:"},1000,
	                      NULL,BL_VER,{"OK","PARA SET:"},200,
	                      NULL,BL_MAC,{"OK","PARA SET:"},200,
	                      NULL,BL_SCAN,{"OK","PARA SET:"},1000,
	                      AtBlueNameCommand,BL_NAME,{"OK","PARA SET:"},1000,
	                      AtBlueUartCommand,BL_UART,{"OK","PARA SET:"},1000,
	                      AtBlueRoleCommand,BL_ROLE,{"OK","PARA SET:"},1000,
	                      AtBlueRoleTxpwr,BL_TXPWR,{"OK","PARA SET:"},200,
	                      AtBlueRoleUuid,BL_UUID,{"OK","PARA SET:"},1000,
	                      AtBlueAdvdataUuid,BL_ADVDATA,{"OK","PARA SET:"},1000,
	                      NULL,BL_ADVIN,{"OK","PARA SET:"},1000,
	                      NULL,BL_CONIN,{"OK","PARA SET:"},1000,
	                      NULL,BL_CONNECT,{"OK","PARA SET:"},1000,
	                      NULL,BL_IBEACON,{"OK","PARA SET:"},1000,
	                      NULL,BL_RSSI,{"OK","PARA SET:"},1000,  
											},0xff};        //CmdNum设置为无效

/***带有查询和设置的AT命令**************************************************************************/

/*查询设置模块 NAME
执行指 令 AT+NAME=CdEbyte_MultiRole 设置当 前模块名 字，不超 过 20 个字符 。成功后 返回 PARA SET:
CdEbyte_MultiRole。
*/

void AtBlueNameCommand(char *name ,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://查询
			p_return=strcat (p_return,BL_NAME);
		
			break;
		case   eBlCommandSetUp://设置
			p_return=strcat (p_return,BL_NAME);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
			p_return=strncat(p_return,name,strlen(name));
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}

/*查询设置串口波特率
执行指令 AT+UART=19200,8,1,0 设置当前模块名字，成功后返回 PARA SET: 115200,8,1,0。
第一个数据代表波特率：1200，2400，4800，9600，19200，38400，57600，115200，128000，230400，256000，
460800，921600；
第二个数据代表数据位：5,6,7,8；
第三个数据代表停止位：0,1；
第四个数据代表校验位：0(none)，1(even)，2(odd))。
*/

void AtBlueUartCommand(char * baud,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://查询
			p_return=strcat (p_return,BL_UART);
			break;
		case   eBlCommandSetUp://设置
			p_return=strcat (p_return,BL_UART);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
		
			p_return=strncat(p_return,baud,strlen(baud));
			p_return=strncat(p_return,",",1);
			p_return=strncat(p_return,"8",1);
			p_return=strncat(p_return,",",1);
			p_return=strncat(p_return,"1",1);
			p_return=strncat(p_return,",",1);
			p_return=strncat(p_return,"0",1);
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}
/*查询设置模块角色
执行指令 AT+ROLE=0 设置当前模块角色为普通模式，成功后返回 PARA SET: Normal；
执行指令 AT+ROLE=1 设置当前模块角色为多角色模式，成功后返回 PARA SET: MultiRole。
*/

void AtBlueRoleCommand(char * role,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://查询
			p_return=strcat (p_return,BL_ROLE);
			break;
		case   eBlCommandSetUp://设置
			p_return=strcat (p_return,BL_ROLE);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
			p_return=strncat(p_return,role,strlen(role));
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}
/*查询设置模块发射功率
执行指令 AT+TXPWR=2dbm 设置当前模块发射功率，成功后返回 PARA SET: AT+TXPWR=2dbm。
其中，设置的功率范围为：2dbm，1dbm，0dbm，-3dbm，-6dbm，-9dbm，-12dbm，-15dbm，-18dbm，-21dbm。
*/

void AtBlueRoleTxpwr(char * txpwr,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://查询
			p_return=strcat (p_return,BL_TXPWR);
			break;
		case   eBlCommandSetUp://设置
			p_return=strcat (p_return,BL_TXPWR);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
		
			p_return=strncat(p_return,txpwr,strlen(txpwr));
			p_return=strcat (p_return,"dbm");
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}
/*查询设置设备 UUID
执行指令 AT+UUID=0,FFF0 设置当前模块名字，成功后返回 PARA SET: AT+UUID=0,FFF0。
第一个参数设置第几个 UUID，其范围 0 ~ 3；*/

void AtBlueRoleUuid(char *param,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://查询
			p_return=strcat (p_return,BL_UUID);
			break;
		case   eBlCommandSetUp://设置
			p_return=strcat (p_return,BL_UUID);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
		
			p_return=strncat(p_return,param,1);
			p_return=strncat(p_return,",",1);
			p_return=strncat(p_return,(char *)&param[2],strlen(param)-2);
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}
/*查询设置广播数据 ADVDATA
执行 AT+ADVDATA=0A00010203040506070809 后，模块将返回 PARA SET: 0x0A00010203040506070809。
其中上面的参数 0A 代表要输入的数据长度，00010203040506070809 代表徐要输入的数据。输入数据按十六进制格式输
入，如 00 代表 0x00，01 代表 0x01，02 代表 0x02。数据长度不超过 23 字节。
*/
//10进制转换16进制
void DecimalConversionHexadecimal(u16 num ,char *data)
{
	u16 remainder=0;
	char i=0;
	char hex[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	while(num>0) 
	{ 
		remainder=num%16; 
		data[i++]=hex[remainder]; 
		num=num/16; 

	} 
	data[i]='\0';
}
//advdata不能大于23个字节的字符表示的16进制
void AtBlueAdvdataUuid(char * advdata,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	char num=0;
	char len_buf[2];
	switch(CommandMode)
	{
		case   eBlCommandInquire://查询
			p_return=strcat (p_return,BL_ADVDATA);
			break;
		case   eBlCommandSetUp://设置
			p_return=strcat (p_return,BL_ADVDATA);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
			num=strlen(advdata)/2;
			DecimalConversionHexadecimal(num ,len_buf);
			p_return=strncat(p_return,len_buf,strlen(len_buf));
		
			p_return=strncat(p_return,advdata,strlen(advdata));
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}

/*
发送蓝牙设置命令
uuid的配置需要单独的函数
void(*StringCommand)(char * ,char* ,eBlCommandMode ) //调用打包命令的函数  

data：要发送的数据命令
*/
#define MAX_BL_AT_LEN 80 //命令的最大长度
void  BlSendSetUpCommand(void(*StringCommand)(char * ,char* ,eBlCommandMode ),char * data)
{
	char commandBuf[MAX_BL_AT_LEN];
	memset(commandBuf,0,MAX_BL_AT_LEN*sizeof(char)); 
	StringCommand(data,commandBuf,eBlCommandSetUp); //拼接命令字符串
	BlSendCommand(commandBuf);//发送配置命令

}
void  BlSendInquireCommand(void(*StringCommand)(char * ,char* ,eBlCommandMode ))
{
	char commandBuf[MAX_BL_AT_LEN];
	memset(commandBuf,0,MAX_BL_AT_LEN*sizeof(char)); 
	StringCommand("",commandBuf,eBlCommandInquire); //拼接命令字符串
	BlSendCommand(commandBuf);//发送配置命令
}
void ScanForCmd(void(*StringCommand)(char * ,char* ,eBlCommandMode ))
{
	unsigned int  cnt=0;
	for(cnt=0;cnt<CMD_NUM_MAX;cnt++)
	{
		if(gstCmdCtrl.stCmdCtrlBase[cnt].pFunc==StringCommand)
		{
			gstCmdCtrl.CmdNum=cnt;
			break;
		}
	}
}
/*
发送一个命令等待状态正确
void(*StringCommand)(char * ,char* ,eBlCommandMode ):命令函数
data：命令的数据
CompareEcho：传入比较的真正的数据
status：比较的状态
 
*/
#define   SEND_CMD_TIMEOUT  10000 //10s超时
void BlSendSetUpCommandEcho(void(*StringCommand)(char * ,char* ,eBlCommandMode ),eBlCommandMode eMode,char * data ,stBlStatus  *status)
{
	 u32  timeout=SEND_CMD_TIMEOUT;
	 char *pCompData=NULL;
	u8  compnum=0;
	if(status->bit.rx_buff_analysis!=eBlRxBuffAnalysisIng)//判断数据有没有解析完
	{ 
		status->bit.command_send_status=eBlCommandSendN;
		do{
			BlRxDatabuffClear();
			BlCommandEchoBuffClear();
			ScanForCmd(StringCommand);
			if(eMode==eBlCommandInquire)
			BlSendInquireCommand(StringCommand);
			else if(eMode==eBlCommandSetUp)
		 	BlSendSetUpCommand(StringCommand,data);//发送命令
			//BlSendCommand("AT+NAME=SkyLin cane\r\n");
			pCompData=(char *)&gstCmdCtrl.stCmdCtrlBase[gstCmdCtrl.CmdNum].CompareStr[eMode][0];
			compnum=eMode?CMD_SET_STR_COMP_LEN:CMD_INQUIRE_STR_COMP_LEN;
			delay_ms(gstCmdCtrl.stCmdCtrlBase[gstCmdCtrl.CmdNum].DelayTime);
			BlCommandReturnAnalysis((char*)gCommandEchoBuff,status);
			if(memcmp(gCommandEchoBuff,pCompData,compnum)==0)//数据相等
			{
				status->bit.command_send_status=eBlCommandSendY;
				#if DEBUG_BLUE				
					printf("Command Send  success\r\n");
				#endif			
			}else{
				status->bit.command_send_status=eBlCommandSendN;
				#if DEBUG_BLUE				
					printf("Command Send 	failure\r\n");
				#endif		
			}
		}while((status->bit.command_send_status!=eBlCommandSendY)&&(timeout--));
	}else{
		delay_ms(10);
	}
}
/*
发送UUID配置命令
num_uuid:第几个uuid
uuid:uuid号
*/
void  BlSendSetUpUuid(char * num_uuid,char *uuid)
{
	char commandBuf[MAX_BL_AT_LEN],  param[10]; 
	memset(commandBuf,0,MAX_BL_AT_LEN*sizeof(char));
  strncat(param,num_uuid,1);	
	strncat(param,",",1);
	strncat(param,uuid,strlen(uuid));
	AtBlueRoleUuid(param,commandBuf,eBlCommandSetUp);
	BlSendCommand(commandBuf);//发送配置命令

}
/*
一些没有参数的配置命令：一些开关命令
*/
void BlSendSwitchCommand(char *command)
{
	BlSendCommand(command);//发送配置命令
}
///*
//发送蓝牙状态查询命令
//*/
//void BlSendInquireCommand(char *command)
//{
//	BlSendCommand(command);//发送配置命令
//}






