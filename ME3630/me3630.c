/*
	4G模块相关的功能函数
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
//将src转换成字符放在des中
//*/
////int 转化成字符
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

////字符 转化成 int  char_len是des字符串的长度
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
//初始状态表示
void Me3630StatusInit(stMe3630Status *Me3630Status)
{
	Me3630Status->data=0;
	Me3630Status->data=~Me3630Status->data;

}
//对发送的循环配置
void me3630SendCtr(stMe3630Ctr *ctr,uint8_t cycle_nunm, uint16_t time_len ,uint8_t cycle_always)
{
	ctr->bit.cycle_nunm=cycle_nunm;  //控制循环的次数
	ctr->bit.time_len=time_len;		//命令等待的时间长度
	ctr->bit.cycle_always=cycle_always; //是否一直循环
}
/*
判断命令有没有发送成功
*/
int gCommandReturnNum=0; //状态反馈的数据位标示
int me3630_SendCommandStatus(void)
{
	
	static int num=0;//标记发送命令后返回的状态的对比较
	if(CommandReturnNum!=0)
	{
		for(num=0;num<CommandReturnNum;num++)
		{
#if DEBUG_ME3630
//			printf("me3630_SendCommand return  %c",USART2_RX_BUF[num]);
#endif			
			if(USART2_RX_COMMAND[num]=='>')//检测发送数据命令返还  +ZIPSENDRAW=1,0040>3210321032103210321032103210321032103210,num=1
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
	发送me_3630的配置命令
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
发送指定长度的数据
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
发送一些4G模块的命令
command：发送的命令 
status：反馈状态

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
	发送数据的状态反馈
*/
int me3630_SendDataStatus(void)
{
	
	static int num=0;//标记发送命令后返回的状态的对比较
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
函数功能：发送数据
data：发送的数据
status：记录发送的状态
ctr：控制发送的方式，
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
登录TCP服务端
*/
void openTCP(stMe3630Status *status)
{
	char csq=0;//判断信号强度
	stMe3630Ctr ctr;
	me3630_Open(status);
	Me3630GetSignal(&csq,status );
	while(csq==99)//没有信号的情况下等待信号恢复
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
me3630数据发送
data:发送数据
len：发送的数据长度
status：数据发送的状态
*/
void me3630_SendData(char *data,uint32_t len,stMe3630Status *status)
{
	stMe3630Ctr ctr;
	char len_char[4]={0};
	char data_buf[MAX_SENDDATA_NUM];
	char *p=0,*p_head=0;
loop: //数据没有发送成功重发数据
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
	添加服务器反馈确定是否重发数据
	*/
//	me3630SendCtr(&ctr,5,500 ,eAlwaysY);
//	me3630SendCommand(p,status,ctr);
}

/*
判断4G模块有没有开启成功
*/
static void me3630_Open(stMe3630Status *status)
{
	stMe3630Ctr ctr;
	me3630SendCtr(&ctr,5,500 ,eAlwaysY);
	me3630SendCommand(AT,status,ctr);
}

//GG
/*
将接收到的JSON的GGA反馈数据转换成本地的数据结构
*/
stMe3630reGgaData * me3630GgaJsonToStruct(cJSON *jsData )
{
	s2j_create_struct_obj(stData, stMe3630reGgaData);
	s2j_struct_get_array_element(stData, jsData, int, data);
	s2j_struct_get_basic_element(stData, jsData, int, length);
	s2j_struct_get_basic_element(stData, jsData, int, ret);
	s2j_delete_json_obj(jsData);//清除掉分配给Json的内存
	return stData;//返回数据结构体的地址
}
/*
解析GGA的数据
对发送GGA的数据反馈进行解析
GgaData:反馈回来的GGA数据
reData:返回给外部使用的数据
DataHead：数据的头
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
	pDataBuf=malloc(DataHead->DataLen);//分配对应长度的一段内存
	memcpy(pDataBuf, (char *)(GgaData+sizeof(stMe3630SendHead)),DataHead->DataLen);
//	if(200<DataHead->DataLen&&DataHead->DataLen<300)
//		while(1);
//	if(DataHead->DataLen>300)
//		while(1);
//	pDataBuf=malloc(strlen((char *)(GgaData+sizeof(stMe3630SendHead))));//发配对应长度的一段内存
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
	reGgaData=me3630GgaJsonToStruct(jsDataBuf);//返回对应的本地结构体
	memcpy(reData, (char *)(reGgaData),sizeof(stMe3630reGgaData));//将数据返回给外部
	s2j_delete_struct_obj(reGgaData);//清除结构体数据内存
	free(pDataBuf);
}
/*
对反馈数据的数据头进行解析
RXData:4G接收到的数据
DataHead 反馈解析出来的数据头
*/
eMe3630ApiLogo me3630RxDataHeaAnalysis(u8 *rxData,stMe3630SendHead *DataHead)
{
//	stMe3630SendHead Head;
#if DEBUG_ME3630
	printf("stMe3630SendHead  len:%d \r\n",sizeof(stMe3630SendHead));
#endif
	memcpy((char *)DataHead, (char *)(rxData), sizeof(stMe3630SendHead)); //将数据拷贝到内存中
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
解析函数
Data:4G接收到的数据
函数功能:
解析接收到数据
*/
#include "rtk.h"
void me3630RxAnalysis(u8 *Data,stMe3630Status *Status)
{
	stMe3630SendHead 	DataHead;
	stMe3630reGgaData	reGgaData;
	switch(me3630RxDataHeaAnalysis(Data,&DataHead))
	{
		//用于接受到数据后，反馈给数据发送方的确认收到消息包
		case   eApiLogoRequestResponse:  
			;
			break;
		//本地向服务器发送GGA数据:解析gga字符串，千寻服务
		case   eApiLogoGGA:  
			me3630RxAnalysisGga(Data,(u8 *)(&reGgaData),&DataHead,Status);
			if(reGgaData.length>0)
			{
				Status->bit.get_rtcm=eGetRtcmY;

				rtk_SendNumByte(reGgaData.data ,reGgaData.length);//将RTCM数据发送给rtk
#if 0//DEBUG_ME3630||DEBUG_TASK   //测试传给RTK的RTCM数据是什么
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
		//上传基站的状态信息：上传基站相关数据
		case   eApiLogoSysStatus:  ;
			break;
		default: break;
	}
}
/*
接收到的数据（解包）
*/

void  me3630RxUnpacking(stMe3630Status *Status)
{
	
//	u8 data_buf[MAX_EXTRACTION_DATA_LEN];
//	uint32_t data_buf=0;
	u8 **data_buf=0;
	uint32_t extraction_data_len=0;
	static uint16_t old_usart2_rx_len=0;
//	data_buf=malloc(MAX_EXTRACTION_DATA_LEN);//发配对应长度的一段内存
//	memset(data_buf,0,MAX_EXTRACTION_DATA_LEN*sizeof(char));
	if(USART2_RX_STA!=0) //数据没有满，数据没有读完
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
	添加解包函数
	*/
	
}
//

/*
包数据提取（拆包）
\r\n+ZIPRECV: 1,120.76.224.208,60000,180,
返回数据长度
*/
#define  HEAD_LEN  50
#define	 MAX_DATA_LEN_CHAR   4
uint32_t  me3630RxDataExtraction(u8 *src,u8 **des)
{

	u8 *p=0;
	u8 status=0;//数据查询的状态
	u8 head_len=0;//数据头的长度
	u8 len_flag=0;//标记查找到数据长度的字符没有
	u8 data_num=0;//标记数据现在到了那一位
//	u8 char_len[MAX_DATA_LEN_CHAR];//存放命令中的数据的长度的字符形式
	u8 *p_len_char=0;//记录长度字符的开头
	u8 len_char_num=0;//数据长度的表示字符站多少个字符
	uint32_t data_len=0;//数据的长度
	p=src;
	
	while(status!=1)
	{
		if((p[data_num++]=='\r')&&(p[data_num++]=='\n'))//数据的头两位是开头
		{
			for(head_len=0;head_len<HEAD_LEN;head_len++)//查找数据的长度
			{
				if(p[data_num++]==',')
				{
					len_flag=len_flag+1;

				}
				if(len_flag==3)break;
		
			}
			if(HEAD_LEN-1==head_len)
			{
				status=1;//数据不正常退出(数据头错误)
			}
			if(len_flag==3)//表示找到了数据中的第二个','到下一个','表示数据的长度
			{
				p_len_char=&p[data_num];//记录长度字符的开头
				for(head_len=0;head_len<MAX_DATA_LEN_CHAR;head_len++)
				{
					if(p[data_num++]==',')
					{
						break;
					}
					len_char_num=len_char_num+1;

				}
				
				data_len=character_to_uint32((char *)p_len_char ,len_char_num);
				
				*des=(u8*)malloc(data_len);//分配一段用作数据提取的数据空间			
			}
			
			//数据拷贝 void *memcpy(void *dest, const void *src, size_t n);
			if((p[data_num+data_len]=='\r')&&(p[data_num+data_len+1]=='\n'))
			{
				memcpy(*des, &p[data_num], data_len);
#if DEBUG_ME3630
				printf("me3630 get data :%s \r\n",*des);
#endif
				status=1;//数据正常退出
				return data_len;//返回数据的长度
			}else{
				status=1;//数据不正常退出（数据丢失）
			}
		}else{
			status=1;//数据不正常退出（没有数据头）
		}
	}

return 0;
}

/*
获取4G信号强度
data :反馈的信号强度数据
status:状态反馈
命令反馈的数据
AT+CSQ 
+CSQ: 17,99 
 
OK 

*/
#define CSQ_RETURN_DATA_LEN	12
void Me3630GetSignal(char *data,stMe3630Status *status )
{
	stMe3630Ctr ctr;
	char buf[CSQ_RETURN_DATA_LEN];//缓存反馈回来的数据
	char RssiBuf[2];//缓存信号强度
	char num=0;//
	u16	 ReturnNum=0;

	gCommandReturnNum=0;
	
	me3630SendCtr(&ctr,5,500 ,eAlwaysY);
	me3630SendCommand(CSQ,status,ctr);
	//数据发送成后解析反馈回来的数据
	ReturnNum=gCommandReturnNum;
	
	if(ReturnNum>CSQ_RETURN_DATA_LEN)
	{
		ReturnNum=ReturnNum-CSQ_RETURN_DATA_LEN;
		memcpy(buf, &USART2_RX_COMMAND[ReturnNum], CSQ_RETURN_DATA_LEN);
		//提取数据
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




