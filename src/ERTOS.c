/*
 * ERTOS.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


#include "ERTOS.h"



extern volatile queue_t readyQueues[PRIORITY_LEVELS];
extern volatile queue_t xTimeOutList;
extern OSThread_t* volatile pxRunning;
extern OSThread_t* volatile pxNext;

extern OSThread_t idleThread;

volatile uint32_t ui32SysTicks = 0;



/* This function puts the running thread in the waiting queue for
 * a period of time equal to ui32Ticks * TICK_PERIOD_MS
 */
void OS_delay(uint32_t ui32Ticks) {

	DISABLE_IRQ;

	// Calling OS_delay from idle thread causes  error
	ASSERT_TRUE(pxRunning->ui32ThreadID != 0);

	if(ui32Ticks > 0) {
		// Put the thread in the time out (waiting) queue
		pxRunning->ui32TimeOut = ui32Ticks;
		OS_queuePushThread(&xTimeOutList, pxRunning);
	}

	// Switch context from this thread
	OS_yield();
	ENABLE_IRQ;
}


/* This function is called every SysTick timer interrupt to keep track
 * the time (ticks), monitor the time out threads and make they ready
 *  when their time out is over
 */
void OS_tick() {

	++ui32SysTicks;

	if(xTimeOutList.ui32NoOfItems == 0)
		return;

	// Reduce the time out of all tasks in the time out list
	OSThread_t* pxIter = xTimeOutList.xHead.pxNext;
	do {


		if(pxIter->ui32TimeOut > 0) {

			pxIter->ui32TimeOut--;

			if(pxIter->ui32TimeOut == 0) {
				//  Move the thread from the waiting list to the ready list
				OSThread_t* pxRdyThread = OS_queuePopThread(&xTimeOutList, pxIter);
				pxIter = pxIter->pxNext;
				OS_queuePushThread(&readyQueues[pxRdyThread->ui32Priority], pxRdyThread);
			}
			else
				pxIter = pxIter->pxNext;

		}
		else
			ASSERT_TRUE(0);	// Error, this should never be executed

	}while(pxIter->ui32ThreadID != 0xffffffff);	// The queue head dummy thread

}



// Operating system timer handler
void SysTick_Handler() {


	SysTick->CTRL |= 1;		// Clear the flag and Start counting again
	OS_tick();

	/* Context switch to the next ready tread when the current thread time slot ends
	 * if a context switch is already pending skip this context switch
	 */
	if(ui32SysTicks % TIME_SLOT == 0 && (!(SCB->ICSR & BIT28))) {

		DISABLE_IRQ;

		// Insert the last running thread back into the ready list before switching
		if(pxRunning != NULL)
			OS_queuePushThread(&readyQueues[pxRunning->ui32Priority], pxRunning);

		OS_threadScheduleNext();
		PEND_SV;
		ENABLE_IRQ;
	}
}



// This functions starts the OS
void OS_run() {

	SysTick->CTRL |= BIT0;	// Start SysTick

	__set_BASEPRI(0);	// Enable all interrupts

	// Change thread mode to unprivileged and use PSP stack
	__asm(" mrs	r0,	msp");
	__asm(" msr psp, r0");
	__asm(" mrs r0,	control");	// load control
	__asm(" orr r0, r0, #3");	// Unprivileged thread mode, PSP
	__asm(" msr control, r0");	// save in control
	__asm("  isb");

	while(1);
}



// This function manages SVC commands in the OS
void SVC_HandlerMain(uint32_t* sp) {

	// Retrieve the SVCall number from the program counter
	uint8_t ui8SVCNo = *((uint32_t*)((uint32_t) sp[6] - 2));

	uint32_t ui32Status;

	if(ui8SVCNo == 1) {
		// Fetch the fifth argument passed through r12
		uint32_t uiR12;
		__ASM volatile ("mov %0, r12"  : "=r" (uiR12) );

		OS_threadCreate((OSThread_t*) sp[0], (OSThreadHandler_t) sp[1],  \
							(uint32_t*) sp[2], sp[3], uiR12);

	} else if(ui8SVCNo == 2) {
		OS_delay(sp[0]);

	} else if(ui8SVCNo == 3) {
		OS_yield();

	}  else if(ui8SVCNo == 10) {
		OS_mutexCreate((mutex_t*) sp[0]);

	} else if(ui8SVCNo == 11) {
		ui32Status = OS_mutexLock((mutex_t*) sp[0]);

	} else if(ui8SVCNo == 12) {
		ui32Status = OS_mutexRelease((mutex_t*) sp[0]);

	} else if(ui8SVCNo == 13) {
		OS_semaphoreCreate((semaphore_t*) sp[0], sp[1]);

	} else if(ui8SVCNo == 14) {
		ui32Status = OS_semaphoreTake((semaphore_t*) sp[0]);

	} else if(ui8SVCNo == 15) {
		ui32Status = OS_semaphoreGive((semaphore_t*) sp[0]);

	}else {
		while(1);
	}

	sp[0] = ui32Status;	// return value (r0)
}




void OS_init(uint32_t* sp, uint32_t stkSize) {

	__set_BASEPRI(1);	//Disable all interrupts except SVC

	// OS interrupt priorities
	__NVIC_SetPriority(SVCall_IRQn, 0);
	__NVIC_SetPriority(SysTick_IRQn, 2);
	__NVIC_SetPriority(PendSV_IRQn, 7);	// PendSV must be set to the lowest possible priority

	// SysTick configuration
	SysTick->CTRL |=  BIT1;	// Enable SysTick interrupt (4MHz clock)
	ASSERT_TRUE(TICK_PERIOD_MS > 0 && (TICK_PERIOD_MS <  0xffffff / (4000000UL/1000) ));
	SysTick->LOAD = (TICK_PERIOD_MS * (4000000UL/1000)) -1;	// (4M / 1000) = 1 ms counts

	OS_threadQueuesInit();

	// Create idle thread
	OS_SVC_threadCreate(&idleThread, &OS_idleThread, sp, stkSize, PRIORITY_LEVELS-1);

}
