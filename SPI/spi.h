#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


extern SPI_HandleTypeDef SPI5_Handler;  //SPI¾ä±ú

void SPI5_Init(void);
void SPI5_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI5_ReadWriteByte(u8 TxData);
#endif
