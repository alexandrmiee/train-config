/*
 * files.h
 *
 *  Created on: {{ date }}
 *      Author: home
 */

#ifndef FILES_H_
#define FILES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"



typedef struct RailFiles{
	void *car;
	uint64_t command;
	uint8_t *response;
	uint16_t respLen;
	uint16_t respBufLen;
}__attribute__ ((__packed__)) RailFiles_st;

typedef struct{
	uint16_t fsize;
	char *fname;
	char *content;
	uint8_t *file;
} FileDesc_st;

#define HTML_CONTENT_TEXT_HTML	"text/html"
#define HTML_CONTENT_TEXT_CSS	"text/css"
#define HTML_CONTENT_TEXT_JS	"application/javascript"
#define HTML_CONTENT_JSON		"application/json"

{% for define in module.defines %}
#define {{ define.name }}	{{ define.value }}
{% endfor %}

int {{ module.station.name }}(void *p);
void {{ module.station.name }}Init(void);

#endif /* FILES_H_ */

