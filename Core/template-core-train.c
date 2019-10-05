/*
 * Create depot
 * Load parcel to train
 *
 *  Created on: {{date}}
 *      Author: Alexandr Lukovnikov
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"

{%if module.traceNode %}
#include "integration-node.h"
#define MODULE_NAME __FILE__
char outputString [{{module.outputStringSize}}];
{% endif %}

Parcel_st *meetTrain(Train_st *train){
	for(uint16_t iBox=0;iBox<train->capacity;iBox++){
		if((*train->box)[iBox].isFree==1){
			(*train->box)[iBox].isFree = 0;
			return &(*train->box)[iBox];
		}
	}
	return NULL;
}

/*
	unload next data from list
	walk around list array of pointers
	find new data with flag isFree==1
	clear flag isFree

	if not found - ?
*/
uint16_t meetTrainBox(Train_st *train,uint16_t box){
	for(uint16_t iBox=box;iBox<train->capacity;iBox++){
		if((*train->box)[iBox].isFree==1){
			(*train->box)[iBox].isFree = 0;
			{%if module.traceNode %}
				sprintf(outputString,
						"action: 'meetTrainBox',"
						"train:{"
							"pointer:'%p',"
							"route: %d,"
							"capacity: %d,"
						"},"
						"box:%d,",
				train,train->route,train->capacity,iBox);
				traceStdOut(MODULE_NAME,outputString);
			{% endif %}
			return iBox;
		}
	}
	return train->capacity+1;
}

/*
	add new Train_st in linked list `tail`
*/
static Train_st *tail=NULL;
int fillDepot(Train_st *train){
	if(tail==NULL){
		tail = train;
		tail->prev = NULL;
	}
	else{
		Train_st *t = tail;
		tail = train;
		tail->prev = t;
	}
	{%if module.traceNode %}
	sprintf(outputString,
			"action:'fillDepot',"
			"train:{"
				"pointer:'%p',"
				"route: %d,"
				"capacity: %d,"
			"},",
	train,train->route,train->capacity);
	traceStdOut(MODULE_NAME,outputString);
	{% endif %}
	return EXIT_SUCCESS;
}

/*
	load data to linked list array buffer
	walk around list array of pointers
	find empty slot with flag isFree==0
	set isFree - new data flag, parcel - pointer to data, sender - identify of call module
	return EXIT_SUCCESS

	if not found - return EXIT_FAILURE
*/
int sendTrainsFromDepot(uint64_t from ,uint64_t to, void *parcel){
	Train_st *train = tail;
	uint16_t iBox=0;
	while(train){
		if(to==train->route){
			for(;iBox<train->capacity;iBox++){
				if((*train->box)[iBox].isFree==0){
					(*train->box)[iBox].sender = from;
					(*train->box)[iBox].parcel = parcel;
					(*train->box)[iBox].isFree = 1;
					{%if module.traceNode %}
					sprintf(
						outputString,
						"action: 'sendTrainsFromDepot',"
						"train:{"
							"pointer:'%p',"
							"route: %d,"
							"capacity: %d,"
							"boxArrayIndex:%d,"
						"},"
						"from:%d,"
						"to:%d,",
						train,train->route,train->capacity,iBox,from,to
					);
					traceStdOut(MODULE_NAME,outputString);
					{% endif %}
					break;
				}
			}
		}
		train = train->prev;
	}
	return EXIT_SUCCESS;
}

