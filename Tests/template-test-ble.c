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

#include "database.h"
#include "json.h"
#include "files.h"
#include "flash.h"
#include "ble.h"
#include "serial.h"


int main(void) {
	serialStationInit();
	owStationInit();
	bleStationInit();

	for(;;) {
		serialStation(NULL);
		owStation(NULL);
		bleStation(NULL);
	}

}

