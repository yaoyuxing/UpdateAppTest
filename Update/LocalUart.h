#ifndef LOCAL_UART_H
#define LOCAL_UART_H

typedef enum 
{
	LOCAL_UART_UPDATE='U',
	LOCAL_UART_GET_INFO='G'
}eLocalUartCmdType;
#define RX_DATA_LEN_MAX  1024
typedef struct rx_data_st
{
	unsigned char  ucCmd;
	unsigned short usDataLen;
	unsigned char  ucData[RX_DATA_LEN_MAX];
}stRxDataType;



void CreateLocalUartUpdateTask(void);
void RunAPP(unsigned int jumpAdr);
void LocalUartInit(unsigned int Baud );

#endif
