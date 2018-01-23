#include "interflash.h"
#include "stm32f7xx_hal.h"
#include "math.h"
#define SECTOR_COUNT_MAX    24
#define BANK_MODE           2 
const unsigned char cgcFlashSector_K[BANK_MODE][SECTOR_COUNT_MAX]={           //sector大小数组K为单位（值从0~255 0代表（0+1）K，255代表（255+1）K）
                                 {
																	 //DUAL BANK1
																	 15,15,15,15,                         //sector0~3        
																	 63,                                   //sector4
																	 127,127,127,127,127,127,127,          //sector5~11
																	 //DUAL BANK2
																	 15,15,15,15,                          //sector12~15
																	 63,                                   //sector16
																	 127,127,127,127,127,127,127         //sector17~23 
																 },
																	 //SINGLE BANK
																 {	 31,31,31,31,                        //sector0~3
																	 127,                               //sector4
																	 255,255,255,255,255,255,255,         //sector5~11
																	 0,0,0,0,0, 0,0,0,0,0, 0,0,  }
};  

#ifdef  INTER_FLASH_TEST 
char buf[100];  
void interflash_test(void)   //start task调用此函数创建测试任务
{
		#define WRITE_ADDR    (0x8000000+70000) 
	InterFlash_Read(WRITE_ADDR,buf,50,FLASH_TYPEPROGRAM_BYTE);
	InterFlash_Program(WRITE_ADDR,"123456789abcdefghijk",50,FLASH_TYPEPROGRAM_BYTE);
	InterFlash_Read(WRITE_ADDR,buf,50,FLASH_TYPEPROGRAM_BYTE);
  while(1)
	{}
} 
#endif

/**
  * @brief  根据实际地址获取擦拭sector编号
  * @note   在擦拭芯片时被调用 
  * @param  useraddr: 用户实际地址
  * @retval 擦拭setor编号 single mode 0~11 dual mode 0~23
  */
static unsigned int  GetSectorAddr(unsigned int useraddr)   
{
	unsigned int sector =0,addrtemp=0;
	unsigned char  nDbankMode=0;
  nDbankMode=InterFlash_GetBankMode();          //获取Bank模式 
	 for(sector=0;sector<23;sector++)
	{
		addrtemp+=(cgcFlashSector_K[nDbankMode][sector]+1)*1024;  //累加sector
		if(addrtemp+INTER_FLASH_BASE>=useraddr) 
			return sector+1; //对比获取sector编号
	}
	return sector;
} 

/**
  * @brief  擦拭内部flash 
  * @note   以sector为单位，擦拭起始sector到offset偏移位置sector
  * @param  Addr: 用户实际地址
  * @param  offset: 对Addr地址偏移
  * @retval ErseError: pointer to variable  that
  *         contains the configuration information on faulty sector in case of error 
  *         (0xFFFFFFFF means that all the sectors have been correctly erased)
  */
int InterFlash_EraseSector(unsigned int  Addr,unsigned int offset)
{
	FLASH_EraseInitTypeDef   EraseInitStruct;
	unsigned int ErseError,FirstSector,NbOfSectors;
	
	FirstSector=GetSectorAddr(Addr);                        //获取sector
	NbOfSectors=GetSectorAddr(Addr+offset)-FirstSector;    //获取需要擦拭sector个数
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;
	HAL_FLASH_Unlock();                                      //解除flash保护
	HAL_FLASHEx_Erase(&EraseInitStruct,&ErseError);         //擦拭sector
	HAL_FLASH_Lock();                                       //锁定flash
	
	return ErseError;
}
/**
  * @brief  内部flash写 
  * @note   内部flash写可以以byte halfword word double word形式写入
  * @param  Addr: 用户实际地址
  * @param  pBuf: 写入数据buf指针
  * @param  DataCnt_Byte: 写入数据大小以byte的个数记
  * @param  DataType: 写入数据类型 FLASH_TYPEPROGRAM_BYTE，FLASH_TYPEPROGRAM_HALFWORD，
                      FLASH_TYPEPROGRAM_WORD，FLASH_TYPEPROGRAM_DOUBLEWORD
  * @retval HAL_StatusTypeDef HAL Status
  */
int InterFlash_Program(unsigned int Addr,void* pBuf,unsigned int DataCnt_Byte,unsigned int  DataType)
{
	unsigned char      *p8Data=(unsigned char *)pBuf;
	unsigned short     *p16Data=(unsigned short *)pBuf;
	unsigned int       *p32Data=(unsigned int *)pBuf;
	unsigned long long *p64Data=(unsigned long long *)pBuf;   //pBuf强制转换为不同类型指针方便取值
  int  Status;                                              
	unsigned char DataTypeSize  ;                           //数据类型尺寸 Byte=1,short=2
	unsigned int  DataMax,cnt=0  ;
	DataTypeSize =pow(2,DataType);                          //2^DataType 得到尺寸
	DataMax =DataCnt_Byte/DataTypeSize ;                   //DataMax写入数据次数 
	InterFlash_EraseSector(Addr,DataCnt_Byte);             //擦拭sector
	HAL_FLASH_Unlock();
	for(cnt=0;cnt<DataMax;cnt++)
	{
	 switch (DataType)
	 { 
		 case FLASH_TYPEPROGRAM_BYTE:
			 Status=HAL_FLASH_Program(DataType,Addr,p8Data[cnt]);   //写入byte
			 break;
		 case FLASH_TYPEPROGRAM_HALFWORD:
			 Status=HAL_FLASH_Program(DataType,Addr,p16Data[cnt]);  //写入halfword
			 break;
     case FLASH_TYPEPROGRAM_WORD:
			 Status=HAL_FLASH_Program(DataType,Addr,p32Data[cnt]);  //写入word
			 break;	
     case FLASH_TYPEPROGRAM_DOUBLEWORD:
			Status=HAL_FLASH_Program(DataType,Addr,p64Data[cnt]);  //写入doubleword
			 break;		 
	 } 
	 Addr+=DataTypeSize;                                     //地址偏移
	}
	HAL_FLASH_Lock();
	return Status;
}
/**
  * @brief  内部flash读 
  * @note   内部flash读可以以byte halfword word double word形式读
  * @param  Addr: 用户实际地址
  * @param  pBuf: 写入数据buf指针
  * @param  DataCnt_Byte: 读数据大小以byte的个数记
  * @param  DataType: 读数据类型 FLASH_TYPEPROGRAM_BYTE，FLASH_TYPEPROGRAM_HALFWORD，
                      FLASH_TYPEPROGRAM_WORD，FLASH_TYPEPROGRAM_DOUBLEWORD 
  */
void InterFlash_Read(unsigned int Addr,void* pBuf,unsigned int DataCnt_Byte ,unsigned int  DataType)
{
	unsigned char      *p8Data=(unsigned char *)pBuf;
	unsigned short     *p16Data=(unsigned short *)pBuf;
	unsigned int       *p32Data=(unsigned int *)pBuf;
	unsigned long long *p64Data=(unsigned long long *)pBuf;//pBuf强制转换为不同类型指针方便取值
	
	unsigned char DataTypeSize  ;//数据类型尺寸 Byte=1,short=2
	unsigned int  DataMax ,cnt=0 ;
	DataTypeSize =pow(2,DataType);                          //2^DataType 得到尺寸
	DataMax =DataCnt_Byte/DataTypeSize;                   //DataMax 读数据次数 
	for(cnt=0;cnt<DataMax;cnt++)
	{
	 switch (DataType)
	 { 
		 case FLASH_TYPEPROGRAM_BYTE: 
		   p8Data[cnt]=*(volatile unsigned char *)Addr;        //读byte数据
			 break;
		 case FLASH_TYPEPROGRAM_HALFWORD: 
		   p16Data[cnt]=*(volatile unsigned short *)Addr;        //读halfword数据
			 break;
     case FLASH_TYPEPROGRAM_WORD: 
		   p32Data[cnt]=*(volatile unsigned int *)Addr;        //读word数据
			 break;	
     case FLASH_TYPEPROGRAM_DOUBLEWORD: 
		   p64Data[cnt]=*(volatile unsigned long long *)Addr;        //读doubleword数据
			 break;		
	   }	
    Addr+=DataTypeSize;	                                     //地址偏移
	 }  
}
/**
  * @brief  内部flash 设置bank模式 
  * @note   两种模式：SINGLE BANK  DUAL BANK 对sector的划分不一样
  * @param  bankmode: 模式eBlockModeType枚举量：SINGLE_BANK_MODE ，DUAL_BANK_MODE 
  * @retval 设置成功返回0，不成功返回1
  */
int InterFlash_SetBankMode(eBlockModeType bankmode)
{
	FLASH_OBProgramInitTypeDef    OBInit;
	HAL_FLASH_Unlock();                //flash解锁
  HAL_FLASH_OB_Unlock();             //option flash解锁
  HAL_FLASHEx_OBGetConfig(&OBInit);      //获取配置信息
	if(bankmode==SINGLE_BANK_MODE)        
	OBInit.USERConfig|=OB_NDBANK_SINGLE_BANK;
	else OBInit.USERConfig&=~OB_NDBANK_SINGLE_BANK;
	HAL_FLASHEx_OBProgram( &OBInit);          //写入配置信息
	HAL_FLASH_Lock();
	HAL_FLASH_OB_Lock();                     //锁定
	if(InterFlash_GetBankMode()==bankmode) return 0; //读状态自检返回是否成功
	else return 1;
}
/**
  * @brief  内部flash 获取bank模式 
  * @note   两种模式：SINGLE BANK  DUAL BANK 对sector的划分不一样 
  * @retval 模式eBlockModeType枚举量：SINGLE_BANK_MODE ，DUAL_BANK_MODE 
  */
eBlockModeType InterFlash_GetBankMode(void)
{
	FLASH_OBProgramInitTypeDef    OBInit;
  HAL_FLASHEx_OBGetConfig(&OBInit);                           //获取配置信息
	if((OBInit.USERConfig&OB_NDBANK_SINGLE_BANK)==OB_NDBANK_SINGLE_BANK) //判断对应位是否被置位
		return SINGLE_BANK_MODE;
	else 
		return DUAL_BANK_MODE;                   //返回结果
}


