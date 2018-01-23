#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H




typedef struct sys_private_val
{
	 unsigned int unPos;         //flashλ�ñ��
	 unsigned int unSysBackUpADDR;  //��������λ��
	 unsigned int unBtLoaderSysInfoSize;//����������Ϣ
	 unsigned int unAppSysInfoSize;  //APP����ϵͳ��Ϣ����
	 unsigned int unBlockDevideCntAmount;//ϵͳ��Ϣ�ܹ���Ϊ���ٸ�
	 unsigned int unInfoSingleSize;      //����ϵͳ��Ϣ��С =���ƽṹ��С+��������ϵͳ��Ϣ��С+APP����ϵͳ��Ϣ��С
     unsigned int  unPosMask;         //λ�����뱣֤���ݲ����ҵ�
	 unsigned char        *pucAppSysInfo ;     //APP����ϵͳ��Ϣָ��
	 unsigned char        *pucBtLoaderSysInfo ;//��������ϵͳ��Ϣָ��
}stSystemInformationType;



/**
  * @brief  ���±���ϵͳ��Ϣ��RAM�����±��ݴ洢���ڳ�ʼ����ʹ��
  */
void RefreshSysInfoBackup(void);
/**
  * @brief  дϵͳ��Ϣ��FLASH���ڳ�ʼ����ʹ��
  */
void SaveSysInfo(void);
/**
  * @brief  ����ϵͳ��Ϣ��RAM���ڳ�ʼ����ʹ��
  */
void RefreshSysInfo(void);
/**
  * @brief   bootloader�г�ʼ��
  *          �̶����������ṹ�Ϳ��ƽṹ������APP���ݿռ�
  *
  */
void SystemInfoInit_BtLoader(void);
/**
  * @brief   bootloader�г�ʼ��
  *          �̶����������ṹ�Ϳ��ƽṹ 
  * @param   pAppSysInfo:APP��Ҫ�洢��ϵͳ��Ϣ��ʼ��ַָ��
  * @param   len: APPϵͳ��Ϣ���ݳ���
  */
void SystemInfoInit_App(void * pAppSysInfo,unsigned int len);



 
#endif 
