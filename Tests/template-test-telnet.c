/*
 * TrainFramework test
 * {{ name }}
 * {{ date }}
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef WIN32
#include<winsock2.h>
#endif

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "database.h"
#include "tcp.h"
#include "telnet.h"
#include "json.h"
#include "http.h"
#include "files.h"


int sTelnet;
int testStation(void* p);

int main(void) {


	{% for module in config.modules %}
	{{ config[module].station.name }}Init();
	{% endfor %}

	printf("all stations init, wait new connection\n");
	sTelnet = getSocket(inet_addr("127.0.0.1"),23,IPPROTO_TCP);
	send(sTelnet,"user>",5,0);

	for(;;) {
		dbStation(NULL);
		if(testStation(NULL)==EXIT_FAILURE)
			return EXIT_FAILURE;
		{% for module in config.modules %}
		{{ config[module].station.name }}(NULL);
		{% endfor %}
	}

	return EXIT_SUCCESS;
}



static char  buffer[1025];
static char *pBuf = buffer;
static int restBuf = sizeof(buffer);

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
	.data = (uint8_t*)buffer
};

static RailTcp_st trains[24];
int testStation(void* p){
	static int iTrain=0;
	int len = 0;

	#ifdef WIN32
	extern WSAEVENT connectEvent;

	WSANETWORKEVENTS netevent;
	int Index = WSAWaitForMultipleEvents(1, &connectEvent, TRUE, 1000, FALSE);
	if ((Index != WSA_WAIT_FAILED) && (Index != WSA_WAIT_TIMEOUT)) {
		printf("New event:\n");
		WSAEnumNetworkEvents(sTelnet,connectEvent,&netevent);
		if (netevent.lNetworkEvents & FD_READ){
			len = recv(sTelnet,pBuf, restBuf,0);
			printf("\treceived %d bytes\n",len);
		}
		if (netevent.lNetworkEvents & FD_WRITE){
			printf("\tsended\n");
		}
		if (netevent.lNetworkEvents & FD_CLOSE){
			printf("\tclosed\n");
			closesocket(sTelnet);
		}
		if (netevent.lNetworkEvents & FD_ACCEPT){
			printf("\taccepted\n");
			struct sockaddr_in client = {};
			int clientSize = sizeof(client);
			sTelnet =  accept(sTelnet, (struct sockaddr*)&client, &clientSize);
			WSAEventSelect(sTelnet,connectEvent, FD_READ | FD_WRITE | FD_CLOSE);
		}

	}
	else{
		printf("Timeout\n");
	}


	if(len<0){
		printf("Failed:\n Error number %d\n Error Code : %d\n",errno,WSAGetLastError());
		return EXIT_FAILURE;
	}
	if(len>0){
		printf("\tdata from telnet received:\n\t%s\n\tlength: %d\n",pBuf,len);

		trains[iTrain].car = NULL;
		trains[iTrain].respLen = 0;
		trains[iTrain].response = NULL;
		trains[iTrain].respBufLen = 0;
		tcp.data = (uint8_t*)pBuf;
		trains[iTrain].packet = &tcp;
		static TcpSocket_st tcpSocket = {0};
		tcpSocket.id = sTelnet;
		trains[iTrain].socket = &tcpSocket;
		trains[iTrain].command = COMMAND_READ;

		sendTrainsFromDepot(ROUTE_ETH,ROUTE_TCP,&trains[iTrain]);
		if(++iTrain>20)iTrain=0;
		pBuf += len;
		restBuf -= len;
		if(restBuf<128){
			memset(buffer,0,sizeof(buffer));
			pBuf= buffer;
			restBuf = sizeof(buffer);
		}
	}


	#endif
	return EXIT_SUCCESS;
}
