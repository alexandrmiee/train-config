/*
 * ble.h
 * Autogenerated code from TrainFramework
 *
 *  Created on: {{ date }}
 *      Author: ALex
 */

#ifndef _SENSOR_H_
#define _SENSOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

{%for module in modules.modules%}
#include "{{module}}.h"
{%endfor%}

typedef struct RailSensor{
	void *car;
	uint64_t command;
	uint8_t *response;
	uint16_t respLen;
	uint16_t respBufLen;
	uint8_t *request;
	uint16_t reqLen;
}__attribute__ ((__packed__)) RailSensor_st;

{% for define in module.defines %}
#define {{ define.name }}	{{ define.value }}
{% endfor %}


int {{ module.station.name }}(void *p);
void {{ module.station.name }}Init(void);

#endif /* _SENSOR_H_ */

