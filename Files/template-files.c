/*
 * files.c
 *
 *  Created on: {{ date }}
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

{%for module in modules%}
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

{% if module.fileSystem.fatfs %}
static FATFS ff;
static FIL fil;
static DIRS dir;
static FILINFO fno;
{% endif %}


{% for file in module.files %}
extern FileDesc_st {{ file.arrayName }}Descriptor;
{% endfor %}

FileDesc_st *files[] = {
	{% for file in module.files %}
	&{{ file.arrayName }}Descriptor,
	{% endfor %}
	NULL
};

char filesBufRx[{{ module.buffer.rx }}];
char filesBufTx[{{ module.buffer.tx }}];

FileDesc_st *findFile(char *fname){
	uint16_t len = strlen(fname);
	for(int i=0;i<(sizeof(files)/sizeof(files[0])-1);i++){
		if(equals(files[i]->fname,fname,len)==0){
			return files[i];
		}
	}
	return NULL;
}

{%if 'http' in modules%}
#define HEADER_OK_JSON	"HTTP/1.0 200 OK\r\n"\
						"Content-Length: %d\r\n"\
						"Content-Type: application/json; charset=utf-8\r\n\r\n"

#define HEADER_OK_TEXT	"HTTP/1.0 200 OK\r\n"\
						"Content-Type: %s; charset=utf-8\r\n"\
						"Content-Length: %d\r\n"\
						"Content-Encoding: gzip\r\n\r\n"

int cd(RailHttp_st *railHitch){
	switch(atoi(railHitch->argv[ARG_DISK_DRIVE])){
		case DISK_SD_SPI:
		case DISK_FLASH_SPI:
		case DISK_FLASH_MC:
			{% if module.fileSystem.fatfs %}
			if(f_chdir(railHitch->argv[ARG_DIR])){
				TRACE(printf("\tFiles ERROR. Cannot change FATFS directory\n"););
				return EXIT_FAILURE;
			}
			else{
				return EXIT_SUCCESS;
			}
			{% endif %}
			return EXIT_SUCCESS;
		case DISK_FLASH_DB:
		case DISK_FLASH_BOOT:
		case DISK_BOOTLOADER:
		case DISK_FLASH_CAN:
		default:
			TRACE(printf("\tFiles ERROR. Cannot change SFS directory\n"););
	}
	return EXIT_FAILURE;
}

int saveFile(RailHttp_st *http,RailFiles_st *railFile){
	TRACE(printf("Save new file\n"););
	TRACE(printf("\tdrive %s\n",http->argv[ARG_DISK_DRIVE]););
	TRACE(printf("\tdirectory %s\n",http->argv[ARG_DIR]););
	TRACE(printf("\tfile name %s\n",http->argv[ARG_FILE]););
	railFile->response = (uint8_t*)filesBufTx;
	railFile->respLen = 0;
	railFile->respBufLen = sizeof(filesBufTx);
	switch(atoi(http->argv[ARG_DISK_DRIVE])){
		case DISK_FLASH_BOOT:
		case DISK_BOOTLOADER:
		case DISK_FLASH_CAN:
		break;
		case DISK_FLASH_DB:
		{
			static void* db = 0;

			/*
			ModuleDb_st *module = getDbConfig();
			if(
				!equals(
					http->argv[ARG_DIR],
					&(module->HTTP.object),
					strConfigLen(&(module->HTTP.object))
				)
			){
				printf("\t%s\n",&(module->HTTP.object));
			}
			else
				printf("\terror object not found %.*s\n",strConfigLen(&(module->HTTP.object)),&(module->HTTP.object));
			*/
			char *tables[1];
			uint16_t iTables = 0;
			tables[0] = NULL;
			findTablesWithKeyValue(
					&tables[0],
					1,
					&iTables,
					getDbInstance(),
					http->argv[ARG_TABLE],
					http->argv[ARG_KEY],
					"*"
			);

			DbTable_st *table = (DbTable_st *)tables[0];
			TRACE(printf("\t%.10s\n",table->primary_index););

			uint32_t len = strConfigTableLen(table->primary_index);
			if(http->attachLen==len)
				memcpy(table->primary_index,http->attach,http->attachLen);
			else{
				TRACE(printf("\t object len error: table len %d, object len: %d",len,http->attachLen););
			}
			break;
		}
		case DISK_FLASH_MC:
		case DISK_SD_SPI:
		case DISK_FLASH_SPI:
		{	{% if module.fileSystem.sfs %}
			//FIXME: multy thread errors
			static uint8_t isNewFile = 1;
			if(isNewFile){
				//find file
				//delete, cause fragmentation
				//create new file
				FileDesc_st *file = malloc(sizeof(file));
				if(!file){
					TRACE(printf("\tFiles ERROR. Cannot malloc memory for SFS file\n"););
					return EXIT_FAILURE;
				}
				uint32_t pFile = getAddress();
				file->fsize = http->attachLen;
				file->fname = (char*)(pFile+sizeof(file));
				file->content = file->fname+strlen(http->attachFileName)+1;
				file->file = (uint8_t*)(file->content+strlen(http->contentType)+1);

				while(writeChunk(&file,sizeof(file),0)){}
				while(writeChunk(http->attachFileName,strlen(http->attachFileName)+1,0)){}
				while(writeChunk(http->contentType,strlen(http->contentType)+1,0)){}

				railFile->respLen += snprintf(
					railFile->response,
					railFile->respBufLen,
					"{\"file\":\"saved\"}"
				);
			}
			isNewFile = http->isStreamEnd;
			while(writeChunk(http->attach,http->attachLen,http->isStreamEnd)){}
			{% endif %}
			{% if module.fileSystem.fatfs %}
			static uint32_t len = 0;
			if(f_write(&fil,http->attach,http->attachLen,&len)){
				TRACE(printf("\tFiles ERROR. Cannot save FATFS file\n"););
				return EXIT_FAILURE;
			}
			{% endif %}
			break;
		}
		default:
			TRACE(printf("\tFiles ERROR. Save file, disk drive not found\n"););
			return EXIT_FAILURE;
	}
	http->respLen =
		snprintf(
			http->response,
			http->respBufLen,
			HEADER_OK_JSON,
			railFile->respLen
		);
	return EXIT_SUCCESS;
}

int makeDir(Hitch_st *railHttp,RailFiles_st *railFile){
	railFile->response = filesBufTx;
	railFile->respLen = 0;
	railFile->respBufLen = sizeof(filesBufTx);
	switch(atoi(railHttp->argv[ARG_DISK_DRIVE])){
		case DISK_SD_SPI:
		case DISK_FLASH_SPI:
			{% if module.fileSystem.fatfs %}
			f_mkdir(argv[ARG_DIR]);
			railFile->respLen += snprintf(
					railFile->response,
					railFile->respBufLen,
					HTTP_DIR,
					argv[ARG_DIR]
				);
		break;
			{% endif %}
		case DISK_FLASH_DB:
		case DISK_FLASH_BOOT:
		case DISK_BOOTLOADER:
		case DISK_FLASH_CAN:
		case DISK_FLASH_MC:
		default:
			TRACE(printf("\tFiles ERROR. Cannot create new directory\n"););
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


int getListDir(RailHttp_st *railHitch,RailFiles_st *railFile){
	railFile->response = filesBufTx;
	railFile->respLen = 0;
	railFile->respBufLen = sizeof(filesBufTx);
	if(cd(railHitch)==EXIT_FAILURE){
		TRACE(printf("\tFiles ERROR. Change drive\n"););
		return EXIT_FAILURE;
	}

	switch(atoi(railHitch->argv[ARG_DISK_DRIVE])){
		case DISK_FLASH_DB:
			railFile->respLen += snprintf(
				railFile->response,
				railFile->respBufLen,
				HTTP_FILE,
				0,
				"database"
			);
		break;
		case DISK_FLASH_BOOT:
		railFile->respLen += snprintf(
				railFile->response,
				railFile->respBufLen,
				HTTP_FILE,
				0,
				"firmware"
			);
		break;
		case DISK_BOOTLOADER:
		railFile->respLen += snprintf(
				railFile->response,
				railFile->respBufLen,
				HTTP_FILE,
				0,
				"bootloader"
			);
		break;
		case DISK_FLASH_MC:
		{ {% if module.fileSystem.sfs %}
			if(!railFile->respBufLen){
				TRACE(printf("\tFiles ERROR. Not enough memory to read directory info\n\t%s\n",railFile->response););
				break;
			}
			railFile->respLen += snprintf(
					railFile->response,
					railFile->respBufLen,
					"["
				);
			railFile->respBufLen--;
			for(int i=0;i<(sizeof(files)/sizeof(files[0])-1);i++){
				uint16_t len = snprintf(
					railFile->response+railFile->respLen,
					railFile->respBufLen,
					HTTP_FILE,
					files[i]->fsize,
					files[i]->fname
				);
				railFile->respLen += len;
				railFile->respBufLen -= len;
				if(railFile->respLen >= railFile->respBufLen){
					TRACE(printf("\tFiles ERROR. Not enough memory to read directory info\n\t%s\n",railFile->response););
					break;
				}
			}

			FileDesc_st file;
			uint32_t addr = 0;
			while(readData(&file,&addr,sizeof(file)));
			while(file.fsize!=0xFFFF){
				if(file.fsize){
					uint16_t len = snprintf(
						railFile->response+railFile->respLen,
						railFile->respBufLen,
						HTTP_FILE,
						file.fsize,
						(char*)*file.fname
					);
					railFile->respLen += len;
					railFile->respBufLen -= len;
					if(railFile->respLen >= railFile->respBufLen){
						TRACE(printf("\tFiles ERROR. Not enough memory to read directory info\n\t%s\n",railFile->response););
						break;
					}
				}
				uint8_t st;
				do{
					st = readData(&file,&addr,sizeof(file));
					addr += sizeof(file)+file.fsize;
					if(st==0xff){
						file.fsize = 0xffff;
						TRACE(printf("\tRead Flash data error. MEMORY_OVERFLOW\n"););
						break;
					}
				}while(st);
			}
			snprintf(
				railFile->response+railFile->respLen-1,
				2,
				"]"
			);
			break;{% endif %}
		}
		case DISK_SD_SPI:
		case DISK_FLASH_SPI:
			{% if module.fileSystem.fatfs %}
			if(f_chdrive(atoi(railHitch->argv[ARG_DISK_DRIVE])){
				TRACE(printf("\tFiles ERROR. Change FATFS drive\n"););
				return EXIT_FAILURE;
			}
			if(f_opendir(&dir, railHitch->argv[ARG_DIR])==FR_OK && f_readdir(&dir, &fno)==FR_OK){
				while(fno.fname[0]){
					if(fno.fattrib & AM_DIR){
						railFile->respLen += snprintf(
								railFile->response,
								railFile->respBufLen,
								HTTP_DIR,
								fno.fname
							);
					}
					else{
						railFile->respLen += snprintf(
								railFile->response,
								railFile->respBufLen,
								HTTP_FILE,
								fno.fsize,
								fno.fname
							);
					}
					if(railFile->respLen == railFile->respBufLen){
						TRACE(printf("\tFiles ERROR. Not enough memory to read directory info\n"););
						return EXIT_FAILURE;
					}
					if(f_readdir(&dir, &fno)){
						TRACE(printf("\tFiles ERROR. Read directory info\n"););
						return EXIT_FAILURE;
					}
				}
			}
			else{
				TRACE(printf("\tFiles ERROR. Open FATFS directory\n"););
				TRACE(printf("\tFiles ERROR. Read FATFS directory info\n"););
			}
			break;
			{% endif %}
		case DISK_FLASH_CAN:
			{% if module.fileSystem.sfs %}
			break;
			{% endif %}
		default:
			TRACE(printf("\tFiles ERROR. Drive not found\n"););
			return EXIT_FAILURE;
	}
	railHitch->respLen =
			snprintf(
				railHitch->response,
				railHitch->respBufLen,
				HEADER_OK_JSON,
				railFile->respLen
			);
	return EXIT_SUCCESS;
}

int getFileHttp(RailHttp_st *railHttp,RailFiles_st *railFile){
	TRACE(printf("Get File\n"););

	FileDesc_st *fileD = findFile(railHttp->argv[0]);
	if(fileD){
		railFile->response = (uint8_t*)fileD->file;
		railFile->respLen = fileD->fsize;

		railHttp->respLen =
			snprintf(
				railHttp->response,
				railHttp->respBufLen,
				HEADER_OK_TEXT,
				fileD->content,
				railFile->respLen
			);
	}
	else{
	}
	return EXIT_SUCCESS;
}

{%endif%}

static Parcel_st {{ module.station.parcel }}[{{ module.station.parcels }}];
static Train_st {{ module.station.train }};
static Parcel_st *pBox[{{ module.station.parcels }}];
void {{ module.station.name }}Init(void){
	fillDepot(&{{ module.station.train }});
	{{ module.station.train }}.box = pBox;//malloc(sizeof(Parcel_st*)*{{ module.station.parcels }});
	for(uint16_t iParcel=0;iParcel<{{ module.station.parcels }};iParcel++){
		{{ module.station.train }}.box[iParcel] = (Parcel_st*)&{{ module.station.parcel }}[iParcel];
	}
	{{ module.station.train }}.capacity = {{ module.station.parcels }};
	{{ module.station.train }}.route = {{ module.station.route }};
}

/*
 * build a railway to Json
 */
int {{  module.station.name }}(void *p){

	uint16_t iBox = meetTrainBox(&{{module.station.train }},0);
	Parcel_st *box = {{module.station.train }}.box[iBox];
	while(box){
		if(iBox>{{module.station.train }}.capacity) return EXIT_SUCCESS;
		{# get the car from the train #}
		static void *car;
		car = box->parcel;
		{# find the car in the train that arrived #}
		for(uint8_t iHitch=0;iHitch<({{  module.station.carNumber }}-1);iHitch++){
			car = ((Hitch_st*)car)->car;
		}

		if(car==NULL) return EXIT_FAILURE;
		{% for path in module.pathways %}
		else if( {{ path.from.command }} == (({{ path.from.railType }} *)car)->command ){

			{# convert from  Hitch_st type to railType #}
			{{ path.from.railType }} *{{ path.from.railName }} = (({{ path.from.railType }} *)car);

			{# create parcel - variable to save result #}
			static {{ module.station.railType }} {{ module.station.railName }}_{{ loop.index0 }};

			TRACE(printf("{{ path.from.command }}\n\t%s\n",{{ path.from.railName }}->argv[0]););
			{# execute command function #}
			if({{ path.from.loader }}({{ path.from.railName }},&{{ module.station.railName }}_{{ loop.index0 }}) ==EXIT_SUCCESS){

			}
			else{
				TRACE(printf("{{ path.from.command }} error\n"););
			}

			{# pack parcel #}
			{{ module.station.railName }}_{{ loop.index0 }}.command = {{ path.to.command }};
			{{ path.from.railName }}->car = &{{ module.station.railName }}_{{ loop.index0 }};
			{# send train with parcel #}
			{{ path.to.loader }}({{ module.station.route }},{{ path.to.route }},box->parcel);
		}
		{% endfor %}
		else{
			{%if module.target=='WIN' %}TRACE(printf("File command %I64u not found\n",((Hitch_st*)car)->command);); {%endif%}
		}
		{# the train arrives #}
		iBox = meetTrainBox(&{{module.station.train }},iBox);
		box = {{module.station.train }}.box[iBox];
	}

	return EXIT_SUCCESS;

}


