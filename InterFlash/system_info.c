#include "bootloader.h"
#include "interflash.h"
#include "string.h"
#include "stm32f7xx_hal.h"
#include "system_info.h"
#include "stdlib.h"


/*  使用说明
    1、RefreshSysInfo();更新系统信息到ram结构体
		2、SaveSysInfo(); 保存数据到flash
		3、SystemInfoInit_App、  SystemInfoInit_BtLoader初始化
    
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

 stSystemInformationType gstSystemInfo ={ .unPosMask=0x12340000 } ; //系统信息结构体
 
#define  SYS_INFO_SECTOR_CNT_MAX           gstSystemInfo.unBlockDevideCntAmount             //128k分的sector数量
#define  SYS_INFO_SECTOR_SIZE 	           gstSystemInfo.unInfoSingleSize                   //每个sector大小
#define  APP_SYS_INFO_SIZE 	               gstSystemInfo.unAppSysInfoSize                    //APP部分数据长度
#define  BOOT_SYS_INFO_SIZE                 BOOT_LOADER_SYS_INFO_SIZE                // (gstSystemInfo.unBtLoaderSysInfoSize) //10// BOOT_LOADER_SYS_INFO_SIZE 
#define  SYS_INFO_CTRL_STRUCT_SIZE         (sizeof(stSystemInformationType))         //控制结构体大小
#define  UNSIGNED_INT_VAL_MAX              (0xffffffff)//(-1)                        //加1为0
#define  NOT_FIRST_TIMES_FLAG_VAL          (0x123456)
#define  APP_SYS_INFO_DEFAULT_SIZE         (4)
 

#define  pstSysInfoBase  ( ( stSystemInformationType * ) SYSTEM_INIT_INFO_ADDR)  //系统信息存储地址转换为结构指针
 
static void RefreshCurrentRam(void);
static void EraseSysInfoBlock(void);   
static void SaveSysInfoBackup(void);
static void RefreshCurrentRam(void);
/**
  * @brief  擦拭系统信息block
  */
static void EraseSysInfoBlock(void)                    
{
	InterFlash_EraseSector(SYSTEM_INIT_INFO_ADDR,1);         
}
/**
  * @brief  更新备份系统信息到RAM并重新备份存储，在初始化后使用
  */
void RefreshSysInfoBackup(void)          
{
	unsigned char *pSource=NULL;                 //数据源指针
	unsigned char *pData=NULL;                   //数据指针
	unsigned int  unAddr=0;                     //数据源地址
	unsigned int  unPos=0;                      //数据源flash上的位置标记
	//unsigned int  punMask=gstSystemInfo.unPosMask; //
	unsigned int  cnt=0;
	unAddr=SYSTEM_INIT_INFO_ADDR;                 
	for(cnt=0;cnt<SYS_INFO_SECTOR_CNT_MAX+1;)       //根据 unPos判断flash最新数据位置
	{
		unPos=((stSystemInformationType * )unAddr)-> unPos;
		if((unPos !=cnt) ||(unPos>=SYS_INFO_SECTOR_CNT_MAX))                              
		{
			break;
		}
		cnt++; 
		unAddr+=SYS_INFO_SECTOR_SIZE;
	}
	if(cnt==0)                                          //从未存储过系统信息
	{
		gstSystemInfo.unPos=SYS_INFO_SECTOR_CNT_MAX;	
		SaveSysInfo();
	}
	else                                                //存入过系统信息
	{              
		unAddr-=SYS_INFO_SECTOR_SIZE;                           //减一指向最新存储位置
	}		
	unAddr=((stSystemInformationType * )unAddr)-> unSysBackUpADDR;                //源地址为备份数据地址
	pSource=(  unsigned char  *)unAddr;             //获取flash源数据指针
	for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE;cnt++) //读取控制结构体数据
	{
		*pData ++= *pSource++;  
	}
	for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE;cnt++) //读取升级结构数据
	{
		*pData ++= *pSource++;  
	} 	
	for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++) //读取APP部分存的数据
	{
		*pData ++= *pSource++;  
	} 
	gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;        //加1为0
	EraseSysInfoBlock();                             //擦拭 
	SaveSysInfo();                                   //做备份pos=0
	SaveSysInfo();                                   //当前数据pos=1
} 
/**
  * @brief  写备份系统信息到FLASH，在初始化后使用
  */
static void SaveSysInfoBackup(void)                            //擦拭保存备份
{
	unsigned char *pData=NULL;                            //数据指针
	unsigned int  unAddr=0;                              //写入地址
//	unsigned int  *punPos=&gstSystemInfo.unPos;          //位置指针
//	unsigned int  punMask=gstSystemInfo.unPosMask;       //位置掩码
	unsigned int  cnt=0;
	unsigned char *pBackUpData=malloc(SYS_INFO_SECTOR_SIZE);                                   //申请一个SECTOR内存空间
	if(pBackUpData==NULL)  while(1);
	memcpy(pBackUpData,(void*)gstSystemInfo.unSysBackUpADDR,SYS_INFO_SECTOR_SIZE);          //取备份数据
	EraseSysInfoBlock();          //擦拭 
	((stSystemInformationType*)pBackUpData)->unPos=0;         //标记位置0
	unAddr=SYSTEM_INIT_INFO_ADDR;
	HAL_FLASH_Unlock();                                //解锁 可写 
	
	//写备份数据
	for(cnt=0,pData=(void*)pBackUpData;cnt<SYS_INFO_SECTOR_SIZE ;cnt++)     //写入备份数据到位置0
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); //写入数据 
		unAddr++;                                      
		pData++;
	}
	free(pBackUpData);                          //释放空间
	pBackUpData=NULL;
	gstSystemInfo.unPos=1;                     //标记当前结构数据位置为1
	gstSystemInfo.unSysBackUpADDR=SYSTEM_INIT_INFO_ADDR;           //备份地址设置为位置0地址

	//写当前数据
	for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE ;cnt++) //写入控制结构体数据
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); //写入当前数据 
		unAddr++;                                     //地址偏移
		pData++;
	}
	for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE ;cnt++) //写入bootloader部分数据
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); //写入当前数据 
		unAddr++;                                     //地址偏移
		pData++;
	}  
	for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++)     //写入APP部分数据
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,unAddr,*pData); //写入当前数据 
		unAddr++;                                     //地址偏移
		pData++;
	} 
	HAL_FLASH_Lock();  
}
/**
  * @brief  写系统信息到FLASH，在初始化后使用
  */
void SaveSysInfo(void)  
{
	unsigned char *pData=NULL;  //当前数据           
	unsigned int  unAddr=0;       //存储地址
	unsigned int  *punPos=&gstSystemInfo.unPos;     //flash上位置
	//unsigned int  punMask=gstSystemInfo.unPosMask; //位置掩码
	unsigned int  cnt=0;
	
	if(((punPos[0])>=SYS_INFO_SECTOR_CNT_MAX-1) &&(punPos[0]!=UNSIGNED_INT_VAL_MAX))       //判断是否是否存满
	{ 
		SaveSysInfoBackup();                          //存满重新擦拭写入最后两块数据
	}
	else                                                                           //未存满
	{ 
		punPos[0]++;                                                          //位置偏移1 ，存入下一个位置
		unAddr=(punPos[0])*SYS_INFO_SECTOR_SIZE+SYSTEM_INIT_INFO_ADDR;           //地址偏移一块
		gstSystemInfo.unSysBackUpADDR=(punPos[0]-1)*SYS_INFO_SECTOR_SIZE+SYSTEM_INIT_INFO_ADDR;//设置备份地址
		if(gstSystemInfo.unSysBackUpADDR<SYSTEM_INIT_INFO_ADDR)                            
			gstSystemInfo.unSysBackUpADDR=SYSTEM_INIT_INFO_ADDR;
		HAL_FLASH_Unlock();                                //解锁可写 
		for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE ;cnt++)//写入当前数据 
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
  * @brief  更新系统信息到RAM，在初始化后使用
  */
void RefreshSysInfo(void)
{ 
	unsigned char *pSource=NULL;    //源数据指针
	unsigned char *pData=NULL;       //RAM数据指针 
	unsigned int  unAddr=0;         //地址
	unsigned int  unPos=0;          //flash位置标记
	unsigned int  punMask=gstSystemInfo.unPosMask;
	unsigned int  cnt=0;
	unAddr=SYSTEM_INIT_INFO_ADDR;                       
	for(cnt=0;cnt<SYS_INFO_SECTOR_CNT_MAX+1;)       //根据 unPos判断flash最新数据位置
	{
		unPos=((stSystemInformationType * )unAddr)-> unPos;         //获取位置
		if((unPos !=cnt) ||(unPos>=SYS_INFO_SECTOR_CNT_MAX))    //判断是否是最新数据位置                           
		{
			break;
		}
		cnt++; 
		unAddr+=SYS_INFO_SECTOR_SIZE;                     //偏移一个sector
	}
	if(cnt==0)                                          //从未存储过系统信息
	{
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //存两次做一次备份
		SaveSysInfo();
		SaveSysInfo();
		unAddr=SYSTEM_INIT_INFO_ADDR+SYS_INFO_SECTOR_SIZE;
	}
	else                                                //存入过系统信息
	{              
		unAddr-=SYS_INFO_SECTOR_SIZE;                           //减一指向最新存储位置
	}		
	pSource=(  unsigned char  *)unAddr;             //获取flash地址指针
	for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE;cnt++) //读取控制结构体数据
	{
		*pData ++= *pSource++;  
	} 
	for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE ;cnt++)//读取bootloader数据
	{
		*pData ++= *pSource++;  
	} 
	for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++) //读取APP系统数据
	{
		*pData ++= *pSource++;  
	}  
}  
/**
  * @brief   更新数据存储的ram地址指针并保存 
  */
static void RefreshCurrentRam(void)
{
    unsigned char *pSource=NULL;    //源数据指针
	unsigned char *pData=NULL;       //RAM数据指针 
	unsigned int  unAddr=0;         //地址
	unsigned int  unPos=0;          //flash位置标记
	unsigned int  punMask=gstSystemInfo.unPosMask;
	unsigned int  cnt=0;
	unAddr=SYSTEM_INIT_INFO_ADDR;                       
	for(cnt=0;cnt<SYS_INFO_SECTOR_CNT_MAX+1;)       //根据 unPos判断flash最新数据位置
	{
		unPos=((stSystemInformationType * )unAddr)-> unPos;         //获取位置
		if((unPos !=cnt) ||(unPos>=SYS_INFO_SECTOR_CNT_MAX))    //判断是否是最新数据位置                           
		{
			break;
		}
		cnt++; 
		unAddr+=SYS_INFO_SECTOR_SIZE;                     //偏移一个sector
	}
	if(cnt==0)                                          //从未存储过系统信息
	{
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //存两次做一次备份
		SaveSysInfo();
		SaveSysInfo();
		unAddr=SYSTEM_INIT_INFO_ADDR+SYS_INFO_SECTOR_SIZE;
	}
	else                                                //存入过系统信息
	{              
		unAddr-=SYS_INFO_SECTOR_SIZE;                           //减一指向最新存储位置
	}		
	pSource=(  unsigned char  *)unAddr;             //获取flash地址指针
	for(cnt=0,pData=(void*)&gstSystemInfo;cnt<SYS_INFO_CTRL_STRUCT_SIZE;cnt++) //读取控制结构体数据
	{
		//*pData ++=
		pSource++;  
	} 
	for(cnt=0,pData=gstSystemInfo.pucBtLoaderSysInfo;cnt<BOOT_SYS_INFO_SIZE ;cnt++)//读取bootloader数据
	{
		*pData ++= *pSource++;  
	} 
	for(cnt=0,pData=gstSystemInfo.pucAppSysInfo;cnt<APP_SYS_INFO_SIZE;cnt++) //读取APP系统数据
	{
		*pData ++= *pSource++;  
	}
	SaveSysInfo();    	
}
/**
  * @brief   bootloader中初始化
  *          升级部分结构和控制结构为固定长度，过程会申请APP数据空间 
  */
void SystemInfoInit_BtLoader(void)     
{
	//InterFlash_SetBankMode(SINGLE_BANK_MODE); //设置为single bank mode  
	
	gstSystemInfo.unBtLoaderSysInfoSize=0;
	gstSystemInfo.unAppSysInfoSize=0;
	SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //初始化总长度   
	gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE); 
	RefreshSysInfo();                            //读gstSystemInfo信息
	if(gstSystemInfo.unPosMask!=NOT_FIRST_TIMES_FLAG_VAL)
	{ 
		gstSystemInfo.unPosMask=NOT_FIRST_TIMES_FLAG_VAL; //设置第一次写标志
		//boot sys info
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;  //bt固定长度
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;   //取升级结构ram地址
		//app sys info
		APP_SYS_INFO_SIZE=APP_SYS_INFO_DEFAULT_SIZE;    //APP默认开1K
		gstSystemInfo.pucAppSysInfo=malloc(APP_SYS_INFO_SIZE);  //开辟空间
		//size 
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //初始化总长度   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE);                //128k系统存储区分的总sector数
		
		
		
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //存两次做一次备份
		EraseSysInfoBlock();
		SaveSysInfo();
		SaveSysInfo();  
		
	}
	else
	{
		//boot 
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;  //固定长度
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;   //取升级结构ram地址
		//size 
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //初始化总长度   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE); 
		//app
		gstSystemInfo.pucAppSysInfo=malloc(APP_SYS_INFO_SIZE);  //开辟空间
		if(gstSystemInfo.pucAppSysInfo==NULL) while(1);  
		
		RefreshCurrentRam();                      
	}  
}
/**
  * @brief   App中初始化
  *          升级结构和控制结构为固定长度
  * @param   pAppSysInfo:APP需要存储的系统信息起始地址指针
  * @param   len: APP系统信息数据长度
  */
void SystemInfoInit_App(void * pAppSysInfo,unsigned int len)               //App 调用系统信息初始化
{
  //	InterFlash_SetBankMode(SINGLE_BANK_MODE); //设置为single bank mode  
	
	gstSystemInfo.unBtLoaderSysInfoSize=0;
	gstSystemInfo.unAppSysInfoSize=0; 
	SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //初始化总长度 
	gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE); 	
	RefreshSysInfo();                            //读gstSystemInfo信息
	if(gstSystemInfo.unPosMask!=NOT_FIRST_TIMES_FLAG_VAL)
	{
		gstSystemInfo.unPosMask=NOT_FIRST_TIMES_FLAG_VAL; //设置第一次写标志
		//boot sys info
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;  //bt固定长度
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;   //取升级结构ram地址
		//app sys info
		APP_SYS_INFO_SIZE=len;    //APP默认开1K 
		gstSystemInfo.pucAppSysInfo=pAppSysInfo;
		//size 
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //初始化总长度   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE);                //128k系统存储区分的总sector数
		//
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //存两次做一次备份
		EraseSysInfoBlock();
		SaveSysInfo();
		SaveSysInfo();  
	}
	else  if(gstSystemInfo.unAppSysInfoSize!=len)
	{ 
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;      //取升级结构ram地址 
		
		gstSystemInfo.pucAppSysInfo=pAppSysInfo;                           //取APP数据指针
		gstSystemInfo.unAppSysInfoSize=len;   
		
		
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //初始化总长度   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE);  
		
		RefreshCurrentRam();  //复位后第一次初始化（将ram上的数据指针保存下来）
		
		gstSystemInfo.unPos=UNSIGNED_INT_VAL_MAX;	        //存两次做一次备份
		EraseSysInfoBlock();
		SaveSysInfo();
		SaveSysInfo(); 
	} 
	else             //长度没有更改
	{
		gstSystemInfo.unBtLoaderSysInfoSize=BOOT_LOADER_SYS_INFO_SIZE;
		gstSystemInfo.pucBtLoaderSysInfo=(unsigned char *)&gstUpdate;      //取升级结构ram地址 
		
		gstSystemInfo.pucAppSysInfo=pAppSysInfo;                           //取APP数据指针
		gstSystemInfo.unAppSysInfoSize=len;    
		
		SYS_INFO_SECTOR_SIZE=(SYS_INFO_CTRL_STRUCT_SIZE+APP_SYS_INFO_SIZE+BOOT_SYS_INFO_SIZE);      //初始化总长度   
		gstSystemInfo.unBlockDevideCntAmount= (NV_SIZE_MAX /SYS_INFO_SECTOR_SIZE); 
		
		RefreshCurrentRam(); //复位后第一次初始化（将ram上的数据指针保存下来）
	}
}













