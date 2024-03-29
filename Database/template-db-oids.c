/*
 * db-oid.c
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
#include "json.h"
#include "telnet.h"
#include "http.h"
#include "mqtt.h"

#define {{ module.debug.level  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

{% for oid in oids %}
#define {{ oid.name }} {{ oid.value }}
{% endfor %}

char **findTableKeyValueByOid(SnmpOid_st *oid){
	static char *values[32];
	uint16_t iValues = 0;
	if(0){/*dummy if*/}
	{% for oid in oids %}
	else if(!memcmp({{ oid.name }},&oid->name,oid->nameLen)){
		findValues(
			&values[0],32,&iValues,
			getDbInstance(),
			{{ oid.table }}, {{ oid.key }},"*"
		);
		return values;
	}
	{% endfor %}
	return NULL;
}
