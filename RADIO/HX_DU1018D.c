/*
���ŵ�̨��ش���
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
��̨�������̣�
1���������ŵ�ƽ���ͻ�+++���ظ���NO CARRIER OK��
2����������
3��AT&W ��AT&W
4�������������ߣ����߷�ATA�˳�����ģʽ
*/
/*
���ų�ʼ��ʧ��
*/
static void Du1018dGpioInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOC_CLK_ENABLE();           		//����GPIOCʱ��
	GPIO_Initure.Pin=DU1018D_MODE_PIN; 			
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		
	GPIO_Initure.Pull=GPIO_PULLUP;          	//����
	GPIO_Initure.Speed=GPIO_SPEED_LOW;     	//����
	HAL_GPIO_Init(DU1018D_MODE_GPIO,&GPIO_Initure);
	

//	while(1)
//	{	
//		DU1018D_MODE(eDU1018D_MODE_Configuration); //��������ģʽ
//		DU1018D_MODE(eDU1018D_MODE_Ordinary); //�˳�����ģʽ
//		DU1018D_MODE(eDU1018D_MODE_Configuration); //��������ģʽ
//		DU1018D_MODE(eDU1018D_MODE_Ordinary); //�˳�����ģʽ
//	}
}
static void Du1018dStatusInit(stDu1018dStatus *Status)
{
	Status->data=~0;

}

/*
��̨�����ʺ�MCU�Ĵ��ڲ�����ƥ��
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
������Լ��
Du1018dStatus:��¼����ķ���״̬
*/
void Du1018dCommandEchoStatus(stDu1018dStatus *Du1018dStatus)
{
#if 0
	static int num=0;//��Ƿ�������󷵻ص�״̬�ĶԱȽ�
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
	static int num=0;//��Ƿ�������󷵻ص�״̬�ĶԱȽ�
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
�����������һ������ȴ����Է��ͳɹ�������ɹ��ط�
command:ƴ�ӵ�����
parameter������Ĳ���
parameterY_N����û�в����ı��
Du1018dStatus:��������Ƿ��ͳɹ�
*/
void Du1018dSendCommand(char * command ,char *parameter,eAtDu1018dCommandParameter parameterY_N,stDu1018dStatus *Du1018dStatus)
{
	char CommandBuf[HX_COMMAND_MAX_LEN];
	memset((char*)CommandBuf,0,HX_COMMAND_MAX_LEN);
	AtDu1018dCommand( command ,parameter,parameterY_N,CommandBuf);
	DU1018D_MODE(eDU1018D_MODE_Configuration); //��������ģʽ
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
	DU1018D_MODE(eDU1018D_MODE_Ordinary); //�˳�����ģʽ
#if DEBUG_HX_DU1018D
	printf("Du1018d Command send :%s:OK\r\n",CommandBuf);
#endif		
}
/*
����Ƶ�ʱ����� 
*/
void Du1018dSendFrequencyTable(char **Table ,stDu1018dStatus *Du1018dStatus)
{
	u8 num=0;
	DU1018D_MODE(eDU1018D_MODE_Configuration); //��������ģʽ
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
	Du1018dSendCommand(AT_W ,0,eAtDu1018dCommandParameterN,Du1018dStatus);//�������е�����
	DU1018D_MODE(eDU1018D_MODE_Ordinary); //�˳�����ģʽ
#if DEBUG_HX_DU1018D
	printf("Du1018d Command send :Frequency Table:OK\r\n");
#endif		
}
/*
����ͨ����

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
���ص��ַ���������ͨ����
*/
void Du1018dChannelExtract(u8 *channel)
{
	static int num=0;//��Ƿ�������󷵻ص�״̬�ĶԱȽ�
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
����̨ͨ����
channel:���ص�ͨ����
*/

void Du1018dReadChannel(u8 *channel,stDu1018dStatus *Du1018dStatus)
{
	char CommandBuf[HX_COMMAND_MAX_LEN];
	memset((char*)CommandBuf,0,HX_COMMAND_MAX_LEN);
	AtDu1018dCommand( "ATS131?\r\n" ,0,eAtDu1018dCommandParameterN,CommandBuf);
	DU1018D_MODE(eDU1018D_MODE_Configuration); //��������ģʽ
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

	DU1018D_MODE(eDU1018D_MODE_Ordinary); //�˳�����ģʽ
#if DEBUG_HX_DU1018D
	printf("Du1018d Command send :%s:OK\r\n",CommandBuf);
#endif		
}
/*
��̨���ó�ʼ����
*/

void  Du1018dConfigurationInit(stDu1018dStatus *Status)
{
	Du1018dGpioInit();
	Du1018dStatusInit(Status);
}
