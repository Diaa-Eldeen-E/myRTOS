/*
 * thread.h
 *
 *  Created on: Jun 2, 2020,	Replacement for list.h created on Mar 24, 2020
 *      Author: Diaa Eldeen
 */

#ifndef INCLUDE_THREAD_H_
#define INCLUDE_THREAD_H_


#include "ERTOS_config.h"

#include "handy.h"





typedef void (*OSThreadHandler_t)();


typedef struct OSThread_t {
	uint32_t ui32ThreadID;
	uint32_t* sp;
	OSThreadHandler_t OSThreadHandler;
	uint32_t ui32TimeOut;
	uint32_t ui32Priority;

	struct OSThread_t* pxNext;
	struct OSThread_t* pxPrev;

}OSThread_t;


typedef struct queue_t {
	uint32_t ui32NoOfItems;

	OSThread_t* pxTail;
	OSThread_t xHead;

}queue_t;


extern void OS_SVC_threadCreate(OSThread_t* pxThread, OSThreadHandler_t pxThreadHandler, \
					uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty);
extern void OS_onIdle();

void yield();
void OS_threadScheduleNext();
void OS_idleThread();
void OS_threadCreate(OSThread_t* pxThread, OSThreadHandler_t pxThreadHandler,  \
					uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty);



void OS_queueInit(volatile queue_t* pxQueue);
void OS_threadQueuesInit();
void OS_queuePushThread(volatile queue_t* pxQueue, OSThread_t* item);
OSThread_t* OS_queuePopThread(volatile queue_t*  pxQueue, OSThread_t* item);

#endif /* INCLUDE_THREAD_H_ */
