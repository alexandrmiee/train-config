/*
 *
 *
 * Create depot
 * Load parcel to train
 *
 *  Created on: 2018 07 05
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "database.h"
#include "tcp.h"
#include "telnet.h"
#include "json.h"
#include "http.h"
#include "files.h"

#define CARET "\r\n"

void traceStdOut(char* module, char* data){
  printf("{module:'%s',data:{%s}}"CARET,module,data);
  fflush(stdout);
  memset(data,0,strlen(data));
}

/*proxy interceptor sent data from stdin to telnet handler*/
static char  tcpBuffer[1024*10];
static char  stdinBuffer[1024*2];
static char *pBuf = stdinBuffer;

static TcpPacket_st tcp={
	.from_port = 0,
	.to_port =	23,
	.seq_num = 0,
	.ack_num = 0,
	.data_offset = 0,
	.flags = 0,
	.window = 0,
	.cksum = 0,
	.urgent_ptr = 0,
	.data = (uint8_t*)tcpBuffer
};

static RailTcp_st trains[24];

// Parcel_st telnetProxyParcel[4];
// Train_st telnetProxyTrain;
// void tcpProxyStationInit(void){
// 	telnetProxyTrain.box = malloc(sizeof(Parcel_st*)*4);
// 	for(uint16_t iParcel=0;iParcel<4;iParcel++){
// 		telnetProxyTrain.box[iParcel] = (Parcel_st*)&telnetProxyParcel[iParcel];
// 	}
// 	telnetProxyTrain.capacity = 4;
// 	telnetProxyTrain.route = ROUTE_HTTP;
// 	fillDepot(&telnetProxyTrain);
// }

void tcpProxyStation(void){
	static int iTrain=0;

  while(fgets(stdinBuffer, sizeof(stdinBuffer), stdin)){
    trains[iTrain].car = NULL;
    trains[iTrain].respLen = 0;
    trains[iTrain].response = NULL;
    trains[iTrain].respBufLen = 0;
    tcp.data = (uint8_t*)pBuf;
    trains[iTrain].packet = &tcp;
    static TcpSocket_st tcpSocket = {0};
    tcpSocket.id = 1;
    trains[iTrain].socket = &tcpSocket;
    trains[iTrain].command = COMMAND_READ;
		sendTrainsFromDepot(ROUTE_ETH,ROUTE_TCP,&trains[iTrain]);
	}
}