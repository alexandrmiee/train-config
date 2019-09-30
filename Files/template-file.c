/*
 * {{ module['array'] }}File.c
 * Autogenerated code from TrainFramework
 *  Created on: {{ date }}
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "files.h"


uint8_t {{ module['array'] }}[] = {
 @bytes@
};

FileDesc_st {{ module['array'] }}Descriptor = {
	.fname = "{{ module['filename'] }}",
	.fsize = @bytesSize@,
	.file = {{ module['array'] }},
	.content = {{ module['content'] }}
};
