
#include "crc.h"
void crc_test()
{
	unsigned char buf[]={0xaa,0xbb,0x01,0xff,0xff};
	uint16_t data=0;
	data = cyg_crc16(buf, 5);

}


