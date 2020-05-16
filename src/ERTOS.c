/*
 * ERTOS.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


#include "ERTOS.h"
static void OS_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty);

extern void OS_onIdle();


list_t readyList[PRIORITY_LEVELS];
list_t* waitingList;
OSThread_t* ptRunning = NULL;	///////NULL
OSThread_t* ptNext = NULL;		///////NULL
uint32_t ui32SysTicks = 0;

uint32_t lrTemp;

OSThread_t idleThread;
uint64_t idleStack[40];
void OS_idleThread(){

	OS_onIdle();

	while(1){

	}
}

//
void SysTick_Handler(){

	++ui32SysTicks;
	SysTick->CTRL |= 1;
//	OS_tick();

	__asm(" cpsid	 i");
	OS_sched();
	__asm(" cpsie	 i");

	SCB->ICSR |= BIT28;	//pendSV
}



void OS_run(){

	OS_sched();

	//load psp with the running thread sp
	__asm("ptNextAddr:		.word	ptNext");
	__asm(" ldr	r0,	ptNextAddr");
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

	ptRunning = ptNext;

	__enable_irq();

}


void OS_sched(){

	//decide the running thread from the ready list
	uint32_t i=0;
	for(i=0; i<PRIORITY_LEVELS; ++i){

		if(readyList[i].ui32NoOfItems > 0)
			break;

	}
	if(i >= PRIORITY_LEVELS)	//No threads found!
		while(1);

	//insert the last running thread back into the ready list before switching
	if(ptRunning != NULL){
		listInsertItemLast(&readyList[ptRunning->ui32Priority], ptRunning);
	}
//	if(ptNext != NULL){
//		listInsertItemLast(&readyList[ptRunning->ui32Priority], ptNext);
//	}

	ptNext = listGetItem(&readyList[i], readyList[i].ptIndex);


}

//add status return
void SVC_HandlerMain(uint32_t* sp){

	uint8_t ui8SVCNo = *((uint32_t*)((uint32_t) sp[6] - 2));//[-2];

	if(ui8SVCNo == 0){
		OS_run();
	}
	else if(ui8SVCNo == 1){

		OS_threadCreate((OSThread_t*)sp[0],(uint32_t*) sp[1], sp[2], sp[3]);	//another argument to be added
	}

	else{
		while(1);
	}




}
//
	//another argument to be added
static void OS_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty){

	static uint32_t ui32NoOfThreads =0;

	//check stack alignment
	if(ui32StkSize % 8 !=0)
		while(1);

	//set sp to the right point
	sp = (uint32_t*) ((uint32_t)sp + ui32StkSize - (18*4* FPU_ENABLED));	//18 for fpu auto context (s0-s15 + FPSCR + aligner)


	*(--sp) = (1U << 24);	//thumb bit state xPSR
	*(--sp) = (uint32_t) me->OSThreadHandler ;	//PC
	//for debugging purposes
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

//	me->item.ui32ThreadID = ui32NoOfThreads++;

	listInsertItem(&readyList[ui32Priorty], me);




}

//#pragma FUNC_ALWAYS_INLINE(OS_SVC_threadCreate)
////another argument to be added
//__always_inline __inline void OS_SVC_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty){
//
//	__asm(" svc	#1");
//	__asm(" bx	lr");
//}





void OS_init(uint32_t* sp, uint32_t stkSize){

	//initialize lists (ready list, waiting list)
	uint32_t i;
	for(i=0; i<PRIORITY_LEVELS; ++i){
		listInit(&readyList[i]);
	}

	//create idle thread
	idleThread.OSThreadHandler = &OS_idleThread;

	SysTick->CTRL |= BIT1 | BIT0;	//enable timer and interrupt (4MHz clock)
	SysTick->LOAD = 3999;	//1ms

	__NVIC_SetPriority(SVCall_IRQn, 0);
	__NVIC_SetPriority(SYSCTL_IRQn, 4);
	__NVIC_SetPriority(PendSV_IRQn, 7);

	__NVIC_EnableIRQ(SVCall_IRQn);	//
	__enable_irq();	//

	OS_SVC_threadCreate(&idleThread, sp, stkSize, PRIORITY_LEVELS-1);


	__NVIC_EnableIRQ(SYSCTL_IRQn);

	__NVIC_EnableIRQ(PendSV_IRQn);

//	__enable_irq();
}
