/*
 *
 * Create depot
 * Load parcel to train
 *
 *  Created on: 2018 07 05
 *      Author: AL
 */

#ifndef CORE_ROUTES_H_
#define CORE_ROUTES_H_

#define ROUTE_ETH 		1000
	#define ROUTE_IPV4 		0x0800
		#define ROUTE_TCP 		6
			#define ROUTE_TELNET 	23
			#define ROUTE_HTTP 		80
			#define ROUTE_MQTT 		1883
				#define COMMAND_MQTT_READ	18831
				#define COMMAND_MQTT_SEND	18832
			#define ROUTE_FTP_CMD 	21
			#define ROUTE_FTP_DATA 	59669
			#define ROUTE_PROXY 	8080

		#define ROUTE_UDP 		17
			#define ROUTE_SNMP 		161
			#define ROUTE_NTP 		123
			#define ROUTE_DHCP 		67
			#define ROUTE_DNS 		53

		#define ROUTE_ICMP 		1

	#define ROUTE_ARP 		0x0806

#define ROUTE_SMS 		2000
	#define ROUTE_SMS_APP 	2001

#define ROUTE_CAN 		3000
	#define ROUTE_CAN_APP 	3001

#define ROUTE_BLE 		4000
	#define ROUTE_BLE_APP 	4001

#define ROUTE_JSON 		5000
	#define ROUTE_JSON_GET_TELNET 	5001
	#define ROUTE_JSON_GET_HTTP 	5002
	#define ROUTE_JSON_GET_MQTT 	5003
	#define ROUTE_JSON_GET_SNMP 	5004
	#define ROUTE_JSON_GETV 		5005
	#define ROUTE_JSON_PUT 			5006
	#define ROUTE_JSON_ERROR		5007

#define ROUTE_DATABASE 	6000

#define ROUTE_REST	7000

#define ROUTE_FILES 8000
	#define ROUTE_FILE_GET 	8001
	#define ROUTE_FILE_PUT 	8002
	#define ROUTE_DIR_GET 	8003

#define ROUTE_SERIAL	9000
	#define COMMAND_SERIAL_COMPLITE	9001

#define ROUTE_OW		1100
	#define COMMAND_ONEWIRE_GET		1101

#define ROUTE_SENSOR	1200
	#define ROUTE_SENSOR_WATER 1201
	#define ROUTE_SENSOR_REED_SWITCH 1202

#define COMMAND_SENSOR_READ	1

#define COMMAND_SERIAL_READ_OBSERVER	1
#define COMMAND_SERIAL_SEND 			2
#define COMMAND_SERIAL_READ_REQUEST 	3

#define COMMAND_REST_GET 		"get "
#define COMMAND_REST_GETV 		"getv "
#define COMMAND_REST_PUT 		"put "
#define COMMAND_REST_SEP_SPACE	" "
#define COMMAND_JSON_RESPONSE 	1
#define COMMAND_READ 			2
#define COMMAND_HTTP_RESPONSE	3
#define COMMAND_SNMP_RESPONSE	4

#define COMMAND_HTTP_GET		"GET"
	#define COMMAND_HTTP_PATH_FILES			"files"
	#define COMMAND_HTTP_PATH_FILES_FORM	"save"
	#define COMMAND_HTTP_PATH_DIR			"dir"
	#define COMMAND_HTTP_PATH_JSON			"json"
	#define COMMAND_HTTP_PATH_PROTOBUF		"pb"

#define COMMAND_HTTP_POST		"POST"
#define COMMAND_HTTP_PUT		"PUT"
#define COMMAND_HTTP_PATCH		"PATCH"
#define COMMAND_HTTP_DELETE		"DELETE"

#endif /*CORE_ROUTES_H_*/
