/*
 * json.c
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

{%for module in modules.modules%}
#include "{{module}}.h"
{%endfor%}


#define {{ module.debug.level  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

{%if module.target=='WIN' %}
#include<windows.h>
{%endif%}

static char jsonBufTx[{{ module.buffer.tx }}];
static char jsonBufRx[{{ module.buffer.rx }}];

/*
 * read database table and write as object to buf
 * char *buf pointer to result buffer
 * uint32_t bufLenMax buf size
 * DbTable_st *table pointer to database table
 */
int extractTableAsJsonToBuf(char *buf,uint32_t bufLenMax,DbTable_st *table){

	char *pBuf=buf;
	int len = bufLenMax;

	pBuf += snprintf(
		pBuf
		,len
		,"\"%.*s\":{"
		,strConfigLen(table->primary_index)
		,table->primary_index
	);
	len = bufLenMax-(uint32_t)(pBuf-buf);
	try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",-1);

	LinkedList_st *parameter = table->parameters;
	while(parameter){
		uint8_t keyLen = strConfigLen(parameter->key);
		uint8_t valLen = strConfigLen(parameter->value);
		try( ((len-(keyLen+valLen+6))>0),"\n!EXIT_NEED_MORE_MEMORY!\n",-1);
		pBuf += snprintf(
			pBuf
			,len
			,"\"%.*s\":\"%.*s\","
			,keyLen
			,parameter->key
			,valLen
			,parameter->value
		);
		len = bufLenMax-(uint32_t)(pBuf-buf);
		try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",-1);

		parameter = parameter->next;
	}
	pBuf--;
	pBuf += snprintf(
		pBuf
		,len
		,"},"
	);
	len = bufLenMax-(uint32_t)(pBuf-buf);
	try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",-1);

	*pBuf = '\0';
	TRACE(printf("table %s\n",buf););
	return (int)(pBuf-buf);
}

/*
 * read database tables and write as object to buf
 * char *buf pointer to result buffer
 * uint32_t bufLenMax buf size
 * char *table array of pointers to database tables
 * uint16_t iTables number of tables
 */
int extractTablesAsJsonToBuf(char *buf,uint32_t bufLenMax,char *tables[], uint16_t iTables){

	char *pBuf=buf;
	int len = bufLenMax;

	pBuf += snprintf(
		pBuf
		,len
		,"["
	);
	len = bufLenMax-(uint32_t)(pBuf-buf);
	try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",0);

	for(uint16_t i=0;i<iTables;i++){
		DbTable_st *table = (DbTable_st *)tables[i];
		pBuf += snprintf(
			pBuf
			,len
			,"{\"%.*s\":{"
			,strConfigLen(table->primary_index)
			,table->primary_index
		);
		len = bufLenMax-(uint32_t)(pBuf-buf);
		try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",0);

		LinkedList_st *parameter = table->parameters;
		while(parameter){
			uint8_t keyLen = strConfigLen(parameter->key);
			uint8_t valLen = strConfigLen(parameter->value);
			try( ((len-(keyLen+valLen+6))>0),"\n!EXIT_NEED_MORE_MEMORY!\n",0);
			pBuf += snprintf(
				pBuf
				,len
				,"\"%.*s\":\"%.*s\","
				,keyLen
				,parameter->key
				,valLen
				,parameter->value
			);
			len = bufLenMax-(uint32_t)(pBuf-buf);
			try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",0);

			parameter = parameter->next;
		}
		pBuf--;
		pBuf += snprintf(
			pBuf
			,len
			,"}},"
		);
		len = bufLenMax-(uint32_t)(pBuf-buf);
		try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",0);

		*pBuf = '\0';
		TRACE(printf("table %s\n",buf););
	}
	snprintf(
		pBuf-1
		,len
		,"]"
	);
	len = bufLenMax-(uint32_t)(pBuf-buf);
	try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",0);
	return (int)(pBuf-buf);
}

/*
 * read database tables and write as object to buf
 * char *buf pointer to result buffer
 * uint32_t bufLenMax buf size
 * char *table array of pointers to database tables
 * uint16_t iTables number of tables
 */
int extractValuesAsJsonToBuf(char *buf,uint32_t bufLenMax,char *values[], uint16_t iValues){

	char *pBuf=buf;
	int len = bufLenMax;

	pBuf += snprintf(
		pBuf
		,len
		,"["
	);
	len = bufLenMax-(uint32_t)(pBuf-buf);
	try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",-1);

	for(uint16_t i=0;i<iValues;i++){
		pBuf += snprintf(
			pBuf
			,len
			,"%.*s,"
			,strConfigLen(values[i])
			,values[i]
		);
		len = bufLenMax-(uint32_t)(pBuf-buf);
		try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",-1);
	}
	pBuf--;
	pBuf += snprintf(
		pBuf
		,len
		,"]"
	);
	len = bufLenMax-(uint32_t)(pBuf-buf);
	try( ((bufLenMax-len)>0),"EXIT_NEED_MORE_MEMORY",-1);

	*pBuf = '\0';
	TRACE(printf("values: %s\n",buf););
	return (int)(pBuf-buf);
}
/*
 * read database tables and write as JSON object to buf
 * char *buf pointer to result buffer
 * uint32_t bufLenMax buf size
 * DbTable_st *table pointer to database
 */
int printDbAsJsonToBuf(char *buf,uint32_t bufLenMax,DbTable_st *db){

	char *pBuf=buf;
	int len = bufLenMax;

	TRACE(printf("JSON start parce database table to buffer %p %d %p\n",buf,bufLenMax,db););

	pBuf += snprintf(
		pBuf
		,len
		,"{"
	);
	len = bufLenMax-(uint32_t)(pBuf-buf);
	try( (bufLenMax-len),"EXIT_NEED_MORE_MEMORY",EXIT_NEED_MORE_MEMORY);

	while(db){
		try( ( (len=(bufLenMax-extractTableAsJsonToBuf(pBuf,len,db)))>0 ) ,"\nDB parcer to JSON error\n",EXIT_NEED_MORE_MEMORY);
		pBuf += (bufLenMax-len);
		db = db->next;
	}
	pBuf--;
	pBuf += snprintf(
		pBuf
		,len
		,"}"
	);
	*pBuf = '\0';
	TRACE(printf("JSON database '%s'\n",buf););
	return EXIT_SUCCESS;
}

/*
 * read database table and write as JSON object to buf
 * char *buf pointer to result buffer
 * uint32_t bufLenMax buf size
 * DbTable_st *table pointer to database table
 */
int printTableAsJsonToBuf(char *buf,uint32_t bufLenMax,DbTable_st *table){

	char *pBuf=buf;
	int len = bufLenMax;

	pBuf += snprintf(
		pBuf
		,len
		,"{"
	);
	len = bufLenMax-(uint32_t)(pBuf-buf);
	try( (bufLenMax-len),"EXIT_NEED_MORE_MEMORY",EXIT_NEED_MORE_MEMORY);

	try( ( (len=(bufLenMax-extractTableAsJsonToBuf(pBuf,len,table)))>0 ) ,"\nDB parcer to JSON error\n",EXIT_NEED_MORE_MEMORY);
	pBuf += (bufLenMax-len-1);

	pBuf += snprintf(
		pBuf
		,len
		,"}"
	);
	*pBuf = '\0';
	TRACE(printf("JSON table '%s'\n",buf););
	return EXIT_SUCCESS;
}



///*
// * find table in database with name, key and value
// * if parameter equal '*' - find all
// * char **result array of pointers to return results
// * uint32_t resultLenMax size of array result
// * DbTable_st *db database
// * char* name primary key
// * char *key parameter
// * char *value
// */
//int findTablesWithKeyValue(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value){
//	try( (db),"Database is null",EXIT_FAILURE);
//	TRACE(printf("JSON search tables %s where key %s = %s in %p\n",name,key,value,db););
//	uint16_t nameSize = strlen(name);
//	uint16_t keySize = strlen(key);
//	uint16_t valueSize = strlen(value);
//	DbTable_st *table = db;
//	while(table){
//		if( !equals(name,"*",1) || !equals(table->primary_index, name,nameSize)){
//			LinkedList_st *parameter = table->parameters;
//			while(parameter){
//				if(  !equals(key,"*",1) || !equals(parameter->key,key,keySize)){
//					if( !equals(value,"*",1) || !equals(parameter->value,value,valueSize)){
//						TRACE(printf("Find \n\t%.1s \t%.1s \t%.1s \n",table->primary_index,parameter->key,parameter->value););
//						argv[*argc] = (char*)table;
//						*argc += 1;
//						break;
//					}
//				}
//				parameter = parameter->next;
//			}
//		}
//		if( (*argc)>resultLenMax ){
//			printf("findTablesWithKeyValue error: (*argc)>resultLenMax");
//			return EXIT_FAILURE;
//		}
//		table = table->next;
//	}
//	return EXIT_SUCCESS;
//}
//
//int findValues(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value){
//	try( (db),"Database is null",EXIT_FAILURE);
//	TRACE(printf("JSON search values %s where key %s = %s in %p\n",name,key,value,db););
//	uint16_t nameSize = strlen(name);
//	uint16_t keySize = strlen(key);
//	uint16_t valueSize = strlen(value);
//	DbTable_st *table = db;
//	while(table){
//		if( !equals(name,"*",1) || !equals(table->primary_index, name,nameSize)){
//			LinkedList_st *parameter = table->parameters;
//			while(parameter){
//				if(  !equals(key,"*",1) || !equals(parameter->key,key,keySize)){
//					if( !equals(value,"*",1) || !equals(parameter->value,value,valueSize)){
//						TRACE(printf("Find \n\t%.1s \t%.1s \t%.1s \n",table->primary_index,parameter->key,parameter->value););
//						argv[*argc] = parameter->value;
//						*argc += 1;
//						break;
//					}
//				}
//				parameter = parameter->next;
//			}
//		}
//		if( (*argc)>resultLenMax ){
//			printf("findTablesWithKeyValue error: (*argc)>resultLenMax");
//			return EXIT_FAILURE;
//		}
//		table = table->next;
//	}
//	return EXIT_SUCCESS;
//}

{%if 'telnet' in modules%}
int jsonGetTelnet(RailTelnet_st *railTelnet,RailJson_st* railJson){
	TRACE(printf("\tget JSON Telnet\n"););
	char *tables[32];
	uint16_t iTables = 0;
	tables[0] = NULL;
	findTablesWithKeyValue(&tables[0],32,&iTables,getDbInstance(),railTelnet->argv[1],railTelnet->argv[2],railTelnet->argv[3]);
	if(tables[0]){
		railTelnet->respLen = extractTablesAsJsonToBuf(railTelnet->response,railTelnet->respBufLen,tables,iTables);
		railTelnet->respLen +=
			snprintf(
				railTelnet->response+railTelnet->respLen,
				railTelnet->respBufLen-railTelnet->respLen,
				"\r\n%s",
				railTelnet->respOk
			);
	}
	else{
		railTelnet->respLen =
			snprintf(
				railTelnet->response,
				railTelnet->respBufLen,
				"\r%s",
				railTelnet->respErr
			);
	}
	return EXIT_SUCCESS;
}
int jsonPut(RailTelnet_st *rail){
	TRACE(printf("\tjsonPut\n"););
	char *values[32];
	uint16_t iValues = 0;
	values[0] = NULL;
	findValues(&values[0],32,&iValues,getDbInstance(),rail->argv[1],rail->argv[2],"*");
	if(values[0]){
		for(int i=0;i<iValues;i++){
			TRACE(printf("put \tlen_n \tlen_old: \n%s\t%d\t%d\n",rail->argv[3],strlen(rail->argv[3]),strConfigLen(values[i])););
			snprintf(values[i],strConfigLen(values[i])+1,"%s",rail->argv[3]);
		}


		rail->respLen = extractValuesAsJsonToBuf(rail->response,rail->respBufLen,values,iValues);
		rail->respLen +=
			snprintf(
				rail->response+rail->respLen,
				rail->respBufLen-rail->respLen,
				"\r\n%s",
				rail->respOk
			);
	}
	else{
		rail->respLen =
			snprintf(
				rail->response,
				rail->respBufLen,
				"\r%s",
				rail->respErr
			);
	}
	return EXIT_SUCCESS;
}
{%endif%}
{%if 'http' in modules%}
#define HEADER_OK_JSON	"HTTP/1.0 200 OK\r\n"\
						"Content-Length: %d\r\n"\
						"Content-Type: application/json; charset=utf-8\r\n\r\n"
#define JSON_ERROR	"{\"Error\":\"table not found\"}"

int jsonGetHttp(RailHttp_st *railHttp,RailJson_st* railJson){
	TRACE(printf("\tget JSON HTTP\n"););
	char *tables[32];
	uint16_t iTables = 0;
	tables[0] = NULL;
	findTablesWithKeyValue(&tables[0],32,&iTables,getDbInstance(),railHttp->argv[1],railHttp->argv[2],railHttp->argv[3]);
	if(tables[0]){
		railJson->response = jsonBufTx;

		railJson->respLen = extractTablesAsJsonToBuf(railJson->response,sizeof(jsonBufTx),tables,iTables);

		railHttp->respLen =
					snprintf(
						railHttp->response,
						sizeof(jsonBufTx),
						HEADER_OK_JSON,
						railJson->respLen
					);
	}
	else{
		railJson->response = jsonBufTx;
		railJson->respLen =
			snprintf(
				railJson->response,
				sizeof(jsonBufTx),
				JSON_ERROR
			);
	}
	return EXIT_SUCCESS;
}
int jsonGetv(RailHttp_st *rail,RailJson_st* railJson){
	TRACE(printf("\tjsonGetv\n"););
	char *tables[32];
	uint16_t iTables = 0;
	tables[0] = NULL;
	findValues(&tables[0],32,&iTables,getDbInstance(),rail->argv[1],rail->argv[2],rail->argv[3]);
	if(tables[0]){
		railJson->response = jsonBufTx;
		railJson->respLen = extractValuesAsJsonToBuf(railJson->response,sizeof(jsonBufTx),tables,iTables);
		rail->respLen =
				snprintf(
					rail->response,
					sizeof(jsonBufTx),
					HEADER_OK_JSON,
					railJson->respLen
				);
	}
	else{
		railJson->response = jsonBufTx;
		railJson->respLen =
			snprintf(
				railJson->response,
				sizeof(jsonBufTx),
				JSON_ERROR
			);
	}
	return EXIT_SUCCESS;
}
{%endif%}
{%if 'mqtt' in modules%}
int jsonGetMqtt(RailMqtt_st *railMqtt,RailJson_st* railJson){
	TRACE(printf("\tget JSON MQTT\n"););
	char *tables[32];
	uint16_t iTables = 0;
	tables[0] = NULL;
	findTablesWithKeyValue(&tables[0],32,&iTables,getDbInstance(),railMqtt->argv[1],railMqtt->argv[2],railMqtt->argv[3]);
	if(tables[0]){
		railJson->response = jsonBufTx;

		railJson->respLen = extractTablesAsJsonToBuf(railJson->response,sizeof(jsonBufTx),tables,iTables);
	}
	else{
		railJson->response = jsonBufTx;
		railJson->respLen =
			snprintf(
				railJson->response,
				sizeof(jsonBufTx),
				JSON_ERROR
			);
	}
	return EXIT_SUCCESS;
}
{%endif%}

{%if ('http' in modules) or ('telnet' in modules) or ('mqtt' in modules) %}
#define REQUEST_TYPE_HTTP	1
#define REQUEST_TYPE_MQTT	2
#define REQUEST_TYPE_TELNET	3

int get(Hitch_st *hitch,RailJson_st* json,uint8_t reqType){
	TRACE(printf("\tjson get\n"););
		char *tables[32];
		uint16_t iTables = 0;
		tables[0] = NULL;
		enum States {get,getv} state=get;
		if(!equals(hitch->argv[0],"getv",strlen("getv"))){
			findValues(&tables[0],32,&iTables,getDbInstance(),hitch->argv[1],hitch->argv[2],hitch->argv[3]);
			state = getv;
		}
		else if(!equals(hitch->argv[0],"get",strlen("get"))){
			findTablesWithKeyValue(&tables[0],32,&iTables,getDbInstance(),hitch->argv[1],hitch->argv[2],hitch->argv[3]);
		}

		if(tables[0]){
			json->response = jsonBufTx;
			if(state==getv)
				json->respLen = extractValuesAsJsonToBuf(json->response,sizeof(jsonBufTx),tables,iTables);
			else
				json->respLen = extractTablesAsJsonToBuf(json->response,sizeof(jsonBufTx),tables,iTables);

			if(reqType==REQUEST_TYPE_HTTP)
				hitch->respLen =
					snprintf(
						hitch->response,
						sizeof(jsonBufTx),
						HEADER_OK_JSON,
						json->respLen
					);
		}
		else{
			json->response = jsonBufTx;
			json->respLen =
				snprintf(
					json->response,
					sizeof(jsonBufTx),
					JSON_ERROR
				);
			if(reqType==REQUEST_TYPE_HTTP)
				hitch->respLen =
					snprintf(
						hitch->response,
						sizeof(jsonBufTx),
						HEADER_OK_JSON,
						json->respLen
					);
		}
	return EXIT_SUCCESS;
}

{%endif%}

static Parcel_st {{ module.station.parcel }}[{{ module.station.parcels }}];
static Train_st {{ module.station.train }};
static Parcel_st *pBox[{{ module.station.parcels }}];
void {{ module.station.name }}Init(void){
	{{ module.station.train }}.box = pBox;//malloc(sizeof(Parcel_st*)*{{ module.station.parcels }});
	for(uint16_t iParcel=0;iParcel<{{ module.station.parcels }};iParcel++){
		{{ module.station.train }}.box[iParcel] = (Parcel_st*)&{{ module.station.parcel }}[iParcel];
	}
	{{ module.station.train }}.capacity = {{ module.station.parcels }};
	{{ module.station.train }}.route = {{ module.station.route }};
	fillDepot(&{{ module.station.train }});
}

/*
 * build a railway to Json
 */
int {{  module.station.name }}(void *p){

	uint16_t iBox = meetTrainBox(&{{module.station.train }},0);
	Parcel_st *box = {{module.station.train }}.box[iBox];
	while(box){
		if(iBox>{{module.station.train }}.capacity) return EXIT_SUCCESS;

		static void *car;
		car = box->parcel;
		//get previous car
		for(uint8_t iHitch=0;iHitch<({{  module.station.carNumber }}-1);iHitch++){
			//get the car from the train
			car = ((Hitch_st*)car)->car;
		}

		switch(((Hitch_st *)car)->command){
			{% for path in module.pathways %}
			case {{ path.from.command }}:
			{
				//get the parcel from the car
				{{ path.from.railType }} *{{ path.from.railName }} = (({{ path.from.railType }} *)car);
				//create the car for the hitch with the train
				static {{ module.station.railType }} {{ module.station.railName }}_{{ loop.index0 }};
				try( ({{ path.from.railName }}), "the rails {{ path.from.railType }} do not go to {{  module.station.name }}\n", EXIT_FAILURE );
				{{ path.from.loader }}({{ path.from.railName }},&{{ module.station.railName }}_{{ loop.index0 }},{{ path.from.reqType }});
				{{ path.from.railName }}->car = &{{ module.station.railName }}_{{ loop.index0 }};
				{{ module.station.railName }}_{{ loop.index0 }}.command = {{ path.to.command }};
				{{ path.to.loader }}({{ module.station.route }},{{ path.to.route }},box->parcel);
			}
			break;
			{% endfor %}

			default:{% if module.defaultPath.to.loader %}
				{{ module.defaultPath.to.loader }}({{ module.station.route }},{{ module.station.routeError }},NULL);
			{% endif %}
		}

		iBox = meetTrainBox(&{{module.station.train }},iBox);
		box = {{module.station.train }}.box[iBox];
	}

	return EXIT_SUCCESS;
}



