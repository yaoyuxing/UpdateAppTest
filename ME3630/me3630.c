/*
	4Gģ����صĹ��ܺ���
*/
#include "debug.h"
#include "delay.h"
#include "usart.h"
#include "me3630.h"
#include "string.h"
#include "MySys.h"
#include <math.h>
#include <stdlib.h>
#include "s2j.h"
#include <stdint.h>
#include <stdio.h>
#include "rtk.h"
#include "MySys.h"

stMe3630Status  	Me3630Status;
static void me3630_Open(stMe3630Status *status);

///*
//��srcת�����ַ�����des��
//*/
////int ת�����ַ�
// void  uint32_to_character(char * des ,uint32_t src)
//{
//	int buf=0;
//	buf=src;
//	des[3]=(buf%10)+'0';
//	buf=buf/10;
//	des[2]=(buf%10)+'0';
//	buf=buf/10;
//	des[1]=(buf%10)+'0';
//	buf=buf/10;
//	des[0]=(buf%10)+'0';
//}

////�ַ� ת���� int  char_len��des�ַ����ĳ���
// uint32_t  character_to_uint32(char * des ,uint8_t char_len)
//{
//	
//	uint32_t return_data=0;
//	uint8_t num=0;
//	for(num=1;num<=char_len;num++)
//	{
//		
//		return_data+=(*(des++)-'0')*(pow(10,char_len-num));
//	}
//	return return_data;
//}
//��ʼ״̬��ʾ
void Me3630StatusInit(stMe3630Status *Me3630Status)
{
	Me3630Status->data=0;
	Me3630Status->data=~Me3630Status->data;

}
//�Է��͵�ѭ������
void me3630SendCtr(stMe3630Ctr *ctr,uint8_t cycle_nunm, uint16_t time_len ,uint8_t cycle_always)
{
	ctr->bit.cycle_nunm=cycle_nunm;  //����ѭ���Ĵ���
	ctr->bit.time_len=time_len;		//����ȴ���ʱ�䳤��
	ctr->bit.cycle_always=cycle_always; //�Ƿ�һֱѭ��
}
/*
�ж�������û�з��ͳɹ�
*/
int gCommandReturnNum=0; //״̬����������λ��ʾ
int me3630_SendCommandStatus(void)
{
	
	static int num=0;//��Ƿ�������󷵻ص�״̬�ĶԱȽ�
	if(CommandReturnNum!=0)
	{
		for(num=0;num<CommandReturnNum;num++)
		{
#if DEBUG_ME3630
//			printf("me3630_SendCommand return  %c",USART2_RX_BUF[num]);
#endif			
			if(USART2_RX_COMMAND[num]=='>')//��ⷢ�����������  +ZIPSENDRAW=1,0040>3210321032103210321032103210321032103210,num=1
			{
#if DEBUG_ME3630
				printf("me3630_SendCommand >\r\n");
#endif	
				USART2_RX_STA=0;
				CommandReturnNum=0;
//				CommandSendOkFlag=eSendCommandOk;
				return eSendCommandOk;
			}
			if((USART2_RX_COMMAND[num]=='O')&&(USART2_RX_COMMAND[num+1]=='K'))
			{
#if DEBUG_ME3630
				printf("me3630_SendCommand OK\r\n");
#endif	
//				CommandSendOkFlag=eSendCommandOk;
				gCommandReturnNum=num;
				USART2_RX_STA=0;
				CommandReturnNum=0;
				return eSendCommandOk;
			}
		}
		CommandReturnNum=0;
		return eSendCommandErr;
	}
	return eSendCommandErr;

}
/*
	����me_3630����������
*/

 void me3630_SendBytes(char *command)
{
	int num=0;
	unsigned int len=0;
	char *p_comm=command;
	len=strlen((const char*)p_comm);

//	CommandSendOkFlag=eSendCommandErr;
	for(num=0;num<len;num++)
	{
		me3630SendByte(*(p_comm+num));
//		delay_xms(1);
	}
}	
/*
����ָ�����ȵ�����
*/
void me3630_SendDataBytes(char*data ,u16 len)
{
	int num=0;
	for(num=0;num<len+1;num++)
	{
		me3630SendByte(*(data+num));
#if 0
	printf("%c",*(data+num));
#endif
//		delay_xms(1);
	}
}
/*
����һЩ4Gģ�������
command�����͵����� 
status������״̬

*/
void me3630SendCommand(char *command,stMe3630Status *status,stMe3630Ctr ctr)
{
	int num=1;
	status->bit.send_commd=eSendCommandErr;
	while(num)
	{
#if DEBUG_ME3630
	printf("me3630_SendCommand=%s,num=%d\r\n",command, num);
#endif
		me3630_SendBytes(command);
		delay_ms(ctr.bit.time_len); 
		if(me3630_SendCommandStatus()==eSendCommandOk)
		{
			num=0;
			status->bit.send_commd=eSendCommandOk;
			status->bit.call_timeout=eTimeoutN;
			
#if DEBUG_ME3630
	printf("me3630_SendCommand	%s OK\r\n" ,command);
#endif			
		}else{

			if(num==ctr.bit.cycle_nunm)
			{
				num=ctr.bit.cycle_always;
				status->bit.send_commd=eSendCommandErr;
				status->bit.call_timeout=eTimeoutY;
#if DEBUG_ME3630
	printf("me3630_SendCommand  timeout\r\n");
#endif
			}else{
				num++;
				status->bit.send_commd=eSendCommandIng;
				status->bit.call_timeout=eTimeoutN;
			}
		}
		
	}
}
/*
	�������ݵ�״̬����
*/
int me3630_SendDataStatus(void)
{
	
	static int num=0;//��Ƿ�������󷵻ص�״̬�ĶԱȽ�
	if(CommandReturnNum!=0)
	{
		for(num=0;num<CommandReturnNum;num++)
		{
#if DEBUG_ME3630
//			printf("me3630_SendCommand return  %c",USART2_RX_BUF[num]);
#endif			
			if((USART2_RX_COMMAND[num]=='O')&&(USART2_RX_COMMAND[num+1]=='K'))
			{
#if DEBUG_ME3630
				printf("me3630_SendCommand OK\r\n");
#endif	
//				CommandSendOkFlag=eSendCommandOk;
				USART2_RX_STA=0;
				CommandReturnNum=0;
				return eSendCommandOk;
			}
		}
		CommandReturnNum=0;
		return eSendCommandErr;
	}
	return eSendCommandErr;

}

/*
�������ܣ���������
data�����͵�����
status����¼���͵�״̬
ctr�����Ʒ��͵ķ�ʽ��
*/
void me3630SendData(char *data,u16 len,stMe3630Status *status,stMe3630Ctr ctr)
{
	int num=1;
	status->bit.send_data=eSendDataErr;
	while(num)
	{
#if DEBUG_ME3630
	printf("me3630SendData=%s,num=%d\r\n",data, num);
#endif
		me3630_SendDataBytes(data,len);
		delay_ms(ctr.bit.time_len); 
		if(me3630_SendDataStatus()==eSendDataOk)
		{
			num=0;
			status->bit.send_data=eSendDataOk;
			status->bit.call_timeout=eTimeoutN;
			
#if DEBUG_ME3630
	printf("me3630SendData	%s OK\r\n" ,data);
#endif			
		}else{
			if(num==ctr.bit.cycle_nunm)
			{
				num=ctr.bit.cycle_always;
				status->bit.send_data=eSendCommandErr;
				status->bit.call_timeout=eTimeoutY;
#if DEBUG_ME3630
	printf("me3630SendData  timeout\r\n");
#endif
			}else{
				num++;
				status->bit.send_data=eSendDataIng;
				status->bit.call_timeout=eTimeoutN;
			}
		}
	}
}
/*
��¼TCP�����
*/
void openTCP(stMe3630Status *status)
{
	char csq=0;//�ж��ź�ǿ��
	stMe3630Ctr ctr;
	me3630_Open(status);
	Me3630GetSignal(&csq,status );
	while(csq==99)//û���źŵ�����µȴ��źŻָ�
	{
		Me3630GetSignal(&csq,status );
		delay_ms(1000); 
	}
	
	me3630SendCtr(&ctr,5,500 ,eAlwaysN);
	me3630SendCommand(TCP_CLOSE,status,ctr);
	me3630SendCommand(CALL_CLOSE,status,ctr);
	me3630SendCtr(&ctr,5,500 ,eAlwaysY);
	me3630SendCommand(CALL_OPEN,status,ctr);
	me3630SendCommand(TCP_OPEN,status,ctr);	
	status->bit.tcp_link=eLinkY;
}

/*
me3630���ݷ���
data:��������
len�����͵����ݳ���
status�����ݷ��͵�״̬
*/
void me3630_SendData(char *data,uint32_t len,stMe3630Status *status)
{
	stMe3630Ctr ctr;
	char len_char[4]={0};
	char data_buf[MAX_SENDDATA_NUM];
	char *p=0,*p_head=0;
loop: //����û�з��ͳɹ��ط�����
	memset(data_buf,0,MAX_SENDDATA_NUM*sizeof(char));
	p=data_buf;
	p=strcat (p,SEND_ORIGINAL_DATA);
	uint32_to_character(len_char,(len));	
	p=strncat(p,len_char,sizeof(uint32_t));
	p=strcat (p,COMMAND_END);
#if DEBUG_ME3630
	printf("%s\r\n",p);
#endif
	me3630SendCtr(&ctr,5,500 ,eAlwaysY);
	me3630SendCommand(p,status,ctr);
	
	memset(data_buf,0,MAX_SENDDATA_NUM*sizeof(char));
	
	
//	p=strncat(p,data,len);
//	p=strcat (p,COMMAND_END);
	p_head=p;
	p=memcpy(p, data, len);
	p=strcat (p+len,COMMAND_END);
#if DEBUG_ME3630
	printf("send data %s \r\n",p);
#endif

	me3630SendCtr(&ctr,1,50 ,eAlwaysN);
	me3630SendData(p_head,len,status,ctr);
	if (status->bit.send_data!=eSendDataOk)
	{
#if DEBUG_ME3630
	printf("send data failure\r\n");
#endif
		goto  loop;
	}
//	me3630_SendDataBytes(p_head ,len);
//	me3630_SendBytes(p_head);
	
	
	/*
	��ӷ���������ȷ���Ƿ��ط�����
	*/
//	me3630SendCtr(&ctr,5,500 ,eAlwaysY);
//	me3630SendCommand(p,status,ctr);
}

/*
�ж�4Gģ����û�п����ɹ�
*/
static void me3630_Open(stMe3630Status *status)
{
	stMe3630Ctr ctr;
	me3630SendCtr(&ctr,5,500 ,eAlwaysY);
	me3630SendCommand(AT,status,ctr);
}

//GG
/*
�����յ���JSON��GGA��������ת���ɱ��ص����ݽṹ
*/
stMe3630reGgaData * me3630GgaJsonToStruct(cJSON *jsData )
{
	s2j_create_struct_obj(stData, stMe3630reGgaData);
	s2j_struct_get_array_element(stData, jsData, int, data);
	s2j_struct_get_basic_element(stData, jsData, int, length);
	s2j_struct_get_basic_element(stData, jsData, int, ret);
	s2j_delete_json_obj(jsData);//����������Json���ڴ�
	return stData;//�������ݽṹ��ĵ�ַ
}
/*
����GGA������
�Է���GGA�����ݷ������н���
GgaData:����������GGA����
reData:���ظ��ⲿʹ�õ�����
DataHead�����ݵ�ͷ
*/
void me3630RxAnalysisGga(u8 *GgaData,u8 *reData,stMe3630SendHead *DataHead,stMe3630Status *Status)
{
	char *pDataBuf=0;
	stMe3630reGgaData  *reGgaData;//
	cJSON * jsDataBuf;
//	if(DataHead->DataLen>0)
//	{
//		Status->bit.get_rtcm=eGetRtcmY;
//	}else{
//		Status->bit.get_rtcm=eGetRtcmN;
//	}
	pDataBuf=malloc(DataHead->DataLen);//�����Ӧ���ȵ�һ���ڴ�
	memcpy(pDataBuf, (char *)(GgaData+sizeof(stMe3630SendHead)),DataHead->DataLen);
//	if(200<DataHead->DataLen&&DataHead->DataLen<300)
//		while(1);
//	if(DataHead->DataLen>300)
//		while(1);
//	pDataBuf=malloc(strlen((char *)(GgaData+sizeof(stMe3630SendHead))));//�����Ӧ���ȵ�һ���ڴ�
//	if(strlen((char *)(GgaData+sizeof(stMe3630SendHead)))>300)
//		while(1);
//	memcpy(pDataBuf, (char *)(GgaData+sizeof(stMe3630SendHead)),strlen((char *)(GgaData+sizeof(stMe3630SendHead))));
#if DEBUG_ME3630||DEBUG_TASK
	//printf("json data len %d\r\n",strlen((char *)(GgaData+sizeof(stMe3630SendHead))));
	printf("json data len %d\r\n",DataHead->DataLen);
//	if(DataHead->DataLen>500)
//		while(1);
#endif		
	
	
	
#if DEBUG_ME3630||DEBUG_TASK
	printf("%s\r\n",pDataBuf);
#endif	
	jsDataBuf=cJSON_Parse(pDataBuf);
	reGgaData=me3630GgaJsonToStruct(jsDataBuf);//���ض�Ӧ�ı��ؽṹ��
	memcpy(reData, (char *)(reGgaData),sizeof(stMe3630reGgaData));//�����ݷ��ظ��ⲿ
	s2j_delete_struct_obj(reGgaData);//����ṹ�������ڴ�
	free(pDataBuf);
}
/*
�Է������ݵ�����ͷ���н���
RXData:4G���յ�������
DataHead ������������������ͷ
*/
eMe3630ApiLogo me3630RxDataHeaAnalysis(u8 *rxData,stMe3630SendHead *DataHead)
{
//	stMe3630SendHead Head;
#if DEBUG_ME3630
	printf("stMe3630SendHead  len:%d \r\n",sizeof(stMe3630SendHead));
#endif
	memcpy((char *)DataHead, (char *)(rxData), sizeof(stMe3630SendHead)); //�����ݿ������ڴ���
	DataHead->ApiLogo=sw32_uint32(DataHead->ApiLogo);		
	DataHead->RequestID=sw32_uint32(DataHead->RequestID);		
	DataHead->DataLen=sw32_uint32(DataHead->DataLen);	
//if(DataHead->DataLen>300||(DataHead->DataLen&0xf0000000)==0xf0000000)	
//	while(1);
	DataHead->DataOutWay=sw32_uint32(DataHead->DataOutWay);		
	DataHead->PackageInsideNum=sw32_uint32(DataHead->PackageInsideNum);
	DataHead->PackageNum=sw32_uint32(DataHead->PackageNum);	
	return DataHead->ApiLogo;
}
/*
��������
Data:4G���յ�������
��������:
�������յ�����
*/
#include "rtk.h"
void me3630RxAnalysis(u8 *Data,stMe3630Status *Status)
{
	stMe3630SendHead 	DataHead;
	stMe3630reGgaData	reGgaData;
	switch(me3630RxDataHeaAnalysis(Data,&DataHead))
	{
		//���ڽ��ܵ����ݺ󣬷��������ݷ��ͷ���ȷ���յ���Ϣ��
		case   eApiLogoRequestResponse:  
			;
			break;
		//���������������GGA����:����gga�ַ�����ǧѰ����
		case   eApiLogoGGA:  
			me3630RxAnalysisGga(Data,(u8 *)(&reGgaData),&DataHead,Status);
			if(reGgaData.length>0)
			{
				Status->bit.get_rtcm=eGetRtcmY;

				rtk_SendNumByte(reGgaData.data ,reGgaData.length);//��RTCM���ݷ��͸�rtk
#if 0//DEBUG_ME3630||DEBUG_TASK   //���Դ���RTK��RTCM������ʲô
				int num=0; 
				printf("rtk_SendNumByte=\r\n");
				for(num=0;num<reGgaData.length;num++)
				{
					printf("%d,",*((char*)(&reGgaData.data)+num));
				}
				printf("\r\n");
#endif
			}else{
				Status->bit.get_rtcm=eGetRtcmN;
			}
			break;
		//�ϴ���վ��״̬��Ϣ���ϴ���վ�������
		case   eApiLogoSysStatus:  ;
			break;
		default: break;
	}
}
/*
���յ������ݣ������
*/

void  me3630RxUnpacking(stMe3630Status *Status)
{
	
//	u8 data_buf[MAX_EXTRACTION_DATA_LEN];
//	uint32_t data_buf=0;
	u8 **data_buf=0;
	uint32_t extraction_data_len=0;
	static uint16_t old_usart2_rx_len=0;
//	data_buf=malloc(MAX_EXTRACTION_DATA_LEN);//�����Ӧ���ȵ�һ���ڴ�
//	memset(data_buf,0,MAX_EXTRACTION_DATA_LEN*sizeof(char));
	if(USART2_RX_STA!=0) //����û����������û�ж���
	{
		if(old_usart2_rx_len==USART2_RX_STA)
		{
			old_usart2_rx_len=USART2_RX_STA;
			extraction_data_len=me3630RxDataExtraction(USART2_RX_BUF,data_buf);
			me3630RxAnalysis(*data_buf,Status);
			free(*data_buf);
			USART2_RX_STA=0;
		}else{
			old_usart2_rx_len=USART2_RX_STA;
		}
	}
//	free(data_buf);
	/*
	��ӽ������
	*/
	
}
//

/*
��������ȡ�������
\r\n+ZIPRECV: 1,120.76.224.208,60000,180,
�������ݳ���
*/
#define  HEAD_LEN  50
#define	 MAX_DATA_LEN_CHAR   4
uint32_t  me3630RxDataExtraction(u8 *src,u8 **des)
{

	u8 *p=0;
	u8 status=0;//���ݲ�ѯ��״̬
	u8 head_len=0;//����ͷ�ĳ���
	u8 len_flag=0;//��ǲ��ҵ����ݳ��ȵ��ַ�û��
	u8 data_num=0;//����������ڵ�����һλ
//	u8 char_len[MAX_DATA_LEN_CHAR];//��������е����ݵĳ��ȵ��ַ���ʽ
	u8 *p_len_char=0;//��¼�����ַ��Ŀ�ͷ
	u8 len_char_num=0;//���ݳ��ȵı�ʾ�ַ�վ���ٸ��ַ�
	uint32_t data_len=0;//���ݵĳ���
	p=src;
	
	while(status!=1)
	{
		if((p[data_num++]=='\r')&&(p[data_num++]=='\n'))//���ݵ�ͷ��λ�ǿ�ͷ
		{
			for(head_len=0;head_len<HEAD_LEN;head_len++)//�������ݵĳ���
			{
				if(p[data_num++]==',')
				{
					len_flag=len_flag+1;

				}
				if(len_flag==3)break;
		
			}
			if(HEAD_LEN-1==head_len)
			{
				status=1;//���ݲ������˳�(����ͷ����)
			}
			if(len_flag==3)//��ʾ�ҵ��������еĵڶ���','����һ��','��ʾ���ݵĳ���
			{
				p_len_char=&p[data_num];//��¼�����ַ��Ŀ�ͷ
				for(head_len=0;head_len<MAX_DATA_LEN_CHAR;head_len++)
				{
					if(p[data_num++]==',')
					{
						break;
					}
					len_char_num=len_char_num+1;

				}
				
				data_len=character_to_uint32((char *)p_len_char ,len_char_num);
				
				*des=(u8*)malloc(data_len);//����һ������������ȡ�����ݿռ�			
			}
			
			//���ݿ��� void *memcpy(void *dest, const void *src, size_t n);
			if((p[data_num+data_len]=='\r')&&(p[data_num+data_len+1]=='\n'))
			{
				memcpy(*des, &p[data_num], data_len);
#if DEBUG_ME3630
				printf("me3630 get data :%s \r\n",*des);
#endif
				status=1;//���������˳�
				return data_len;//�������ݵĳ���
			}else{
				status=1;//���ݲ������˳������ݶ�ʧ��
			}
		}else{
			status=1;//���ݲ������˳���û������ͷ��
		}
	}

return 0;
}

/*
��ȡ4G�ź�ǿ��
data :�������ź�ǿ������
status:״̬����
�����������
AT+CSQ 
+CSQ: 17,99 
 
OK 

*/
#define CSQ_RETURN_DATA_LEN	12
void Me3630GetSignal(char *data,stMe3630Status *status )
{
	stMe3630Ctr ctr;
	char buf[CSQ_RETURN_DATA_LEN];//���淴������������
	char RssiBuf[2];//�����ź�ǿ��
	char num=0;//
	u16	 ReturnNum=0;

	gCommandReturnNum=0;
	
	me3630SendCtr(&ctr,5,500 ,eAlwaysY);
	me3630SendCommand(CSQ,status,ctr);
	//���ݷ��ͳɺ������������������
	ReturnNum=gCommandReturnNum;
	
	if(ReturnNum>CSQ_RETURN_DATA_LEN)
	{
		ReturnNum=ReturnNum-CSQ_RETURN_DATA_LEN;
		memcpy(buf, &USART2_RX_COMMAND[ReturnNum], CSQ_RETURN_DATA_LEN);
		//��ȡ����
		for(num=0;num<CSQ_RETURN_DATA_LEN;num++)
		{
			if(buf[num]==':')
			{
				*RssiBuf=buf[num+2];
				*data=*RssiBuf-'0';
#if DEBUG_ME3630
//				printf("%c",*RssiBuf);
#endif
				if(buf[num]!=',')
				{
					*(RssiBuf+1)=buf[num+3];
					*data=*data*10+(*(RssiBuf+1)-'0');
#if DEBUG_ME3630
//					printf("%c",*(RssiBuf+1));
					printf("Signal strength %d \r\n",*data);
#endif
				}
			}

		}
	}else{
		
		
	}
}




