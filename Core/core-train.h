/*
 *
 *
 * Create depot
 * Load parcel to train
 *
 *  Created on: 2018 07 05
 *      Author: AL
 */

#ifndef CORE_TRAIN_H_
#define CORE_TRAIN_H_

typedef struct Parcel{
	uint64_t sender;
	uint8_t isFree;
	void *parcel;
}__attribute__ ((__packed__)) Parcel_st;

typedef struct {
	void *car;
	uint64_t command;
	char *response;
	uint16_t respLen;
	uint16_t respBufLen;
	char **argv;
	uint16_t argc;
}__attribute__ ((__packed__)) Hitch_st;

typedef struct Train{
	Parcel_st **box;
	uint16_t capacity;
	uint64_t route;
	struct Train *prev;
}__attribute__ ((__packed__)) Train_st;

Parcel_st *meetTrain(Train_st *train);
uint16_t meetTrainBox(Train_st *train,uint16_t box);
// int sendTrain(uint64_t from ,Train_st *train, void *parcel);
int fillDepot(Train_st *train);
int sendTrainsFromDepot(uint64_t from ,uint64_t to, void *parcel);

#endif /*CORE_TRAIN_H_*/
