/*
蓝牙相关函数设置
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
蓝牙解析分为命令模式和透传模式
1：命令模式（每一次发送数据前将数据缓存空间清空等待数据命令的回显，判断数据是否发送成功）
（1）：缓存中没有数据进入命令模式（）
（2）：缓存中有数据进入命令模式（检查读取完数据后，将数据缓存空间清空再发送命令）

2：透传模式
（1）：命令模式后进去透传模式（清空掉缓存中的数据，开始接收数据）
（2）：连续的数据传输（不考虑数据的循环解析一包数据解析完请求下一包的数据）

3：每次开启模块的
（1）DIO_6引脚拉低，进入RX接收模式
（2）等待链接上蓝牙：DIO_8输出低电平
（3）发送“+++”得到CMD IN表示是命令模式  ，CMD OUT透传模式
*/


/*
全局变量定义
*/
stBlStatus  gBlStatus;
char		gCommandEchoBuff[MAX_BL_COMMAND_ECHO];//存储命令回显的buff
/*
函数定义
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
状态数据初始化
*/
void BlStatusInit(stBlStatus  *status)
{
	status->data=0;
	status->data=~status->data;
}

/*
	初始化蓝牙等待连接蓝牙
	等待设备连接
	切换到数据透传模式
*/
void BlInit(stBlStatus  *status)
{
	BlStatusInit(status);
	
	BluetoothGPIO();//初始化IO口
	BlReset();
	BL_MRDY_PIN_RESET();//切换到RX接收模式
	uart5_init(19200);	//蓝牙默认115200
//	BlSendSetUpCommandEcho(AtBlueUartCommand,"19200","PARA SET: 115200,8,1,0",status);
////	BlSendSetUpCommand(AtBlueUartCommand,"19200");//配置蓝牙串口的波特率
//	uart5_init(19200);	//蓝牙19200 是蓝牙透传模式的最大波特率
	
	BlSendSetUpCommandEcho(AtBlueRoleTxpwr,eBlCommandSetUp,"-3",status);
//	BlSendSetUpCommandEcho(AtBlueUartCommand,eBlCommandSetUp,"9600",&status);
	
	
	//BlSendSetUpCommand(AtBlueRoleTxpwr,"-3");
//	BlSendSetUpUuid("1","1111");
	//等待蓝牙连接
	do{
		if(!BL_CONNECTION_STATUS())//有设备连接上
		{
				#if DEBUG_BLUE				
					printf("BLE is  connected   \r\n");
				#endif
			BlUPenetrateMode(status);//切换到透传模式
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
在运行过程中断连了，等待重新建立连接

*/
#define deBlReconnectionTimeOut   20  //等待重连的超时时间
void BlReconnection(stBlStatus  *status)
{
	uint16_t time_out=0;
		//等待蓝牙连接
	do{
		if(!BL_CONNECTION_STATUS())//有设备连接上
		{
			BlUPenetrateMode(status);//切换到透传模式
			status->bit.connection=eBlConnectionY;
		}else{
			status->bit.connection=eBlConnectionN;
			delay_ms(300);
			time_out++;
			
			if(time_out>deBlReconnectionTimeOut)
			{
				time_out=0;
				
				BlSendSwitchCommand(BL_RESET);//命令复位(软件复位)
			}
		}
	}while(status->bit.connection==eBlConnectionN);
}



/*
切换到命令模式(串口数据模式)
*/
void BlUCommandMode(stBlStatus  *status)
{
	if(status->bit.rx_buff_analysis!=eBlRxBuffAnalysisIng)//判断数据有没有解析完
	{
		BlRxDatabuffClear();
		BlCommandEchoBuffClear();
	do{
			BlSendSwitchCommand(BL_MODE);
			BlCommandReturnAnalysis((char*)gCommandEchoBuff,status);
			if(strcmp(gCommandEchoBuff,COMMAND_MODE_RETURN)==0)//数据相等
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
切换到透传模式(串口数据模式)
*/
char gTestFlag =0;
void BlUPenetrateMode(stBlStatus  *status)
{
	if(status->bit.rx_buff_analysis!=eBlRxBuffAnalysisIng)//判断数据有没有解析完
	{
		BlRxDatabuffClear();
		BlCommandEchoBuffClear();
	do{
			BlSendSwitchCommand(BL_MODE);
			delay_ms(2000);
			BlCommandReturnAnalysis((char*)gCommandEchoBuff,status);
			if(strcmp(gCommandEchoBuff,PENETRATE_MODE_RETURN)==0)//数据相等
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
命令模式下的命令回传数据获取
*/
void BlCommandReturnAnalysis(char *return_data,stBlStatus *BlStatus)
{
	uint16_t num=0;
	if(BLUETOOTH_RX_BUF[0]!='\0')//表示有数据
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
		//虽然有数据接口但是没有找到数据的结尾数据提取错误
		BlStatus->bit.command_echo=eBlCommandEchoN;
		BlCommandEchoBuffClear();
		BlRxDatabuffClear();
		return;
	}
	BlRxDatabuffClear();

}
/*
清空蓝牙的回显数据缓存空间
*/
void  BlCommandEchoBuffClear(void)
{
	memset(gCommandEchoBuff,0,MAX_BL_COMMAND_ECHO*sizeof(char));

}
/*
清空蓝牙的数据缓存空间
*/
void BlRxDatabuffClear(void)
{
	BLUETOOTH_UART_NUM=0;
	memset(BLUETOOTH_RX_BUF,0,BLUETOOTH_REC_LEN*sizeof(char));
}
/*
将接收的数据转义出来
src:需要转义的数据
data_return：转义后的数据
data_len:待转义的数据长度
*/

void BLDataEscapedAnalysis(char *src,char *data_return,uint16_t data_len)
{
	uint16_t num=0;//标记数据位置
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
num：解析到缓存池中的位置
data_status:数据解析状态标记
*/
#define BluetoothAnalysisCopyTimeOut 200
uint32_t BluetoothAnalysisCopy(char **data_return,uint32_t  num,u8  *data_status,stBlStatus *BlStatus)
{
	stBlDataHead head;
	uint16_t data_len=0;
	uint16_t data_all_len=0;//标记数据总长(转义后的)
	uint16_t data_len_flag=0;
	uint16_t data_head_len=0;//数据头的长度
#if 1
	uint16_t time_out=0 ;
	uint16_t data_Escaped_len=0;//中断接收到的数据长度
	if((num<BLUETOOTH_UART_NUM)&&(BLUETOOTH_RX_BUF[num]==BLUETOOTH_DATA_HEAD))
	{
		delay_ms((u32)(10));	//等待1
		//取出数据头
		data_head_len=sizeof(stBlDataHead);
		memset(&head,0,sizeof(stBlDataHead));
		memcpy(&head,&BLUETOOTH_RX_BUF[num+1],data_head_len);
		//串口过来的数据长度
		data_Escaped_len=data_head_len+head.DataEscapedLen;
		//分配数据空间(存放转义后的数据空间)
		data_all_len=data_head_len+head.DataLen;//加1是包含的结束符号
		*data_return=(char *)malloc(data_all_len);//分配对应长度的空间
		memset(*data_return,0,data_all_len);//清空分配的内存
		
		BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisIng;//正在开始解析数据，数据没有解析完成
		//等待数据接收结束
		while(BLUETOOTH_RX_BUF[num+data_Escaped_len+1]==0&&time_out<BluetoothAnalysisCopyTimeOut)//1S的超时时间
		{
			delay_ms((u32)(1));	//等待1
			time_out++;
		}
		//数据接收结束
		if(BLUETOOTH_RX_BUF[num+data_Escaped_len+1]==BLUETOOTH_DATA_END)	
		{
			//提取数做数据转义
			BLDataEscapedAnalysis((char *)&BLUETOOTH_RX_BUF[num+1],*data_return,data_Escaped_len);
			BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisY;
			//将num向后移
//			memset(&BLUETOOTH_RX_BUF[num],0,data_Escaped_len+1);//清空掉原来的数据
//			num=num+data_Escaped_len;
			
		}else{
			BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisN;
		}
		
//		memset(&BLUETOOTH_RX_BUF[num],0,data_Escaped_len+1);//清空掉原来的数据
//		num=num+data_Escaped_len+1;
		num=0;
		BlRxDatabuffClear();//数据解析成功清除掉原来的数据，重新接收新的数据
	}

	return num;
	
#else	
	
	
	if((num<BLUETOOTH_UART_NUM)&&(BLUETOOTH_RX_BUF[num]==BLUETOOTH_DATA_HEAD))
	{
		//取出数据头
		memset(&head,0,sizeof(stBlDataHead));
		memcpy(&head,&BLUETOOTH_RX_BUF[num+1],sizeof(stBlDataHead));
		*data_return=(char *)malloc((sizeof(stBlDataHead))+head.DataLen);
		data_all_len=sizeof(stBlDataHead)+head.DataLen;
		memset(*data_return,0,(sizeof(stBlDataHead))+head.DataLen);
		BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisIng;//正在开始解析数据，数据没有解析完成
		//开始解析数据
		while((num<BLUETOOTH_UART_NUM)&&(num<BLUETOOTH_REC_LEN))
		{
#if 0
			++data_len_flag;
			if((BLUETOOTH_RX_BUF[++num]==BLUETOOTH_DATA_END)&&(data_len_flag==data_all_len+1))
			{
				BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisY;//表示数据解析完成
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
				BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisY;//表示数据解析完成
				#if DEBUG_BLUE				
					printf("\r\n");
				#endif			
				BlRxDatabuffClear();//数据解析成功清除掉原来的数据，重新接收新的数据
				break;
			}			
			delay_us((u32)(10));	//0.01ms

			
#endif	
			
		}
		BlRxDatabuffClear();//数据解析成功清除掉原来的数据，重新接收新的数据
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
缓存池中的数据接口解析
*/
void BluetoothAnalysis(char **data_return,stBlStatus *BlStatus)
{
	static uint32_t  num=0;
	u8  data_status=0;  //第一位表示数据为空  第二位表示数据接收中有错误码  

	if((BLUETOOTH_RX_STATUS&0x1)==0)//表示串口接收还没有超出串口缓存BUF
	{
		for(num=0;num<BLUETOOTH_UART_NUM;num++)
		{
			num=BluetoothAnalysisCopy(data_return,num,&data_status,BlStatus);
			
		}
//		num=0;
	}else if((BLUETOOTH_RX_STATUS&0x1)==1)
	{//表示串口接收超出串口缓存BUF
		if((num<BLUETOOTH_REC_LEN)&&(num>BLUETOOTH_UART_NUM))//表示数据更新了但新的数据还没有追上解析的数据
		{
			for(;num<BLUETOOTH_REC_LEN;num++)
			{
				num=BluetoothAnalysisCopy(data_return,num,&data_status,BlStatus);
				
			}
//			num=0;
		}else if(((num<BLUETOOTH_REC_LEN)&&(num<BLUETOOTH_UART_NUM))||(num==BLUETOOTH_REC_LEN))//表示数据更新了但新的数据追上解析的数据
		{
			
			BlStatus->bit.rx_buff_analysis=eBlRxBuffAnalysisN;
			memset(&BLUETOOTH_RX_BUF[BLUETOOTH_UART_NUM],0,(BLUETOOTH_REC_LEN-BLUETOOTH_UART_NUM)*sizeof(char));
			num=0;
			BLUETOOTH_RX_STATUS&=~BLUETOOTH_RX_STATUS;
		}
	}
}
/*
蓝牙的相关引脚控制和状态监控的引脚初始化
*/
void BluetoothGPIO(void )
{
	GPIO_InitTypeDef GPIO_Initure;
	BL_MRDY_RCC();           		//开启GPIOB时钟
	BL_CONNECTION_RCC()	;
	//DIO_6 输入/输出 MRDY 引脚，用于触发模块的串口接收功能（低电平接收）
	GPIO_Initure.Pin=BL_MRDY_PIN; 					//
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	
	HAL_GPIO_Init(BL_MRDY_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(BL_MRDY_GPIO,BL_MRDY_PIN,GPIO_PIN_RESET);	
	//DIO_8 输入/输出 连接指示，当模块建立连接后输出低电平
	GPIO_Initure.Pin=BL_CONNECTION_PIN; 		//
	GPIO_Initure.Mode=GPIO_MODE_INPUT;  		//输入模式
	HAL_GPIO_Init(BL_CONNECTION_GPIO,&GPIO_Initure);
	//nRESET 输入 复位脚，内部有上拉（详见 CC2650 手册）
	BL_RESET_RCC();
	GPIO_Initure.Pin=BL_RESET_PIN; 					//
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	
	HAL_GPIO_Init(BL_RESET_GPIO,&GPIO_Initure);
	HAL_GPIO_WritePin(BL_RESET_GPIO,BL_RESET_PIN,GPIO_PIN_SET);	
	
}
/*
硬件复位
*/
void BlReset(void)
{
	BL_RESET_PIN_RESET();
	delay_ms(20);
	BL_RESET_PIN_SET();
}
/*
蓝牙给手机的数据（(读数据)请求模式：数据类型）
1：设备类型（手持、基站）设备的类型（这台设备是什么设备），设备的编号（这台设备是第	几个同类型的设备）
2：GGA数据：（给手机作为RTCM数据的反馈）
3：电台的相关配置：（信道的读取）
4：系统的工作状态：（作为系统的检测:在手机端监控）


手机给蓝牙的数据（（写数据）设置模式：数据类型）
1：电台的相关配置：（信道设置）
2：基站手持杖模式切换（1给8BIT的数据）
	{
		（0）：表示基站模式
		（1）：表示手持杖模式
	}
3：RTCM数据

*/
/*
通过蓝牙：发送数据到手机
发送一包数据
data：发送的数据
data_len：发送数据的长度
BlStatus：状态反馈
DataHead:数据的头数据类型（通过函数BlDataType返回数据的类型）
《我发给手机端的数据都是请求数据》
*/

void BlSendDataPack(void* data,u16 data_len, stBlDataHead *DataHead)
{
#if 1
	char	*p=0;
	char	*p_escape=0; //p_escape用作转义的内存空间
	char head = BLUETOOTH_DATA_HEAD,end = BLUETOOTH_DATA_END;
	uint16_t num_src=0;//记录原始数据查找到第几位
	uint16_t num_escape=0;//记录转义后的数据查找到第几位
	uint16_t data_all_len=0;
	data_all_len=2+data_len*2+sizeof(stBlDataHead);//data_len*2 //给转义字符预留的一段空间
	p=(char*)malloc(data_all_len);
	memset(p,0,data_all_len);
	//分配一段用来作为转义的内存空间
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
//		DataHead->DataEscapedLen=num_escape;//转义后的数据的长度
		
//		memcpy(p+1+sizeof(stBlDataHead),(char *)data,data_len);
		
//		memcpy(p+1+sizeof(stBlDataHead)+data_len,&end,sizeof(end));
		memcpy(p+1+sizeof(stBlDataHead)+num_escape,&end,sizeof(end));
		data_all_len=data_all_len-data_len*2+num_escape;
		DataHead->DataLen=sw32_uint32(data_len);
		DataHead->DataEscapedLen=sw32_uint32(num_escape);
		memcpy(p+1,(char *)DataHead,sizeof(stBlDataHead));

		BlSendDataLen((char *)p,data_all_len);//发送一定长度的数据
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
		BlSendDataLen((char *)p,data_all_len);//发送一定长度的数据
	#endif
	#if DEBUG_BLUE	
		printf("data_all_len=%d\r\n",data_all_len);
		printf("data=%s\r\n",p);
	#endif
	free(p);
#endif

#if 0
	BlSendByte(BLUETOOTH_DATA_HEAD);//发送头
	
	DataHead->DataLen=data_len;
	BlSendDataLen((char *)DataHead,sizeof(stBlDataHead));//发送数据类型信息
	
	BlSendDataLen((char *)data,data_len);//发送一定长度的数据
	BlSendByte(BLUETOOTH_DATA_END);	//发送尾
//#if DEBUG_BLUE	
//	printf("data_all_len=%d\r\n",data_all_len);
//	printf("data=%s\r\n",p);
//#endif
#endif
}

/*
请求数据的数据头拼接
RequestDataTypes :请求的数据类型
DataHead//返回的数据头
*/
void BlRequestDataHead(eBlRequestDataTypes	RequestDataTypes,stBlDataHead *DataHead  )
{
	DataHead->DataTypes.bit.DataMode=eBlDataRequestMode;
	DataHead->DataTypes.bit.DataTypes=BlDataTypes(RequestDataTypes);//RequestDataTypes;//BlDataTypes(RequestDataTypes);
}
/*
请求数据的数据头拼接
eBlSetUpDataTypes :请求的数据类型
DataHead//返回的数据头
*/
void BlSetStatusDataHead(eBlSetUpDataTypes	SetUpDataTypes,stBlDataHead *DataHead  )
{
	DataHead->DataTypes.bit.DataMode=eBlDataSetStatusUpMode;
	DataHead->DataTypes.bit.DataTypes=BlDataTypes(SetUpDataTypes);//RequestDataTypes;//BlDataTypes(RequestDataTypes);
}
/*
数据类型设置
BlDataType:函数配合BlSendDataPack函数使用
拼接对应的数据头
*/
void BlDataType(
				eBlDataMode	DataMode,
				eBlSetUpDataTypes	SetUpDataTypes,
				eBlRequestDataTypes	RequestDataTypes ,
				stBlDataHead *DataHead //返回的数据头
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
接收到的数据：数据头提取
在数据接收中断中添加标志
data:接收到的数据
head：数据头
status：数据的状态
*/
void BlReceiveDataGetHead(char** data,stBlDataHead *head ,stBlStatus *status)
{

	memcpy(head,*data,sizeof(stBlDataHead));
	
}
/*
请求数据ID:数据返回给手机
传入的数据ID
*/
#define BL_BlRequest_DeviceId_TEST	 1


void BlRequest_DeviceId(stBlDataHead *BlDataHead)
{
	BlRequestDataHead(eBlDataRequestTypes_DeviceId,BlDataHead);//拼接数据头
#if	BL_BlRequest_DeviceId_TEST
	BlSendDataPack("0123456789",10, BlDataHead);
	#if DEBUG_BLUE	
		printf("Bl Request Device Id\r\n");
	#endif
	
#else 
	
#endif

}
/*
请求GGA数据:数据返回给手机
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
	if(RtkStatus->bit.get_gga==eGetGgaY)//获得了正确的GGA数据
	{
		BlSendDataPack(gSysGgaDataBuffer,strlen(gSysGgaDataBuffer),BlDataHead); //gGgaDataBuffer:是系统去全局变量，用来作为GGA数据的缓存区
		#if DEBUG_BLUE	
			printf("Bl Request  GGA\r\n");
		#endif
	}
#endif	
	
}
/*
电台信道请求
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
蓝牙设置命令接收状态反馈
*/
void BleSetStatus(eBlSetUpDataTypes	SetUpDataTypes)
{
	stBlDataHead DataHead;
	BlSetStatusDataHead(SetUpDataTypes,&DataHead);
	BlSendDataPack(0,0,&DataHead); 
}

/*
数据拆包
*/

void BleSetRtkPosition(char *src_data,stBlDataHead *head,stRtkStatus *RtkStatus);
void BlSysModeSwitch(char *src_data,stBlDataHead *head,stBlStatus *BlStatus);
void  BleSetRadioChannel(char *src_data,stBlDataHead *head);
typedef void (*BlSendDataCallback)(char *,stBlDataHead *);//发送数据给手机的数据回调函数定义
char gBlRtcmOpenFlag =0;

void BlReceiveDataUnpacking( char** data,stBlStatus *status,stRtkStatus *RtkStatus)
{
	stBlDataHead BlDataHead;
	BlReceiveDataGetHead(data,&BlDataHead,status); //取出数据的头
	switch(BlDataHead.DataTypes.bit.DataMode) //判断是请求数据还是设置数据
	{
		case	eBlDataRequestMode://请求模式	
			switch (BlDataHead.DataTypes.bit.DataTypes)
				{	
					case	BlDataTypes(eBlDataRequestTypes_DeviceId)://eBlDataRequestTypes_DeviceId		://请求设备ID号
							BlRequest_DeviceId(&BlDataHead);
							break;
					case	BlDataTypes(eBlDataRequestTypes_GGA)				://gga数据
						BlRequest_GGA(&BlDataHead,RtkStatus);
//						gBlRtcmOpenFlag=1;
						break;
					case	BlDataTypes(eBlDataRequestTypes_Channel)			://信道
						BlRequest_RadioChannel(&BlDataHead,&gRadioCommandParameter,&gDu1018dStatus);
						break;
					case	BlDataTypes(eBlDataRequestTypes_SysStatus)	://系统状态
						
						break;
					default: break;
			}	
			break;
		case	eBlDataSetUpMode://设置模式
				switch (BlDataHead.DataTypes.bit.DataTypes)
				{	
					case	BlDataTypes(eBlDataSetUpTypes_Channel)		:	//电台的相关配置：（信道设置）
						BleSetRadioChannel(*data,&BlDataHead);
						BleSetStatus(eBlDataSetUpTypes_Channel);
						break;
					case	BlDataTypes(eBlDataSetUpTypes_ModeSwitch)	://基站手持杖模式切换
						BlSysModeSwitch(*data,&BlDataHead,status);
						BleSetStatus(eBlDataSetUpTypes_ModeSwitch);
						break;
					case	BlDataTypes(eBlDataSetUpTypes_Rtcm)			://RTCM数据
						gBlRtcmOpenFlag=1;
						BleSetStatus(eBlDataSetUpTypes_Rtcm);
						#if DEBUG_BLUE	
							printf("Ble Set Rtk Position\r\n");
						#endif
						 //BlRTCMSet(*data,&BlDataHead,status);
						break;
					case	BlDataTypes(eBlDataSetUpTypes_SelfCalibration)			://自校准模式
						RtkSelfCalibration(RtkStatus);
						BleSetStatus(eBlDataSetUpTypes_SelfCalibration);
						break;
					case	BlDataTypes(eBlDataSetUpTypes_SetPosition)			://设置基站，由手机来设置
						
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
设置电台信道
*/

void  BleSetRadioChannel(char *src_data,stBlDataHead *head)
{
#if RADIO_E71
	char *pDataHead=0;//数据的开始地址
	uint8_t channel=0;
	pDataHead=(char *)(src_data+(sizeof(stBlDataHead)));
	channel=*pDataHead;
	RadioChannelSet(channel,&gRadioCommandParameter,&gRadioStatus);
	#if DEBUG_BLUE	
		printf("Ble Set Radio Channel %d \r\n",channel);
	#endif
#endif
#if RADIO_DU1018D
	char *pDataHead=0;//数据的开始地址
	pDataHead=(char *)(src_data+(sizeof(stBlDataHead)));
	Du1018dSetChannel((u8*)pDataHead,head->DataEscapedLen,&gDu1018dStatus);
	#if DEBUG_BLUE	
		printf("Ble Set Radio Channel %d \r\n",character_to_uint32(pDataHead ,head->DataEscapedLen));
	#endif	
#endif	

}
/*
通过蓝牙设置基站的Position信息
data:蓝牙的数据信息
*/
void BleSetRtkPosition(char *src_data,stBlDataHead *head,stRtkStatus *RtkStatus)
{
	stRtkBestposData BestposData;//数据信息
	u8 data_num=0;
	char *pDataHead=0;//数据的开始地址
	memset((char *)&BestposData ,0,sizeof(stRtkBestposData));
	pDataHead=(char *)(src_data+(sizeof(stBlDataHead)));
	
	while((MAX_BESTPOS_DATA_LEN>data_num)&&(*(pDataHead)!=','))
	{
		BestposData.latitude[data_num++]=*(pDataHead++);//纬度
	}
	pDataHead++;
	data_num=0;
	while((MAX_BESTPOS_DATA_LEN>data_num)&&(*(pDataHead)!=','))
	{
		BestposData.accuracy[data_num++]=*(pDataHead++);//经度
	}
	pDataHead++;
	data_num=0;
	while((MAX_BESTPOS_DATA_LEN>data_num)&&(*(pDataHead)!=','))
	{
		BestposData.heigh[data_num++]=*(pDataHead++);//高度
	}
	data_num=0;
	RtkSendPosition(&BestposData,RtkStatus);
	#if DEBUG_BLUE	
		printf("Ble Set Rtk Position\r\n");
	#endif
}



/*
设置RTCM数据
*/

void BlRTCMSet(char *src_data,stBlDataHead *head,stBlStatus *BlStatus)
{
	static uint16_t pack_num=0;
#if DEBUG_BLUE	
		uint16_t num=0;
#endif	
	if(BlStatus->bit.rx_buff_analysis==eBlRxBuffAnalysisY)//表示数据提取成功
	{
		rtk_SendNumByte((char *)(src_data+(sizeof(stBlDataHead))) ,head->DataLen);//将RTCM数据发送给rtk
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
工作模式切换
	src_data:蓝牙源数据
	head：数据头
	BlStatus：蓝牙解析的状态
*/
	
void BlSysModeSwitch(char *src_data,stBlDataHead *head,stBlStatus *BlStatus)
{
	char WorkMode=0;
	if(BlStatus->bit.rx_buff_analysis==eBlRxBuffAnalysisY)//表示数据提取成功
	{
		memcpy((char *)&WorkMode,(char *)(src_data+(sizeof(stBlDataHead))) ,head->DataLen);//将数据拷贝到系统状态全局变量中；
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
