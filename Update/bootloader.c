#include "bootloader.h"
#include "ff.h"
#include "stdlib.h"
#include "interflash.h"
#include "system_info.h"
#include "ble_update.h"
#include "string.h"

volatile stUpdateInfoType  gstUpdate={0}; 

static void UpadateError(void); 
void CheckUpdateStatus(void)
{
	switch((eUpdateStatusType )gstUpdate.eAppUpdateStatus)
	{
		case  USR1_UPDATE_FINISH: 
			 gstUpdate.eAppUpdateStatus=NO_UPDATE;
		   SaveSysInfo();
		 // RunAPP(gstUpdate.unCurrentAppADDR);
			break;
		case  USR2_UPDATE_FINISH: 
			gstUpdate.eAppUpdateStatus=NO_UPDATE;
		  SaveSysInfo();
		//  RunAPP(gstUpdate.unCurrentAppADDR);
			break;
		case  USR1_START_UPDATE:
			 RefreshSysInfoBackup();
//			 gstUpdate.unCurrentAppADDR=(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)?USER2_APP_ADDR:USER1_APP_ADDR;
//		   
//		   gstUpdate.eAppUpdateStatus=NO_UPDATE;
//		   
//		   SaveSysInfo();
		   UpadateError();
			break;
		case  USR2_START_UPDATE:
			 RefreshSysInfoBackup();
//			 gstUpdate.unCurrentAppADDR=(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)?USER2_APP_ADDR:USER1_APP_ADDR;
//		   gstUpdate.eAppUpdateStatus=NO_UPDATE;
//		   SaveSysInfo();
		   UpadateError(); 
			break;
		case START_UPDATE:
			break;
		case NO_UPDATE:
		//	 RunAPP(gstUpdate.unCurrentAppADDR);
			break;
		default :
			break;
	} 
}
void SaveAppBackup(char *pData,unsigned int len )
{
//	unsigned char res=0;
//	unsigned int  len=0;
//	FIL *file=malloc(sizeof (FIL));
//	res=f_open(file,APP_PATH,FA_CREATE_ALWAYS|FA_WRITE|FA_READ);
//	res=f_write(file,gucAppBuf,gstUpdate.unCurrentAppSize,&len);
//	res=f_close(file);
//	free(file);
//	file=NULL;
}
void RunAPP(unsigned int jumpAdr)
{
	pFunType JumpFun; 
	__set_FAULTMASK(1);//  ��תǰ���ж�
	JumpFun=(pFunType)(*(__IO u32 *)(jumpAdr+4)); //����APP
	JumpFun(); 
}

void UpadateError(void)
{
	
}
/**
  * @brief  APP��ʼ��������Ϣ��������SystemInfoInit_App�� ���ж�ǰ
  */
void UpdateInit_App(void)
{
	char temp[2]={0};
	__set_FAULTMASK(0);  
	RefreshSysInfo();              //��ȡϵͳ��Ϣ
	if((gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)||(gstUpdate.unCurrentAppADDR==USER2_APP_ADDR)) //��ȷ������λ�þ������ж�����ƫ��
		SCB->VTOR = FLASH_BASE|(gstUpdate.unCurrentAppADDR-FLASH_BASE);
	if(gstUpdate.eAppUpdateStatus==USR1_START_UPDATE)      //usr1����
	{
		if(gstUpdate.unCurrentAppADDR==USER1_APP_ADDR)        //�жϵ�ַ
		{
			gstUpdate.eAppUpdateStatus=USR1_UPDATE_FINISH;      //�����ɹ�
			SaveSysInfo();   
			temp[0]=UPDATE_SUCCEED_ACK_BYTE;
			//BleSend2Phone(temp);
		}
	} 
	else if(gstUpdate.eAppUpdateStatus==USR2_START_UPDATE)// usr2����
	{
		if(gstUpdate.unCurrentAppADDR==USER2_APP_ADDR)   
		{
			gstUpdate.eAppUpdateStatus=USR2_UPDATE_FINISH;  //�����ɹ�
			SaveSysInfo();
			temp[0]=UPDATE_SUCCEED_ACK_BYTE;
			//BleSend2Phone(temp);
		}
	}
}
 
eUpdateErrType CheckAppBin(unsigned int *AppBuf)
{ 
	if((AppBuf[0]&0xf0000000)!=0x20000000)
		return RAM_ADDR_ERR;
	return BOOT_NO_ERR;
}

void BootFactoryInit(void)
{
	// InterFlash_EraseSector(SYSTEM_INIT_INFO_ADDR,1);          //����  
	// gstSystemInfo.unAppSysInfoSize=sizeof gstSystemInfo  ;
}
char * pGetAppBin(char *pBinData,unsigned int len,unsigned int Addr)
{
	char * pReturn=pBinData;
	if(Addr==USER1_APP_ADDR)
	{
		return pBinData;
	}
	else if(Addr==USER2_APP_ADDR)
	{
		while(len--)
		{
			if(*pReturn==*UPDATE_APP_SEPARATE_STR) //�Ƚ��Ƿ�ͷָ��ַ���ͷ��ͬ
			{
				if(memcmp(pReturn,UPDATE_APP_SEPARATE_STR,strlen(UPDATE_APP_SEPARATE_STR))==0)  //�Ƚ��Ƿ�ͷָ��ַ�����ͬ
					return (pReturn+strlen(UPDATE_APP_SEPARATE_STR));//����ƫ�Ʒָ��ַ�����ָ��
			}
		}
	}
	return NULL; //δ�ҵ��ָ��ַ���
}

/**************************                       ���ú���                               ******************************/
#include "string.h"
//�����ַ���
#define RX_BUF_MAX 1024
typedef struct str_buf
{
  u8 Data[RX_BUF_MAX];
  u32 Len;
}sBufStrType;
/*
* �ڸ���buf�����ַ���
*/
int  findstr(u8 *rx_buf,unsigned int rx_counter,char *str)
{
	char * pTemp=str;
	u32  strLen=0;
	sBufStrType big,target;
	u32 counter1=0,counter2=0,temp,times=0;
	for(;*pTemp!='\0';)
	{ 
		pTemp++;
		strLen++;
	}
	memset(big.Data,0,RX_BUF_MAX);
	memset(target.Data,0,RX_BUF_MAX);
	memcpy(big.Data,(u8 *)rx_buf,rx_counter);
	memcpy(target.Data,str,strLen);
	big.Len=rx_counter;
	target.Len=strLen ;
	temp=counter1;
	if(big.Len<target.Len)  return 0;
	for(;counter1<big.Len;)    
	{
		if(big.Data[counter1]==target.Data[0])
		{
			while((big.Data[counter1]==target.Data[counter2])&&(counter2<target.Len))
			{
				counter1++;
				counter2++;
			}
		}
		if(counter2==target.Len)
		{
			times++; 
		}
		counter2=0; 
		temp=temp+1;
		counter1=temp;
	}
	if(times>0) 
		return 1;
	else       
	 return 0;
}
/*
* �ַ��������ӵ�ǰλ��ƫ�Ƶ�Ŀ��㷵�ؾ���
*
*/
unsigned int bytepos(char *str,char byte)
{
	unsigned int len=0;
	for(len=0;*str!=byte;len++)
	{
		str++;
	}
	return len;
}
/*********************************************************************************************************************/








