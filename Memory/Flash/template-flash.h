#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

{% for define in module.defines %}
#define {{ define.name }}	{{ define.value }}
{% endfor %}

uint8_t writeChunk(uint8_t *buf, uint32_t len,uint8_t isEnd);

#endif /*_FLASH_H_*/

