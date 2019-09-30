/*
 * Telnet.c
 * Autogenerated code from TrainFramework
 *  Created on: {{ date }}
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "json.h"
#include "tcp.h"
#include "RestFullController.h"

#define ERROR_CODE	"{\"error\":\"%s receive %d parameter(s) instead of %d\"}\r\n%s"
#define EQUAL(command,data)	(restParser(data,telnetBufRx,sizeof(telnetBufRx),(command)," ",&argv[0],&argc,32)==EXIT_SUCCESS)
#define ARGS_CHECK(v,c)	((c)!=4)

#define {{ module['debug.level']  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif
char telnetBufRx[128];
char telnetBufTx[128];

#define buffer	telnetBufRx
char telnetCursor[] = "\ruser>";

uint16_t sendArgError(char *buf,uint16_t bufLen, char * command,uint8_t argc, uint8_t iParams){
	TRACE(printf("format error:\n\t%s\n",command););
	return snprintf(
			buf,
			sizeof(bufLen),
			ERROR_CODE,
			command,
			argc,
			iParams,
			telnetCursor
		);
}

static char *argv[32];
static uint8_t argc;


Parcel_st {{ module['station.parcel'] }}[{{ module['station.parcels'] }}];
Train_st {{ module['station.train'] }};
void {{ module['station.name'] }}Init(void){
	fillDepot(&{{ module['station.train'] }});
	{{ module['station.train'] }}.box = malloc(sizeof(Parcel_st*)*{{ module['station.parcels'] }});
	for(uint16_t iParcel=0;iParcel<{{ module['station.parcels'] }};iParcel++){
		{{ module['station.train'] }}.box[iParcel] = (Parcel_st*)&{{ module['station.parcel'] }}[iParcel];
	}
	{{ module['station.train'] }}.capacity = {{ module['station.parcels'] }};
	{{ module['station.train'] }}.route = {{ module['station.route'] }};
}

int {{ module['station.name'] }}(void *p){
	uint16_t iBox = meetTrainBox(&{{module['station.train'] }},0);
	Parcel_st *box = {{module['station.train'] }}.box[iBox];
	while(box){
		{# the train arrives #}
		//box=meetTrain( &{{ module['station.train'] }} );
		//try( (box),"HTTP train is empty",EXIT_SUCCESS);

		if(iBox>{{module['station.train'] }}.capacity) return EXIT_SUCCESS;

		static void *car;
		car = box->parcel;

		if( (box->sender==ROUTE_TCP) & !equals((char*)(((RailTcp_st*)car)->packet->data),"\r\n",2) ){
			((RailTcp_st*)car)->response = (uint8_t*)telnetCursor;
			((RailTcp_st*)car)->respLen = strlen(telnetCursor);
			((RailTcp_st*)car)->command = ROUTE_JSON_ERROR;
			sendTrainsFromDepot(ROUTE_TELNET,ROUTE_TCP,box->parcel);
		}


		{% for path in module['pathways'] %}

		else if(EQUAL({{ path['from.command'] }},(char*)((({{ path['from.railType'] }}*)car)->packet->data))){
			if(ARGS_CHECK(argv,argc))
				TRACE(printf("Telnet ERROR. Arguments error %d",argc););
//				sendTrainsFromDepot({{ module['station.route'] }},ROUTE_TCP,box->parcel);
			else{
				{# get the parcel from the car #}
				{{ path['from.railType'] }} *{{ path['from.railName'] }} = (({{ path['from.railType'] }} *)car);
				{# create the car for the hitch with the train #}
				static {{ module['station.railType'] }} {{ module['station.railName'] }}_{{ loop.index0 }};

				try( ({{ path['from.railName'] }}), "the rails {{ path['from.railType'] }} do not go to {{  module['station.name'] }}\n", EXIT_FAILURE );

				(({{ path['from.railType'] }}*)car)->car = &{{ module['station.railName'] }}_{{ loop.index0 }};
				{{ path['to.loader'] }}({{ module['station.route'] }},{{ path['to.route'] }},box->parcel);

				{{ module['station.railName'] }}_{{ loop.index0 }}.command = {{ path['to.command'] }};
				{{ module['station.railName'] }}_{{ loop.index0 }}.argv = &argv[0];
				{{ module['station.railName'] }}_{{ loop.index0 }}.argc = argc;
				{{ module['station.railName'] }}_{{ loop.index0 }}.response = telnetBufTx;
				{{ module['station.railName'] }}_{{ loop.index0 }}.respBufLen = sizeof(telnetBufTx);
				{{ module['station.railName'] }}_{{ loop.index0 }}.respOk = telnetCursor;
				{{ module['station.railName'] }}_{{ loop.index0 }}.respErr = telnetCursor;
			}
		}

		{% endfor %}



		if( (box->sender==ROUTE_TCP) & (((RailTcp_st*)car)->respLen) )
			sendTrainsFromDepot({{ module['station.route'] }},ROUTE_TCP,box->parcel);

		iBox = meetTrainBox(&{{module['station.train'] }},iBox);
		box = {{module['station.train'] }}.box[iBox];
	}

	return EXIT_SUCCESS;
}
