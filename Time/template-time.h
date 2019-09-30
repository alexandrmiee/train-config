#ifndef __TIME_H__
#define __TIME_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint64_t dtime_t;

#ifdef WIN32
#define RTC_TIME(t) t=time(NULL)
#else
extern dtime_t rtcTime_s;

#define UPRTC	do{dtime_t tmp = TIM_GetCounter(TIM_RTC);\
					rtcTime_s += (tmp>=rtcTime_s)?(tmp-rtcTime_s):(UINT32_MAX-rtcTime_s+tmp);\
				}while(0)

#define RTC_TIME(t) do{UPRTC;t=rtcTime_s;}while(0)
#endif

#endif /*__TIME_H__*/
