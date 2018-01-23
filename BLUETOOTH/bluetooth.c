/*
������غ�������
*/
#include "uart5.h"
#include "delay.h"
#include "string.h"
#include "sys.h"
#include "bluetooth.h"
#include "debug.h"
#include "AT_bluetooth.h"
#include "MySys.h"
#include "my_task.h"
#include <stdlib.h>
#include "MySys.h"
#include "MySysStatus.h"
#include "radio.h"
#include "HX_DU1018D.h"
/*
����������Ϊ����ģʽ��͸��ģʽ
1������ģʽ��ÿһ�η�������ǰ�����ݻ���ռ���յȴ���������Ļ��ԣ��ж������Ƿ��ͳɹ���
��1����������û�����ݽ�������ģʽ����
��2���������������ݽ�������ģʽ������ȡ�����ݺ󣬽����ݻ���ռ�����ٷ������

2��͸��ģʽ
��1��������ģʽ���ȥ͸��ģʽ����յ������е����ݣ���ʼ�������ݣ�
��2�������������ݴ��䣨���������ݵ�ѭ������һ�����ݽ�����������һ�������ݣ�

3��ÿ�ο���ģ���
��1��DIO_6�������ͣ�����RX����ģʽ
��2���ȴ�������������DIO_8����͵�ƽ
��3�����͡�+++���õ�CMD IN��ʾ������ģʽ  ��CMD OUT͸��ģʽ
*/


/*
ȫ�ֱ�������
*/
stBlStatus  gBlStatus;
char		gCommandEchoBuff[MAX_BL_COMMAND_ECHO];//�洢������Ե�buff
/*
��������
*/
extern char* pszJsonText;
//extern u8 ParseScanCodeJson(const char* pszJsonText);
extern int json_test(void) ;
extern void  eeprom_test(void);

extern void  sdram_test(void);
extern void  w25qxx_test(void);

typedef	struct{

	
	u8	dataL:4;
	u8	dataH:4;
}stTest;

typedef union{
	u8		data;
	stTest	bit;
}stTestUn;
void test_bluetooth(void)
{
	stBlStatus  status;
	u16 num=0;
	stTestUn test_data;
	test_data.bit.dataL=0xf;
	test_data.bit.dataH=0x0;
	
//	uart5_init(BLE_UART_BOUND);	
//	for(num=0;num<strlen(PENETRATE_MODE_RETURN);num++)
//	{
//		gCommandEchoBuff[num]=PENETRATE_MODE_RETURN[num];
//		printf("%c",gCommandEchoBuff[num]);
//	}
//	printf("\r\n");
	BlTest();
	BlInit(&status);
	while(1)
	{
		if(num==100)
		{
			num=0;
//			printf("------0-------\r\n");
//			printf("%c",test_data.data);

//			crc_test();
			w25qxx_test();
//			sdram_test();
//			eeprom_test();
//			oled_test();
//			json_test() ;
//			ParseScanCodeJson(pszJsonText);
//			printf("------1-------\r\n");
//			UART5_SendBytes("0123456789\r\n");
//			BluetoothAnalysis(&a);
		}
		num++;

		delay_ms(1);
	}
}
/*
״̬���ݳ�ʼ��
*/
void BlStatusInit(stBlStatus  *status)
{
	status->data=0;
	status->data=~status->data;
}

/*
	��ʼ�������ȴ���������
	�ȴ��豸����
	�л�������͸��ģʽ
*/
void BlInit(stBlStatus  *status)
{
	BlStatusInit(status);
	
	BluetoothGPIO();//��ʼ��IO��
	BlReset();
	BL_MRDY_PIN_RESET();//�л���RX����ģʽ
	uart5_init(19200);	//����Ĭ��115200
//	BlSendSetUpCommandEcho(AtBlueUartCommand,"19200","PARA SET: 115200,8,1,0",status);
////	BlSendSetUpCommand(AtBlueUartCommand,"19200");//�����������ڵĲ�����
//	uart5_init(19200);	//����19200 ������͸��ģʽ���������
	
	BlSendSetUpCommandEcho(AtBlueRoleTxpwr,eBlCommandSetUp,"-3",status);
//	BlSendSetUpCommandEcho(AtBlueUartCommand,eBlCommandSetUp,"9600",&status);
	
	
	//BlSendSetUpCommand(AtBlueRoleTxpwr,"-3");
//	BlSendSetUpUuid("1","1111");
	//�ȴ���������
	do{
		if(!BL_CONNECTION_STATUS())//���豸������
		{
				#if DEBUG_BLUE				
					printf("BLE is  connected   \r\n");
				#endif
			BlUPenetrateMode(status);//�л���͸��ģʽ
			status->bit.connection=eBlConnectionY;
		}else{
				#if DEBUG_BLUE
					printf("BLE is not connected   \r\n");
				#endif
			status->bit.connection=eBlConnectionN;
			delay_ms(1000);
		}
	}while(status->bit.connection==eBlConnectionN);
}
/*
�����й����ж����ˣ��ȴ����½�������

*/
#define deBlReconnectionTimeOut   20  //�ȴ������ĳ�ʱʱ��
void BlReconnection(stBlStatus  *status)
{
	uint16_t time_out=0;
		//�ȴ���������
	do{
		if(!BL_CONNECTION_STATUS())//���豸������
		{
			BlUPenetrateMode(status);//�л���͸��ģʽ
			status->bit.connection=eBlConnectionY;
		}else{
			status->bit.connection=eBlConnectionN;
			delay_ms(300);
			time_out++;
			
			if(time_out>deBlReconnectionTimeOut)
			{
				time_out=0;
				
				BlSendSwitchCommand(BL_RESET);//���λ(�����λ)
			}
		}
	}while(status->bit.connection==eBlConnectionN);
}



/*
�л�������ģʽ(��������ģʽ)
*/
void BlUCommandMode(stBlStatus  *status)
{
	if(status->bit.rx_buff_analysis!=eBlRxBuffAnalysisIng)//�ж�������û�н�����
	{
		BlRxDatabuffClear();
		BlCommandEchoBuffClear();
	do{
			BlSendSwitchCommand(BL_MODE);
			BlCommandReturnAnalysis((char*)gCommandEchoBuff,status);
			if(strcmp(gCommandEchoBuff,COMMAND_MODE_RETURN)==0)//�������
			{
				status->bit.mode=eBlUPenetrateMode;
				#if DEBUG_BLUE				
					printf("Into the command mode  success\r\n");
				#endif			
			}else{
				#if DEBUG_BLUE				
					printf("Into the command mode	failure\r\n");
				#endif		
			}
			delay_ms(10);
		}while(status->bit.mode==eBlUPenetrateMode);
	}else{
		delay_ms(10);
	}
}
/*
�л���͸��ģʽ(��������ģʽ)
*/
char gTestFlag =0;
void BlUPenetrateMode(stBlStatus  *status)
{
	if(status->bit.rx_buff_analysis!=eBlRxBuffAnalysisIng)//�ж�������û�н�����
	{
		BlRxDatabuffClear();
		BlCommandEchoBuffClear();
	do{
			BlSendSwitchCommand(BL_MODE);
			delay_ms(2000);
			BlCommandReturnAnalysis((char*)gCommandEchoBuff,status);
			if(strcmp(gCommandEchoBuff,PENETRATE_MODE_RETURN)==0)//�������
			{
				status->bit.mode=eBlUPenetrateMode;
				
				gTestFlag=1;
				
				#if DEBUG_BLUE				
					printf("Into the Penetrate mode  success\r\n");
				#endif			
			}else{
				#if DEBUG_BLUE				
					printf("Into the Penetrate mode	failure\r\n");
				#endif		
			}
		}while(status->bit.mode!=eBlUPenetrateMode);
	}else{
		delay_ms(10);
	}
}

/*
����ģʽ�µ�����ش����ݻ�ȡ
*/
void BlCommandReturnAnalysis(char *return_data,stBlStatus *BlStatus)
{
	uint16_t num=0;
	if(BLUETOOTH_RX_BUF[0]!='\0')//��ʾ������
	{
		for(num=0;num<BLUETOOTH_REC_LEN;num++)
		{
			if((BLUETOOTH_RX_BUF[num]!=BL_SEND_COMMAND_ECHO_1)&&(BLUETOOTH_RX_BUF[num+1]!=BL_SEND_COMMAND_ECHO_2))
			{
				return_data[num]=BLUETOOTH_RX_BUF[num];
			}else{
				
				BlStatus->bit.command_echo=eBlCommandEchoY;
				return_data[num]='\0';
				BlRxDatabuffClear();
				return;
			}
		}
		//��Ȼ�����ݽӿڵ���û���ҵ����ݵĽ�β������ȡ����
		BlStatus->bit.command_echo=eBlCommandEchoN;
		BlCommandEchoBuffClear();
		BlRxDatabuffClear();
		return;
	}
	BlRxDatabuffClear();

}
/*
��������Ļ������ݻ���ռ�
*/
void  BlCommandEchoBuffClear(void)
{
	memset(gCommandEchoBuff,0,MAX_BL_COMMAND_ECHO*sizeof(char));

}
/*
������������ݻ���ռ�
*/
void BlRxDatabuffClear(void)
{
	BLUETOOTH_UART_NUM=0;
	memset(BLUETOOTH_RX_BUF,0,BLUETOOTH_REC_LEN*sizeof(char));
}
/*
�����յ�����ת�����
src:��Ҫת�������
data_return��ת��������
data_len:��ת������ݳ���
*/

void BLDataEscapedAnalysis(char *src,char *data_return,uint16_t data_len)
{
	uint16_t num=0;//�������λ��
	for(num=0;num<data_len;num++)
	{
		if(*(src+num)==ESCAPE_CHARACTER)
		{
			num+=1;
			*(data_return++)=XOR_VALUE^*(src+num);
		}else{
			*(data_return++)=*(src+num);
		}
	}
}
/*
num��������������е�λ��
data_status:���ݽ���״̬���
*/
#define BluetoothAnalysisCopyTimeOut 200
uint32_t BluetoothAnalysisCopy(char **data_return,uint32_t  num,u8  *data_status,stBlStatus *BlStatus)
{
	stBlDataHead head;
	uint16_t data_len=0;
	uint16_t data_all_len=0;//��������ܳ�(ת����)
	uint16_t data_len_flag=0;
	uint16_t data_head_len=0;//����ͷ�ĳ���
#if 1
	uint16_t time_out=0 ;
	uint16_t data_Escaped_len=0;//�жϽ��յ������ݳ���
	if((num<BLUETOOTH_UART_NUM)&&(BLUETOOTH_RX_BUF[num]==BLUETOOTH_DATA_HEAD))
	{
		delay_ms((u32)(10));	//�ȴ�1
		//ȡ������ͷ
		data_head_len=sizeof(stBlDataHead);
		memset(&head,0,sizeof(stBlDataHead));
		memcpy(&head,&BLUETOOTH_RX_BUF[num+1],data_head_len);
		//���ڹ��������ݳ���
		data_Escaped_len=data_head_len+head.DataEscapedLen;
		//�������ݿռ�(���ת�������ݿռ�)
		data_all_len=data_head_len+head.DataLen;//��1�ǰ����Ľ�������
		*data_return=(char *)malloc(data_all_len);//�����Ӧ���ȵĿռ�
		memset(*data_return,0,data_all_len);//��շ�����ڴ�
		
		BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisIng;//���ڿ�ʼ�������ݣ�����û�н������
		//�ȴ����ݽ��ս���
		while(BLUETOOTH_RX_BUF[num+data_Escaped_len+1]==0&&time_out<BluetoothAnalysisCopyTimeOut)//1S�ĳ�ʱʱ��
		{
			delay_ms((u32)(1));	//�ȴ�1
			time_out++;
		}
		//���ݽ��ս���
		if(BLUETOOTH_RX_BUF[num+data_Escaped_len+1]==BLUETOOTH_DATA_END)	
		{
			//��ȡ��������ת��
			BLDataEscapedAnalysis((char *)&BLUETOOTH_RX_BUF[num+1],*data_return,data_Escaped_len);
			BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisY;
			//��num�����
//			memset(&BLUETOOTH_RX_BUF[num],0,data_Escaped_len+1);//��յ�ԭ��������
//			num=num+data_Escaped_len;
			
		}else{
			BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisN;
		}
		
//		memset(&BLUETOOTH_RX_BUF[num],0,data_Escaped_len+1);//��յ�ԭ��������
//		num=num+data_Escaped_len+1;
		num=0;
		BlRxDatabuffClear();//���ݽ����ɹ������ԭ�������ݣ����½����µ�����
	}

	return num;
	
#else	
	
	
	if((num<BLUETOOTH_UART_NUM)&&(BLUETOOTH_RX_BUF[num]==BLUETOOTH_DATA_HEAD))
	{
		//ȡ������ͷ
		memset(&head,0,sizeof(stBlDataHead));
		memcpy(&head,&BLUETOOTH_RX_BUF[num+1],sizeof(stBlDataHead));
		*data_return=(char *)malloc((sizeof(stBlDataHead))+head.DataLen);
		data_all_len=sizeof(stBlDataHead)+head.DataLen;
		memset(*data_return,0,(sizeof(stBlDataHead))+head.DataLen);
		BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisIng;//���ڿ�ʼ�������ݣ�����û�н������
		//��ʼ��������
		while((num<BLUETOOTH_UART_NUM)&&(num<BLUETOOTH_REC_LEN))
		{
#if 0
			++data_len_flag;
			if((BLUETOOTH_RX_BUF[++num]==BLUETOOTH_DATA_END)&&(data_len_flag==data_all_len+1))
			{
				BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisY;//��ʾ���ݽ������
				break;
			}else{
			
				*(*data_return+data_len++)=BLUETOOTH_RX_BUF[num];
				delay_us((u32)(400));	//0.01ms
			}
#else

			if(BLUETOOTH_RX_BUF[++num]!=BLUETOOTH_DATA_END)//BLUETOOTH_RX_BUF[++num]!=BLUETOOTH_DATA_END
			{			
				
				#if DEBUG_BLUE				
//					printf("%x ",BLUETOOTH_RX_BUF[num]);			
				#endif
				*(*data_return+data_len++)=BLUETOOTH_RX_BUF[num];
			}else{
				BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisY;//��ʾ���ݽ������
				#if DEBUG_BLUE				
					printf("\r\n");
				#endif			
				BlRxDatabuffClear();//���ݽ����ɹ������ԭ�������ݣ����½����µ�����
				break;
			}			
			delay_us((u32)(10));	//0.01ms

			
#endif	
			
		}
		BlRxDatabuffClear();//���ݽ����ɹ������ԭ�������ݣ����½����µ�����
		if(BlStatus->bit.rx_buff_analysis!=eBlRxBuffAnalysisY)
		{
			BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisN;
		}
	}else{
		delay_us((u32)(10));	//0.01ms
	}
#endif
	return num;
}

/*
������е����ݽӿڽ���
*/
void BluetoothAnalysis(char **data_return,stBlStatus *BlStatus)
{
	static uint32_t  num=0;
	u8  data_status=0;  //��һλ��ʾ����Ϊ��  �ڶ�λ��ʾ���ݽ������д�����  

	if((BLUETOOTH_RX_STATUS&0x1)==0)//��ʾ���ڽ��ջ�û�г������ڻ���BUF
	{
		for(num=0;num<BLUETOOTH_UART_NUM;num++)
		{
			num=BluetoothAnalysisCopy(data_return,num,&data_status,BlStatus);
			
		}
//		num=0;
	}else if((BLUETOOTH_RX_STATUS&0x1)==1)
	{//��ʾ���ڽ��ճ������ڻ���BUF
		if((num<BLUETOOTH_REC_LEN)&&(num>BLUETOOTH_UART_NUM))//��ʾ���ݸ����˵��µ����ݻ�û��׷�Ͻ���������
		{
			for(;num<BLUETOOTH_REC_LEN;num++)
			{
				num=BluetoothAnalysisCopy(data_return,num,&data_status,BlStatus);
				
			}
//			num=0;
		}else if(((num<BLUETOOTH_REC_LEN)&&(num<BLUETOOTH_UART_NUM))||(num==BLUETOOTH_REC_LEN))//��ʾ���ݸ����˵��µ�����׷�Ͻ���������
		{
			
			BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisN;
			memset(&BLUETOOTH_RX_BUF[BLUETOOTH_UART_NUM],0,(BLUETOOTH_REC_LEN-BLUETOOTH_UART_NUM)*sizeof(char));
			num=0;
			BLUETOOTH_RX_STATUS&=~BLUETOOTH_RX_STATUS;
		}
	}
}
/*
������������ſ��ƺ�״̬��ص����ų�ʼ��
*/
void BluetoothGPIO(void )
{
	GPIO_InitTypeDef GPIO_Initure;
	BL_MRDY_RCC();           		//����GPIOBʱ��
	BL_CONNECTION_RCC()	;
	//DIO_6 ����/��� MRDY ���ţ����ڴ���ģ��Ĵ��ڽ��չ��ܣ��͵�ƽ���գ�
	GPIO_Initure.Pin=BL_MRDY_PIN; 					//
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	
	HAL_GPIO_Init(BL_MRDY_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_RESET);	
	//DIO_8 ����/��� ����ָʾ����ģ�齨�����Ӻ�����͵�ƽ
	GPIO_Initure.Pin=BL_CONNECTION_PIN; 		//
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  		//����ģʽ
	HAL_GPIO_Init(BL_CONNECTION_GPIO,&GPIO_Initure);
	//nRESET ���� ��λ�ţ��ڲ������������ CC2650 �ֲᣩ
	BL_RESET_RCC();
	GPIO_Initure.Pin=BL_RESET_PIN; 					//
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
	GPIO_Initure.Pull=GPIO_PULLUP;          //����
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
	
	HAL_GPIO_Init(BL_RESET_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_SET);	
	
}
/*
Ӳ����λ
*/
void BlReset(void)
{
	BL_RESET_PIN_RESET();
	delay_ms(20);
	BL_RESET_PIN_SET();
}
/*
�������ֻ������ݣ�(������)����ģʽ���������ͣ�
1���豸���ͣ��ֳ֡���վ���豸�����ͣ���̨�豸��ʲô�豸�����豸�ı�ţ���̨�豸�ǵ�	����ͬ���͵��豸��
2��GGA���ݣ������ֻ���ΪRTCM���ݵķ�����
3����̨��������ã����ŵ��Ķ�ȡ��
4��ϵͳ�Ĺ���״̬������Ϊϵͳ�ļ��:���ֻ��˼�أ�


�ֻ������������ݣ���д���ݣ�����ģʽ���������ͣ�
1����̨��������ã����ŵ����ã�
2����վ�ֳ���ģʽ�л���1��8BIT�����ݣ�
	{
		��0������ʾ��վģʽ
		��1������ʾ�ֳ���ģʽ
	}
3��RTCM����

*/
/*
ͨ���������������ݵ��ֻ�
����һ������
data�����͵�����
data_len���������ݵĳ���
BlStatus��״̬����
DataHead:���ݵ�ͷ�������ͣ�ͨ������BlDataType�������ݵ����ͣ�
���ҷ����ֻ��˵����ݶ����������ݡ�
*/

void BlSendDataPack(void* data,u16 data_len, stBlDataHead *DataHead)
{
#if 1
	char	*p=0;
	char	*p_escape=0; //p_escape����ת����ڴ�ռ�
	char head = BLUETOOTH_DATA_HEAD,end = BLUETOOTH_DATA_END;
	uint16_t num_src=0;//��¼ԭʼ���ݲ��ҵ��ڼ�λ
	uint16_t num_escape=0;//��¼ת�������ݲ��ҵ��ڼ�λ
	uint16_t data_all_len=0;
	data_all_len=2+data_len*2+sizeof(stBlDataHead);//data_len*2 //��ת���ַ�Ԥ����һ�οռ�
	p=(char*)malloc(data_all_len);
	memset(p,0,data_all_len);
	//����һ��������Ϊת����ڴ�ռ�
//	p_escape=(char*)malloc(data_len*2);
//	memset(p_escape,0,data_len*2);
	
	
	#if 1
//		DataHead->DataLen=data_len;
	
		DataHead->DataTypes.data=sw32_uint32(DataHead->DataTypes.data);

		memcpy(p,&head,sizeof(head));
		
		for(num_src=0,num_escape=0;num_src<data_len;num_src++,num_escape++)
		{
			if(*((char *)data+num_src)==ESCAPE_CHARACTER||*((char *)data+num_src)==BLUETOOTH_DATA_HEAD||*((char *)data+num_src)==BLUETOOTH_DATA_END)
			{
				*(char*)(p+1+sizeof(stBlDataHead)+num_escape)=ESCAPE_CHARACTER;
				num_escape+=1;
				*(char*)(p+1+sizeof(stBlDataHead)+num_escape)=XOR_VALUE^*((char *)data+num_src);
				
			}else{
				*(char*)(p+1+sizeof(stBlDataHead)+num_escape)=*((char *)data+num_src);
			}
		}
//		DataHead->DataEscapedLen=num_escape;//ת�������ݵĳ���
		
//		memcpy(p+1+sizeof(stBlDataHead),(char *)data,data_len);
		
//		memcpy(p+1+sizeof(stBlDataHead)+data_len,&end,sizeof(end));
		memcpy(p+1+sizeof(stBlDataHead)+num_escape,&end,sizeof(end));
		data_all_len=data_all_len-data_len*2+num_escape;
		DataHead->DataLen=sw32_uint32(data_len);
		DataHead->DataEscapedLen=sw32_uint32(num_escape);
		memcpy(p+1,(char *)DataHead,sizeof(stBlDataHead));

		BlSendDataLen((char *)p,data_all_len);//����һ�����ȵ�����
	#endif
	
	#if 0
		p=strcat (p,"$");
		memcpy(p+1,(char *)DataHead,sizeof(stBlDataHead));
		p=strncat(p+2+sizeof(stBlDataHead),"\0",1);
		#if DEBUG_BLUE	
			u8 test_num=0;
			printf("head=");
			for(test_num=0;test_num<sizeof(stBlDataHead);test_num++)
				printf("%x,",*((char *)DataHead+test_num));
		#endif
		p=strncat(p,(char *)data,data_len);
		p=strncat(p,"@\0",2);
		BlSendDataLen((char *)p,data_all_len);//����һ�����ȵ�����
	#endif
	#if DEBUG_BLUE	
		printf("data_all_len=%d\r\n",data_all_len);
		printf("data=%s\r\n",p);
	#endif
	free(p);
#endif

#if 0
	BlSendByte(BLUETOOTH_DATA_HEAD);//����ͷ
	
	DataHead->DataLen=data_len;
	BlSendDataLen((char *)DataHead,sizeof(stBlDataHead));//��������������Ϣ
	
	BlSendDataLen((char *)data,data_len);//����һ�����ȵ�����
	BlSendByte(BLUETOOTH_DATA_END);	//����β
//#if DEBUG_BLUE	
//	printf("data_all_len=%d\r\n",data_all_len);
//	printf("data=%s\r\n",p);
//#endif
#endif
}

/*
�������ݵ�����ͷƴ��
RequestDataTypes :�������������
DataHead//���ص�����ͷ
*/
void BlRequestDataHead(eBlRequestDataTypes	RequestDataTypes,stBlDataHead *DataHead  )
{
	DataHead->DataTypes.bit.DataMode=eBlDataRequestMode;
	DataHead->DataTypes.bit.DataTypes=BlDataTypes(RequestDataTypes);//RequestDataTypes;//BlDataTypes(RequestDataTypes);
}
/*
�������ݵ�����ͷƴ��
eBlSetUpDataTypes :�������������
DataHead//���ص�����ͷ
*/
void BlSetStatusDataHead(eBlSetUpDataTypes	SetUpDataTypes,stBlDataHead *DataHead  )
{
	DataHead->DataTypes.bit.DataMode=eBlDataSetStatusUpMode;
	DataHead->DataTypes.bit.DataTypes=BlDataTypes(SetUpDataTypes);//RequestDataTypes;//BlDataTypes(RequestDataTypes);
}
/*
������������
BlDataType:�������BlSendDataPack����ʹ��
ƴ�Ӷ�Ӧ������ͷ
*/
void BlDataType(
				eBlDataMode	DataMode,
				eBlSetUpDataTypes	SetUpDataTypes,
				eBlRequestDataTypes	RequestDataTypes ,
				stBlDataHead *DataHead //���ص�����ͷ
			)
{
	DataHead->DataTypes.bit.DataMode=DataMode;
	if(DataMode==eBlDataRequestMode)
	{
		DataHead->DataTypes.bit.DataTypes=BlDataTypes(RequestDataTypes);
	}else if(DataMode==eBlDataSetUpMode)
	{
		DataHead->DataTypes.bit.DataTypes=BlDataTypes(SetUpDataTypes);
	}
}
/*
���յ������ݣ�����ͷ��ȡ
�����ݽ����ж�����ӱ�־
data:���յ�������
head������ͷ
status�����ݵ�״̬
*/
void BlReceiveDataGetHead(char** data,stBlDataHead *head ,stBlStatus *status)
{

	memcpy(head,*data,sizeof(stBlDataHead));
	
}
/*
��������ID:���ݷ��ظ��ֻ�
���������ID
*/
#define BL_BlRequest_DeviceId_TEST	 1


void BlRequest_DeviceId(stBlDataHead *BlDataHead)
{
	BlRequestDataHead(eBlDataRequestTypes_DeviceId,BlDataHead);//ƴ������ͷ
#if	BL_BlRequest_DeviceId_TEST
	BlSendDataPack("0123456789",10, BlDataHead);
	#if DEBUG_BLUE	
		printf("Bl Request Device Id\r\n");
	#endif
	
#else 
	
#endif

}
/*
����GGA����:���ݷ��ظ��ֻ�
*/
#include "rtk.h"
#define BL_BlGGATestDat_TEST 0
#if BL_BlGGATestDat_TEST	
char *BlGGATestDat={"$GPGGA,000001,3112.518576,N,12127.901251,E,1,12,1,0,M,-32,M,3,0*4B"};
#endif

void BlRequest_GGA(stBlDataHead *BlDataHead,stRtkStatus		*RtkStatus )
{
	BlRequestDataHead(eBlDataRequestTypes_GGA,BlDataHead);
#if BL_BlGGATestDat_TEST	
	BlSendDataPack(BlGGATestDat,strlen(BlGGATestDat),BlDataHead);
#else
	if(RtkStatus->bit.get_gga==eGetGgaY)//�������ȷ��GGA����
	{
		BlSendDataPack(gSysGgaDataBuffer,strlen(gSysGgaDataBuffer),BlDataHead); //gGgaDataBuffer:��ϵͳȥȫ�ֱ�����������ΪGGA���ݵĻ�����
		#if DEBUG_BLUE	
			printf("Bl Request  GGA\r\n");
		#endif
	}
#endif	
	
}
/*
��̨�ŵ�����
*/
void BlRequest_RadioChannel(stBlDataHead *BlDataHead,stRadioCommandParameter *parameter,stDu1018dStatus *Du1018dStatus)
{
	
#if RADIO_E71
		u8 channel;
		channel=RadioChannelRead(parameter);
		BlRequestDataHead(eBlDataRequestTypes_Channel,BlDataHead);
		BlSendDataPack(&channel,sizeof(channel),BlDataHead);
#endif
#if RADIO_DU1018D
	u8 channel;
	channel=Du1018dStatus->bit.Channel;
	
//	Du1018dReadChannel(&channel,Du1018dStatus);
	BlRequestDataHead(eBlDataRequestTypes_Channel,BlDataHead);
	BlSendDataPack(&channel,sizeof(channel),BlDataHead);

#endif	

}
/*
���������������״̬����
*/
void BleSetStatus(eBlSetUpDataTypes	SetUpDataTypes)
{
	stBlDataHead DataHead;
	BlSetStatusDataHead(SetUpDataTypes,&DataHead);
	BlSendDataPack(0,0,&DataHead); 
}

/*
���ݲ��
*/

void BleSetRtkPosition(char *src_data,stBlDataHead *head,stRtkStatus *RtkStatus);
void BlSysModeSwitch(char *src_data,stBlDataHead *head,stBlStatus *BlStatus);
void  BleSetRadioChannel(char *src_data,stBlDataHead *head);
typedef void (*BlSendDataCallback)(char *,stBlDataHead *);//�������ݸ��ֻ������ݻص���������
char gBlRtcmOpenFlag =0;

void BlReceiveDataUnpacking( char** data,stBlStatus *status,stRtkStatus *RtkStatus)
{
	stBlDataHead BlDataHead;
	BlReceiveDataGetHead(data,&BlDataHead,status); //ȡ�����ݵ�ͷ
	switch(BlDataHead.DataTypes.bit.DataMode) //�ж����������ݻ�����������
	{
		case	eBlDataRequestMode://����ģʽ	
			switch (BlDataHead.DataTypes.bit.DataTypes)
				{	
					case	BlDataTypes(eBlDataRequestTypes_DeviceId)://eBlDataRequestTypes_DeviceId		://�����豸ID��
							BlRequest_DeviceId(&BlDataHead);
							break;
					case	BlDataTypes(eBlDataRequestTypes_GGA)				://gga����
						BlRequest_GGA(&BlDataHead,RtkStatus);
//						gBlRtcmOpenFlag=1;
						break;
					case	BlDataTypes(eBlDataRequestTypes_Channel)			://�ŵ�
						BlRequest_RadioChannel(&BlDataHead,&gRadioCommandParameter,&gDu1018dStatus);
						break;
					case	BlDataTypes(eBlDataRequestTypes_SysStatus)	://ϵͳ״̬
						
						break;
					default: break;
			}	
			break;
		case	eBlDataSetUpMode://����ģʽ
				switch (BlDataHead.DataTypes.bit.DataTypes)
				{	
					case	BlDataTypes(eBlDataSetUpTypes_Channel)		:	//��̨��������ã����ŵ����ã�
						BleSetRadioChannel(*data,&BlDataHead);
						BleSetStatus(eBlDataSetUpTypes_Channel);
						break;
					case	BlDataTypes(eBlDataSetUpTypes_ModeSwitch)	://��վ�ֳ���ģʽ�л�
						BlSysModeSwitch(*data,&BlDataHead,status);
						BleSetStatus(eBlDataSetUpTypes_ModeSwitch);
						break;
					case	BlDataTypes(eBlDataSetUpTypes_Rtcm)			://RTCM����
						gBlRtcmOpenFlag=1;
						BleSetStatus(eBlDataSetUpTypes_Rtcm);
						#if DEBUG_BLUE	
							printf("Ble Set Rtk Position\r\n");
						#endif
						 //BlRTCMSet(*data,&BlDataHead,status);
						break;
					case	BlDataTypes(eBlDataSetUpTypes_SelfCalibration)			://��У׼ģʽ
						RtkSelfCalibration(RtkStatus);
						BleSetStatus(eBlDataSetUpTypes_SelfCalibration);
						break;
					case	BlDataTypes(eBlDataSetUpTypes_SetPosition)			://���û�վ�����ֻ�������
						
						BleSetRtkPosition(*data,&BlDataHead,RtkStatus);
						printf("BleSetRtkPosition\r\n");
						BleSetStatus(eBlDataSetUpTypes_SetPosition);
						printf("BleSetStatus\r\n");
						break;
					default: break;
			}
			break;		
		default: break;
	}
}
/*
���õ�̨�ŵ�
*/

void  BleSetRadioChannel(char *src_data,stBlDataHead *head)
{
#if RADIO_E71
	char *pDataHead=0;//���ݵĿ�ʼ��ַ
	uint8_t channel=0;
	pDataHead=(char *)(src_data+(sizeof(stBlDataHead)));
	channel=*pDataHead;
	RadioChannelSet(channel,&gRadioCommandParameter,&gRadioStatus);
	#if DEBUG_BLUE	
		printf("Ble Set Radio Channel %d \r\n",channel);
	#endif
#endif
#if RADIO_DU1018D
	char *pDataHead=0;//���ݵĿ�ʼ��ַ
	pDataHead=(char *)(src_data+(sizeof(stBlDataHead)));
	Du1018dSetChannel((u8*)pDataHead,head->DataEscapedLen,&gDu1018dStatus);
	#if DEBUG_BLUE	
		printf("Ble Set Radio Channel %d \r\n",character_to_uint32(pDataHead ,head->DataEscapedLen));
	#endif	
#endif	

}
/*
ͨ���������û�վ��Position��Ϣ
data:������������Ϣ
*/
void BleSetRtkPosition(char *src_data,stBlDataHead *head,stRtkStatus *RtkStatus)
{
	stRtkBestposData BestposData;//������Ϣ
	u8 data_num=0;
	char *pDataHead=0;//���ݵĿ�ʼ��ַ
	memset((char *)&BestposData ,0,sizeof(stRtkBestposData));
	pDataHead=(char *)(src_data+(sizeof(stBlDataHead)));
	
	while((MAX_BESTPOS_DATA_LEN>data_num)&&(*(pDataHead)!=','))
	{
		BestposData.latitude[data_num++]=*(pDataHead++);//γ��
	}
	pDataHead++;
	data_num=0;
	while((MAX_BESTPOS_DATA_LEN>data_num)&&(*(pDataHead)!=','))
	{
		BestposData.accuracy[data_num++]=*(pDataHead++);//����
	}
	pDataHead++;
	data_num=0;
	while((MAX_BESTPOS_DATA_LEN>data_num)&&(*(pDataHead)!=','))
	{
		BestposData.heigh[data_num++]=*(pDataHead++);//�߶�
	}
	data_num=0;
	RtkSendPosition(&BestposData,RtkStatus);
	#if DEBUG_BLUE	
		printf("Ble Set Rtk Position\r\n");
	#endif
}



/*
����RTCM����
*/

void BlRTCMSet(char *src_data,stBlDataHead *head,stBlStatus *BlStatus)
{
	static uint16_t pack_num=0;
#if DEBUG_BLUE	
		uint16_t num=0;
#endif	
	if(BlStatus->bit.rx_buff_analysis==eBlRxBuffAnalysisY)//��ʾ������ȡ�ɹ�
	{
		rtk_SendNumByte((char *)(src_data+(sizeof(stBlDataHead))) ,head->DataLen);//��RTCM���ݷ��͸�rtk
#if DEBUG_BLUE	
		
		printf("Bl rtcm data len %d :pack_num=%d\r\n",head->DataLen,pack_num++);
//		printf("Bl rtcm send ok  \r\n");
//		for(num=0;num<head->DataLen;num++)
//		{
//			printf("%x,",*((uint8_t *)(src_data+(sizeof(stBlDataHead)))+num));
//		}
//		printf("\r\n");
		
#endif	
	}else{
#if DEBUG_BLUE	
		printf("BL data is not taken\r\n");
#endif		
	}
}
	
/*
����ģʽ�л�
	src_data:����Դ����
	head������ͷ
	BlStatus������������״̬
*/
	
void BlSysModeSwitch(char *src_data,stBlDataHead *head,stBlStatus *BlStatus)
{
	char WorkMode=0;
	if(BlStatus->bit.rx_buff_analysis==eBlRxBuffAnalysisY)//��ʾ������ȡ�ɹ�
	{
		memcpy((char *)&WorkMode,(char *)(src_data+(sizeof(stBlDataHead))) ,head->DataLen);//�����ݿ�����ϵͳ״̬ȫ�ֱ����У�
		gMySysStatus.bit.WorkMode=WorkMode;
#if DEBUG_BLUE	
		if(gMySysStatus.bit.WorkMode==eMySys_BaseStationMode)
		{
			printf(" Base Station Mode \r\n");
		}else if(gMySysStatus.bit.WorkMode==eMySys_HandheldMode)
		{
			printf(" Handheld Mode \r\n");
		}

#endif	
	}else{
#if DEBUG_BLUE	

#endif		
	}
}
