/*
 * ble.c
 * Autogenerated code from TrainFramework
 *
 *  Created on: {{ date }}
 *      Author: ALex
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "database.h"
#include "serial.h"
#include "ow.h"

#define {{ module.debug.level  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

#define ONEWIRE_PRESENCE	0xFF
#define ONEWIRE_SKIP_ROM	0xCC
#define ONEWIRE_CONVERT_T	0x44CC
#define ONEWIRE_READ_SCRATCH	0xBECC

static scrathpad_t scrathpad;
static scrathpad_t command;
static Parcel_st {{ module.station.parcel }}[{{ module.station.parcels }}];
static Train_st {{ module.station.train }};
static Parcel_st *pBox[{{ module.station.parcels }}];

static int stretchByte(uint8_t* buf,uint16_t bufLen, uint16_t byte){
	if(bufLen<16){
		TRACE(printf("1wire stretchByte buffer error. not enough memory"););
		return EXIT_FAILURE;
	}

	uint16_t mask = 1;
	for(uint8_t iBit=0; iBit<(sizeof(byte)*8); iBit++){
		if(byte&mask) buf[iBit] = 0xFF;
		else buf[iBit] = 0x00;
		mask = mask<<1;
	}
	return EXIT_SUCCESS;
}

static float convertValue(uint8_t* buf,uint16_t bufLen){
	if(bufLen<16){
		TRACE(printf("1wire convertValue buffer error. not enough memory"););
		return EXIT_FAILURE;
	}

	float temp = 0.0;
	uint8_t sig = 0xFF;


	float prec = 1.0/16.0;
	for(uint8_t iByte=0; iByte<11; iByte++, prec*=2){
		if(buf[iByte]==0xFF) temp += prec;
	}

	for(uint8_t iByte=11; iByte<16; iByte++) sig&=buf[iByte];

	if(sig==0xFF) temp = -temp;

	return temp;
}


static void rwUart(uint8_t* buf, uint32_t lenOut, uint8_t* mes, uint32_t lenIn,uint32_t baud){
	static RailOw_st ow;
	Parcel_st *box = {{module.station.train }}.box[0];
	ow.command = COMMAND_SERIAL_SEND;
	ow.request = mes;
	ow.reqLen = lenOut;
	ow.response = buf;
	ow.respBufLen = lenIn;
	ow.baud = baud;
	box->parcel = &ow;
	sendTrainsFromDepot({{ module.station.route }},ROUTE_SERIAL,box->parcel);
}

static int owCycle(stateOwreset_t state){
	static uint8_t owServiceReq = 0x0F;
	static uint8_t owServiceResp = 0;
	switch((uint8_t)state){
		case ow_start:
			owServiceReq = 0x0F;
			owServiceResp = 0;
			rwUart(&owServiceReq,1,&owServiceResp,1,ONEWIRE_UART_BAUD_LOW);
		break;
		case ow_measure:
			if(owServiceResp){
				memset(command.commandRead,0xFFFF,sizeof(command));
				stretchByte(command.commandRead,16,ONEWIRE_CONVERT_T);
				rwUart(
					command.commandRead,
					sizeof(command.commandRead),
					(void*)scrathpad.commandRead,
					sizeof(scrathpad.commandRead),
					ONEWIRE_UART_BAUD_HI
				);
			}
			else return EXIT_FAILURE;
		break;
		case ow_convert:
			owServiceReq = ONEWIRE_PRESENCE;
			owServiceResp = 0;
			rwUart(&owServiceReq,1,&owServiceResp,1,ONEWIRE_UART_BAUD_HI);
		break;
		case ow_reset:
			if(owServiceResp){
				owServiceReq = 0x0F;
				owServiceResp = 0;
				rwUart(&owServiceReq,1,&owServiceResp,1,ONEWIRE_UART_BAUD_LOW);
			}
			else return EXIT_FAILURE;
		break;
		case ow_read:
			if(owServiceResp){
				memset(command.commandRead,0xFFFF,sizeof(command));
				stretchByte(command.commandRead,16,ONEWIRE_READ_SCRATCH);
				rwUart(
					command.commandRead,
					sizeof(command),
					(void*)scrathpad.commandRead,
					sizeof(scrathpad),
					ONEWIRE_UART_BAUD_HI
				);
			}
			else return EXIT_FAILURE;
		break;
		case ow_end:
			//float T = convertValue(scrathpad.teperature);
		break;
		default:
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


void {{ module.station.name }}Init(void){
	{{ module.station.train }}.box = pBox;//malloc(sizeof(Parcel_st*)*{{ module.station.parcels }});
	for(uint16_t iParcel=0;iParcel<{{ module.station.parcels }};iParcel++){
		{{ module.station.train }}.box[iParcel] = (Parcel_st*)&{{ module.station.parcel }}[iParcel];
	}
	{{ module.station.train }}.capacity = {{ module.station.parcels }};
	{{ module.station.train }}.route = {{ module.station.route }};
	fillDepot(&{{ module.station.train }});

}

static stateOwreset_t state = (stateOwreset_t)0;

stateOwreset_t getOwState(){
	return state;
}

int {{  module.station.name }}(void *p){
	uint16_t iBox = meetTrainBox(&{{module.station.train }},0);
	Parcel_st *box = {{module.station.train }}.box[iBox];
	while(box){
		if(iBox>{{module.station.train }}.capacity) return EXIT_SUCCESS;

		static void *car;
		car = box->parcel;
		if(car==NULL) return EXIT_FAILURE;
		{% for path in module.pathways %}
		else if( {{ path.from.command }} == (({{ path.from.railType }} *)car)->command ){
			state = (stateOwreset_t)({{ path.state }});
			//get the parcel from the car
			{{ path.from.railType }} *{{ path.from.railName }} = (({{ path.from.railType }} *)car);
			try( ({{ path.from.railName }}), "the rails {{ path.from.railType }} do not go to {{  module.station.name }}\n", EXIT_FAILURE );
			{% if path.from.loader %}
			{{ path.from.loader }}(state);
			{% endif %}
			{% if path.to.loader %}
			{{ path.from.railName }}->car = &{{ module.station.railName }}_{{ loop.index0 }};
			{{ module.station.railName }}_{{ loop.index0 }}.command = {{ path.to.command }};
			{{ path.to.loader }}({{ module.station.route }},{{ path.to.route }},box->parcel);
			{% endif %}
		}
		{% endfor %}
		{% if module.defaultPath.to.loader %}
		else{
			{{ module.defaultPath.to.loader }}({{ module.station.route }},{{ module.station.routeError }},NULL);
		}
		{% endif %}

		iBox = meetTrainBox(&{{module.station.train }},iBox);
		box = {{module.station.train }}.box[iBox];
	}

	return EXIT_SUCCESS;
}


