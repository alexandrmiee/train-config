/*
 * RestFullController.c
 *
 *  Created on: 3 èþë. 2018 ã.
 *      Author: home
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "RestFullController.h"

/*
 * Request string parser
 * 1) find command in request
 * 2) copy request string to restBuf
 * 3) split request string into command arguments by tokens
 * 4) create an array of arguments and calculate number of arguments
 *
 * char *request pointer to request string
 *
 * char *restBuf pointer to rest buffer
 * uint16_t restBufLen length of rest buffer
 *
 * char* command find in request string
 *
 * char *restToken split token
 *
 * char **argv array of arguments
 * uint8_t *argc number of arguments
 * uint8_t argcLen maximum number of arguments
 *
 */
int restParser(
	char *request,
	char *restBuf,	uint16_t restBufLen,
	char* command, char *restToken,
	char **argv, uint8_t *argc, uint8_t argcLen
){
	uint16_t reqLen = strlen(request);
	//if(reqLen>restBufLen) try( (0),"Rest Parser error: request size >rest buffer size\n",EXIT_FAILURE);

	try( (request),"Rest Parser error: null pointer request\n",EXIT_FAILURE);
	try( (restBuf),"Rest Parser error: null pointer restBuf\n",EXIT_FAILURE);
	try( (argcLen),"Rest Parser error: null argcLen\n",EXIT_FAILURE);
	char *cmdPos = strstr(request,command);
	if(command){
		try( (cmdPos),"Rest Parser error: command in request not found\n",EXIT_FAILURE);
	}//else if command = null skip checking

//	if(reqLen>restBufLen) printf("Rest Parser warning: request size >rest buffer size\n");
	restBuf = cmdPos;

	char *p = strtok(restBuf, restToken);
	*argc = 0;
	while (p!= NULL){
		argv[*argc]= cmdPos+(p-restBuf);
		*argc += 1;
		if((*argc)>=argcLen){
//			printf("Rest Parser: REST_MAX_PARAMS limit reached\n");
			p = strtok(NULL, "");
			argv[*argc]= cmdPos+(p-restBuf);
			return EXIT_FAILURE;
		}
		p = strtok(NULL, restToken);
	}

	return EXIT_SUCCESS;
}

