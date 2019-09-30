#ifndef __MQTT__
#define __MQTT__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "tcp.h"

typedef struct Timer {
	dtime_t cur;
	dtime_t end;
}Timer;


typedef struct Network{
	int (*mqttread) (struct Network*, unsigned char*, int, int);
	int (*mqttwrite) (struct Network*, unsigned char*, int, int);
	void (*disconnect) (struct Network*);
	RailTcp_st *railTcp;
	int socket;
}Network;

typedef struct RailMqtt {
	void *car;
	uint64_t command;
	char *response;
	uint16_t respLen;
	uint16_t respBufLen;
	char **argv;
	uint16_t argc;
	char *topic;
}__attribute__ ((__packed__)) RailMqtt_st;

void mqtt_poll(void);
uint8_t initMqtt(void);
uint8_t connectMqtt(uint8_t id);
void mqttSend(char *topic, char* msg, int msgLen);
void mqttSendError(char *topic, uint8_t* msg, uint32_t msgLen);

void NetworkInit(Network*);

int NetworkConnect(Network*, char*, int);

void TimerInit(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int TimerLeftMS(Timer*);

int {{ module.station.name }}(void *p);
void {{ module.station.name }}Init(void);
#endif
