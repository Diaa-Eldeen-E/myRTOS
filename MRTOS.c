/*
 * MRTOS.c
 *
 *  Created on: Oct 9, 2019
 *      Author: DiaaEldeen
 */



#include "MRTOS.h"


extern volatile uint32_t sysTicks;

//volatile must be placed after(*) for the pointer it self to be volatile
//if it's placed before the, the struct would be volatile not  the pointer
OSThread* volatile OS_curr;	//pointer to current thread
OSThread* volatile OS_next;	//pointer to next thread

#define MAX_THREADS		32
OSThread* OS_thread[32+1];
uint8_t cThread;	//counts how many threads we started
uint8_t iThread;	//index of the current working thread
uint32_t OS_thread_rdy;


OSThread idleThread;
void idle_thread(){
	while(1){
		OS_onIdle();
	}
}


//you don't need to disable interrupts for this function because
//it is called from an ISR which cannot be preempted by a thread
//since OS_delay is the only thread that could change the timeout variable so, no race conditions could happen
void OS_tick(){
	uint8_t i=0;
	for(i=1; i<cThread; i++){
		//if the timeout is higher than zero decrement it, if it reached 0 make it ready
		if (OS_thread[i]->timeout > 0) {
			--OS_thread[i]->timeout;
			if (OS_thread[i]->timeout == 0) OS_thread_rdy |= ( 1U << (i-1) );
		}
	}
}

void OS_delay(uint32_t t){
	__disable_irq();
	if (OS_curr == OS_thread[0]) {
		//error, the idle thread should never call OS_delay
		while(1);
	}
	OS_thread[iThread]->timeout = t;
	OS_thread_rdy &=~ (1<<iThread-1);
	OS_sched();	//so the scheduler could switch the context away from that thread so it can be blocked
	__enable_irq();
}


void OS_run(){
	//OS_onStartup();
	//__disable irq();
	//OS_sched();
	//__enable_irq();
	//the following code should never execute
	//error and exit
	__enable_irq();
	while(1){
	}
}


void SysTick_Handler(){
	++sysTicks;
	OS_tick();

	__asm(" cpsid	 i");
	OS_sched();
	__asm(" cpsie	 i");

}

void PendSV_Handler(){
	__nop();
	__asm(
			//the external global variables
			//" .text\n"
			" .global		OS_curr\n"
			".OS_currAddr:\n\t"
			" .word			OS_curr\n"

			" .global		OS_next\n"
			".OS_nextAddr:\n\t"
			" .word			OS_next\n"

			//the function code
			//" .global         PendSV_Handler\n"
			//"PendSV_Handler:\n\t"
			//disable interrupt
			" CPSID         i\n\t"
			//if (OS_curr != (OSThread*)0 )	//in case it's the first call, so there will be no current thread yet
			" ldr        r0, .OS_currAddr\n\t"
			" ldr        r0, [r0]\n\t"
			" cbz        r0,PendSV_restore\n\t"
			//if statement body, saving the context of the previous thread
			" push		{r4-r11}\n\t"
			" ldr        r1, .OS_currAddr\n\t"
			" ldr        r1, [r1]\n\t"
			" str        sp, [r1]\n"

			//restore the context of the next thread
			"PendSV_restore:\n\t"
			" ldr        r0, .OS_nextAddr\n\t"
			" ldr        r0, [r0]\n\t"
			" ldr        sp, [r0]\n\t"
			" ldr        r0, .OS_nextAddr\n\t"
			" ldr        r1, .OS_currAddr\n\t"
			" ldr        r0, [r0]\n\t"
			" str        r0, [r1]\n\t"
			" pop			{r4-r11}\n\t"
			//returning
			" CPSIE		i\n\t"
	);
}





void OS_sched(){

	//round robin with idle thread (with OS_delay)
	if (OS_thread_rdy == 0U)	iThread = 0;	//idle thread
	else {
		//see who is rdy and run it
		do{
			iThread++;
			if (iThread == MAX_THREADS) iThread = 1;

		}while( !(OS_thread_rdy & ( 1U <<(iThread - 1U) ) ) );
	}

	OS_next = OS_thread[iThread];

	if(OS_next != OS_curr) {
		SCB->ICSR |= BIT28;
	}

}

void OS_init(void* stkSto, uint32_t stkSize){

	OSThread_Start(&idleThread, &idle_thread, stkSto, stkSize);

	SysTick->CTRL |= BIT1 | BIT0;	//enable timer and interrupt (4MHz clock)
	SysTick->LOAD = 3999;	//1ms

	SCB->SHP[10] = 0xE0;
	__NVIC_SetPriority(SysTick_IRQn, 6);	//SysTick must have the lowest priorty

}


void OSThread_Start(
		OSThread* me,
		OSThreadHandler thread_handler,
		void* stkSto, uint32_t stkSize) {


	uint32_t* sp = (uint32_t*)((((uint32_t)stkSto + stkSize) / 8)* 8);	//for aligning the stack to 8 bytes
	uint32_t* stk_limit;

	*(--sp) = (1U << 24);	//thumb bit state xPSR
	*(--sp) = (uint32_t) thread_handler ;	//PC
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


	me->sp = sp;

	stk_limit = (uint32_t*) (((((uint32_t)stkSto + 1U) / 8) + 1U)* 8);

	//filling the stack with a value to make it easy in debugging
	for (sp = sp-1U; sp>= stk_limit; --sp) {
		*sp = 0xDEADBEEFU;
	}



	//exclude the first thread (idle thread)
	if (cThread != 0)	OS_thread_rdy |= (1<<(cThread-1));

	if (cThread < MAX_THREADS)	OS_thread[cThread++] = me;



}
