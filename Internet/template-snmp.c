/*
 * snmp.c
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
#include "snmp.h"
#include "udp.h"
#include "telnet.h"
#include "http.h"
#include "mqtt.h"

#define {{ module['debug.level']  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif


{% for community in module['communities'] %}
#define {{ community['name'] }} {{ community['value'] }}
{% endfor %}



static char snmpBufTx[{{ module['buffer.tx'] }}];
static char snmpBufRx[{{ module['buffer.rx'] }}];

int snmpParser(uint8_t *request, uint16_t reqLen,uint8_t *response,uint16_t respMaxLen){
	uint8_t res = SNMP_ERR_NO;
	static uint8_t *varBindLen, *pduType, *pduLen, *respLen, *pp;

	//pointer to request
	SnmpHeader_st* requestHeader = (SnmpHeader_st*)request;
	respLen = response+1;

	//check community
	//if(requestHeader->communityLen>16) res = SNMP_ERR_FORMAT;
	//if(memcmp(community,&requestHeader->community,requestHeader->communityLen)) res = SNMP_ERR_ACCES;
	TRACE(printf("\tcommunity: %.*s\n",requestHeader->communityLen,&requestHeader->community););

	//get PDU REQUEST TYPE
	//pdu request after community
	pp = (void*)(&requestHeader->communityLen+requestHeader->communityLen+1);
	pp++;
	pduType = response+(pp-request)-1;

	pp++;
	pduLen = response+(pp-request)-1;

	//scip all errors
	while(*pp!=SNMP_TYPE_SEQUENCE){
		//tp---tp--alen--len-----next tp
		pp = pp + 1 + *(pp+1) +1;
//		if( (pp-request>=len ) return 0;
	}

	//get request oids
	varBindLen = pp + 1;
	//memcpy(cmdBuf,(pp+2),reqLen-(pp+2-request));
	SnmpOid_st *oids = (SnmpOid_st*)(pp+2);//(void*)cmdBuf;

	uint8_t *oidSequence;
	if(SNMP_ERR_NO==res){
		//faster way to create response header
		if(respMaxLen>(pp-request)){
			memcpy(response,request,pp-request);
			pp = response + (pp-request);
			TRACE(printf("\tCreate header\n"););
		}
		else{
			TRACE(printf("SNMP ERROR. Not enough memory to create response header\n"););
			return EXIT_FAILURE;
		}
		uint8_t errs;
		//read all OIDs and create response
		while(oids->sequenceType==SNMP_TYPE_SEQUENCE){

			char *args[1];
			//char *value;
			char *value = "123,";
			//if read next OID -> read next table#row from database
			if(*pduType==SNMP_TYPE_PDU_GETNEXT){
			}
			//if read ID -> read table#row from database
			else{
//				args = findTableKeyValueByOid(oids);
			}
//			value = args[0];

			//start response
			//set PDU type
			*pp++ = SNMP_TYPE_SEQUENCE;
			//store PDU length position
			uint8_t* bigSeqLen = pp++;
			*bigSeqLen = 0;
			TRACE(printf("\tfind OID %d length %d\n",oids->name,oids->nameLen););
			if(value){
				//case command
				if(*pduType==SNMP_TYPE_PDU_SETREQUEST){
					//put new value
					char *pVal = (void*)(&oids->name+oids->nameLen+2);
//					setJsonKey(NULL,name,key,pVal);
				}
				else{
					uint16_t valLen = strConfigLen(value);
					TRACE(printf("\tfind value %.*s\n",valLen,value););
					if(respMaxLen<(pp-response)+oids->nameLen+2+valLen+2+2){
						TRACE(printf("SNMP ERROR. Not enough memory to store OID and it value\n"););
						return EXIT_FAILURE;
					}
					//get value response
					//set PDU type
					*pp++ = SNMP_TYPE_SEQUENCE;
					//store PDU length position
					uint8_t* oidSeqLen = pp++;
					*oidSeqLen = 0;

					//write OID
					SnmpData_st *pvar = (void*)pp;
					oidSequence = (uint8_t*)pp;
					pvar->mesType = SNMP_TYPE_OID;
					memcpy(pvar->mes,&oids->name,oids->nameLen);
					pvar->mesLen = oids->nameLen;
					*oidSeqLen += pvar->mesLen+2;

					//write table#row#key#value as string
					pp += pvar->mesLen+2;
					pvar = (void*)pp;
					pvar->mesType = SNMP_TYPE_STRING;
					memcpy(pvar->mes,&oids->name,valLen);
					pvar->mesLen = valLen;
					pp += pvar->mesLen+2;
					*oidSeqLen += pvar->mesLen+2;
					*bigSeqLen += *oidSeqLen+2;
				}
			}
			else {
				errs = SNMP_ERR_NONAME;
				*varBindLen = 0;
			}

			if(errs){
				TRACE(printf("\terror %d\n",errs););
				uint8_t *pd = pduType+3; //id_l
				pd += *pd+3; //error
				*pd = errs;
				pd += *(pd+2)+2;
				*pd = 1;//error_id
				break;
			}
			oids = (void*)(oids+oids->sequenceLen+2);
		}
		TRACE(printf("\tOIDs ends\n"););
	}
	else{
		*varBindLen = 0;
		uint8_t *pd = pduType+3; //id_l
		pd += *pd+3; //error
		*pd = res;
		pd += *(pd+2)+2;
		*pd = 1;//error_id
	}
	if(*pduType!=SNMP_TYPE_PDU_SETREQUEST){
		*pduType = SNMP_TYPE_PDU_GETRESPONSE;
		*pduLen = (pp-pduLen-1);
		*respLen = (pp-request)-2;
	}

	return *respLen+2;
}


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

/*
 * build a railway to SNMP
 */
int {{ module['station.name'] }}(void *p){
	uint16_t iBox = meetTrainBox(&{{module['station.train'] }},0);
	Parcel_st *box = {{module['station.train'] }}.box[iBox];
	while(box){
		{# the train arrives #}
		if(iBox>{{module['station.train'] }}.capacity) return EXIT_SUCCESS;
		printf("\nSNMP Station\n");
		{# get the car from the train #}
		static void *car;
		car = box->parcel;
		try((car),"\tempty car on SNMP train",EXIT_FAILURE);
		uint8_t i=0;
		while(car){
			/*
			 * Read and parse new UDP packet
			 */
			if( (0 == i) && (0 == (((RailUdp_st*)car)->command)) ){
				TRACE(printf("\tSNMP UDP read\n"););
				RailUdp_st *railUdp =  ((RailUdp_st*)car);
				static RailSnmp_st railSnmp;

				railSnmp.respLen = snmpParser(
						((RailUdp_st*)car)->packet->data,
						((RailUdp_st*)car)->packet->len,
						(uint8_t*)snmpBufTx,
						sizeof(snmpBufTx)
				);
				railSnmp.response = snmpBufTx;

				try( (railUdp), "the rails railUdp do not go to snmpStation\n", EXIT_FAILURE );

				railUdp->car = &railSnmp;
				railSnmp.command = COMMAND_SNMP_RESPONSE;


				sendTrainsFromDepot(ROUTE_SNMP,ROUTE_UDP,box->parcel);
			}
			else{
				TRACE(printf("SNMP command (Hitch_st) %I64u not found\n",(((Hitch_st*)car)->command)););
			}
			car = ((Hitch_st*)car)->car;
			i++;
		}
		iBox = meetTrainBox(&{{module['station.train'] }},iBox);
		box = {{module['station.train'] }}.box[iBox];
	}

	return EXIT_SUCCESS;
}





//
////void snmpTrap(eth_frame_t *frame){
//void snmpTrap(uint8_t *response,uint16_t respMaxLen){
//	static dtime_t lastTime = 0;
//	//if(flags&isEnable_trap){
//		if((getRtcTime()-lastTime)>(snmp_timeout)){
//		lastTime = getRtcTime();
//		snmp_timeout = 10000;
//
//		ip_packet_t *ip = (void*)(frame->data);
//		udp_packet_t *udpPack = (void*)(ip->data);
//
//		ip->to_addr = trap_ip_s;
//		udpPack->to_port = trap_port_s;
//		udpPack->from_port = snmp_port;
//
//		uint8_t *udp = udpPack->data;
//		//uint8_t* start = udpPack->data;
//
//		SnmpHeader_st* hdr = (void*)udp;
//
//		hdr->type = SNMP_TYPE_SEQUENCE;
//		hdr->len = 5;
//		hdr->versionType = SNMP_TYPE_INTEGER;
//		hdr->versionLen = 1;
//		hdr->version = 0;
//		hdr->communityType = SNMP_TYPE_STRING;
//		hdr->communityLen = strlen("public");
//		sprintf((char*)&hdr->community,"public");
//		hdr->len += hdr->communityLen;
//
//		uint8_t *pp = udp + hdr->len+2;
//		snmp_trap_st *ptr = (void*)pp;
//		ptr-> pduType = SNMP_TYPE_PDU_TRAP;
//		ptr-> pduLen = sizeof(snmp_trap_st)-2;
//		ptr-> ent_t = SNMP_TYPE_OID;
//		char sysId[] = "1.3.6.1.2.1.1.2";
//		ptr-> ent_l = oidToHex(sysId,ptr-> ent,7);
//		ptr-> ip_t = SNMP_TYPE_IP;
//		ptr-> ip_l = 4;
//		ptr-> ip = ip_adr;
//		ptr-> trap_t = SNMP_TYPE_INTEGER;
//		ptr-> trap_l = 1;
//		ptr-> trap = 6;
//		ptr-> tcode_t = SNMP_TYPE_INTEGER;
//		ptr-> tcode_l = 1;
//		ptr-> tcode = 0;
//
//		ptr-> time_t = SNMP_TYPE_TIMETICKS;
//		ptr-> time_l = 5;
//		dtime_t timc = getRtcTime()/10;
//		memcpy(ptr->time,&timc,5);
//
//		ptr-> varlst_t = SNMP_TYPE_SEQUENCE;
//		ptr-> varlst_l = 0;
//
//		pp = &ptr-> varlst_l + 1;
//
//		inet_conf_st *conf = findType("sens",pEndConf);
//		while(conf){
//			if(getUintParam(conf->name,"alarm")){
//				*pp++ = SNMP_TYPE_SEQUENCE;
//				uint8_t* seq_len = pp++;
//				ptr->varlst_l += 2;
//
//				SnmpData_st *pvar = (void*)pp;
//				pvar->mesType = SNMP_TYPE_OID;
//				pvar->mes = (void*)&pvar->mes;
//				pvar->mesLen = nameToOid(conf->name,"state",pvar->mes);
//				ptr-> varlst_l += pvar->mesLen+2;
//				*seq_len += pvar->mesLen+2;
//
//				pp += pvar->mesLen+2;
//				pvar = (void*)pp;
//				pvar->mesType = SNMP_TYPE_STRING;
//				uint16_t len = 128;
//				pvar->mes = (void*)&pvar->mes;
//				getJsonValue(conf->name,"state",(char*)pvar->mes,&len);
//				pvar->mesLen = len;
//				pp += len+2;
//				ptr->varlst_l += pvar->mesLen+2;
//				*seq_len += pvar->mesLen+2;
//
//			}
//			conf = findType("sens",getNextConfStr(conf));
//		}
//		#warning error if hdr->len>256, send in separated trap?
//		ptr->pduLen += ptr->varlst_l;
//		hdr->len += ptr->pduLen+2;
//		udp_send(frame, hdr->len+2);
//	}
// //}
//}
