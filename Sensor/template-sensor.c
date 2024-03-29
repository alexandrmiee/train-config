/*
 * serial.c
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

{%for module in modules.modules%}#include "{{module}}.h"
{%endfor%}

#define {{ module.debug.level  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

{% if module.hal %}#include "halInterface.h"
{% endif %}

static char serialBufTx[{{ module.buffer.tx }}];
static char serialBufRx[{{ module.buffer.rx }}];
static Parcel_st {{ module.station.parcel }}[{{ module.station.parcels }}];
static Train_st {{ module.station.train }};
static Parcel_st *pBox[{{ module.station.parcels }}];


void {{ module.station.name }}Init(void){
	{{ module.station.train }}.box = pBox;
	for(uint16_t iParcel=0;iParcel<{{ module.station.parcels }};iParcel++){
		{{ module.station.train }}.box[iParcel] = (Parcel_st*)&{{ module.station.parcel }}[iParcel];
	}
	{{ module.station.train }}.capacity = {{ module.station.parcels }};
	{{ module.station.train }}.route = {{ module.station.route }};
	fillDepot(&{{ module.station.train }});
}


static void measure(uint8_t type, uint16_t thh, uint16_t thl, uint32_t value, char *hystState, uint64_t routeFrom, uint64_t routeTo){
	uint16_t tmp = 0;
	uint8_t fHist = 1;
	switch(type){
		case MEASURE_TYPE_WINDOW_OUT:
			tmp = thl;
			thl = thh;
			thh = tmp;
		case MEASURE_TYPE_WINDOW_IN:
			if( value>thh || value<thl ){
				sendTrainsFromDepot(routeFrom,routeTo,&value);
			}
		break;

		case MEASURE_TYPE_HYSTERESIS_HI:
			if( value>thh ){
				sendTrainsFromDepot(routeFrom,routeTo,&value);
			}
			else if( value>thl && atoi(hystState) ){
				sendTrainsFromDepot(routeFrom,routeTo,&value);
			}
			else if( value<thl )
				fHist = 0;
		break;

		case MEASURE_TYPE_HYSTERESIS_LOW:
			if( value<thl ){
				sendTrainsFromDepot(routeFrom,routeTo,&value);
			}
			else if( value<thh && atoi(hystState) ){
				sendTrainsFromDepot(routeFrom,routeTo,&value);
			}
			else if( value>thh )
				fHist = 0;
		break;

		case MEASURE_TYPE_KEY_OPEN:
			tmp = 1;
		case MEASURE_TYPE_KEY_CLOSE:
			tmp = 0;
			if( value==tmp ){
				sendTrainsFromDepot(routeFrom,routeTo,&value);
			}
		break;

		default: break;
	}
	*hystState = '0'+fHist;
}

ModuleDb_st *module;
void sensorsPoll(void *p){
	module = getDbConfig();
	{% for sensor in module.sensors %}
	{% if module.hal %}
	static uint64_t lastTime{{sensor.name}} = 0;
	if((_getRtcTime()-lastTime{{sensor.name}})>(atoi((module->{{sensor.name}}.periodVal)))){
		lastTime{{sensor.name}} = _getRtcTime();
		_poll{{sensor.name}}(module);
	{%endif%}
		measure(
			atoi((module->{{sensor.name}}.typeVal)),
			atoi((module->{{sensor.name}}.thhVal)),
			atoi((module->{{sensor.name}}.thlVal)),
			atoi((module->{{sensor.name}}.valueVal)),
			(module->{{sensor.name}}.hystStateVal),
			{{sensor.route}},
			(uint64_t)(module->{{sensor.name}}.trapVal)
		);
	{% if module.hal %}
	}
	{%endif%}
	{%endfor%}
}

int {{  module.station.name }}(void *p){

	sensorsPoll(NULL);

	uint16_t iBox = meetTrainBox(&{{module.station.train }},0);
	Parcel_st *box = {{module.station.train }}.box[iBox];
	while(box){
		if(iBox>{{module.station.train }}.capacity) return EXIT_SUCCESS;

		static void *car;
		car = box->parcel;
		uint8_t i=0;

		while(car){
			{%if module.target=='WIN' %}TRACE(printf("\tSerial car number %d command (Hitch_st) %I64u\n",i,((Hitch_st *)car)->command);); {%endif%}
			if(0==1){}
			{% for path in module.pathways %}
			else if( {{ path.from.command }} == (({{ path.from.railType }} *)car)->command ){

				{# get the parcel from the car #}
				{{ path.from.railType }} *{{ path.from.railName }} = (({{ path.from.railType }} *)car);
				{# create the car for the hitch with the train #}
				static {{ module.station.railType }} {{ module.station.railName }}_{{ loop.index0 }};

				try( ({{ path.from.railName }}), "the rails {{ path.from.railType }} do not go to {{  module.station.name }}\n", EXIT_FAILURE );

				{% if path.from.loader %}
				{{ path.from.loader }}((Hitch_st*){{ path.from.railName }},&{{ module.station.railName }}_{{ loop.index0 }});
				{% endif %}

				{% if path.to.loader %}
				{{ path.from.railName }}->car = &{{ module.station.railName }}_{{ loop.index0 }};
				{{ module.station.railName }}_{{ loop.index0 }}.command = {{ path.to.command }};

				{{ path.to.loader }}((Hitch_st*){{ module.station.route }},{{ path.to.route }},box->parcel);
				{% endif %}
			}

			{% endfor %}
			{% if module.defaultPath.to.loader %}
			else{
				{{ module.defaultPath.to.loader }}({{ module.station.route }},{{ module.station.routeError }},NULL);
			}
			{% endif %}
			car = ((Hitch_st*)car)->car;
			i++;
		}

		iBox = meetTrainBox(&{{module.station.train }},iBox);
		box = {{module.station.train }}.box[iBox];
	}

	return EXIT_SUCCESS;
}


