/*
 * ERTOS.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


#include "ERTOS.h"
#include "thread.h"



extern queue_t readyQueues[PRIORITY_LEVELS];
extern OSThread_t* pxRunning;
extern OSThread_t* pxNext;

extern OSThread_t idleThread;

extern queue_t xTimeOutList;


volatile uint32_t ui32SysTicks = 0;

uint32_t svcEXEReturn;



void OS_delay(uint32_t ui32Ticks) {

	DISABLE_IRQ;

	// Calling OS_delay from idle thread causes  error
	ASSERT_TRUE(pxRunning->ui32ThreadID != 0);

	OS_threadScheduleNext();

	pxRunning->ui32TimeOut = ui32Ticks;
	OS_queuePushThread(&xTimeOutList, pxRunning);
	PEND_SV;
	ENABLE_IRQ;
}


void OS_tick() {

	++ui32SysTicks;

	// Iterate over all tasks in the time out list
	OSThread_t* pxIter = &xTimeOutList.xHead;
	uint32_t i=0;
	for(i=0; i<xTimeOutList.ui32NoOfItems; ++i){

		pxIter = pxIter->pxNext;

		// Reduce the time out period and when it is over move the task to the ready list
		if(pxIter->ui32TimeOut > 0) {

			pxIter->ui32TimeOut--;
			if(pxIter->ui32TimeOut == 0) {

				OSThread_t* pxRdyThread = OS_queuePopThread(&xTimeOutList, pxIter);
				pxIter = pxIter->pxNext;
				i++;
				OS_queuePushThread(&readyQueues[pxIter->ui32Priority], pxRdyThread);
			}
		}
		else {
			OSThread_t* pxRdyThread = OS_queuePopThread(&xTimeOutList, pxIter);
			pxIter = pxIter->pxNext;
			i++;
			OS_queuePushThread(&readyQueues[pxIter->ui32Priority], pxRdyThread);
		}

	}

}



// Operating system timer handler
void SysTick_Handler() {

	SysTick->CTRL |= 1;		// Clear the flag and Start counting again
	OS_tick();

	// Switch to the next ready task when the time slot ends
	if(ui32SysTicks % TIME_SLOT == 0) {

		DISABLE_IRQ;

		// Insert the last running thread back into the ready list before switching
		if(pxRunning != NULL) {
			OS_queuePushThread(&readyQueues[pxRunning->ui32Priority], pxRunning);
		}

		OS_threadScheduleNext();
		PEND_SV;
		ENABLE_IRQ;
	}
}




void OS_run() {

	SysTick->CTRL |= BIT0;	// Start SysTick

	// Change thread mode to unprivileged and use PSP stack
	__asm(" mrs	r0,	msp");
	__asm(" msr psp, r0");
	__asm(" mrs r0,	control");	// load control
	__asm(" orr r0, r0, #3");	// Unprivileged thread mode, PSP
	__asm(" msr control, r0");	// save in control
	__asm("  isb");

	while(1);
}



//add status return
void SVC_HandlerMain(uint32_t* sp) {

	uint8_t ui8SVCNo = *((uint32_t*)((uint32_t) sp[6] - 2));


	if(ui8SVCNo == 1) {
		// Fetch the fifth argument passed through r12
		uint32_t uiR12;
		__ASM volatile ("mov %0, r12"  : "=r" (uiR12) );

		OS_threadCreate((OSThread_t*) sp[0], (OSThreadHandler_t) sp[1],  \
							(uint32_t*) sp[2], sp[3], uiR12);

	} else if(ui8SVCNo == 2) {
		OS_delay(sp[0]);

	} else {
		while(1);
	}

}




void OS_init(uint32_t* sp, uint32_t stkSize) {

	// OS interrupt priorities
	__NVIC_SetPriority(SVCall_IRQn, 0);
	__NVIC_SetPriority(SysTick_IRQn, 4);
	__NVIC_SetPriority(PendSV_IRQn, 7);

	OS_threadQueuesInit();

	// Create idle thread
	OS_SVC_threadCreate(&idleThread, &OS_idleThread, sp, stkSize, PRIORITY_LEVELS-1);

	// SysTick configuration
	SysTick->CTRL |=  BIT1;	// Enable SysTick interrupt (4MHz clock)
	ASSERT_TRUE(TICK_PERIOD_MS > 0 && (TICK_PERIOD_MS <  0xffffff / (4000000UL/1000) ));
	SysTick->LOAD = (TICK_PERIOD_MS * (4000000UL/1000)) -1;	// (4M / 1000) = 1 ms counts

	//ENABLE_IRQ;

}
