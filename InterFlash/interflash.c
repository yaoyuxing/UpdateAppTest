#include "interflash.h"
#include "stm32f7xx_hal.h"
#include "math.h"
#define SECTOR_COUNT_MAX    24
#define BANK_MODE           2 
const unsigned char cgcFlashSector_K[BANK_MODE][SECTOR_COUNT_MAX]={           //sector��С����KΪ��λ��ֵ��0~255 0����0+1��K��255����255+1��K��
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
void interflash_test(void)   //start task���ô˺���������������
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
  * @brief  ����ʵ�ʵ�ַ��ȡ����sector���
  * @note   �ڲ���оƬʱ������ 
  * @param  useraddr: �û�ʵ�ʵ�ַ
  * @retval ����setor��� single mode 0~11 dual mode 0~23
  */
static unsigned int  GetSectorAddr(unsigned int useraddr)   
{
	unsigned int sector =0,addrtemp=0;
	unsigned char  nDbankMode=0;
  nDbankMode=InterFlash_GetBankMode();          //��ȡBankģʽ 
	 for(sector=0;sector<23;sector++)
	{
		addrtemp+=(cgcFlashSector_K[nDbankMode][sector]+1)*1024;  //�ۼ�sector
		if(addrtemp+INTER_FLASH_BASE>=useraddr) 
			return sector+1; //�ԱȻ�ȡsector���
	}
	return sector;
} 

/**
  * @brief  �����ڲ�flash 
  * @note   ��sectorΪ��λ��������ʼsector��offsetƫ��λ��sector
  * @param  Addr: �û�ʵ�ʵ�ַ
  * @param  offset: ��Addr��ַƫ��
  * @retval ErseError: pointer to variable  that
  *         contains the configuration information on faulty sector in case of error 
  *         (0xFFFFFFFF means that all the sectors have been correctly erased)
  */
int InterFlash_EraseSector(unsigned int  Addr,unsigned int offset)
{
	FLASH_EraseInitTypeDef   EraseInitStruct;
	unsigned int ErseError,FirstSector,NbOfSectors;
	
	FirstSector=GetSectorAddr(Addr);                        //��ȡsector
	NbOfSectors=GetSectorAddr(Addr+offset)-FirstSector;    //��ȡ��Ҫ����sector����
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;
	HAL_FLASH_Unlock();                                      //���flash����
	HAL_FLASHEx_Erase(&EraseInitStruct,&ErseError);         //����sector
	HAL_FLASH_Lock();                                       //����flash
	
	return ErseError;
}
/**
  * @brief  �ڲ�flashд 
  * @note   �ڲ�flashд������byte halfword word double word��ʽд��
  * @param  Addr: �û�ʵ�ʵ�ַ
  * @param  pBuf: д������bufָ��
  * @param  DataCnt_Byte: д�����ݴ�С��byte�ĸ�����
  * @param  DataType: д���������� FLASH_TYPEPROGRAM_BYTE��FLASH_TYPEPROGRAM_HALFWORD��
                      FLASH_TYPEPROGRAM_WORD��FLASH_TYPEPROGRAM_DOUBLEWORD
  * @retval HAL_StatusTypeDef HAL Status
  */
int InterFlash_Program(unsigned int Addr,void* pBuf,unsigned int DataCnt_Byte,unsigned int  DataType)
{
	unsigned char      *p8Data=(unsigned char *)pBuf;
	unsigned short     *p16Data=(unsigned short *)pBuf;
	unsigned int       *p32Data=(unsigned int *)pBuf;
	unsigned long long *p64Data=(unsigned long long *)pBuf;   //pBufǿ��ת��Ϊ��ͬ����ָ�뷽��ȡֵ
  int  Status;                                              
	unsigned char DataTypeSize  ;                           //�������ͳߴ� Byte=1,short=2
	unsigned int  DataMax,cnt=0  ;
	DataTypeSize =pow(2,DataType);                          //2^DataType �õ��ߴ�
	DataMax =DataCnt_Byte/DataTypeSize ;                   //DataMaxд�����ݴ��� 
	InterFlash_EraseSector(Addr,DataCnt_Byte);             //����sector
	HAL_FLASH_Unlock();
	for(cnt=0;cnt<DataMax;cnt++)
	{
	 switch (DataType)
	 { 
		 case FLASH_TYPEPROGRAM_BYTE:
			 Status=HAL_FLASH_Program(DataType,Addr,p8Data[cnt]);   //д��byte
			 break;
		 case FLASH_TYPEPROGRAM_HALFWORD:
			 Status=HAL_FLASH_Program(DataType,Addr,p16Data[cnt]);  //д��halfword
			 break;
     case FLASH_TYPEPROGRAM_WORD:
			 Status=HAL_FLASH_Program(DataType,Addr,p32Data[cnt]);  //д��word
			 break;	
     case FLASH_TYPEPROGRAM_DOUBLEWORD:
			Status=HAL_FLASH_Program(DataType,Addr,p64Data[cnt]);  //д��doubleword
			 break;		 
	 } 
	 Addr+=DataTypeSize;                                     //��ַƫ��
	}
	HAL_FLASH_Lock();
	return Status;
}
/**
  * @brief  �ڲ�flash�� 
  * @note   �ڲ�flash��������byte halfword word double word��ʽ��
  * @param  Addr: �û�ʵ�ʵ�ַ
  * @param  pBuf: д������bufָ��
  * @param  DataCnt_Byte: �����ݴ�С��byte�ĸ�����
  * @param  DataType: ���������� FLASH_TYPEPROGRAM_BYTE��FLASH_TYPEPROGRAM_HALFWORD��
                      FLASH_TYPEPROGRAM_WORD��FLASH_TYPEPROGRAM_DOUBLEWORD 
  */
void InterFlash_Read(unsigned int Addr,void* pBuf,unsigned int DataCnt_Byte ,unsigned int  DataType)
{
	unsigned char      *p8Data=(unsigned char *)pBuf;
	unsigned short     *p16Data=(unsigned short *)pBuf;
	unsigned int       *p32Data=(unsigned int *)pBuf;
	unsigned long long *p64Data=(unsigned long long *)pBuf;//pBufǿ��ת��Ϊ��ͬ����ָ�뷽��ȡֵ
	
	unsigned char DataTypeSize  ;//�������ͳߴ� Byte=1,short=2
	unsigned int  DataMax ,cnt=0 ;
	DataTypeSize =pow(2,DataType);                          //2^DataType �õ��ߴ�
	DataMax =DataCnt_Byte/DataTypeSize;                   //DataMax �����ݴ��� 
	for(cnt=0;cnt<DataMax;cnt++)
	{
	 switch (DataType)
	 { 
		 case FLASH_TYPEPROGRAM_BYTE: 
		   p8Data[cnt]=*(volatile unsigned char *)Addr;        //��byte����
			 break;
		 case FLASH_TYPEPROGRAM_HALFWORD: 
		   p16Data[cnt]=*(volatile unsigned short *)Addr;        //��halfword����
			 break;
     case FLASH_TYPEPROGRAM_WORD: 
		   p32Data[cnt]=*(volatile unsigned int *)Addr;        //��word����
			 break;	
     case FLASH_TYPEPROGRAM_DOUBLEWORD: 
		   p64Data[cnt]=*(volatile unsigned long long *)Addr;        //��doubleword����
			 break;		
	   }	
    Addr+=DataTypeSize;	                                     //��ַƫ��
	 }  
}
/**
  * @brief  �ڲ�flash ����bankģʽ 
  * @note   ����ģʽ��SINGLE BANK  DUAL BANK ��sector�Ļ��ֲ�һ��
  * @param  bankmode: ģʽeBlockModeTypeö������SINGLE_BANK_MODE ��DUAL_BANK_MODE 
  * @retval ���óɹ�����0�����ɹ�����1
  */
int InterFlash_SetBankMode(eBlockModeType bankmode)
{
	FLASH_OBProgramInitTypeDef    OBInit;
	HAL_FLASH_Unlock();                //flash����
  HAL_FLASH_OB_Unlock();             //option flash����
  HAL_FLASHEx_OBGetConfig(&OBInit);      //��ȡ������Ϣ
	if(bankmode==SINGLE_BANK_MODE)        
	OBInit.USERConfig|=OB_NDBANK_SINGLE_BANK;
	else OBInit.USERConfig&=~OB_NDBANK_SINGLE_BANK;
	HAL_FLASHEx_OBProgram( &OBInit);          //д��������Ϣ
	HAL_FLASH_Lock();
	HAL_FLASH_OB_Lock();                     //����
	if(InterFlash_GetBankMode()==bankmode) return 0; //��״̬�Լ췵���Ƿ�ɹ�
	else return 1;
}
/**
  * @brief  �ڲ�flash ��ȡbankģʽ 
  * @note   ����ģʽ��SINGLE BANK  DUAL BANK ��sector�Ļ��ֲ�һ�� 
  * @retval ģʽeBlockModeTypeö������SINGLE_BANK_MODE ��DUAL_BANK_MODE 
  */
eBlockModeType InterFlash_GetBankMode(void)
{
	FLASH_OBProgramInitTypeDef    OBInit;
  HAL_FLASHEx_OBGetConfig(&OBInit);                           //��ȡ������Ϣ
	if((OBInit.USERConfig&OB_NDBANK_SINGLE_BANK)==OB_NDBANK_SINGLE_BANK) //�ж϶�Ӧλ�Ƿ���λ
		return SINGLE_BANK_MODE;
	else 
		return DUAL_BANK_MODE;                   //���ؽ��
}


