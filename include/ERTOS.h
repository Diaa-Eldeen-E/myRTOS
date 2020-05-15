/*
 * ERTOS.h
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */

#ifndef ERTOS_H_
#define ERTOS_H_



#include "ERTOS_config.h"
#include "list.h"






//typedef struct {
//	uint32_t ui32ThreadID;
//
//	uint32_t* sp;
//
//	OSThreadHandler_t OSThreadHandler;
//
//	listItem_t item;
//
//	uint32_t ui32TimeOut;
//
//	uint32_t ui32Priority;
//
//}OSThread_t;

extern void OS_SVC_run();

void OS_run();
void OS_sched();
static OS_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty);
void OS_SVC_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty);
void OS_init();

#endif /* ERTOS_H_ */
