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

{%for module in modules.modules%}#include "{{module}}.h"
{%endfor%}


int main(void) {

	initAdapter();

	{% for module in modules.modules %}{% if modules[module].station.name %}
	{{ modules[module].station.name }}Init();{% endif %}{% endfor %}

	
	for(;;) {
		dbStation(NULL);
		{% for module in modules.modules %}{% if modules[module].station.name %}
		{{ modules[module].station.name }}(NULL);{% endif %}{% endfor %}
		tcpProxyStation();
	}

	return EXIT_SUCCESS;
}

