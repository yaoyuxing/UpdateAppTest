#include "HX_DU1018D_AT.h"
#include "HX_DU1018D.h"
#include "delay.h"
//µÁÃ®≤‚ ‘¥˙¬Î
void Du1018d_Test(void )
{
	u8 channel='6';
	UART7_init(9600);
	Du1018dConfigurationInit(&gDu1018dStatus);

	Du1018dSetChannel(&channel,1,&gDu1018dStatus);
	while(1)
	{
		Du1018dSendByteS("#012345678\r\n");

		delay_ms(100);
	}


}




