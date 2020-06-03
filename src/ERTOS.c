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
extern uint64_t idleStack[40];

extern queue_t xTimeOutList;


volatile uint32_t ui32SysTicks = 0;

uint32_t lrTemp;






void OS_delay(uint32_t ui32Ticks) {

	DISABLE_IRQ;

	// Calling OS_delay from idle thread causes  error
	ASSERT_TRUE(pxRunning->ui32ThreadID != 0);

	OS_threadScheduleNext();

	pxRunning->ui32TimeOut = ui32Ticks;
	OS_threadQueuePush(&xTimeOutList, pxRunning);
	PEND_SV;
	ENABLE_IRQ;
}


void OS_tick() {

	++ui32SysTicks;

	// Iterate over all tasks in the time out list
	OSThread_t* iter = &xTimeOutList.xHead;
	uint32_t i=0;
	for(i=0; i<xTimeOutList.ui32NoOfItems; ++i){

		iter = iter->pxNext;

		// Reduce the time out period and when it is over move the task to the ready list
		if(iter->ui32TimeOut > 0) {
			iter->ui32TimeOut--;
			if(iter->ui32TimeOut == 0)
				OS_threadQueuePush(&readyQueues[iter->ui32Priority], OS_threadQueuePop(&xTimeOutList, iter));

		} else {
			OS_threadQueuePush(&readyQueues[iter->ui32Priority], OS_threadQueuePop(&xTimeOutList, iter));
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
			OS_threadQueuePush(&readyQueues[pxRunning->ui32Priority], pxRunning);
		}

		OS_threadScheduleNext();
		PEND_SV;
		ENABLE_IRQ;
	}
}




void OS_run() {

	OS_threadScheduleNext();

	//load psp with the running thread sp
	__asm("pxNextAddr:		.word	pxNext");
	__asm(" ldr	r0,	pxNextAddr");
	__asm(" ldr   r0,	[r0]");
	__asm(" ldr   r0, [r0, #4]");
	if(FPU_ENABLED)
		__asm(" add   r0, #104");	//10 (r3-r11 + lr , control) + 16 (s16-s31)
	else
		__asm(" add   r0, #40");
	__asm(" msr psp, r0");

	lrTemp = 0xFFFFFFED | ((!FPU_ENABLED) << 4);	//exception Return	( (fpu/no-fpu), non privilege, psp)

	//change thread access to non-privilege
	__asm(" mrs r0,	control");	//load control
	__asm(" orr r0, r0, #3");
	__asm(" msr control, r0");	//save in control
	__asm("  isb");

	pxRunning = pxNext;

	ENABLE_IRQ;
}



//add status return
void SVC_HandlerMain(uint32_t* sp) {

	uint8_t ui8SVCNo = *((uint32_t*)((uint32_t) sp[6] - 2));

	if(ui8SVCNo == 0) {
		OS_run();

	} else if(ui8SVCNo == 1) {

		OS_threadCreate((OSThread_t*)sp[0],(uint32_t*) sp[1], sp[2], sp[3]);	//another argument to be added

	} else if(ui8SVCNo == 2) {
		OS_delay(sp[0]);

	}

	else {
		while(1);
	}


}




void OS_init(uint32_t* sp, uint32_t stkSize) {

	OS_threadQueuesInit();

	// Create idle thread
	idleThread.OSThreadHandler = &OS_idleThread;

	// SysTick configuration
	SysTick->CTRL |= BIT0 | BIT1;	// Enable timer and interrupt (4MHz clock)
	ASSERT_TRUE(TICK_PERIOD_MS > 0 && (TICK_PERIOD_MS <  0xffffff / (4000000UL/1000) ));
	SysTick->LOAD = (TICK_PERIOD_MS * (4000000UL/1000)) -1;	// (4M / 1000) = 1 ms counts

	__NVIC_SetPriority(SVCall_IRQn, 0);
	__NVIC_SetPriority(SYSCTL_IRQn, 4);
	__NVIC_SetPriority(PendSV_IRQn, 7);

	__NVIC_EnableIRQ(SVCall_IRQn);	//

	//ENABLE_IRQ;

	OS_SVC_threadCreate(&idleThread, sp, stkSize, PRIORITY_LEVELS-1);


	__NVIC_EnableIRQ(SYSCTL_IRQn);

	__NVIC_EnableIRQ(PendSV_IRQn);
}
