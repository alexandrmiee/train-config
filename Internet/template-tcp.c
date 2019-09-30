/*
 * Tcp.c
 * Autogenerated code from TrainFramework
 *
 *  Created on: {{ date }}
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "tcp.h"
#include "json.h"
#include "telnet.h"
#include "http.h"
#include "files.h"

#ifdef WIN32
#include 	<windows.h>
#include	<winsock2.h>
#include 	<ws2tcpip.h>
#endif

#define {{ module.debug.level  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

int clientSoket = INVALID_SOCKET;
int getListenSocket(){
	return clientSoket;
}

int initAdapter(void){
	#ifdef WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return EXIT_FAILURE;
	}
	#endif
	return EXIT_SUCCESS;
}

int _initNetAdapter(uint32_t inetAddress,uint16_t port, uint8_t protocol){
	#ifdef WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return EXIT_FAILURE;
	}

	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inetAddress;
	service.sin_port = htons(port);
	//create socket
	clientSoket=socket(AF_INET, SOCK_STREAM, protocol);
	if( clientSoket == INVALID_SOCKET ){
		printf("Error: Socket Failed\n%d\n", errno);
		return INVALID_SOCKET;
	}
	int i=1;
	setsockopt(clientSoket,SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof (i));

	//bind socket
	if(bind(clientSoket, (SOCKADDR *) &service, sizeof (service))== SOCKET_ERROR){
		printf("bind failed with error %u\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	//server socket listen
	if(listen(clientSoket,SOMAXCONN) == SOCKET_ERROR){
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return INVALID_SOCKET;
	}
	#endif
	return EXIT_SUCCESS;
}

int initServer(uint32_t inetAddress,uint16_t port, uint8_t protocol){
	#ifdef WIN32
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
//		printf("Failed. Error Code : %d\n",WSAGetLastError());
//		return EXIT_FAILURE;
//	}

	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inetAddress;
	service.sin_port = htons(port);
	//create socket
	clientSoket=socket(AF_INET, SOCK_STREAM, protocol);
	if( clientSoket == INVALID_SOCKET ){
		printf("Error: Socket Failed\n%d\n", errno);
		return INVALID_SOCKET;
	}
	int i=1;
	setsockopt(clientSoket,SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof (i));

	//bind socket
	if(bind(clientSoket, (SOCKADDR *) &service, sizeof (service))== SOCKET_ERROR){
		printf("bind failed with error %u\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	//server socket listen
	if(listen(clientSoket,SOMAXCONN) == SOCKET_ERROR){
		printf("Failed. Error Code : %d\n",WSAGetLastError());
		return INVALID_SOCKET;
	}
	#endif
	return EXIT_SUCCESS;
}

#ifdef WIN32
SOCKET ConnectSocket = INVALID_SOCKET;
#endif
int initClient(uint32_t inetAddress,uint16_t port, uint8_t protocol){
	#ifdef WIN32
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0){
//		printf("Failed. Error Code : %d\n",WSAGetLastError());
//		return EXIT_FAILURE;
//	}



	// Create a SOCKET for connecting to server
	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inetAddress;
	service.sin_port = htons(port);
	//create socket
	ConnectSocket=socket(AF_INET, SOCK_STREAM, protocol);

	if (ConnectSocket == INVALID_SOCKET) {
	    printf("Error at socket(): %d\n", WSAGetLastError());
	    WSACleanup();
	    return 1;
	}

	// Connect to server.
	int iResult = connect(ConnectSocket, (SOCKADDR *) &service, sizeof (service));
	if (iResult == SOCKET_ERROR) {
	    closesocket(ConnectSocket);
	    ConnectSocket = INVALID_SOCKET;
	}

	if (ConnectSocket == INVALID_SOCKET) {
	    printf("Unable to connect to server!\n");
	    WSACleanup();
	    return 1;
	}

	#endif
	return ConnectSocket;
}

int getClientSocket(){
	return ConnectSocket;
}

int getServerSocket(){
	#ifdef WIN32
	struct sockaddr_in client = {};
	int clientSize = sizeof(client);
	int nSocket =  accept(clientSoket, (struct sockaddr*)&client, &clientSize);
	return nSocket;
	#endif
	return INVALID_SOCKET;
}

int getSocket(){
	#ifdef WIN32
	struct sockaddr_in client = {};
	int clientSize = sizeof(client);
	int nSocket =  accept(clientSoket, (struct sockaddr*)&client, &clientSize);
	return nSocket;
	#endif
	return INVALID_SOCKET;
}

Parcel_st {{ module.station.parcel }}[{{ module.station.parcels }}];
Train_st {{ module.station.train }};
void {{ module.station.name }}Init(void){
	fillDepot(&{{ module.station.train }});
	{{ module.station.train }}.box = malloc(sizeof(Parcel_st*)*{{ module.station.parcels }});
	for(uint16_t iParcel=0;iParcel<{{ module.station.parcels }};iParcel++){
		{{ module.station.train }}.box[iParcel] = (Parcel_st*)&{{ module.station.parcel }}[iParcel];
	}
	{{ module.station.train }}.capacity = {{ module.station.parcels }};
	{{ module.station.train }}.route = {{ module.station.route }};
}

/*
 * TCP Railway Station
 */
int {{ module.station.name }}(void *p){
	//Parcel_st *box = meetTrain(&{{module.station.train }});
	uint16_t iBox = meetTrainBox(&{{module.station.train }},0);
	Parcel_st *box = {{module.station.train }}.box[iBox];
	while(box){
		if(iBox>{{module.station.train }}.capacity) return EXIT_SUCCESS;
		printf("\nTCP Station\n");
		printf("\t%p %d %p\n",box,iBox,&tcpParcel[iBox]);
		static void *car;
		car = box->parcel;

		try((car),"\tempty car on tcp train",EXIT_FAILURE);
		int socket = 0;
		try(( (({{ module.station.railType }}*)car)->socket ),"\tInvalid socket",EXIT_FAILURE);
		socket = (({{ module.station.railType }}*)car)->socket->id;
		uint8_t i=0;
		while(car){
			TRACE(printf("\tCar number %d command (Hitch_st) %I64u \n",i,((Hitch_st *)car)->command););
			if(car==NULL) return EXIT_FAILURE;
			{% for path in module.pathways %}

			else if( ({{  path.from.carNumber }} == i) & ( {{ path.from.command }} == (({{ path.from.railType }} *)car)->command ) ){
				TRACE(printf("\t{{ path.from.command }} load\n"););
				{# get the parcel from the car #}
				{{ path.from.railType }} *{{ path.from.railName }} = (({{ path.from.railType }} *)car);

				{# create the car for the hitch with the train #}

				static {{ module.station.railType }} {{ module.station.railName }}_{{ loop.index0 }}={NULL};


				try( ({{ path.from.railName }}), "\tthe rails {{ path.from.railType }} do not go to {{  module.station.name }}\n", EXIT_FAILURE );

				{# load the parcel from the train #}
				{%- if path.from.loader is not none -%}
				{{ module.station.railName }}_{{ loop.index0 }}.response = (uint8_t*){{ path.from.railName }}->response;
				{{ module.station.railName }}_{{ loop.index0 }}.respLen = {{ path.from.railName }}->respLen;
				TRACE(printf("\tSend response:\n\t\t%d bytes\n\t\t%s\n",{{ module.station.railName }}_{{ loop.index0 }}.respLen,{{ module.station.railName }}_{{ loop.index0 }}.response););
				{%- else %}
				{%- endif %}


				{# select loader for new train #}
				{%- if path.to.loader=='send' -%}
				/*
				 * send response to TCP client or
				 * send request to TCP server
				 */
				try( (socket), "\tthe rails {{ path.from.railType }} do not go to {{  module.station.name }}\n", EXIT_FAILURE );
				{{ path.to.loader }}(
					socket,
					(char*){{ module.station.railName }}_{{ loop.index0 }}.response,
					{{ module.station.railName }}_{{ loop.index0 }}.respLen,
					0
				);
				{%- else %}
				/*
				 * send message to TCP observers
				 * example  railTcp->to_port=80 send message to ROUTE_HTTP
				 */
				{{ path.from.railName }}->car = &{{ module.station.railName }}_{{ loop.index0 }};
				{{ path.to.loader }}({{ module.station.route }},{{ path.to.route }},box->parcel);
				{%- endif %}
				(({{ path.from.railType }} *)car)->command = 0;
			}

			{% endfor %}

			car = ((Hitch_st*)car)->car;
			i++;
		}

		//box=meetTrain(&{{module.station.train }});
		iBox = meetTrainBox(&{{module.station.train }},iBox);
		box = {{module.station.train }}.box[iBox];
	}
	return EXIT_SUCCESS;
}
