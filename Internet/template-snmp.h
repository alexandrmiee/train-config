#ifndef _SNMP_H_
#define _SNMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#define SNMP_TYPE_INTEGER		0x02
#define SNMP_TYPE_STRING		0x04
#define SNMP_TYPE_NULL			0x05
#define SNMP_TYPE_OID			0x06
#define SNMP_TYPE_SEQUENCE		0x30
#define SNMP_TYPE_IP			0x40
#define SNMP_TYPE_TIMETICKS		0x43

#define SNMP_TYPE_PDU_GETREQUEST	0xA0
#define SNMP_TYPE_PDU_GETNEXT		0xA1
#define SNMP_TYPE_PDU_GETRESPONSE	0xA2
#define SNMP_TYPE_PDU_SETREQUEST	0xA3
#define SNMP_TYPE_PDU_TRAP			0xA4
#define SNMP_TYPE_PDU_GETBULK		0xA5

#define SNMP_ERR_NO		0
#define SNMP_ERR_TBIG	1
#define SNMP_ERR_NONAME	2
#define SNMP_ERR_BADVAL	3
#define SNMP_ERR_RONLY	4
#define SNMP_ERR_GENERR	5
#define SNMP_ERR_FORMAT 6
#define SNMP_ERR_ACCES	7

#define NOF_OID_DEV	10

typedef struct{
	uint8_t mesType;
	uint8_t mesLen;
	uint8_t* mes;
}__attribute__ ((__packed__)) SnmpData_st;


typedef struct{
	uint8_t type;
	uint8_t len;
	uint8_t versionType;
	uint8_t versionLen;
	uint8_t version;
	uint8_t communityType;
	uint8_t communityLen;
	char community;
}__attribute__ ((__packed__)) SnmpHeader_st;

typedef struct{
	uint8_t sequenceType;
	uint8_t sequenceLen;
	uint8_t nameType;
	uint8_t nameLen;
	uint8_t name;
}__attribute__ ((__packed__)) SnmpOid_st;


//
//typedef struct{
//	uint8_t pdu_t;
//	uint8_t pdu_l;
//	uint8_t ent_t;
//	uint8_t ent_l;
//	uint8_t ent[7];
//	uint8_t ip_t;
//	uint8_t ip_l;
//	uint32_t ip;
//	uint8_t trap_t;
//	uint8_t trap_l;
//	uint8_t trap;
//	uint8_t tcode_t;
//	uint8_t tcode_l;
//	uint8_t tcode;
//	uint8_t time_t;
//	uint8_t time_l;
//	uint8_t time[5];
//	uint8_t varlst_t;
//	uint8_t varlst_l;
//}__attribute__ ((__packed__)) snmp_trap_st;
//
//typedef struct{
//	uint8_t val_t;
//	uint8_t val_l;
//	uint8_t val;
//}__attribute__ ((__packed__)) snmp_varb_st;
//
//
//typedef struct{
//	uint8_t* pdu;
//	uint8_t* requestId;
//	uint8_t* oids[NOF_OID_DEV];
//}__attribute__ ((__packed__)) snmp_get_st;
//
//typedef struct{
//	snmp_var_st* data;
//	void *next;
//}__attribute__ ((__packed__)) snmp_list;

typedef struct RailSnmp{
	void *car;
	uint64_t command;
	char *response;
	uint16_t respLen;
	uint16_t respBufLen;
	char **argv;
	uint16_t argc;
}__attribute__ ((__packed__)) RailSnmp_st;

int {{ module['station.name'] }}(void *p);
void {{ module['station.name'] }}Init(void);
#endif /*_SNMP_H_*/
