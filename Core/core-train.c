/*
 *
 *
 * Create depot
 * Load parcel to train
 *
 *  Created on: 2018 07 05
 *      Author: AL
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "core-train.h"
#include "core-routes.h"
#include "core-utils.h"


//#define DEBUG_TRACE
#ifdef DEBUG_TRACE
	#define TRACE(trace)	do{\
								trace;\
							}while(0)
#else
	#define TRACE(trace)
#endif



Parcel_st *meetTrain(Train_st *train){
	for(uint16_t iBox=0;iBox<train->capacity;iBox++){
		if((*train->box)[iBox].isFree==1){
			(*train->box)[iBox].isFree = 0;
			return &(*train->box)[iBox];
		}
	}
	return NULL;
}

uint16_t meetTrainBox(Train_st *train,uint16_t box){
	for(uint16_t iBox=box;iBox<train->capacity;iBox++){
		if((*train->box)[iBox].isFree==1){
			(*train->box)[iBox].isFree = 0;
			TRACE(printf("train # %d arrived\n",iBox););
			return iBox;
		}
	}
	return train->capacity+1;
}

int sendTrain(uint64_t from ,Train_st *train, void *parcel){
	TRACE(printf("Load Train %p with %d cars\n",train,train->capacity););
	for(uint16_t iBox=0;iBox<train->capacity;iBox++){
		if((*train->box)[iBox].isFree==0){
			(*train->box)[iBox].parcel = parcel;
			(*train->box)[iBox].sender = from;
			(*train->box)[iBox].isFree = 1;
			return EXIT_SUCCESS;
		}
		else TRACE(printf("Car # %d busy\n",iBox););
	}
	TRACE(printf("Error: the train is full\n"););
	return EXIT_FAILURE;
}

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
	return EXIT_SUCCESS;
}

int sendTrainsFromDepot(uint64_t from ,uint64_t to, void *parcel){
	Train_st *train = tail;
	//TRACE(printf("broadcast send parcel from %I64u to %I64u\n",from,to););
	while(train){
		if(to==train->route){
			TRACE(printf("find passing train %p\n",train););
			for(uint16_t iBox=0;iBox<train->capacity;iBox++){
				if((*train->box)[iBox].isFree==0){
					(*train->box)[iBox].sender = from;
					(*train->box)[iBox].parcel = parcel;
					(*train->box)[iBox].isFree = 1;
					TRACE(printf("Load # %d car\n",iBox););
					break;
				}
				else TRACE(printf("Car # %d busy\n",iBox););
			}
		}
		//else TRACE(printf("Train %p is going to %I64u\n",train,train->route););
		train = train->prev;
	}
	return EXIT_SUCCESS;
}

