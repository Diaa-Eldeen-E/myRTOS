/*
 * ERTOS.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


#include "ERTOS.h"


extern void OS_onIdle();


list_t readyList[PRIORITY_LEVELS];
list_t* waitingList;

OSThread_t idleThread;
uint64_t idleStack[40];
void OS_idleThread(){

	OS_onIdle();

	while(1){

	}
}


void SysTick_Handler(){
//	++sysTicks;
//	OS_tick();
//
//	__asm(" cpsid	 i");
//	OS_sched();
//	__asm(" cpsie	 i");

}


void OS_run(){




	while(1){

	}
}

//add status return
void SVC_HandlerMain(uint32_t* sp){

	uint32_t ui32SVCNo = ((uint32_t*) sp[6])[-2];

	//if(1) -> thread create
	if(ui32SVCNo == 1){

		OS_threadCreate((OSThread_t*)sp[0],(uint32_t*) sp[1], sp[2], sp[3]);	//another argument to be added
	}

	else{
		while(1);
	}




}

	//another argument to be added
static OS_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty){

	static uint32_t ui32NoOfThreads =0;

	//check stack alignment

	//set sp to the right point
	sp = (uint32_t*)((((uint32_t)sp + ui32StkSize  ) / 8)* 8);	//for aligning the stack to 8 bytes


	uint32_t* stk_limit;

	*(--sp) = (1U << 24);	//thumb bit state xPSR
	*(--sp) = (uint32_t) me->OSThreadHandler ;	//PC
	//for debugging purposes
	*(--sp) = 0xEU;	//LR
	*(--sp) = 0xCU;	//R12
	*(--sp) = 0x3U;	//R3
	*(--sp) = 0x2U;	//R2
	*(--sp) = 0x1U;	//R1
	*(--sp) = 0x0U;	//R0

	*(--sp) = 0xBU;	//R11
	*(--sp) = 0xAU;	//R10
	*(--sp) = 0x9U;	//R9
	*(--sp) = 0x8U;	//R8
	*(--sp) = 0x7U;	//R7
	*(--sp) = 0x6U;	//R6
	*(--sp) = 0x5U;	//R5
	*(--sp) = 0x4U;	//R4

	*(--sp) = 0x4U;	//control
	*(--sp) = 0x4U;	//EXCRETURN


	stk_limit = (uint32_t*) (((((uint32_t)sp + 1U) / 8) + 1U)* 8);

	//filling the stack with a value to make it easy in debugging
	for (sp = sp-1U; sp>= stk_limit; --sp) {
		*sp = 0xDEADBEEFU;
	}




	me->sp = sp;
	me->ui32ThreadID = ui32NoOfThreads;
	me->ui32Priority = ui32Priorty;
	me->ui32TimeOut = 0;

	me->item->ui32ThreadID = ui32NoOfThreads++;

	listInsertItem(readyList, me->item);




}

//another argument to be added
void OS_SVC_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty){

	asm(" svc	#1");
	asm(" bx	lr");
}





void OS_init(uint32_t* sp, uint32_t stkSize){

	//initialize lists (ready list, waiting list)
	listInit(readyList);
	//create idle thread
	idleThread.OSThreadHandler = &OS_idleThread;
	OS_SVC_threadCreate(&idleThread, sp, stkSize, PRIORITY_LEVELS-1);

	//set current working thread to the idle thread



}
