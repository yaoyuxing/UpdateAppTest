/*
RTK配置流程

当启动RTK时，配置rtk的输出

1：输gpgga：用作千寻的信息获取
log com2 gpgga ontime 1  
2：输bestpos：用作提取纬度经度高度 （10进制的）
log com2 bestpos ontime 1  
3：配置com口的收发协议
interfacemode com2 rtcmv3 novatel on
4：设置进入基准站模式
fix position 纬度 经度 高度
5：还原到移动站
fix none

6：配置成：115200（默认9600）
com com2 115200
*/

/*
RTK com1 做命令配置口
RTK com2 做RTK数据输入输出口

*/
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "debug.h"
#include "rtk.h"
#include "MySys.h"
#include "me3630.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtk_gpio.h"
#define  CRT "\r\n\0"

stRtkStatus	gRtkStatus;

char gCommandBuf[MAX_RTK_COMMAND_BUF];
/*
	发送me_3630的配置命令
*/

 void rtk_SendBytes(char *command)
{
	int num=0;
	unsigned int len=0;
	char *p_comm=command;
	len=strlen((const char*)p_comm);
	for(num=0;num<len;num++)
	{
		rtkSendByte(*(p_comm+num));
	}
}
/*
发送指定长度的数据到RTK
*/
void rtk_SendNumByte(char *data ,uint16_t len)
{
	int num=0;
	for(num=0;num<len;num++)
	{
		rtkSendByte(*(data+num));
	}
	
}
/*
自校准基站模式命令
state = on off
maxtime:等待时间
maxhstd:水平误差
maxvstd：高度误差
*/

void RtkCommandPosave(char *state,char *maxtime,char *maxhstd ,char *maxvstd ,char* return_command)
{
	char* p_return =return_command;
	char space=' ';
	p_return=strcat (p_return,"posave ");
	if(strcmp(state,"on")==0)
	{
		p_return=strncat(p_return,state,strlen(state));
		p_return=strncat(p_return,&space,1);
			
		p_return=strncat(p_return,maxtime,strlen(maxtime));
		p_return=strncat(p_return,&space,1);
			
		p_return=strncat(p_return,maxhstd,strlen(maxhstd));
		p_return=strncat(p_return,&space,1);	
			
		p_return=strncat(p_return,maxvstd,strlen(maxvstd));
		p_return=strncat(p_return,&space,1);	
	
	}else{
		p_return=strncat(p_return,state,strlen(state));
		p_return=strncat(p_return,&space,1);
	}
	p_return=strncat(p_return,CRT,strlen(CRT));
}
/*
拼接LOG命令
GGA
bestpos
log com2 gpgga ontime 1
*/

void LogCommand(char *command,char *comx,char *delaytime ,char* return_command)
{
	char* p_return =return_command;
	char space=' ';
	p_return=strcat (p_return,"log ");
	p_return=strncat(p_return,comx,strlen(comx));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,command,strlen(command));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,"ontime",strlen("ontime"));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,delaytime,strlen(delaytime));
	p_return=strncat(p_return,CRT,strlen(CRT));
}

/*
设置进入基准站模式的命令拼接
fix position 纬度 经度 高度（这个经纬度高度是bestpos的）
纬度：latitude
经度：Accuracy
高度：height
*/

void FixPositionCommand(char *latitude,char *accuracy ,char* height,char* return_command)
{
	char* p_return =return_command;
	char space=' ';
	p_return=strcat (p_return,"fix position ");
	p_return=strncat(p_return,latitude,MAX_BESTPOS_DATA_LEN);
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,accuracy,MAX_BESTPOS_DATA_LEN);
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,height,MAX_BESTPOS_DATA_LEN);	
	p_return=strncat(p_return,CRT,strlen(CRT));

}
	
/*
设置进入基准站模式的命令拼接
fix none
*/

void FixNoneCommand(char* return_command)
{
	char* p_return =return_command;
	p_return=strcat (p_return,"fix none\r\0");
}
/*
com com2 115200
设置COM的波特率
*/
void RtkComBaudCommand(char * comx,char *baud,char* return_command)
{
	char* p_return =return_command;
	char space=' ';
	p_return=strcat (p_return,"com ");
	p_return=strncat(p_return,comx,strlen(comx));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,baud,strlen(baud));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,CRT,strlen(CRT));
}


/*
配置com口的收发协议
interfacemode com2 rtcmv3 novatel on
INTERFACEMODE [port] rxtype txtype [responses]
*/
void RtkInterfaceCommand(char * comx,char *rxtype,char *txtype,char* return_command)
{
	char* p_return =return_command;
	char space=' ';
	char *on="on";
	p_return=strcat (p_return,"interfacemode ");
	p_return=strncat(p_return,comx,strlen(comx));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,rxtype,strlen(rxtype));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,txtype,strlen(txtype));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,on,strlen(on));	
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,CRT,strlen(CRT));
}
/*
RTK复位到出厂设置
freset
*/
void RtkFreset(char* return_command)
{
	char* p_return =return_command;
	p_return=strcat (p_return,"freset\r\0");
}
/*
关闭对应COM口的对应命令输出
unlog com1 gpgga 
关闭对应的命令输出
*/

void UnLogCommand(char *comx,char *command,char* return_command)
{
	char* p_return =return_command;
	char space=' ';
	p_return=strcat (p_return,"unlog ");
	p_return=strncat(p_return,comx,strlen(comx));
	p_return=strncat(p_return,&space,1);
	p_return=strncat(p_return,command,strlen(command));
	p_return=strncat(p_return,CRT,strlen(CRT));
}
/*

unlogall com1
清除相应的端口的所有输出
*/

void UnlogallCommandCom(char *comx,char* return_command)
{
	char* p_return =return_command;
	char space=' ';
	p_return=strcat (p_return,"unlogall ");
	p_return=strncat(p_return,comx,strlen(comx));
	p_return=strncat(p_return,CRT,strlen(CRT));
}
/*
关闭所有COM口的数据输出
unlogall
*/
void UnLogAllCommand(char* return_command)
{
	char* p_return =return_command;
	p_return=strcat (p_return,"unlogall\r\n\0");
}
/*
配置命令后判断数据有没有配置成功
RtkStatus：RTK的发送状态
*/

void RtkSendCommandStatus(stRtkStatus *RtkStatus)
{
	static int num=0;//标记发送命令后返回的状态的对比较
	if(gRTK_RX_LEN!=0)
	{
		for(num=0;num<gRTK_RX_LEN;num++)
		{
			if((USART3_RX[num]=='O')&&(USART3_RX[num+1]=='K'))
			{
				RtkStatus->bit.send_command=eSendCommandY;
#if DEBUG_RTK
				printf("Send rtk Command OK\r\n");
#endif	
			}
		}
		if(RtkStatus->bit.send_command!=eSendCommandY)
		{
			RtkStatus->bit.send_command=eSendCommandN;
		}
		gRTK_RX_LEN=0;
	}
}
/*
发送RTK配置命令
command：发送的命令
RtkStatus：RTK的发送状态
*/
void RtkSendCommand(char *command,stRtkStatus *RtkStatus)
{
	gRTK_RX_LEN=0;//数据接收区清零
	memset(USART3_RX,0,USART3_REC_LEN*sizeof(char));//清掉串口接收的到的数据 等待串口的新数据
	do{
		rtk_SendBytes(command);
		delay_ms(20);
		RtkSendCommandStatus(RtkStatus);
#if DEBUG_RTK
		if(RtkStatus->bit.send_command==eSendCommandN)
		{
			printf("Send rtk Command = %s  :failure\r\n",command);
		
		}
#endif
	}while(RtkStatus->bit.send_command==eSendCommandN);
	RtkStatus->bit.send_command=eSendCommandN;
	
}

/*
GGA数据提取：
*/
uint16_t RtkGetGpggaData(uint32_t len,stRtkStatus *RtkStatus)
{
	uint16_t num;
	for(num=0;(num<sizeof(gSysGgaDataBuffer))&&(len+num<gRTK_RX_LEN);num++)
	{
		if(USART3_RX[len+num]=='\r'&&USART3_RX[len+num+1]=='\n')
		{
			memset(gSysGgaDataBuffer,0,sizeof(gSysGgaDataBuffer));
			memcpy(gSysGgaDataBuffer,&USART3_RX[len],num);
			gSysGgaDataBuffer[num+1]='\0';
			RtkStatus->bit.get_gga=eGetGgaY;

#if DEBUG_RTK||DEBUG_TASK&&(GGA_TASK_TEST!=1)	
			static uint16_t test_num=0;
			if(test_num==80)
			{
				printf("get gga data = %s\r\n",gSysGgaDataBuffer);
				printf("\r\n");
				test_num=0;
			}else{
				test_num++;
			}
	
#endif
			return num+1;
		}
	}
	if(!((num<sizeof(gSysGgaDataBuffer))&&(len+num<gRTK_RX_LEN)))
	{
		RtkStatus->bit.get_gga=eGetGgaN;
	}
#if DEBUG_RTK		
	printf("get gga data failure\r\n");
#endif
	return 0;
	
}
/*
发送RTK GGA的状态解析
[COM2]$GPGGA,100625.00,3030.5032,N,10407.1052,E,7,08,2.0,502.33,M,-43.20,M,,*4F
*/
#define DEBUG_RtkGpggaStatus 0
#define  METHOD   0
void RtkGpggaStatus(stRtkStatus *RtkStatus)
{
#if METHOD	

#else
	static uint32_t  num=0;
	u8 flag=0;
	char  buf;
	if((gRTK_RX_STATUS&0x1)==0)//表示串口接收还没有超出串口缓存BUF
	{
		for(num=0;num<gRTK_RX_LEN;num++)
		{
			if(USART3_RX[num]=='$')
			{
				RtkGetGpggaData(num,RtkStatus);//拷贝出GGA数据
				//开始解析数据
				while((num<gRTK_RX_LEN)&&(num<USART3_REC_LEN))
				{
					if(USART3_RX[num++]==',')
					{
						flag+=1;
					}
					if(flag==6)
					{
						flag=0;
						buf=USART3_RX[num++];
#if DEBUG_RTK&&DEBUG_RtkGpggaStatus		
	printf("now rtk status gpgga 1 = %c\r\n",buf);
#endif					
						switch(buf)
						{
							case '1':
								RtkStatus->bit.gpgga_status=eGpggaStatus1;
								break;
							case '4':
								RtkStatus->bit.gpgga_status=eGpggaStatus4;
								break;
							case '7':
								RtkStatus->bit.gpgga_status=eGpggaStatus7;
								break;
							default: 
								RtkStatus->bit.gpgga_status=eGpggaStatusN;
								break;	
						}
						buf='0';
						if(RtkStatus->bit.gpgga_status==eGpggaStatusN)
						{
							break;//退出本次while循环
						}else{
							return; //
						}
					}

					delay_us((u32)(10));	//0.01ms
				}
			}else{
				delay_us((u32)(10));	//0.01ms
			}
		}
		num=0;
	}else if((gRTK_RX_STATUS&0x1)==1){//表示串口接收超出串口缓存BUF
		if((num<USART3_REC_LEN)&&(num>gRTK_RX_LEN))//表示数据更新了但新的数据还没有追上解析的数据
		{
			for(;num<gRTK_RX_LEN;num++)
			{
				if(USART3_RX[num]=='$')
				{
					RtkGetGpggaData(num,RtkStatus);//拷贝出GGA数据
					//开始解析数据
					while((num<gRTK_RX_LEN)&&(num<USART3_REC_LEN))
					{
						if(USART3_RX[num++]==',')
						{
							flag+=1;
						}
						if(flag==6)
						{
							flag=0;
							buf=USART3_RX[num++];
#if DEBUG_RTK&&DEBUG_RtkGpggaStatus		
	printf("now rtk status gpgga 2= %c",buf);
#endif					
							switch(buf)
							{
								case '1':
									RtkStatus->bit.gpgga_status=eGpggaStatus1;
									break;
								case '4':
									RtkStatus->bit.gpgga_status=eGpggaStatus4;
									break;
								case '6':
									RtkStatus->bit.gpgga_status=eGpggaStatus7;
									break;
								default: 
									RtkStatus->bit.gpgga_status=eGpggaStatusN;
									break;	
							}
							buf='0';
						}
						if(RtkStatus->bit.gpgga_status==eGpggaStatusN)
						{
							break;//退出本次while循环
						}else{
							return; //
						}
					}
				}else{
					delay_us((u32)(10));	//0.01ms
				}
			}
			num=0;
		}else if(((gRTK_RX_STATUS&0x1)==1)&&(num<USART3_REC_LEN)&&(num<gRTK_RX_LEN))//表示数据更新了但新的数据追上解析的数据
		{
#if DEBUG_RTK&&DEBUG_RtkGpggaStatus		
			printf("now rtk status gpgga 3= %c",buf);
#endif	
			num=0;
			gRTK_RX_STATUS&=~gRTK_RX_STATUS;
		}
	
	}
#endif
}

void BestposDataCopy(stRtkBestposData *BestposData )
{



}

/*
解析Bestpos数据的
*/
void RtkBestposAnalysis(stRtkBestposData *BestposData,stRtkStatus *RtkStatus)
{
	static uint32_t  num=0;
	u8 flag=0;
	u8  data_len=0;
	u8  data_status=0;  //第一位表示数据为空  第二位表示数据接收中有错误码  
	RtkStatus->bit.bestpos_status=eBestposStatusN;
	memset(BestposData->accuracy,0,sizeof(BestposData->accuracy));
	memset(BestposData->latitude,0,sizeof(BestposData->latitude));
	memset(BestposData->heigh,0,sizeof(BestposData->heigh));
	if((gRTK_RX_STATUS&0x1)==0)//表示串口接收还没有超出串口缓存BUF
	{
		for(num=0;num<gRTK_RX_LEN;num++)
		{
			if((num<USART3_REC_LEN)&&(USART3_RX[num]=='<')&&(USART3_RX[num+1]==' '))
			{
				//开始解析数据
				while((num<gRTK_RX_LEN)&&(num<USART3_REC_LEN))
				{
					if(USART3_RX[num++]==' ')
					{
						flag+=1;
					}
					if(flag==7)
					{
						flag=0;
						data_status=0x0;	//状态标志清除
						for(data_len=0;((data_len<MAX_BESTPOS_DATA_LEN)&&(USART3_RX[num]!=' ')&&((data_status&0x2)!=0x2));data_len++)
						{
							if(USART3_RX[num]=='.')
							{
								BestposData->latitude[data_len]=USART3_RX[num++];
							}else if(('0'<=USART3_RX[num])&&(USART3_RX[num]<='9'))
							{
								BestposData->latitude[data_len]=USART3_RX[num++];
							}else{
								data_status|=POSITION(1);
								break;
							}
							
						}
						if((data_status&0x2)!=0x2)
						{
							BestposData->latitude[data_len+1]='\0';
							num++;
						}else{
							break;
						}
						for(data_len=0;((data_len<MAX_BESTPOS_DATA_LEN)&&(USART3_RX[num]!=' ')&&((data_status&0x2)!=0x2));data_len++)
						{
							if(USART3_RX[num]=='.')
							{
								BestposData->accuracy[data_len]=USART3_RX[num++];
							}else if(('0'<=USART3_RX[num])&&(USART3_RX[num]<='9'))
							{
								BestposData->accuracy[data_len]=USART3_RX[num++];
							}else{
								data_status|=POSITION(1);
								break;
							}
						}
						if((data_status&0x2)!=0x2)
						{
							BestposData->accuracy[data_len+1]='\0';
							num++;
						}else{
							break;
						}
						for(data_len=0;((data_len<MAX_BESTPOS_DATA_LEN)&&(USART3_RX[num]!=' ')&&((data_status&0x2)!=0x2));data_len++)
						{
							if(USART3_RX[num]=='.')
							{
								BestposData->heigh[data_len]=USART3_RX[num++];
							}else if(('0'<=USART3_RX[num])&&(USART3_RX[num]<='9'))
							{
								BestposData->heigh[data_len]=USART3_RX[num++];
							}else{
								data_status|=POSITION(1);
							}
						}
						if((data_status&0x2)!=0x2)
						{
							BestposData->heigh[data_len+1]='\0';
						}else{
							break;
						}
						if(strcmp(BestposData->latitude,"0.00000000000")==0)
						{
							data_status|=0x1;
							RtkStatus->bit.bestpos_status=eBestposStatusN;
						}
						if(strcmp(BestposData->accuracy,"0.00000000000")==0)
						{
							data_status|=0x1;
							RtkStatus->bit.bestpos_status=eBestposStatusN;
						}
						if(strcmp(BestposData->heigh,"0.0000")==0)
						{
							data_status|=0x1;
							RtkStatus->bit.bestpos_status=eBestposStatusN;
						}
						if(((data_status&0x1)!=0x1)&&((data_status&0x2)!=0x2))
						{
							RtkStatus->bit.bestpos_status=eBestposStatusY;
						}
#if DEBUG_RTK		
	printf("atitude 1 = %s\r\n",BestposData->latitude);
	printf("accuracy 1 = %s\r\n",BestposData->accuracy);
	printf("heigh 1 = %s\r\n",BestposData->heigh);						
#endif					
						if(RtkStatus->bit.bestpos_status==eBestposStatusN)
						{
							break;//退出本次while循环
						}else{
							return; //
						}
					}

					delay_us((u32)(10));	//0.01ms
				}
			}else{
				delay_us((u32)(10));	//0.01ms
			}
			
		}
		num=0;
	}else if((gRTK_RX_STATUS&0x1)==1)
	{//表示串口接收超出串口缓存BUF
		if((num<USART3_REC_LEN)&&(num>gRTK_RX_LEN))//表示数据更新了但新的数据还没有追上解析的数据
		{
			for(;num<USART3_REC_LEN;num++)
			{

				if((num<USART3_REC_LEN)&&(USART3_RX[num]=='<')&&(USART3_RX[num+1]==' '))
				{
					//开始解析数据
					while((num>gRTK_RX_LEN)&&(num<USART3_REC_LEN))
					{
						if(USART3_RX[num++]==' ')
						{
							flag+=1;
						}
						if(flag==7)
						{
							flag=0;
							data_status=0x0;	//状态标志清除
							for(data_len=0;((data_len<MAX_BESTPOS_DATA_LEN)&&(USART3_RX[num]!=' ')&&((data_status&0x2)!=0x2));data_len++)
							{
								if(USART3_RX[num]=='.')
								{
									BestposData->latitude[data_len]=USART3_RX[num++];
								}else if(('0'<=USART3_RX[num])&&(USART3_RX[num]<='9'))
								{
									BestposData->latitude[data_len]=USART3_RX[num++];
								}else{
									data_status|=POSITION(1);
									break;
								}
								
							}
							if((data_status&0x2)!=0x2)
							{
								BestposData->latitude[data_len+1]='\0';
								num++;
							}else{
								break;
							}
							for(data_len=0;((data_len<MAX_BESTPOS_DATA_LEN)&&(USART3_RX[num]!=' ')&&((data_status&0x2)!=0x2));data_len++)
							{
								if(USART3_RX[num]=='.')
								{
									BestposData->accuracy[data_len]=USART3_RX[num++];
								}else if(('0'<=USART3_RX[num])&&(USART3_RX[num]<='9'))
								{
									BestposData->accuracy[data_len]=USART3_RX[num++];
								}else{
									data_status|=POSITION(1);
									break;
								}
							}
							if((data_status&0x2)!=0x2)
							{
								BestposData->accuracy[data_len+1]='\0';
								num++;
							}else{
								break;
							}
							for(data_len=0;((data_len<MAX_BESTPOS_DATA_LEN)&&(USART3_RX[num]!=' ')&&((data_status&0x2)!=0x2));data_len++)
							{
								if(USART3_RX[num]=='.')
								{
									BestposData->heigh[data_len]=USART3_RX[num++];
								}else if(('0'<=USART3_RX[num])&&(USART3_RX[num]<='9'))
								{
									BestposData->heigh[data_len]=USART3_RX[num++];
								}else{
									data_status|=POSITION(1);
								}
							}
							if((data_status&0x2)!=0x2)
							{
								BestposData->heigh[data_len+1]='\0';
							}else{
								break;
							}
							if(strcmp(BestposData->latitude,"0.00000000000")==0)
							{
								data_status|=0x1;
								RtkStatus->bit.bestpos_status=eBestposStatusN;
							}
							if(strcmp(BestposData->accuracy,"0.00000000000")==0)
							{
								data_status|=0x1;
								RtkStatus->bit.bestpos_status=eBestposStatusN;
							}
							if(strcmp(BestposData->heigh,"0.0000")==0)
							{
								data_status|=0x1;
								RtkStatus->bit.bestpos_status=eBestposStatusN;
							}
							if(((data_status&0x1)!=0x1)&&((data_status&0x2)!=0x2))
							{
								RtkStatus->bit.bestpos_status=eBestposStatusY;
							}
	#if DEBUG_RTK		
		printf("atitude 2 = %s\r\n",BestposData->latitude);
		printf("accuracy 2 = %s\r\n",BestposData->accuracy);
		printf("heigh 2 = %s\r\n",BestposData->heigh);						
	#endif					
							if(RtkStatus->bit.bestpos_status==eBestposStatusN)
							{
								break;//退出本次while循环
							}else{
								return; //
							}
						}

						delay_us((u32)(10));	//0.01ms
					}
				}else{
					delay_us((u32)(10));	//0.01ms
				}

			}
			num=0;
		}else if(((gRTK_RX_STATUS&0x1)==1)&&(num<USART3_REC_LEN)&&(num<gRTK_RX_LEN))//表示数据更新了但新的数据追上解析的数据
		{
#if DEBUG_RTK		
			printf(" rtk Bestpos status 3\r\n");
#endif	
			num=0;
			gRTK_RX_STATUS&=~gRTK_RX_STATUS;
		}
	}
}



/*
发送unlogall
*/
void RtkSendUnlogall(stRtkStatus *RtkStatus)
{
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	UnLogAllCommand(gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}

/*
发送清除命令COM的数据输出
*/
void RtkSendUnlogcom(char *command,char *com,stRtkStatus *RtkStatus)
{
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	UnLogCommand(RTK_COMMAND_COM,command,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}
/*
发送清除命令COM的数据输出
*/
void RtkSendUnlogallcom(char *com,stRtkStatus *RtkStatus)
{
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	UnlogallCommandCom(com,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}
/*
发送GGA命令并读取状态
log com2 gpgga ontime 1
*/
void RtkSendGpgga(stRtkStatus *RtkStatus,char *time)
{
//	RtkSendUnlogcom(RtkStatus); //清除命令端口的所有数据输出
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("gpgga",RTK_COMMAND_COM,time ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}

/*
发送GGA命令并读取状态:指定一个端口
comx:rtk端口号
time：发送频率
log com2 gpgga ontime 1
*/
void RtkSendGpggaComx(char * comx,char *time,stRtkStatus *RtkStatus)
{
//	RtkSendUnlogcom(RtkStatus); //清除命令端口的所有数据输出
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("gpgga",comx,time ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}

/*
设置进入基准站模式
fix position 纬度 经度 高度
*/

void RtkSendPosition(stRtkBestposData *BestposData,stRtkStatus *RtkStatus)
{
	RtkStatus->bit.base_station=eBaseStationN;
	RtkSendUnlogallcom(RTK_COMMAND_COM,RtkStatus);//清除命令端口的所有数据输出
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	FixPositionCommand(BestposData->latitude,BestposData->accuracy ,BestposData->heigh,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);	
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	memset(USART3_RX,0,USART3_REC_LEN*sizeof(char));
	//发送GGA命令
	LogCommand("gpgga",RTK_COMMAND_COM,"1" ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);	

//		//判断GGA状态有没有到7
//		do{
//			RtkGpggaStatus(RtkStatus);
//			delay_ms(1200); 
//		}while(RtkStatus->bit.gpgga_status!=eGpggaStatus7);
//		RtkStatus->bit.base_station=eBaseStationY;
}

/*
向RTK发送RTCM数据
*/
void RtkSendRtcm(char *rtcm_data,uint16_t len)
{
	rtk_SendNumByte(rtcm_data,len);
	
}
/*
RTK初始化：
开始状态数据初始化
默认RTK的波特率是9600（但是为了配合蓝牙的数据接收，将波特率配置成57600）
//usart 3作为命令配置端口  com1
usart 4作为命令配置端口  com2
*/
void RtkInit(stRtkStatus *RtkStatus)
{
	RtkStatus->data=0xffffffff;
	RtkDevInit();
	//命令口初始化配置
	//初始化命令配置串口波特率的默认波特率
	RtkSendCommandUartInit(character_to_uint32(RTK_DATA_COM_BAUD_INIT,strlen(RTK_DATA_COM_BAUD_INIT))); 
	//配置RTK的波特率到设置的波特率
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkComBaudCommand(RTK_DATA_COM,RTK_DATA_COM_BAUD_SET,gCommandBuf);//配置数据口
	RtkSendCommand(gCommandBuf,RtkStatus);
	
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkComBaudCommand(RTK_COMMAND_COM,RTK_DATA_COM_BAUD_SET,gCommandBuf);//配置命令口
	RtkSendCommand(gCommandBuf,RtkStatus);
	//将串口波特率配置的设置的波特率
	RtkSendCommandUartInit(character_to_uint32(RTK_DATA_COM_BAUD_SET,strlen(RTK_DATA_COM_BAUD_SET))); 	//设置命令口波特率
	RtkSendRtcmUartInit(character_to_uint32(RTK_DATA_COM_BAUD_SET,strlen(RTK_DATA_COM_BAUD_SET))); 		//设置数据口的波特率
	//清除掉所有的输出
	RtkSendUnlogall(RtkStatus);
	/*RTK端口收发模式配置*/
	//配置命令口的收发模式
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkInterfaceCommand(RTK_COMMAND_COM,"novatel","novatel",gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
	//数据端口的收发模式
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkInterfaceCommand(RTK_DATA_COM,"rtcmv3","novatel",gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
	//发送GGA命令
	//命令端口的命令端口
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("gpgga",RTK_COMMAND_COM,"1" ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);	
	//数据端口输出gga
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("gpgga",RTK_DATA_COM,"1" ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);	
}

/*
RTCM数据发送,判断gga的状态有没有到达4
*/
void RtkSendRtcmDataStatus(stRtkStatus *RtkStatus,char *rtcm_data)
{
	u8 num = 0;
	while(RtkStatus->bit.gpgga_status!=eGpggaStatus4)
	{
		//发送rtcm 数据
//		RtkSendRtcm(rtcm_data);
		delay_ms(100);
		//发送gga命令
		RtkSendGpgga(RtkStatus,"1");
		for(num=0;num<20;num++)
		{
			RtkGpggaStatus(RtkStatus);
			if(RtkStatus->bit.gpgga_status==eGpggaStatus4)
			{
				return;
			}
			delay_ms(100);
		}
	}

}

/*
函数功能：
1：配置RTK输出LOG GGA的数据
2：提起GGA的数据，判断GGA的状态
3：将GGA数据发给服务器
同时在另一个任务判断有没有收到rtcm数据
如果有收到将数据发送给RTK（函数me3630RxAnalysis）
*/

void RtkGetGga(stRtkStatus *RtkStatus,stMe3630Status *Me3630Status)
{
	uint16_t num=0;
	while(RtkStatus->bit.gpgga_status!=eGpggaStatus4)
	{
		if(Me3630Status->bit.tcp_link==eLinkY)
		{
			RtkGpggaStatus(RtkStatus); //得到GGA数据并且把 状态反馈
//			if(RtkStatus->bit.get_gga==eGetGgaY)//的GGA的数据将数据通过4G将数据给服务器
//			{
//				me3630_SendData(gSysGgaDataBuffer,strlen(gSysGgaDataBuffer),Me3630Status);
//				RtkStatus->bit.get_gga=eGetGgaN;
//			}
		}
		delay_ms(500);
	}
}

/*
RTK的状态标记初始化
*/

void RtkStatusInit(stRtkStatus *RtkStatus)
{
	RtkStatus->data=0;
	RtkStatus->data=~RtkStatus->data;
}
/*
RTK:自校准
	校准流程：用基站自己的GGA数据
*/

void RtkSelfCalibration(stRtkStatus *RtkStatus)
{
	
#if 1

#if DEBUG_RTK	
	printf(" Rtk Self Calibration start \r\n");
#endif	
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkCommandPosave("on","0.01","1","2",gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
#else
	
	
	
	stRtkBestposData BestposData;
#if DEBUG_RTK	
	printf(" Rtk Self Calibration start \r\n");
#endif
	while(RtkStatus->bit.gpgga_status!=eGpggaStatus4)
	{
		RtkSendCommandUartChars(gSysGgaDataBuffer,strlen(gSysGgaDataBuffer));//将GGA回写到本地
		RtkGpggaStatus(RtkStatus); //得到GGA数据并且把 状态反馈
		delay_ms(1500);
#if DEBUG_RTK	
		printf(" Gpgga Status != 4 \r\n");
#endif
	}
#if DEBUG_RTK	
	printf(" Gpgga Status == 4 \r\n");
#endif
	RtkSendUnlogallcom(RTK_COMMAND_COM,RtkStatus);//清除命令端口的所有数据输出
	//打开命令端口的 bestpos输出
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("bestpos",RTK_DATA_COM,"1" ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
	
	//解析bestpos的数据
	if(RtkStatus->bit.bestpos_status!=eBestposStatusY)
	{
		RtkBestposAnalysis(&BestposData,RtkStatus);
	}
#if DEBUG_RTK	
	printf(" Bestpos  Analysis ok\r\n");
#endif
	//设置进入基准站模式:
	RtkSendPosition(&BestposData,RtkStatus);
#if DEBUG_RTK	
	printf(" Rtk Send Position ok\r\n");
#endif
	
#endif

}








