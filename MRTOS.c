/*
 * MRTOS.c
 *
 *  Created on: Oct 9, 2019
 *      Author: DiaaEldeen
 */



#include "MRTOS.h"

//volatile must be placed after(*) for the pointer it self to be volatile
//if it's placed before the, the struct would be volatile not  the pointer
OSThread* volatile OS_curr;	//pointer to current thread
OSThread* volatile OS_next;	//pointer to next thread

//for making the systick interrupt inactive inside the pendSV handler
uint32_t* SCB_ptr = (uint32_t*) 0xE000ED24;



//pop 8 registers (systick registers saved) from the stack
__asm(
		//the external global variables
		" .text\n"
		" .global		OS_curr\n"
		".OS_currAddr:\n\t"
		" .word			OS_curr\n"

		" .global		OS_next\n"
		".OS_nextAddr:\n\t"
		" .word			OS_next\n"

		" .global		SCB_ptr\n"
		".SCB_ptrAddr:\n\t"
		" .word			SCB_ptr\n"

		//the function code
		" .global         PendSV_Handler\n"
		"PendSV_Handler:\n\t"
		//disable interrupt
		" CPSID         i\n\t"

		//pop the saved SysTick registers
		" pop		{r0-r3}\n\t"
		" pop		{r0-r3}\n\t"
		" pop		{r0,r1}\n\t"

		//clearing the SysTick active interrupt
		" ldr		r1, .SCB_ptrAddr\n\t"	//$C$CON5
		" ldr		r1, [r1]\n\t"	//dereferencing the pointer
		" ldr       r0, [r1]\n\t"	//[r1, #0x20]
		" bic        r0, r0, #0x800\n\t"
		" str        r0, [r1]\n\t"	//[r1, #0x20]
		//if (OS_curr != (OSThread*)0 )	//in case it's the first call, so there will be no current thread yet
		" ldr        r0, .OS_currAddr\n\t"	//$C$CON4
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
		" mvn		lr, #0x6\n\t"
		"bx			lr");



void OS_sched(){

	if ( OS_curr == &blinky1)
		OS_next = &blinky2;
	else
		OS_next = &blinky1;

	if(OS_next != OS_curr) {
		SCB->ICSR |= BIT28;
	}

}

void OS_init(){

	SysTick->CTRL |= BIT1 | BIT0;	//enable timer and interrupt (4MHz clock)
	SysTick->LOAD = 3999;	//1ms

	__NVIC_SetPriority(SysTick_IRQn,7);
	__enable_irq();
}


void OSThread_Start(
		OSThread* me,
		OSThreadHandler thread_handler,
		void* stkSto, uint32_t stkSize) {

	uint32_t* sp = (uint32_t*)((((uint32_t)stkSto + stkSize) / 8)* 8);	//for alligning the stack to 8 bytes
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

	for (sp = sp-1U; sp>= stk_limit; --sp) {
		*sp = 0xDEADBEEFU;
	}

}
