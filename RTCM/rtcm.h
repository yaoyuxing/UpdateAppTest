#ifndef __RTCM_H__
#define __RTCM_H__
#include "qxwz_rtcm.h"
/*
����RTCM����typedef void(*qxwz_rtcm_response)(char* rtcm, size_t length);
*/
void  QxRtcmResponse(char* rtcm, size_t length);

/*
״̬��������
typedef void(*qxwz_status_response)(qxwz_rtcm_status_code status);
*/
void  QxStatusResponse(qxwz_rtcm_status_code status);

#endif
