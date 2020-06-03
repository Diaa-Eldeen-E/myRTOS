/*
 * thread.c
 *
 *  Created on: Jun 2, 2020,	Replacement for list.c created on Mar 24, 2020
 *      Author: Diaa Eldeen
 */


#include "thread.h"


queue_t readyQueues[PRIORITY_LEVELS];
queue_t xTimeOutList;

OSThread_t* pxRunning = NULL;
OSThread_t* pxNext = NULL;



OSThread_t idleThread;
uint64_t idleStack[40];
void OS_idleThread(){

	OS_onIdle();

	while(1){

	}
}



void OS_threadScheduleNext() {

	// Determine the highest priority non-empty queue in the ready pxQueue
	uint32_t i=0;
	for(i=0; i<PRIORITY_LEVELS; ++i)
		if(readyQueues[i].ui32NoOfItems > 0)
			break;

	ASSERT_TRUE(i < PRIORITY_LEVELS);

	pxNext = OS_queuePopThread(&readyQueues[i], readyQueues[i].xHead.pxNext);
}



//
	// another argument to be added
void OS_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty) {

	static uint32_t ui32NoOfThreads =0;

	ASSERT_TRUE(me != NULL && sp != NULL);
	ASSERT_TRUE(ui32Priorty < PRIORITY_LEVELS);	// Check priority value
	ASSERT_TRUE(ui32StkSize % 8 == 0);	// check stack alignment
	ASSERT_TRUE_WARN(ui32StkSize > sizeof(uint32_t) * 40);

	// set SP to the right point
	// divide SP by 8 and multiply by 8 for 8-byte stack alignment
	// 18 for FPU auto context (s0-s15 + FPSCR + aligner)
	sp = (uint32_t*) ( ((uint32_t)sp / 8 * 8 ) + ui32StkSize - (18*4* FPU_ENABLED) );


	*(--sp) = (1U << 24);	//thumb bit state xPSR
	*(--sp) = (uint32_t) me->OSThreadHandler ;	//PC
	// for debugging purposes
	*(--sp) = 0xEU;	//LR
	*(--sp) = 0xCU;	//R12
	*(--sp) = 0x3U;	//R3
	*(--sp) = 0x2U;	//R2
	*(--sp) = 0x1U;	//R1
	*(--sp) = 0x0U;	//R0

	sp -= 16 * FPU_ENABLED;	//fpu manual context (s16-s31)

	*(--sp) = 0xBU;	//R11
	*(--sp) = 0xAU;	//R10
	*(--sp) = 0x9U;	//R9
	*(--sp) = 0x8U;	//R8
	*(--sp) = 0x7U;	//R7
	*(--sp) = 0x6U;	//R6
	*(--sp) = 0x5U;	//R5
	*(--sp) = 0x4U;	//R4

	*(--sp) = 0x3U;	//control
	*(--sp) = 0xFFFFFFED | ((!FPU_ENABLED) << 4);	//Exception return,	( (fpu/no-fpu), non prev, psp)


	me->sp = sp;
	me->ui32ThreadID = ui32NoOfThreads++;
	me->ui32Priority = ui32Priorty;
	me->ui32TimeOut = 0;

	OS_queuePushThread(&readyQueues[ui32Priorty], me);
}




// ------------------------------- Queues API ------------------------------- //


// Remove the OSThread item from the pxQueue and return it
OSThread_t* OS_queuePopThread(queue_t* pxQueue, OSThread_t* OSThread) {

	ASSERT_TRUE(pxQueue != NULL && OSThread != NULL);
	ASSERT_TRUE(pxQueue->ui32NoOfItems >= 1);

	OSThread->pxNext->pxPrev = OSThread->pxPrev;
	OSThread->pxPrev->pxNext = OSThread->pxNext;

	// If the tail thread is removed, update the tail to point the new last one
	if(pxQueue->pxTail == OSThread)
		pxQueue->pxTail = OSThread->pxPrev;

	pxQueue->ui32NoOfItems--;

	return OSThread;
}



// Push the OSThread to the end of the pxQueue
 void OS_queuePushThread(queue_t* pxQueue, OSThread_t* OSThread) {

	 ASSERT_TRUE(pxQueue != NULL && OSThread != NULL);

	 if(pxQueue->ui32NoOfItems == 0)
		 pxQueue->pxTail->pxPrev = OSThread;

	 pxQueue->pxTail->pxNext = OSThread;
	 OSThread->pxNext = &pxQueue->xHead;
	 OSThread->pxPrev = pxQueue->pxTail;
	 pxQueue->pxTail = OSThread;

	 pxQueue->ui32NoOfItems++;
}


// Initialize the OS queues (ready queues and wait queues)
void OS_threadQueuesInit() {

	uint32_t i;
	for(i=0; i<PRIORITY_LEVELS; ++i) {		// Loop over ready queues
		readyQueues[i].ui32NoOfItems = 0;
		readyQueues[i].xHead.pxNext = &readyQueues[i].xHead;
		readyQueues[i].xHead.pxPrev = &readyQueues[i].xHead;
		readyQueues[i].pxTail = &readyQueues[i].xHead;
	}

	xTimeOutList.ui32NoOfItems = 0;
	xTimeOutList.xHead.pxNext = &xTimeOutList.xHead;
	xTimeOutList.xHead.pxPrev = &xTimeOutList.xHead;
	xTimeOutList.pxTail = &xTimeOutList.xHead;
}

