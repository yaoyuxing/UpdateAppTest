/*
华信电台相关代码
*/
#include "sys.h"
#include "HX_DU1018D.h"
#include "HX_DU1018D_AT.h"
#include "string.h"
#include "delay.h"
#include "MySys.h"
#include "debug.h"
stDu1018dStatus  gDu1018dStatus ;

/*
电台配置流程：
1：配置引脚电平拉低或发+++：回复“NO CARRIER OK”
2：配置命令
3：AT&W ：AT&W
4：配置引脚拉高：或者发ATA退出配置模式
*/
/*
引脚初始化失败
*/
static void Du1018dGpioInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOC_CLK_ENABLE();           		//开启GPIOC时钟
	GPIO_Initure.Pin=DU1018D_MODE_PIN; 			
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		
	GPIO_Initure.Pull=GPIO_PULLUP;          	//上拉
	GPIO_Initure.Speed=GPIO_SPEED_LOW;     	//高速
	HAL_GPIO_Init(DU1018D_MODE_GPIO,&GPIO_Initure);
	

//	while(1)
//	{	
//		DU1018D_MODE(eDU1018D_MODE_Configuration); //开启配置模式
//		DU1018D_MODE(eDU1018D_MODE_Ordinary); //退出配置模式
//		DU1018D_MODE(eDU1018D_MODE_Configuration); //开启配置模式
//		DU1018D_MODE(eDU1018D_MODE_Ordinary); //退出配置模式
//	}
}
static void Du1018dStatusInit(stDu1018dStatus *Status)
{
	Status->data=~0;

}

/*
电台波特率和MCU的串口波特率匹配
*/
void Du1018dAndUsartMatch(eUsartBaud102 UsartBaud)
{
	switch(UsartBaud)
	{
		case	eSerialBaud_115200	:
				Du1018dUartBound(115200);
				break;
		case	eSerialBaud_57600	:
				Du1018dUartBound(57600	);
				break;
		case	eSerialBaud_38400	:
				Du1018dUartBound(38400	);
				break;
		case	eSerialBaud_28800	:
				Du1018dUartBound(28800	);
				break;
		case	eSerialBaud_19200	:
				Du1018dUartBound(19200	);
				break;
		case	eSerialBaud_14400	:
				Du1018dUartBound(14400	);
				break;
		case	eSerialBaud_9600	:
				Du1018dUartBound(9600	);
				break;
		default: 
				Du1018dUartBound(9600	);
				break;
	}

}


/*
命令回显检测
Du1018dStatus:记录命令的发送状态
*/
void Du1018dCommandEchoStatus(stDu1018dStatus *Du1018dStatus)
{
#if 0
	static int num=0;//标记发送命令后返回的状态的对比较
	if(Du1018d_RX_BUF_NUM!=0)
	{
		for(num=0;num<Du1018d_RX_BUF_NUM;num++)
		{
			if((Du1018d_RX_BUF[num]=='\r')&&(Du1018d_RX_BUF[num+1]=='\n'))
			{
				Du1018dStatus->bit.CommandSend=eDU1018D_CommandSendY;
#if DEBUG_HX_DU1018D
				printf("Du1018d Command send OK\r\n");
#endif	
			}
		}
		if(Du1018dStatus->bit.CommandSend!=eDU1018D_CommandSendY)
		{
			Du1018dStatus->bit.CommandSend=eDU1018D_CommandSendN;
#if DEBUG_HX_DU1018D
				printf("Du1018d Command send failure\r\n");
#endif	
		}
		Du1018d_RX_BUF_NUM=0;
	}	
#endif
	
#if 1	
	static int num=0;//标记发送命令后返回的状态的对比较
	if(Du1018d_RX_BUF_NUM!=0)
	{
		for(num=0;num<Du1018d_RX_BUF_NUM;num++)
		{
			if((Du1018d_RX_BUF[num]=='O')&&(Du1018d_RX_BUF[num+1]=='K'))
			{
				Du1018dStatus->bit.CommandSend=eDU1018D_CommandSendY;
#if DEBUG_HX_DU1018D
				printf("Du1018d Command send OK\r\n");
#endif	
			}
		}
		if(Du1018dStatus->bit.CommandSend!=eDU1018D_CommandSendY)
		{
			Du1018dStatus->bit.CommandSend=eDU1018D_CommandSendN;
#if DEBUG_HX_DU1018D
				printf("Du1018d Command send failure\r\n");
#endif	
		}
		Du1018d_RX_BUF_NUM=0;
	}
#endif	
}
/*
发送命令：发送一条命令等待回显发送成功如果不成功重发
command:拼接的命令
parameter：命令的参数
parameterY_N：有没有参数的标记
Du1018dStatus:标记命令是否发送成功
*/
void Du1018dSendCommand(char * command ,char *parameter,eAtDu1018dCommandParameter parameterY_N,stDu1018dStatus *Du1018dStatus)
{
	char CommandBuf[HX_COMMAND_MAX_LEN];
	memset((char*)CommandBuf,0,HX_COMMAND_MAX_LEN);
	AtDu1018dCommand( command ,parameter,parameterY_N,CommandBuf);
	DU1018D_MODE(eDU1018D_MODE_Configuration); //开启配置模式
	delay_ms(500);
	do{
		Du1018d_RX_BUF_NUM=0;
		memset((char*)Du1018d_RX_BUF,0,Du1018d_RX_BUF_LEN);
		Du1018dSendByteS(CommandBuf);
		delay_ms(100);
		Du1018dCommandEchoStatus(Du1018dStatus);
#if DEBUG_HX_DU1018D
		if(Du1018dStatus->bit.CommandSend==eDU1018D_CommandSendN)
			printf("Du1018d Command send :%s:failure\r\n",CommandBuf);
#endif		
	}while(Du1018dStatus->bit.CommandSend!=eDU1018D_CommandSendY);
	DU1018D_MODE(eDU1018D_MODE_Ordinary); //退出配置模式
#if DEBUG_HX_DU1018D
	printf("Du1018d Command send :%s:OK\r\n",CommandBuf);
#endif		
}
/*
工作频率表配置 
*/
void Du1018dSendFrequencyTable(char **Table ,stDu1018dStatus *Du1018dStatus)
{
	u8 num=0;
	DU1018D_MODE(eDU1018D_MODE_Configuration); //开启配置模式
	for(num=0;num<FREQUENCY_TABLE_MAX_NUM;num++)
	{
		do{
			Du1018dSendByteS(Table[num]);
			delay_ms(30);
			Du1018dCommandEchoStatus(Du1018dStatus);
			#if DEBUG_HX_DU1018D
				if(Du1018dStatus->bit.CommandSend==eDU1018D_CommandSendN)
					printf("Du1018d Command send :%s:failure\r\n",Table[num]);
			#endif	
		}while(Du1018dStatus->bit.CommandSend!=eDU1018D_CommandSendY);
	}
	Du1018dSendCommand(AT_W ,0,eAtDu1018dCommandParameterN,Du1018dStatus);//保存所有的配置
	DU1018D_MODE(eDU1018D_MODE_Ordinary); //退出配置模式
#if DEBUG_HX_DU1018D
	printf("Du1018d Command send :Frequency Table:OK\r\n");
#endif		
}
/*
设置通道：

*/
#include <stdlib.h>
void Du1018dSetChannel(u8 *channel,u8 data_len,stDu1018dStatus *Du1018dStatus)
{
	char * channel_buf=0;
	u8  read_channel=0,write_channel=0;
	
	channel_buf=(char *)malloc(data_len+1);
	memcpy(channel_buf,channel,data_len);
	*(channel_buf+data_len+1)='\0';
	write_channel=character_to_uint32((char*)channel ,data_len);
	do{
		Du1018dSendCommand(AT_TXChannelSettings ,channel_buf,eAtDu1018dCommandParameterY,Du1018dStatus);
		Du1018dReadChannel(&read_channel,Du1018dStatus);
		delay_ms(20);
	}while(read_channel!=write_channel);
	Du1018dStatus->bit.Channel=read_channel;
	free(channel_buf);
}
/*
返回的字符串中提起通道号
*/
void Du1018dChannelExtract(u8 *channel)
{
	static int num=0;//标记发送命令后返回的状态的对比较
	if(Du1018d_RX_BUF_NUM!=0)
	{
		for(num=0;num<Du1018d_RX_BUF_NUM;num++)
		{
			if(Du1018d_RX_BUF[num]=='?')
			{
				*channel=character_to_uint32((char*)(&Du1018d_RX_BUF[num+3]) ,2);
#if DEBUG_HX_DU1018D
				printf("Du1018d channel =%d\r\n",*channel);
#endif	
			}
		}

		Du1018d_RX_BUF_NUM=0;
	}

}
	
	

/*
读电台通道号
channel:返回的通道号
*/

void Du1018dReadChannel(u8 *channel,stDu1018dStatus *Du1018dStatus)
{
	char CommandBuf[HX_COMMAND_MAX_LEN];
	memset((char*)CommandBuf,0,HX_COMMAND_MAX_LEN);
	AtDu1018dCommand( "ATS131?\r\n" ,0,eAtDu1018dCommandParameterN,CommandBuf);
	DU1018D_MODE(eDU1018D_MODE_Configuration); //开启配置模式
	delay_ms(500);
	do{
		Du1018d_RX_BUF_NUM=0;
		memset((char*)Du1018d_RX_BUF,0,Du1018d_RX_BUF_LEN);
		Du1018dSendByteS(CommandBuf);
		delay_ms(100);
		Du1018dChannelExtract(channel);
		Du1018dCommandEchoStatus(Du1018dStatus);
#if DEBUG_HX_DU1018D
		if(Du1018dStatus->bit.CommandSend==eDU1018D_CommandSendN)
			printf("Du1018d Command send :%s:failure\r\n",CommandBuf);
#endif		
	}while(Du1018dStatus->bit.CommandSend!=eDU1018D_CommandSendY);

	DU1018D_MODE(eDU1018D_MODE_Ordinary); //退出配置模式
#if DEBUG_HX_DU1018D
	printf("Du1018d Command send :%s:OK\r\n",CommandBuf);
#endif		
}
/*
电台配置初始化：
*/

void  Du1018dConfigurationInit(stDu1018dStatus *Status)
{
	Du1018dGpioInit();
	Du1018dStatusInit(Status);
}
