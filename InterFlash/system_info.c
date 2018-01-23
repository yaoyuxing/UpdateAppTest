#include "bootloader.h"
#include "interflash.h"
#include "string.h"
#include "stm32f7xx_hal.h"
#include "system_info.h"
#include "stdlib.h"


/*  ʹ��˵��
    1��RefreshSysInfo();����ϵͳ��Ϣ��ram�ṹ��
		2��SaveSysInfo(); �������ݵ�flash
		3��SystemInfoInit_App��  SystemInfoInit_BtLoader��ʼ��
    
	bootloader	eg: 
								unsigned char buf[10]="abcd123456";
								SystemInfoInit_BtLoader(); 
								memcpy(buf,"123456",6); 
								SaveSysInfo();
								RefreshSysInfo(); 
	App   eg:
								uint8_t buf[1024]="abcdef1234";
								SystemInfoInit_App(buf,1024); 
								memcpy(buf,"123456",6); 
								SaveSysInfo();
								RefreshSysInfo(); 
*/

 stSystemInformationType gstSystemInfo ={ .unPosMask=0x12340000 } ; //ϵͳ��Ϣ�ṹ��
 
#define  SYS_INFO_SECTOR_CNT_MAX           gstSystemInfo.unBlockDevideCntAmount             //128k�ֵ�sector����
#define  SYS_INFO_SECTOR_SIZE 	           gstSystemInfo.unInfoSingleSize                   //ÿ��sector��С
#define  APP_SYS_INFO_SIZE 	               gstSystemInfo.unAppSysInfoSize                    //APP�������ݳ���
#define  BOOT_SYS_INFO_SIZE                 BOOT_LOADER_SYS_INFO_SIZE                // (gstSystemInfo.unBtLoaderSysInfoSize) //10// BOOT_LOADER_SYS_INFO_SIZE 
#define  SYS_INFO_CTRL_STRUCT_SIZE         (sizeof(stSystemInformationType))         //���ƽṹ���С
#define  UNSIGNED_INT_VAL_MAX              (0xffffffff)//(-1)                        //��1Ϊ0
#define  NOT_FIRST_TIMES_FLAG_VAL          (0x123456)
#define  APP_SYS_INFO_DEFAULT_SIZE         (4)
 

#define  pstSysInfoBase  ( ( stSystemInformationType * ) SYSTEM_INIT_INFO_ADDR)  //ϵͳ��Ϣ�洢��ַת��Ϊ�ṹָ��
 
static void RefreshCurrentRam(void);
static void EraseSysInfoBlock(void);   
static void SaveSysInfoBackup(void);
static void RefreshCurrentRam(void);
/**
  * @brief  ����ϵͳ��Ϣblock
  */
static void EraseSysInfoBlock(void)                    
{
	InterFlash_EraseSector(SYSTEM_INIT_INFO_ADDR,1);         
}
/**
  * @brief  ���±���ϵͳ��Ϣ��RAM�����±��ݴ洢���ڳ�ʼ����ʹ��
  */
void RefreshSysInfoBackup(void)          
{
	unsigned char *pSource=NULL;                 //����Դָ��
	unsigned char *pData=NULL;                   //����ָ��
	unsigned int  unAddr=0;                     //����Դ��ַ
	unsigned int  unPos=0;                      //����Դflash�ϵ�λ�ñ��
	//unsigned int  punMask=gstSystemInfo.unPosMask; //
	unsigned int  cnt=0;
	unAddr=SYSTEM_INIT_INFO_ADDR;                 
	for(cnt=0;cnt<SYS_INFO_SECTOR_CNT_MAX+1;)       //���� unPos�ж�flash��������λ��
	{
		unPos=((stSystemInformationType * )unAddr)-> unPos;
		if((unPos !=cnt) ||(unPos>=SYS_INFO_SECTOR_CNT_MAX))                              
		{
			break;
		}
		cnt++; 
		unAddr+=SYS_INFO_SECTOR_SIZE;
	}
	if(cnt==0)                                          //��δ�洢��ϵͳ��Ϣ
	{
		gstSystemInfo.unPos=SYS_INFO_SECTOR_CNT_MAX;	
		SaveSysInfo();
	}
	else                                                //�����ϵͳ��Ϣ
	{              
		unAddr-=SYS_INFO_SECTOR_SIZE;                           //��һָ�����´洢λ��
	}		
	unAddr=((stSystemInformationType * )unAddr)-> unSysBackUpADDR;                //Դ��ַΪ�������ݵ�ַ
	pSource=(  unsigned char  *)unAddr;             //��ȡflashԴ����ָ��
	for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE;cnt++) //��ȡ���ƽṹ������
	{
		*pData ++= *pSource++;  
	}
	for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE;cnt++) //��ȡ�����ṹ����
	{
		*pData ++= *pSource++;  
	} 	
	for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++) //��ȡAPP���ִ������
	{
		*pData ++= *pSource++;  
	} 
	gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;        //��1Ϊ0
	EraseSysInfoBlock();                             //���� 
	SaveSysInfo();                                   //������pos=0
	SaveSysInfo();                                   //��ǰ����pos=1
} 
/**
  * @brief  д����ϵͳ��Ϣ��FLASH���ڳ�ʼ����ʹ��
  */
static void SaveSysInfoBackup(void)                            //���ñ��汸��
{
	unsigned char *pData=NULL;                            //����ָ��
	unsigned int  unAddr=0;                              //д���ַ
//	unsigned int  *punPos=&gstSystemInfo.unPos;          //λ��ָ��
//	unsigned int  punMask=gstSystemInfo.unPosMask;       //λ������
	unsigned int  cnt=0;
	unsigned char *pBackUpData=malloc(SYS_INFO_SECTOR_SIZE);                                   //����һ��SECTOR�ڴ�ռ�
	if(pBackUpData==NULL)  while(1);
	memcpy(pBackUpData,(void*)gstSystemInfo.unSysBackUpADDR,SYS_INFO_SECTOR_SIZE);          //ȡ��������
	EraseSysInfoBlock();          //���� 
	((stSystemInformationType*)pBackUpData)->unPos=0;         //���λ��0
	unAddr=SYSTEM_INIT_INFO_ADDR;
	HAL_FLASH_Unlock();                                //���� ��д 
	
	//д��������
	for(cnt=0,pData=(void*)pBackUpData;cnt<SYS_INFO_SECTOR_SIZE ;cnt++)     //д�뱸�����ݵ�λ��0
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); //д������ 
		unAddr++;                                      
		pData++;
	}
	free(pBackUpData);                          //�ͷſռ�
	pBackUpData=NULL;
	gstSystemInfo.unPos=1;                     //��ǵ�ǰ�ṹ����λ��Ϊ1
	gstSystemInfo.unSysBackUpADDR=SYSTEM_INIT_INFO_ADDR;           //���ݵ�ַ����Ϊλ��0��ַ

	//д��ǰ����
	for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE ;cnt++) //д����ƽṹ������
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); //д�뵱ǰ���� 
		unAddr++;                                     //��ַƫ��
		pData++;
	}
	for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE ;cnt++) //д��bootloader��������
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); //д�뵱ǰ���� 
		unAddr++;                                     //��ַƫ��
		pData++;
	}  
	for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++)     //д��APP��������
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); //д�뵱ǰ���� 
		unAddr++;                                     //��ַƫ��
		pData++;
	} 
	HAL_FLASH_Lock();  
}
/**
  * @brief  дϵͳ��Ϣ��FLASH���ڳ�ʼ����ʹ��
  */
void SaveSysInfo(void)  
{
	unsigned char *pData=NULL;  //��ǰ����           
	unsigned int  unAddr=0;       //�洢��ַ
	unsigned int  *punPos=&gstSystemInfo.unPos;     //flash��λ��
	//unsigned int  punMask=gstSystemInfo.unPosMask; //λ������
	unsigned int  cnt=0;
	
	if(((punPos[0])>=SYS_INFO_SECTOR_CNT_MAX-1) &&(punPos[0]!=UNSIGNED_INT_VAL_MAX))       //�ж��Ƿ��Ƿ����
	{ 
		SaveSysInfoBackup();                          //�������²���д�������������
	}
	else                                                                           //δ����
	{ 
		punPos[0]++;                                                          //λ��ƫ��1 ��������һ��λ��
		unAddr=(punPos[0])*SYS_INFO_SECTOR_SIZE+SYSTEM_INIT_INFO_ADDR;           //��ַƫ��һ��
		gstSystemInfo.unSysBackUpADDR=(punPos[0]-1)*SYS_INFO_SECTOR_SIZE+SYSTEM_INIT_INFO_ADDR;//���ñ��ݵ�ַ
		if(gstSystemInfo.unSysBackUpADDR<SYSTEM_INIT_INFO_ADDR)                            
			gstSystemInfo.unSysBackUpADDR=SYSTEM_INIT_INFO_ADDR;
		HAL_FLASH_Unlock();                                //������д 
		for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE ;cnt++)//д�뵱ǰ���� 
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); 
			unAddr++;                                      
			pData++;
		}
		for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE ;cnt++)  
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData);
			unAddr++;                                     
			pData++;
		}  
		for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++) 
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); 
			unAddr++;                                    
			pData++;
		} 
		HAL_FLASH_Lock();  
	} 
}
/**
  * @brief  ����ϵͳ��Ϣ��RAM���ڳ�ʼ����ʹ��
  */
void RefreshSysInfo(void)
{ 
	unsigned char *pSource=NULL;    //Դ����ָ��
	unsigned char *pData=NULL;       //RAM����ָ�� 
	unsigned int  unAddr=0;         //��ַ
	unsigned int  unPos=0;          //flashλ�ñ��
	unsigned int  punMask=gstSystemInfo.unPosMask;
	unsigned int  cnt=0;
	unAddr=SYSTEM_INIT_INFO_ADDR;                       
	for(cnt=0;cnt<SYS_INFO_SECTOR_CNT_MAX+1;)       //���� unPos�ж�flash��������λ��
	{
		unPos=((stSystemInformationType * )unAddr)-> unPos;         //��ȡλ��
		if((unPos !=cnt) ||(unPos>=SYS_INFO_SECTOR_CNT_MAX))    //�ж��Ƿ�����������λ��                           
		{
			break;
		}
		cnt++; 
		unAddr+=SYS_INFO_SECTOR_SIZE;                     //ƫ��һ��sector
	}
	if(cnt==0)                                          //��δ�洢��ϵͳ��Ϣ
	{
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //��������һ�α���
		SaveSysInfo();
		SaveSysInfo();
		unAddr=SYSTEM_INIT_INFO_ADDR+SYS_INFO_SECTOR_SIZE;
	}
	else                                                //�����ϵͳ��Ϣ
	{              
		unAddr-=SYS_INFO_SECTOR_SIZE;                           //��һָ�����´洢λ��
	}		
	pSource=(  unsigned char  *)unAddr;             //��ȡflash��ַָ��
	for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE;cnt++) //��ȡ���ƽṹ������
	{
		*pData ++= *pSource++;  
	} 
	for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE ;cnt++)//��ȡbootloader����
	{
		*pData ++= *pSource++;  
	} 
	for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++) //��ȡAPPϵͳ����
	{
		*pData ++= *pSource++;  
	}  
}  
/**
  * @brief   �������ݴ洢��ram��ַָ�벢���� 
  */
static void RefreshCurrentRam(void)
{
    unsigned char *pSource=NULL;    //Դ����ָ��
	unsigned char *pData=NULL;       //RAM����ָ�� 
	unsigned int  unAddr=0;         //��ַ
	unsigned int  unPos=0;          //flashλ�ñ��
	unsigned int  punMask=gstSystemInfo.unPosMask;
	unsigned int  cnt=0;
	unAddr=SYSTEM_INIT_INFO_ADDR;                       
	for(cnt=0;cnt<SYS_INFO_SECTOR_CNT_MAX+1;)       //���� unPos�ж�flash��������λ��
	{
		unPos=((stSystemInformationType * )unAddr)-> unPos;         //��ȡλ��
		if((unPos !=cnt) ||(unPos>=SYS_INFO_SECTOR_CNT_MAX))    //�ж��Ƿ�����������λ��                           
		{
			break;
		}
		cnt++; 
		unAddr+=SYS_INFO_SECTOR_SIZE;                     //ƫ��һ��sector
	}
	if(cnt==0)                                          //��δ�洢��ϵͳ��Ϣ
	{
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //��������һ�α���
		SaveSysInfo();
		SaveSysInfo();
		unAddr=SYSTEM_INIT_INFO_ADDR+SYS_INFO_SECTOR_SIZE;
	}
	else                                                //�����ϵͳ��Ϣ
	{              
		unAddr-=SYS_INFO_SECTOR_SIZE;                           //��һָ�����´洢λ��
	}		
	pSource=(  unsigned char  *)unAddr;             //��ȡflash��ַָ��
	for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE;cnt++) //��ȡ���ƽṹ������
	{
		//*pData ++=
		pSource++;  
	} 
	for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE ;cnt++)//��ȡbootloader����
	{
		*pData ++= *pSource++;  
	} 
	for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++) //��ȡAPPϵͳ����
	{
		*pData ++= *pSource++;  
	}
	SaveSysInfo();    	
}
/**
  * @brief   bootloader�г�ʼ��
  *          �������ֽṹ�Ϳ��ƽṹΪ�̶����ȣ����̻�����APP���ݿռ� 
  */
void SystemInfoInit_BtLoader(void)     
{
	//InterFlash_SetBankMode(SINGLE_BANK_MODE); //����Ϊsingle bank mode  
	
	gstSystemInfo.unBtLoaderSysInfoSize=0;
	gstSystemInfo.unAppSysInfoSize=0;
	SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //��ʼ���ܳ���   
	gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE); 
	RefreshSysInfo();                            //��gstSystemInfo��Ϣ
	if(gstSystemInfo.unPosMask!=NOT_FIRST_TIMES_FLAG_VAL)
	{ 
		gstSystemInfo.unPosMask=NOT_FIRST_TIMES_FLAG_VAL; //���õ�һ��д��־
		//boot sys info
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;  //bt�̶�����
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;   //ȡ�����ṹram��ַ
		//app sys info
		APP_SYS_INFO_SIZE=APP_SYS_INFO_DEFAULT_SIZE;    //APPĬ�Ͽ�1K
		gstSystemInfo.pucAppSysInfo=malloc(APP_SYS_INFO_SIZE);  //���ٿռ�
		//size 
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //��ʼ���ܳ���   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE);                //128kϵͳ�洢���ֵ���sector��
		
		
		
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //��������һ�α���
		EraseSysInfoBlock();
		SaveSysInfo();
		SaveSysInfo();  
		
	}
	else
	{
		//boot 
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;  //�̶�����
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;   //ȡ�����ṹram��ַ
		//size 
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //��ʼ���ܳ���   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE); 
		//app
		gstSystemInfo.pucAppSysInfo=malloc(APP_SYS_INFO_SIZE);  //���ٿռ�
		if(gstSystemInfo.pucAppSysInfo==NULL) while(1);  
		
		RefreshCurrentRam();                      
	}  
}
/**
  * @brief   App�г�ʼ��
  *          �����ṹ�Ϳ��ƽṹΪ�̶�����
  * @param   pAppSysInfo:APP��Ҫ�洢��ϵͳ��Ϣ��ʼ��ַָ��
  * @param   len: APPϵͳ��Ϣ���ݳ���
  */
void SystemInfoInit_App(void * pAppSysInfo,unsigned int len)               //App ����ϵͳ��Ϣ��ʼ��
{
  //	InterFlash_SetBankMode(SINGLE_BANK_MODE); //����Ϊsingle bank mode  
	
	gstSystemInfo.unBtLoaderSysInfoSize=0;
	gstSystemInfo.unAppSysInfoSize=0; 
	SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //��ʼ���ܳ��� 
	gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE); 	
	RefreshSysInfo();                            //��gstSystemInfo��Ϣ
	if(gstSystemInfo.unPosMask!=NOT_FIRST_TIMES_FLAG_VAL)
	{
		gstSystemInfo.unPosMask=NOT_FIRST_TIMES_FLAG_VAL; //���õ�һ��д��־
		//boot sys info
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;  //bt�̶�����
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;   //ȡ�����ṹram��ַ
		//app sys info
		APP_SYS_INFO_SIZE=len;    //APPĬ�Ͽ�1K 
		gstSystemInfo.pucAppSysInfo=pAppSysInfo;
		//size 
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //��ʼ���ܳ���   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE);                //128kϵͳ�洢���ֵ���sector��
		//
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //��������һ�α���
		EraseSysInfoBlock();
		SaveSysInfo();
		SaveSysInfo();  
	}
	else  if(gstSystemInfo.unAppSysInfoSize!=len)
	{ 
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;      //ȡ�����ṹram��ַ 
		
		gstSystemInfo.pucAppSysInfo=pAppSysInfo;                           //ȡAPP����ָ��
		gstSystemInfo.unAppSysInfoSize=len;   
		
		
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //��ʼ���ܳ���   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE);  
		
		RefreshCurrentRam();  //��λ���һ�γ�ʼ������ram�ϵ�����ָ�뱣��������
		
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //��������һ�α���
		EraseSysInfoBlock();
		SaveSysInfo();
		SaveSysInfo(); 
	} 
	else             //����û�и���
	{
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;      //ȡ�����ṹram��ַ 
		
		gstSystemInfo.pucAppSysInfo=pAppSysInfo;                           //ȡAPP����ָ��
		gstSystemInfo.unAppSysInfoSize=len;    
		
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //��ʼ���ܳ���   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE); 
		
		RefreshCurrentRam(); //��λ���һ�γ�ʼ������ram�ϵ�����ָ�뱣��������
	}
}













