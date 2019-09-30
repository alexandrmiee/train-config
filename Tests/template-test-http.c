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

#ifdef WIN32
#define MAX_THREADS	20
HANDLE  hThreadArray[MAX_THREADS];
DWORD   dwThreadIdArray[MAX_THREADS];
int args[MAX_THREADS];
DWORD WINAPI testStation(void* p);
HANDLE ghMutex;
#endif

int main(void) {

	_initNetAdapter(inet_addr("127.0.0.1"),80,IPPROTO_TCP);

	{% for module in config['modules'] %}
	{{ config[module]['station.name'] }}Init();
	{% endfor %}

	#ifdef WIN32
	// Create a mutex with no initial owner
	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (ghMutex == NULL){
		printf("CreateMutex error: %ld\n", GetLastError());
		return 1;
	}
	for( int i=0; i<MAX_THREADS; i++ ){
		args[i]=i;
		//sHttp = getSocket(inet_addr("127.0.0.1"),80,IPPROTO_TCP);
		hThreadArray[i] = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size
			(LPTHREAD_START_ROUTINE) testStation,       // thread function name
			&args[i],          // argument to thread function
			0,                      // use default creation flags
			&dwThreadIdArray[i]);   // returns the thread identifier


		// Check the return value for success.
		// If CreateThread fails, terminate execution.
		//try(hThreadArray[i],"Thread error\n",3);
		 if (hThreadArray[i] == NULL) printf("Thread %d error\n",i);
	}
	#endif

	for(;;) {
		dbStation(NULL);
		{% for module in config['modules'] %}
		{{ config[module]['station.name'] }}(NULL);
		{% endfor %}
	}

	printf("\nEXIT\n");
	return EXIT_SUCCESS;
}



#ifdef WIN32
RailTcp_st trains[24];
DWORD WINAPI testStation(void* p){
	char  buffer[10240];
	char *pBuf = buffer;
	int restBuf = sizeof(buffer);

	TcpPacket_st tcp={
		.from_port = 0,
		.to_port =	80,
		.seq_num = 0,
		.ack_num = 0,
		.data_offset = 0,
		.flags = 0,
		.window = 0,
		.cksum = 0,
		.urgent_ptr = 0,
		.data = (uint8_t*)buffer
	};


	int nThread = *((int*)p);
	int iTrain = nThread;

	trains[iTrain].car = NULL;
	trains[iTrain].respLen = 0;
	trains[iTrain].response = NULL;
	trains[iTrain].respBufLen = 0;
	trains[iTrain].command = COMMAND_READ;

	TcpSocket_st tcpSocket = {0};

	int sHttp = INVALID_SOCKET;
	WSAEVENT connectEvent = WSACreateEvent();
	for(;;){
		if(sHttp == INVALID_SOCKET){
			printf("Wait new connection %d\n",nThread);
			sHttp = getSocket();
			if(sHttp == INVALID_SOCKET){
				printf("Failed create new socket on %d thread\n",nThread);
				return EXIT_FAILURE;
			}
			printf("Create new socket %d on %d thread\n",sHttp,nThread);
			WSAEventSelect(sHttp,connectEvent, FD_READ | FD_WRITE | FD_CLOSE | FD_ACCEPT | FD_CONNECT);


			trains[iTrain].packet = &tcp;

			tcpSocket.id = sHttp;
			trains[iTrain].socket = &tcpSocket;
		}

		int len = 0;

		WSANETWORKEVENTS netevent;
		int Index = WSAWaitForMultipleEvents(1, &connectEvent, TRUE, 1000, FALSE);
		if ((Index != WSA_WAIT_FAILED) && (Index != WSA_WAIT_TIMEOUT)) {
			printf("New socket event %d thread:\n",nThread);
			WSAEnumNetworkEvents(sHttp,connectEvent,&netevent);
			if (netevent.lNetworkEvents & FD_READ){
				len = recv(sHttp,pBuf, restBuf,0);
				printf("\treceived %d bytes\n",len);
			}
			if (netevent.lNetworkEvents & FD_WRITE){
				printf("\tsended\n");
			}
			if (netevent.lNetworkEvents & FD_CLOSE){
				printf("\tclosed\n");
				sHttp = INVALID_SOCKET;
				//closesocket(sHttp);
				//return EXIT_SUCCESS;
			}
			if (netevent.lNetworkEvents & FD_ACCEPT){
				printf("\taccepted\n");
			}
			if (netevent.lNetworkEvents & FD_CONNECT){
				printf("\tconnected\n");
			}

		}//end if ((Index != WSA_WAIT_FAILED) && (Index != WSA_WAIT_TIMEOUT))
		else{
			printf("Error %d thread %u\n",nThread,WSAGetLastError());
		}


		if(len>0){
			printf("Data from socket %d thread received:\n\t%s\n\tlength: %d\n\tsocket: %d\n",nThread,pBuf,len,sHttp);

			tcp.data = (uint8_t*)pBuf;

			DWORD dwWaitResult =
				WaitForSingleObject(
					ghMutex,    // handle to mutex
					INFINITE	// no time-out interval
				);
			if(WAIT_OBJECT_0 == dwWaitResult){
				Sleep(iTrain*100);
				sendTrainsFromDepot(ROUTE_ETH,ROUTE_TCP,&trains[iTrain]);
			}
			else
				printf("Mutex error\n");
			ReleaseMutex(ghMutex);

			if(++iTrain>20)iTrain=0;
			pBuf += len;
			restBuf -= len;
			if(restBuf<128){
				memset(buffer,0,sizeof(buffer));
				pBuf= buffer;
				restBuf = sizeof(buffer);
			}
		}//end if(len>0)
		else if(len<0){
			printf("Failed %d thread:\n Error number %d\n Error Code : %d\n",nThread,errno,WSAGetLastError());
			return EXIT_FAILURE;
		}//end els if(len<0)
	}

	return EXIT_SUCCESS;
}
#endif
