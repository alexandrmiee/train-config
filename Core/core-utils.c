/*
 * utils.c
 *
 *  Created on: 2018 07 06
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef WIN32
	#include<windows.h>
	HANDLE hConsole;
	WORD saved_attributes;
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
#endif
/*
 * compare two strings
 * char *base string
 * char *serach string
 * uint16_t len of search sctring
 * return
 * -1 if(base<search)
 * 1 if(search not equal base)
 * 0 if base contain search
 */
int equals(char *base,char *search,uint16_t len){
	for(uint16_t i=0;i<len;i++){
		if(base[i]=='\0') return -1;
		if(search[i]!=base[i]) return 1;
	}
	return 0;
}
