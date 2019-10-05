/*
 * MQTT.c
 * Autogenerated code from TrainFramework
 *  Created on: {{ date }}
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

#include "tcp.h"
#include "json.h"
#include "http.h"
#include "mqtt.h"
#include "RestFullController.h"

#include "MQTTClient.h"


#define {{ module.debug.level  }}
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif

#define MQTT_TIMEOUT	{{ module.timeout }}
#define MODULE_ID "{{ module.options.id }}"
{% for topic in module.topics %}
#define {{ topic.name }}	"{{ topic.value }}"
{% endfor %}

void TimerInit(Timer* timer){
}

char TimerIsExpired(Timer* timer){
	if(!timer)return 0;
	RTC_TIME(timer->cur);
	return (char)(timer->cur > timer->end);
}

//set timer in ms
void TimerCountdownMS(Timer* timer, unsigned int timeout){
	if(!timer)return;
	RTC_TIME(timer->end);
	timer->end += timeout;
}
//set timer in s
void TimerCountdown(Timer* timer, unsigned int timeout){
	if(!timer)return;
	RTC_TIME(timer->end);
	timer->end += timeout*1000;
}
//update timer
int TimerLeftMS(Timer* timer){
	if(!timer)return 0;
	RTC_TIME(timer->cur);
	return (timer->cur > timer->end) ? 0 : (timer->end - timer->cur);
}



static Network n;
static MQTTClient c;
static uint8_t mqttBufRx[{{ module.buffer.rx }}];
static uint8_t mqttBufTx[{{ module.buffer.tx }}];


int mqttRead(Network* n, unsigned char* buffer, int len, int timeout_ms) {
	if(!n->railTcp){
		//TRACE(printf("MQTT ERROR mqttRead:\n\tno data received\n"););
		return 0;
	}
	else if(n->railTcp->respLen >= len){
		memcpy(buffer,n->railTcp->packet->data,len);

		TRACE(printf("MQTT read %d bytes\n\t",len););
		for(int i=0;i<len;i++){
			printf("0x%02X ",n->railTcp->packet->data[i]);
		}
		printf("\n");

		n->railTcp->packet->data += len;
		n->railTcp->respLen -= len;
	}
	else{
		//TRACE(printf("MQTT ERROR.mqttRead too many read:\n\t remain %d\n\tneed %d\n",n->railTcp->respLen,len););
		return n->railTcp->respLen;
	}
	return len;
}


int mqttWrite(Network* n, unsigned char* buffer, int len, int timeout_ms) {
	TRACE(printf("MQTT write:\n\t%s\n\t%d\n\tsocket: %d\n",buffer,len,n->socket););
	#ifdef WIN32
	send(
		n->socket,
		(char*)buffer,
		len,
		0
	);
	#endif
	return len;
}


void mqttDisconnect(Network* n) {
	TRACE(printf("MQTT disconnected\n"););
}

void NetworkInit(Network* n) {
	//n->railTcp = initTcpClient();
	n->mqttread = mqttRead;
	n->mqttwrite = mqttWrite;
	n->disconnect = mqttDisconnect;
	n->railTcp = NULL;
	#ifdef WIN32
	n->socket =	initClient(inet_addr("127.0.0.1"),{{ module.port }},IPPROTO_TCP);
	#elif __linux__
	n->socket =	initClient(inet_addr("127.0.0.1"),{{ module.port }},NULL);
	#endif
}


int NetworkConnect(Network* n, char* addr, int port){
	return 0;
}

static Parcel_st *box;
static uint16_t iBox;
static void *car;
{% for topic in module.topics %}
void {{ topic.handler }}(MessageData* md){
	TRACE(printf("\tExecute {{ topic.handler}} %s\n",(char*)md->message->payload););

	static char *argv[32];
	static uint8_t argc;

	restParser(
		(char*)md->message->payload,
		(char*)md->message->payload,
		md->message->payloadlen,
		{{ topic.command}},
		{{ topic.separator}},
		&argv[0],
		&argc,
		32
	);

	static {{ module.station.railType }} {{ module.station.railName }}_{{ loop.index0 }};

	{{ module.station.railName }}_{{ loop.index0 }}.command = {{ topic.to.command }};
	{{ module.station.railName }}_{{ loop.index0 }}.argv = &argv[0];
	{{ module.station.railName }}_{{ loop.index0 }}.argc = argc;
	{{ module.station.railName }}_{{ loop.index0 }}.topic = MODULE_ID {{ topic.name}};

	(({{ topic.from.railType }}*)car)->car = &{{ module.station.railName }}_{{ loop.index0 }};

	sendTrainsFromDepot({{ module.station.route }},{{ topic.to.route }},box->parcel);
}
{% endfor %}

static Parcel_st {{ module.station.parcel }}[{{ module.station.parcels }}];
static Train_st {{ module.station.train }};
static Parcel_st *pBox[{{ module.station.parcels }}];
void {{ module.station.name }}Init(void){
	{{ module.station.train }}.box = pBox;//malloc(sizeof(Parcel_st*)*{{ module.station.parcels }});
	TRACE(printf("MQTT initializing...\n"););
	for(uint16_t iParcel=0;iParcel<{{ module.station.parcels }};iParcel++){
		{{ module.station.train }}.box[iParcel] = (Parcel_st*)&{{ module.station.parcel }}[iParcel];
	}
	{{ module.station.train }}.capacity = {{ module.station.parcels }};
	{{ module.station.train }}.route = {{ module.station.route }};
	fillDepot(&{{ module.station.train }});

	TRACE(printf("\tClient TCP connecting...\n"););
	NetworkInit(&n);
	TRACE(printf("\tClient TCP connected...\n"););

	TRACE(printf("\tMQTT client initializing...\n"););
	MQTTClientInit(
		&c, /*MQTTClient* c*/
		&n, /*Network* network*/
		{{ module.timeout }}, /*unsigned int command_timeout_ms*/
		mqttBufTx, /*unsigned char* sendbuf*/
		sizeof(mqttBufTx), /*size_t sendbuf_size*/
		mqttBufRx, /*unsigned char* readbuf*/
		sizeof(mqttBufRx) /*size_t readbuf_size*/
	);

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.willFlag = 1;
	data.MQTTVersion = 4;
	data.clientID.cstring = "{{ module.options.id }}";
	data.username.cstring = "{{ module.options.uname }}";
	data.password.cstring = "{{ module.options.password }}";

	data.keepAliveInterval = {{ module.options.keepalive }};
	data.cleansession = 1;

	data.will.qos = {{ module.options.qos }};
	data.will.topicName.cstring = "amq.topic";
	TRACE(printf("\tMQTT client initialized...\n"););

	TRACE(printf("\tMQTT client connecting...\n"););
	MQTTConnect(&c, &data);

}

int mqttParser(MQTTClient* c){
	uint8_t rc = MQTT_SUCCESS;
	uint8_t sessionPresent = 0;

	MQTTSubackData data;
	int count = 0;
	unsigned short mypacketid;

	MQTTString topicName;
	MQTTMessage msg;
	int intQoS,len;

	int packet_type = mqttReadPacket(c, NULL);     /* read the socket, see what work is due */
	switch (packet_type){
		default:
			/* no more data to read, unrecoverable. Or read packet fails due to unexpected network error */
			rc = FAILURE;
			TRACE(printf("\tMQTT ERROR. mqttParser command %d\n",packet_type););
		break;
		case SUBACK:
			TRACE(printf("\tMQTT subscribe acknowledgment\n"););
			data.grantedQoS = QOS0;
			if (MQTTDeserialize_suback(&mypacketid, 1, &count, (int*)&data.grantedQoS, c->readbuf, c->readbuf_size) == 1){
				if (data.grantedQoS == 0x80)
					TRACE(printf("\tMQTT ERROR. SUBACK"););
				else
					TRACE(printf("\tMQTT clients subscribed\n"););
			}
		break;
		case CONNACK:
			TRACE(printf("\tMQTT connection acknowledgment\n"););
			MQTTDeserialize_connack(&sessionPresent, &rc, c->readbuf, c->readbuf_size);
			if ( rc== 0 ){
				c->isconnected = 1;
				c->ping_outstanding = 0;
				TRACE(printf("\tMQTT clinet connected\n"););

				TRACE(printf("\tMQTT clients subscribing...\n"););
				{% for topic in module.topics %}
				MQTTSubscribe(
					c,	/*MQTTClient* c*/
					{{ topic.name }}, /*const char* topicFilter*/
					{{ topic.qos }}, /*enum QoS qos*/
					{{ topic.handler }} /*messageHandler messageHandler*/
				);
				{% endfor %}
			}
			else if(rc==1){
				TRACE(printf("\tConnection Refused, unacceptable protocol version.The Server does not support the level of the MQTT protocol requested by the Client.\n"););
				rc = FAILURE;
			}
			else if(rc==2){
				TRACE(printf("\tConnection Refused, identifier rejected.The Client identifier is correct UTF-8 but not allowed by the Server.\n"););
				rc = FAILURE;
			}
			else if(rc==3){
				TRACE(printf("\tConnection Refused, Server unavailable.The Network Connection has been made but the MQTT service is unavailable.\n"););
				rc = FAILURE;
			}
			else if(rc==4){
				TRACE(printf("\tConnection Refused, bad user name or password.The data in the user name or password is malformed.\n"););
				rc = FAILURE;
			}
			else if(rc==5){
				TRACE(printf("\tConnection Refused, not authorized.The Client is not authorized to connect.\n"););
				rc = FAILURE;
			}
			else{
				TRACE(printf("\tConnected with error status %d\n",rc););
				rc = FAILURE;
			}
		break;
		case PUBLISH:
			msg.payloadlen = 0; /* this is a size_t, but deserialize publish sets this as int */
			if (MQTTDeserialize_publish(&msg.dup, &intQoS, &msg.retained, &msg.id, &topicName,
			   (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->readbuf, c->readbuf_size) != 1)
				break;
			msg.qos = (enum QoS)intQoS;
			deliverMessage(c, &topicName, &msg);
			if (msg.qos != QOS0){
				if (msg.qos == QOS1)
					len = MQTTSerialize_ack(c->buf, c->buf_size, PUBACK, 0, msg.id);
				else if (msg.qos == QOS2)
					len = MQTTSerialize_ack(c->buf, c->buf_size, PUBREC, 0, msg.id);
//				if (len <= 0)
//					rc = FAILURE;
//				else
//					rc = sendPacket(c, len, NULL);
			}
		break;
	}
	return rc;
}





int {{ module.station.name }}(void *p){
	iBox = meetTrainBox(&{{module.station.train }},0);
	box = {{module.station.train }}.box[iBox];
	while(box){
		{# the train arrives #}
		if(iBox>{{module.station.train }}.capacity) return EXIT_SUCCESS;
		{# get the car from the train #}
//		static void *car;
		car = box->parcel;
		try((car),"\tempty car on MQTT train",EXIT_FAILURE);
		uint8_t i=0;
		char *topic;
		while(car){
			/*
			 * Read and parse new TCP packet
			 */
			if( (0 == i) && (0 == (((RailTcp_st*)car)->command)) ){
				TRACE(printf("MQTT TCP read\n"););
				n.railTcp = ((RailTcp_st*)car);
				mqttParser(&c);
			}
			else if( (1 == i) && (ROUTE_JSON_GET_MQTT == (((RailJson_st*)car)->command)) ){
				topic = ((RailMqtt_st *)car)->topic;
			}
			{% for path in module.pathways %}
			else if( ({{  path.from.carNumber }} == i) && ({{ path.from.command }} == ((({{ path.from.railType }}*)car)->command)) ){
				TRACE(printf("\tCOMMAND_MQTT_SEND\n"););
				if(c.isconnected){
					{{ path.from.railType }} *{{ path.from.railName }} = (({{ path.from.railType }} *)car);
					static MQTTMessage pubmsg;
					pubmsg.payload = {{ path.from.railName }}->response;
					pubmsg.payloadlen = {{ path.from.railName }}->respLen;
					pubmsg.qos = 0;
					pubmsg.retained = 0;
					pubmsg.dup = 0;
					TRACE(printf("\tresponse: %s\n\tlen: %d\n\ttopic: %s\n",(char*)pubmsg.payload,pubmsg.payloadlen,topic););
					MQTTPublish(&c, topic, &pubmsg);
				}
				else{
					TRACE(printf("\tMQTT ERROR. Client disconnected\n"););
				}
			}
			{% endfor %}
			else{
				TRACE(printf("MQTT command (Hitch) %I64u not found\n",(((RailTcp_st*)car)->command)););
			}
			car = ((Hitch_st*)car)->car;
			i++;
		}
		iBox = meetTrainBox(&{{module.station.train }},iBox);
		box = {{module.station.train }}.box[iBox];
	}

	return EXIT_SUCCESS;
}
