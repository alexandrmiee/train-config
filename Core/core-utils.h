/*
 * utils.h
 *
 *  Created on: 2018 07 06.
 *      Author: AL
 */

#ifndef CORE_UTILS_H_
#define CORE_UTILS_H_

#define EXIT_NEED_MORE_MEMORY	2

#ifdef WIN32
	#include<windows.h>
	extern HANDLE hConsole;
	extern WORD saved_attributes;
	extern CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	#define COLOR_RED do{\
						hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
						GetConsoleScreenBufferInfo(hConsole, &consoleInfo);\
						saved_attributes = consoleInfo.wAttributes;\
						SetConsoleTextAttribute(hConsole, FOREGROUND_RED);\
						}while(0)
	#define COLOR_RESET do{\
							SetConsoleTextAttribute(hConsole, saved_attributes);\
						}while(0)
	#define try(express,exception,exit) do{\
											if(!express){\
												COLOR_RED;\
												printf(exception" in line %d of file \"%s\".\n",__LINE__, __FILE__);\
												COLOR_RESET;\
												COLOR_RESET;\
												return exit;\
											}\
										}while(0)
#else
	#ifdef DEBUG_TRACE
		#define TR(trace)	do{\
									trace;\
								}while(0)
	#else
		#define TR(trace)
	#endif
	#define try(express,exception,exit) do{\
											if(!express){\
												TR(printf(exception" in line %d of file \"%s\".\n",__LINE__, __FILE__););\
												return exit;\
											}\
										}while(0)

#endif

#define getTime() 1

int equals(char *base,char *search,uint16_t len);

typedef uint64_t dtime_t;

#ifdef WIN32
#define RTC_TIME(t) t=time(NULL)
#elif __linux__
#define RTC_TIME(t) t=time(NULL)
#else
extern dtime_t rtcTime_s;

#define UPRTC	do{dtime_t tmp = TIM_GetCounter(TIM_RTC);\
					rtcTime_s += (tmp>=rtcTime_s)?(tmp-rtcTime_s):(UINT32_MAX-rtcTime_s+tmp);\
				}while(0)

#define RTC_TIME(t) do{UPRTC;t=rtcTime_s;}while(0)
#endif

#endif /* CORE_UTILS_H_ */
