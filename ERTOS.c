/*
 * ERTOS.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


#include "ERTOS.h"


typedef struct {
	uint32_t ui32ThreadID;

	//function pointer
	//item list
	//timeout
	//priority

}OSThread_t;


typedef struct {
	uint32_t ui32ThreadID;
	uint32_t ui32NoOfItems;

	listItem_t* pliIndex;
	listitem_t* pliEnd;

	//function pointer
	//item list
	//timeout
	//priority

}list_t;


typedef struct {
	uint32_t ui32ThreadID;

	listItem_t* pliNext;
	listItem_t* pliPrev;
	//function pointer
	//item list
	//timeout
	//priority

}listItem_t;


void OS_run(){




	while(1){

	}
}



void OSCreateThread(OSThread* me, void* sp, uint32_t ui32StkSize, uint32_t ui32Priorty){



}





void OSInitialize(){




}
