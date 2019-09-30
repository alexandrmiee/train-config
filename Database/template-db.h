/*
 * database.h
 *
 *  Created on: {{ date }}
 *      Author: AlexL
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"


typedef struct LinkedList{
	char *key;
	char *value;
	struct LinkedList *next;
}LinkedList_st;

typedef struct Table{
	char *primary_index;
	char *secondary_index;
	LinkedList_st *parameters;
	struct Table *next;
}DbTable_st;


typedef struct RailDb{
	char *params;
	uint32_t sizeMax;
	int(*function)(struct RailDb *params, Parcel_st* box);
}RailDb_st;

{{ module.typedef }}

uint8_t strConfigLen(char *param);
uint8_t strConfigTableLen(char *param);
int reindex(char *address, uint32_t sizeMax);
DbTable_st * getDbInstance(void);
ModuleDb_st *getDbConfig(void);


int findValues(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value);
int findTablesWithKeyValue(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value);
int {{ module.station.name }}(void *p);
void {{ module.station.name }}Init(void);

#endif /* DATABASE_H_ */


