/*
 * TrainFramework test
 * {{ name }}
 * {{ date }}
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

{%for module in config.modules%}#include "{{module}}.h"
{%endfor%}
#include "halInterface.h"

int main(void) {
	_init();
	{% for module in config.modules %}
	{% if config[module].station.name %}
	{{ config[module].station.name }}Init();
	{% endif %}
	{% endfor %}


	for(;;) {
		{% for module in config.modules %}
		{% if config[module].station.name %}
		{{ config[module].station.name }}(NULL);
		{% endif %}
		{% endfor %}
	}

}



