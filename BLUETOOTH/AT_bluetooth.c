//������ATָ������

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
	                      NULL,BL_MODE,{"OK","PARA SET:"},200, //��ʼ��������ƽṹ
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
											},0xff};        //CmdNum����Ϊ��Ч

/***���в�ѯ�����õ�AT����**************************************************************************/

/*��ѯ����ģ�� NAME
ִ��ָ �� AT+NAME=CdEbyte_MultiRole ���õ� ǰģ���� �֣����� �� 20 ���ַ� ���ɹ��� ���� PARA SET:
CdEbyte_MultiRole��
*/

void AtBlueNameCommand(char *name ,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://��ѯ
			p_return=strcat (p_return,BL_NAME);
		
			break;
		case   eBlCommandSetUp://����
			p_return=strcat (p_return,BL_NAME);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
			p_return=strncat(p_return,name,strlen(name));
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}

/*��ѯ���ô��ڲ�����
ִ��ָ�� AT+UART=19200,8,1,0 ���õ�ǰģ�����֣��ɹ��󷵻� PARA SET: 115200,8,1,0��
��һ�����ݴ������ʣ�1200��2400��4800��9600��19200��38400��57600��115200��128000��230400��256000��
460800��921600��
�ڶ������ݴ�������λ��5,6,7,8��
���������ݴ���ֹͣλ��0,1��
���ĸ����ݴ���У��λ��0(none)��1(even)��2(odd))��
*/

void AtBlueUartCommand(char * baud,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://��ѯ
			p_return=strcat (p_return,BL_UART);
			break;
		case   eBlCommandSetUp://����
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
/*��ѯ����ģ���ɫ
ִ��ָ�� AT+ROLE=0 ���õ�ǰģ���ɫΪ��ͨģʽ���ɹ��󷵻� PARA SET: Normal��
ִ��ָ�� AT+ROLE=1 ���õ�ǰģ���ɫΪ���ɫģʽ���ɹ��󷵻� PARA SET: MultiRole��
*/

void AtBlueRoleCommand(char * role,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://��ѯ
			p_return=strcat (p_return,BL_ROLE);
			break;
		case   eBlCommandSetUp://����
			p_return=strcat (p_return,BL_ROLE);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
			p_return=strncat(p_return,role,strlen(role));
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}
/*��ѯ����ģ�鷢�书��
ִ��ָ�� AT+TXPWR=2dbm ���õ�ǰģ�鷢�书�ʣ��ɹ��󷵻� PARA SET: AT+TXPWR=2dbm��
���У����õĹ��ʷ�ΧΪ��2dbm��1dbm��0dbm��-3dbm��-6dbm��-9dbm��-12dbm��-15dbm��-18dbm��-21dbm��
*/

void AtBlueRoleTxpwr(char * txpwr,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://��ѯ
			p_return=strcat (p_return,BL_TXPWR);
			break;
		case   eBlCommandSetUp://����
			p_return=strcat (p_return,BL_TXPWR);
			p_return=strncat(p_return,BL_EQUAL,strlen(BL_EQUAL));
		
			p_return=strncat(p_return,txpwr,strlen(txpwr));
			p_return=strcat (p_return,"dbm");
			p_return=strncat(p_return,BL_COMMAND_END,strlen(BL_COMMAND_END));
			break;
		 default: break;
	}
}
/*��ѯ�����豸 UUID
ִ��ָ�� AT+UUID=0,FFF0 ���õ�ǰģ�����֣��ɹ��󷵻� PARA SET: AT+UUID=0,FFF0��
��һ���������õڼ��� UUID���䷶Χ 0 ~ 3��*/

void AtBlueRoleUuid(char *param,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	switch(CommandMode)
	{
		case   eBlCommandInquire://��ѯ
			p_return=strcat (p_return,BL_UUID);
			break;
		case   eBlCommandSetUp://����
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
/*��ѯ���ù㲥���� ADVDATA
ִ�� AT+ADVDATA=0A00010203040506070809 ��ģ�齫���� PARA SET: 0x0A00010203040506070809��
��������Ĳ��� 0A ����Ҫ��������ݳ��ȣ�00010203040506070809 ������Ҫ��������ݡ��������ݰ�ʮ�����Ƹ�ʽ��
�룬�� 00 ���� 0x00��01 ���� 0x01��02 ���� 0x02�����ݳ��Ȳ����� 23 �ֽڡ�
*/
//10����ת��16����
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
//advdata���ܴ���23���ֽڵ��ַ���ʾ��16����
void AtBlueAdvdataUuid(char * advdata,char* return_command,eBlCommandMode CommandMode)
{
	char* p_return =return_command;
	char num=0;
	char len_buf[2];
	switch(CommandMode)
	{
		case   eBlCommandInquire://��ѯ
			p_return=strcat (p_return,BL_ADVDATA);
			break;
		case   eBlCommandSetUp://����
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
����������������
uuid��������Ҫ�����ĺ���
void(*StringCommand)(char * ,char* ,eBlCommandMode ) //���ô������ĺ���  

data��Ҫ���͵���������
*/
#define MAX_BL_AT_LEN 80 //�������󳤶�
void  BlSendSetUpCommand(void(*StringCommand)(char * ,char* ,eBlCommandMode ),char * data)
{
	char commandBuf[MAX_BL_AT_LEN];
	memset(commandBuf,0,MAX_BL_AT_LEN*sizeof(char)); 
	StringCommand(data,commandBuf,eBlCommandSetUp); //ƴ�������ַ���
	BlSendCommand(commandBuf);//������������

}
void  BlSendInquireCommand(void(*StringCommand)(char * ,char* ,eBlCommandMode ))
{
	char commandBuf[MAX_BL_AT_LEN];
	memset(commandBuf,0,MAX_BL_AT_LEN*sizeof(char)); 
	StringCommand("",commandBuf,eBlCommandInquire); //ƴ�������ַ���
	BlSendCommand(commandBuf);//������������
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
����һ������ȴ�״̬��ȷ
void(*StringCommand)(char * ,char* ,eBlCommandMode ):�����
data�����������
CompareEcho������Ƚϵ�����������
status���Ƚϵ�״̬
 
*/
#define   SEND_CMD_TIMEOUT  10000 //10s��ʱ
void BlSendSetUpCommandEcho(void(*StringCommand)(char * ,char* ,eBlCommandMode ),eBlCommandMode eMode,char * data ,stBlStatus  *status)
{
	 u32  timeout=SEND_CMD_TIMEOUT;
	 char *pCompData=NULL;
	u8  compnum=0;
	if(status->bit.rx_buff_analysis!=eBlRxBuffAnalysisIng)//�ж�������û�н�����
	{ 
		status->bit.command_send_status=eBlCommandSendN;
		do{
			BlRxDatabuffClear();
			BlCommandEchoBuffClear();
			ScanForCmd(StringCommand);
			if(eMode==eBlCommandInquire)
			BlSendInquireCommand(StringCommand);
			else if(eMode==eBlCommandSetUp)
		 	BlSendSetUpCommand(StringCommand,data);//��������
			//BlSendCommand("AT+NAME=SkyLin cane\r\n");
			pCompData=(char *)&gstCmdCtrl.stCmdCtrlBase[gstCmdCtrl.CmdNum].CompareStr[eMode][0];
			compnum=eMode?CMD_SET_STR_COMP_LEN:CMD_INQUIRE_STR_COMP_LEN;
			delay_ms(gstCmdCtrl.stCmdCtrlBase[gstCmdCtrl.CmdNum].DelayTime);
			BlCommandReturnAnalysis((char*)gCommandEchoBuff,status);
			if(memcmp(gCommandEchoBuff,pCompData,compnum)==0)//�������
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
����UUID��������
num_uuid:�ڼ���uuid
uuid:uuid��
*/
void  BlSendSetUpUuid(char * num_uuid,char *uuid)
{
	char commandBuf[MAX_BL_AT_LEN],  param[10]; 
	memset(commandBuf,0,MAX_BL_AT_LEN*sizeof(char));
  strncat(param,num_uuid,1);	
	strncat(param,",",1);
	strncat(param,uuid,strlen(uuid));
	AtBlueRoleUuid(param,commandBuf,eBlCommandSetUp);
	BlSendCommand(commandBuf);//������������

}
/*
һЩû�в������������һЩ��������
*/
void BlSendSwitchCommand(char *command)
{
	BlSendCommand(command);//������������
}
///*
//��������״̬��ѯ����
//*/
//void BlSendInquireCommand(char *command)
//{
//	BlSendCommand(command);//������������
//}






