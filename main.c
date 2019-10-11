/*
 * main.c
 */



#include "handy.h"
#include "UART.h"
#include "MRTOS.h"

uint32_t F_SysClk = 80000000;


volatile uint32_t sysTicks =0;

void delay(uint32_t t){

	uint32_t begin = sysTicks;
	while(sysTicks - begin < t );
}


uint32_t stack1[40];
OSThread blinky1;

uint32_t stack2[40];
OSThread blinky2;

void main1(){

	while(1){
		LED1_ON
		delay_ms(500);
		LED1_OFF
		delay_ms(500);
	}
}

void main2(){
	while(1){
		LED2_ON
		delay_ms(500);
		LED2_OFF
		delay_ms(500);
	}
}






int main(void) {

	clock_setup(80);
	LEDs_EK_setup();
	OS_init();

//
	OSThread_Start(&blinky1, &main1, stack1, sizeof(stack1));
	OSThread_Start(&blinky2, &main2, stack2, sizeof(stack2));

	while(1)
	{

	}
}

void SysTick_Handler(){
	//int read = SysTick->CTRL; 	//clear the COUNT bit by reading the register

	++sysTicks;

	__disable_irq();
	OS_sched();
	//__enable_irq();
	__asm("  cpsie i");
}


void GPIOJ_Handler()
{

	GPIO_button->ICR |= P0 | P1;
}

