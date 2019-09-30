/*
 * database.c
 *
 *  Created on: {{ date }}
 *      Author: AlexL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "database.h"


#ifdef WIN32
#include<winsock2.h>
#endif

#define DB_TABLE_KEYS_MAX	30	//@db.table.keys.max
#define DB_TABLE_KEY_SEPARATOR_START "{"	//@db.table.keys.separator.start
#define DB_TABLE_KEY_SEPARATOR_VAL	":"	//@db.table.keys.separator.val
#define DB_TABLE_KEY_SEPARATOR_NEW	","	//@db.table.keys.separator.new
#define DB_TABLE_KEY_SEPARATOR_END	"}"	//@db.table.keys.separator.end

//#define DB_TRACE
//#undef DB_TRACE

#define {{ module.debug.level  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

static DbTable_st *dbIndexTable;

DbTable_st * getDbInstance(){
	return dbIndexTable;
}
/*
 * parcer configuration file,
 * file structure
 * a{b:c,d:e,...,f:g,}h{..}...{...}'0'
 * primary key -> a
 * secondary key -> not set here
 * key[0] -> b
 * value[0] -> c
 */
DbTable_st dbTable[{{module.tables}}];
LinkedList_st ll[{{module.rows}}];
int reindex(char *address, uint32_t sizeMax){
	if(address==0) return EXIT_FAILURE;
	char *db = address;

//	DbTable_st *table=NULL;
//	table = malloc(sizeof(DbTable_st));
//	memset(table,0,sizeof(DbTable_st));
	uint16_t 	iTable = 0,
				iLlist = 0;
	DbTable_st *table = &dbTable[iTable++];
	{%if module.target=='WIN' %}	try( (table),"Malloc create db indexes table error",EXIT_FAILURE);
	TRACE(printf("DbTable_st size %d\n",sizeof(DbTable_st)););
	{%else%}if(!table) return EXIT_FAILURE;{%endif%}
	dbIndexTable = table;
	while(*db){
		//db=&a
		table->primary_index = db;
		TRACE(printf("pi %s\n",table->primary_index););
		//db=&b
		db = strstr(db,DB_TABLE_KEY_SEPARATOR_START)+1;
		uint8_t i=0;
		//end=&h
		char *end = strstr(db,DB_TABLE_KEY_SEPARATOR_END);
		TRACE(printf("end %s\n",end););
		if(!table->parameters){
//			table->parameters = malloc(sizeof(LinkedList_st));
			table->parameters = &ll[iLlist++];
			{%if module.target=='WIN' %}try( (table->parameters),"malloc LinkedList_st error",EXIT_FAILURE);
			{%else%}if(!table->parameters) return EXIT_FAILURE;{%endif%}
		}
		LinkedList_st *parameters = table->parameters;
		memset(parameters,0,sizeof(LinkedList_st));
		while(i++<DB_TABLE_KEYS_MAX){
			//key[0]=&b
			parameters->key = db;
			//TRACE(printf("key %s\n",parameters->key););
			//value[0]=&c
			parameters->value = strstr(db,DB_TABLE_KEY_SEPARATOR_VAL)+1;
			//TRACE(printf("val %s\n",parameters->value););
			//db=&d
			db = strstr(db,DB_TABLE_KEY_SEPARATOR_NEW)+1;
			TRACE(printf("%s,%p,%p\n",db,db,end););
			if(db>=end)break;
			if(!parameters->next){
//				parameters->next = malloc(sizeof(LinkedList_st));
				parameters->next = &ll[iLlist++];;
				{%if module.target=='WIN' %}try( (parameters->next),"malloc LinkedList_st error",EXIT_FAILURE);
				{%else%}if(!parameters->next) return EXIT_FAILURE;{%endif%}
			}
			parameters = parameters->next;
			memset(parameters,0,sizeof(LinkedList_st));
		}
		//db=&h
		db = end+1;
		if(!(*db)) break;
//		table->next = malloc(sizeof(DbTable_st));
		table->next = &dbTable[iTable++];
		{%if module.target=='WIN' %}try( (table->next ),"Malloc create db indexes teble error",EXIT_FAILURE);
		{%else%}if(!table->next) return EXIT_FAILURE;{%endif%}
		table = table->next;
		memset(table,0,sizeof(DbTable_st));
	}//EOF

	return EXIT_SUCCESS;
}

/*
 * find end of parameter
 * char *param start address for search
 * return length of parameter string in bytes
 * end separators
 *  DB_TABLE_KEY_SEPARATOR_END
 *  DB_TABLE_KEY_SEPARATOR_NEW
 *  DB_TABLE_KEY_SEPARATOR_START
 *  DB_TABLE_KEY_SEPARATOR_VAL
 */
uint8_t strConfigLen(char *param){
	uint8_t i = 0;
	while(
			param[i]
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_END
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_NEW
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_START
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_VAL
	){i++;}
	return i;
}

/*
 * find end of table
 * char *param start address for search
 * return length of parameter string in bytes
 * end separators
 *  DB_TABLE_KEY_SEPARATOR_END
 */
uint8_t strConfigTableLen(char *param){
	uint8_t i = 0;
	while(
			param[i]
			&& param[i] != *DB_TABLE_KEY_SEPARATOR_END
	){i++;}
	return i;
}

/*
 * loader in for a train
 * RailDb_st *rail message protocol
 * Parcel_st *msg message from sender
 */
int setIndex(RailDb_st *rail, Parcel_st *msg){
	TRACE(printf(">>receive parcel from %I64u\n",msg->sender););
	{%if module.target=='WIN' %}try( (rail->params), "the parcel was broken (in rail->params)\n", -1 );
	{%else%}if(!rail->params) return EXIT_FAILURE;{%endif%}
	reindex(rail->params,rail->sizeMax);
	/*
	 * yes, parcel was arrived and database's indexes was updated
	 */
	//sendTrain(msg->from,msg->address,NULL);
	return EXIT_SUCCESS;
}



/*
 * find table in database with name, key and value
 * if parameter equal '*' - find all
 * char **result array of pointers to return results
 * uint32_t resultLenMax size of array result
 * DbTable_st *db database
 * char* name primary key
 * char *key parameter
 * char *value
 */
int findTablesWithKeyValue(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value){
	{%if module.target=='WIN' %}try( (db),"Database is null",EXIT_FAILURE);
	{%else%}if(!db) return EXIT_FAILURE;{%endif%}
	TRACE(printf("Database search tables %s where key %s = %s in %p\n",name,key,value,db););
	uint16_t nameSize = strlen(name);
	uint16_t keySize = strlen(key);
	uint16_t valueSize = strlen(value);
	DbTable_st *table = db;
	while(table){
		if( !equals(name,"*",1) || !equals(table->primary_index, name,nameSize)){
			LinkedList_st *parameter = table->parameters;
			while(parameter){
				if(  !equals(key,"*",1) || !equals(parameter->key,key,keySize)){
					if( !equals(value,"*",1) || !equals(parameter->value,value,valueSize)){
						TRACE(printf("Find \n\t%.1s \t%.1s \t%.1s \n",table->primary_index,parameter->key,parameter->value););
						argv[*argc] = (char*)table;
						*argc += 1;
						break;
					}
				}
				parameter = parameter->next;
			}
		}
		if( (*argc)>resultLenMax ){
			TRACE(printf("findTablesWithKeyValue error: (*argc)>resultLenMax"););
			return EXIT_FAILURE;
		}
		table = table->next;
	}
	return EXIT_SUCCESS;
}

int findValues(char **argv,uint32_t resultLenMax,uint16_t* argc,DbTable_st *db,char* name,char *key, char *value){
	{%if module.target=='WIN' %}try( (db),"Database is null",EXIT_FAILURE);
	{%else%}if(!db) return EXIT_FAILURE;{%endif%}
	TRACE(printf("Database search values %s where key %s = %s in %p\n",name,key,value,db););
	uint16_t nameSize = strlen(name);
	uint16_t keySize = strlen(key);
	uint16_t valueSize = strlen(value);
	DbTable_st *table = db;
	while(table){
		if( !equals(name,"*",1) || !equals(table->primary_index, name,nameSize)){
			LinkedList_st *parameter = table->parameters;
			while(parameter){
				if(  !equals(key,"*",1) || !equals(parameter->key,key,keySize)){
					if( !equals(value,"*",1) || !equals(parameter->value,value,valueSize)){
						TRACE(printf("Find \n\t%.1s \t%.1s \t%.1s \n",table->primary_index,parameter->key,parameter->value););
						argv[*argc] = parameter->value;
						*argc += 1;
						break;
					}
				}
				parameter = parameter->next;
			}
		}
		if( (*argc)>resultLenMax ){
			TRACE(printf("findTablesWithKeyValue error: (*argc)>resultLenMax"););
			return EXIT_FAILURE;
		}
		table = table->next;
	}
	return EXIT_SUCCESS;
}

char dbConfig[] =
{{ module.database }};

ModuleDb_st *getDbConfig(){
	return (ModuleDb_st*)dbConfig;
}

static Parcel_st {{ module.station.parcel }}[{{ module.station.parcels }}];
static Train_st {{ module.station.train }};
static Parcel_st* pBox[{{ module.station.parcels }}];
void {{ module.station.name }}Init(void){
	fillDepot(&{{ module.station.train }});
	{{ module.station.train }}.box = pBox;
	for(uint16_t iParcel=0;iParcel<{{ module.station.parcels }};iParcel++){
		{{ module.station.train }}.box[iParcel] = (Parcel_st*)&{{ module.station.parcel }}[iParcel];
	}
	{{ module.station.train }}.capacity = {{ module.station.parcels }};
	{{ module.station.train }}.route = {{ module.station.route }};
	reindex(dbConfig,sizeof(dbConfig));
}

int {{ module.station.name }}(void *p){
	Parcel_st *msg=meetTrain(&dbTrain);
	while(msg){
		msg=meetTrain(&dbTrain);
	}

	return EXIT_SUCCESS;
}


