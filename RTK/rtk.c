/*
RTK��������

������RTKʱ������rtk�����

1����gpgga������ǧѰ����Ϣ��ȡ
log com2 gpgga ontime 1  
2����bestpos��������ȡγ�Ⱦ��ȸ߶� ��10���Ƶģ�
log com2 bestpos ontime 1  
3������com�ڵ��շ�Э��
interfacemode com2 rtcmv3 novatel on
4�����ý����׼վģʽ
fix position γ�� ���� �߶�
5����ԭ���ƶ�վ
fix none

6�����óɣ�115200��Ĭ��9600��
com com2 115200
*/

/*
RTK com1 ���������ÿ�
RTK com2 ��RTK�������������

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
	����me_3630����������
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
����ָ�����ȵ����ݵ�RTK
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
��У׼��վģʽ����
state = on off
maxtime:�ȴ�ʱ��
maxhstd:ˮƽ���
maxvstd���߶����
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
ƴ��LOG����
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
���ý����׼վģʽ������ƴ��
fix position γ�� ���� �߶ȣ������γ�ȸ߶���bestpos�ģ�
γ�ȣ�latitude
���ȣ�Accuracy
�߶ȣ�height
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
���ý����׼վģʽ������ƴ��
fix none
*/

void FixNoneCommand(char* return_command)
{
	char* p_return =return_command;
	p_return=strcat (p_return,"fix none\r\0");
}
/*
com com2 115200
����COM�Ĳ�����
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
����com�ڵ��շ�Э��
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
RTK��λ����������
freset
*/
void RtkFreset(char* return_command)
{
	char* p_return =return_command;
	p_return=strcat (p_return,"freset\r\0");
}
/*
�رն�ӦCOM�ڵĶ�Ӧ�������
unlog com1 gpgga 
�رն�Ӧ���������
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
�����Ӧ�Ķ˿ڵ��������
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
�ر�����COM�ڵ��������
unlogall
*/
void UnLogAllCommand(char* return_command)
{
	char* p_return =return_command;
	p_return=strcat (p_return,"unlogall\r\n\0");
}
/*
����������ж�������û�����óɹ�
RtkStatus��RTK�ķ���״̬
*/

void RtkSendCommandStatus(stRtkStatus *RtkStatus)
{
	static int num=0;//��Ƿ�������󷵻ص�״̬�ĶԱȽ�
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
����RTK��������
command�����͵�����
RtkStatus��RTK�ķ���״̬
*/
void RtkSendCommand(char *command,stRtkStatus *RtkStatus)
{
	gRTK_RX_LEN=0;//���ݽ���������
	memset(USART3_RX,0,USART3_REC_LEN*sizeof(char));//������ڽ��յĵ������� �ȴ����ڵ�������
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
GGA������ȡ��
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
����RTK GGA��״̬����
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
	if((gRTK_RX_STATUS&0x1)==0)//��ʾ���ڽ��ջ�û�г������ڻ���BUF
	{
		for(num=0;num<gRTK_RX_LEN;num++)
		{
			if(USART3_RX[num]=='$')
			{
				RtkGetGpggaData(num,RtkStatus);//������GGA����
				//��ʼ��������
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
							break;//�˳�����whileѭ��
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
	}else if((gRTK_RX_STATUS&0x1)==1){//��ʾ���ڽ��ճ������ڻ���BUF
		if((num<USART3_REC_LEN)&&(num>gRTK_RX_LEN))//��ʾ���ݸ����˵��µ����ݻ�û��׷�Ͻ���������
		{
			for(;num<gRTK_RX_LEN;num++)
			{
				if(USART3_RX[num]=='$')
				{
					RtkGetGpggaData(num,RtkStatus);//������GGA����
					//��ʼ��������
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
							break;//�˳�����whileѭ��
						}else{
							return; //
						}
					}
				}else{
					delay_us((u32)(10));	//0.01ms
				}
			}
			num=0;
		}else if(((gRTK_RX_STATUS&0x1)==1)&&(num<USART3_REC_LEN)&&(num<gRTK_RX_LEN))//��ʾ���ݸ����˵��µ�����׷�Ͻ���������
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
����Bestpos���ݵ�
*/
void RtkBestposAnalysis(stRtkBestposData *BestposData,stRtkStatus *RtkStatus)
{
	static uint32_t  num=0;
	u8 flag=0;
	u8  data_len=0;
	u8  data_status=0;  //��һλ��ʾ����Ϊ��  �ڶ�λ��ʾ���ݽ������д�����  
	RtkStatus->bit.bestpos_status=eBestposStatusN;
	memset(BestposData->accuracy,0,sizeof(BestposData->accuracy));
	memset(BestposData->latitude,0,sizeof(BestposData->latitude));
	memset(BestposData->heigh,0,sizeof(BestposData->heigh));
	if((gRTK_RX_STATUS&0x1)==0)//��ʾ���ڽ��ջ�û�г������ڻ���BUF
	{
		for(num=0;num<gRTK_RX_LEN;num++)
		{
			if((num<USART3_REC_LEN)&&(USART3_RX[num]=='<')&&(USART3_RX[num+1]==' '))
			{
				//��ʼ��������
				while((num<gRTK_RX_LEN)&&(num<USART3_REC_LEN))
				{
					if(USART3_RX[num++]==' ')
					{
						flag+=1;
					}
					if(flag==7)
					{
						flag=0;
						data_status=0x0;	//״̬��־���
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
							break;//�˳�����whileѭ��
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
	{//��ʾ���ڽ��ճ������ڻ���BUF
		if((num<USART3_REC_LEN)&&(num>gRTK_RX_LEN))//��ʾ���ݸ����˵��µ����ݻ�û��׷�Ͻ���������
		{
			for(;num<USART3_REC_LEN;num++)
			{

				if((num<USART3_REC_LEN)&&(USART3_RX[num]=='<')&&(USART3_RX[num+1]==' '))
				{
					//��ʼ��������
					while((num>gRTK_RX_LEN)&&(num<USART3_REC_LEN))
					{
						if(USART3_RX[num++]==' ')
						{
							flag+=1;
						}
						if(flag==7)
						{
							flag=0;
							data_status=0x0;	//״̬��־���
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
								break;//�˳�����whileѭ��
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
		}else if(((gRTK_RX_STATUS&0x1)==1)&&(num<USART3_REC_LEN)&&(num<gRTK_RX_LEN))//��ʾ���ݸ����˵��µ�����׷�Ͻ���������
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
����unlogall
*/
void RtkSendUnlogall(stRtkStatus *RtkStatus)
{
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	UnLogAllCommand(gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}

/*
�����������COM���������
*/
void RtkSendUnlogcom(char *command,char *com,stRtkStatus *RtkStatus)
{
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	UnLogCommand(RTK_COMMAND_COM,command,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}
/*
�����������COM���������
*/
void RtkSendUnlogallcom(char *com,stRtkStatus *RtkStatus)
{
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	UnlogallCommandCom(com,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}
/*
����GGA�����ȡ״̬
log com2 gpgga ontime 1
*/
void RtkSendGpgga(stRtkStatus *RtkStatus,char *time)
{
//	RtkSendUnlogcom(RtkStatus); //�������˿ڵ������������
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("gpgga",RTK_COMMAND_COM,time ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}

/*
����GGA�����ȡ״̬:ָ��һ���˿�
comx:rtk�˿ں�
time������Ƶ��
log com2 gpgga ontime 1
*/
void RtkSendGpggaComx(char * comx,char *time,stRtkStatus *RtkStatus)
{
//	RtkSendUnlogcom(RtkStatus); //�������˿ڵ������������
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("gpgga",comx,time ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
}

/*
���ý����׼վģʽ
fix position γ�� ���� �߶�
*/

void RtkSendPosition(stRtkBestposData *BestposData,stRtkStatus *RtkStatus)
{
	RtkStatus->bit.base_station=eBaseStationN;
	RtkSendUnlogallcom(RTK_COMMAND_COM,RtkStatus);//�������˿ڵ������������
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	FixPositionCommand(BestposData->latitude,BestposData->accuracy ,BestposData->heigh,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);	
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	memset(USART3_RX,0,USART3_REC_LEN*sizeof(char));
	//����GGA����
	LogCommand("gpgga",RTK_COMMAND_COM,"1" ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);	

//		//�ж�GGA״̬��û�е�7
//		do{
//			RtkGpggaStatus(RtkStatus);
//			delay_ms(1200); 
//		}while(RtkStatus->bit.gpgga_status!=eGpggaStatus7);
//		RtkStatus->bit.base_station=eBaseStationY;
}

/*
��RTK����RTCM����
*/
void RtkSendRtcm(char *rtcm_data,uint16_t len)
{
	rtk_SendNumByte(rtcm_data,len);
	
}
/*
RTK��ʼ����
��ʼ״̬���ݳ�ʼ��
Ĭ��RTK�Ĳ�������9600������Ϊ��������������ݽ��գ������������ó�57600��
//usart 3��Ϊ�������ö˿�  com1
usart 4��Ϊ�������ö˿�  com2
*/
void RtkInit(stRtkStatus *RtkStatus)
{
	RtkStatus->data=0xffffffff;
	RtkDevInit();
	//����ڳ�ʼ������
	//��ʼ���������ô��ڲ����ʵ�Ĭ�ϲ�����
	RtkSendCommandUartInit(character_to_uint32(RTK_DATA_COM_BAUD_INIT,strlen(RTK_DATA_COM_BAUD_INIT))); 
	//����RTK�Ĳ����ʵ����õĲ�����
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkComBaudCommand(RTK_DATA_COM,RTK_DATA_COM_BAUD_SET,gCommandBuf);//�������ݿ�
	RtkSendCommand(gCommandBuf,RtkStatus);
	
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkComBaudCommand(RTK_COMMAND_COM,RTK_DATA_COM_BAUD_SET,gCommandBuf);//���������
	RtkSendCommand(gCommandBuf,RtkStatus);
	//�����ڲ��������õ����õĲ�����
	RtkSendCommandUartInit(character_to_uint32(RTK_DATA_COM_BAUD_SET,strlen(RTK_DATA_COM_BAUD_SET))); 	//��������ڲ�����
	RtkSendRtcmUartInit(character_to_uint32(RTK_DATA_COM_BAUD_SET,strlen(RTK_DATA_COM_BAUD_SET))); 		//�������ݿڵĲ�����
	//��������е����
	RtkSendUnlogall(RtkStatus);
	/*RTK�˿��շ�ģʽ����*/
	//��������ڵ��շ�ģʽ
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkInterfaceCommand(RTK_COMMAND_COM,"novatel","novatel",gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
	//���ݶ˿ڵ��շ�ģʽ
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	RtkInterfaceCommand(RTK_DATA_COM,"rtcmv3","novatel",gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
	//����GGA����
	//����˿ڵ�����˿�
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("gpgga",RTK_COMMAND_COM,"1" ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);	
	//���ݶ˿����gga
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("gpgga",RTK_DATA_COM,"1" ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);	
}

/*
RTCM���ݷ���,�ж�gga��״̬��û�е���4
*/
void RtkSendRtcmDataStatus(stRtkStatus *RtkStatus,char *rtcm_data)
{
	u8 num = 0;
	while(RtkStatus->bit.gpgga_status!=eGpggaStatus4)
	{
		//����rtcm ����
//		RtkSendRtcm(rtcm_data);
		delay_ms(100);
		//����gga����
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
�������ܣ�
1������RTK���LOG GGA������
2������GGA�����ݣ��ж�GGA��״̬
3����GGA���ݷ���������
ͬʱ����һ�������ж���û���յ�rtcm����
������յ������ݷ��͸�RTK������me3630RxAnalysis��
*/

void RtkGetGga(stRtkStatus *RtkStatus,stMe3630Status *Me3630Status)
{
	uint16_t num=0;
	while(RtkStatus->bit.gpgga_status!=eGpggaStatus4)
	{
		if(Me3630Status->bit.tcp_link==eLinkY)
		{
			RtkGpggaStatus(RtkStatus); //�õ�GGA���ݲ��Ұ� ״̬����
//			if(RtkStatus->bit.get_gga==eGetGgaY)//��GGA�����ݽ�����ͨ��4G�����ݸ�������
//			{
//				me3630_SendData(gSysGgaDataBuffer,strlen(gSysGgaDataBuffer),Me3630Status);
//				RtkStatus->bit.get_gga=eGetGgaN;
//			}
		}
		delay_ms(500);
	}
}

/*
RTK��״̬��ǳ�ʼ��
*/

void RtkStatusInit(stRtkStatus *RtkStatus)
{
	RtkStatus->data=0;
	RtkStatus->data=~RtkStatus->data;
}
/*
RTK:��У׼
	У׼���̣��û�վ�Լ���GGA����
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
		RtkSendCommandUartChars(gSysGgaDataBuffer,strlen(gSysGgaDataBuffer));//��GGA��д������
		RtkGpggaStatus(RtkStatus); //�õ�GGA���ݲ��Ұ� ״̬����
		delay_ms(1500);
#if DEBUG_RTK	
		printf(" Gpgga Status != 4 \r\n");
#endif
	}
#if DEBUG_RTK	
	printf(" Gpgga Status == 4 \r\n");
#endif
	RtkSendUnlogallcom(RTK_COMMAND_COM,RtkStatus);//�������˿ڵ������������
	//������˿ڵ� bestpos���
	memset(gCommandBuf,0,MAX_RTK_COMMAND_BUF*sizeof(char));
	LogCommand("bestpos",RTK_DATA_COM,"1" ,gCommandBuf);
	RtkSendCommand(gCommandBuf,RtkStatus);
	
	//����bestpos������
	if(RtkStatus->bit.bestpos_status!=eBestposStatusY)
	{
		RtkBestposAnalysis(&BestposData,RtkStatus);
	}
#if DEBUG_RTK	
	printf(" Bestpos  Analysis ok\r\n");
#endif
	//���ý����׼վģʽ:
	RtkSendPosition(&BestposData,RtkStatus);
#if DEBUG_RTK	
	printf(" Rtk Send Position ok\r\n");
#endif
	
#endif

}








