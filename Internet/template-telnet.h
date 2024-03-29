/*
 *	Autogenerated code from TrainFramework
 *  Created on: {{ date }}
 *      Author: AL
 */

#ifndef TELNET_H_
#define TELNET_H_
typedef struct RailTelnet {
	void *car;
	uint64_t command;
	char *response;
	uint16_t respLen;
	uint16_t respBufLen;
	char **argv;
	uint16_t argc;
	char *respOk;
	char *respErr;
}__attribute__ ((__packed__)) RailTelnet_st;

int {{ module.station.name }}(void *p);
void {{ module.station.name }}Init(void);
#endif /* TELNET_H_ */
