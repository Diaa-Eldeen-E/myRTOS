/*
 * main.c
 */



#include "handy.h"
#include "UART.h"
#include "ERTOS.h"



uint32_t F_SysClk = 80000000;


volatile uint32_t sysTicks =0;






uint32_t stack1[40];
OSThread_t blinky1;
void main1(){

	while(1){
		UART_send_stringL("Hello main 1");
		LED1_ON
		uint32_t i=0;
		for(i=0; i<1000000; ++i);
//		OS_delay(500);
		LED1_OFF
//		OS_delay(500);


	}
}


uint32_t stack2[40];
OSThread_t blinky2;
void main2(){
	while(1){
		UART_send_stringL("Hello main 2");
		LED2_ON
		uint32_t i=0;
		for(i=0; i<1000000; ++i);
//		OS_delay(1000);
		LED2_OFF
//		OS_delay(1000);

	}
}

uint32_t stack3[40];
OSThread_t blinky3;
void main3(){
	while(1){
		UART_send_stringL("Hello main 3");
		LED3_ON
		uint32_t i=0;
		for(i=0; i<1000000; ++i);
//		OS_delay(250);
		LED3_OFF
//		OS_delay(250);

	}
}

uint32_t stack4[40];
OSThread_t UART;
void main4(){
	while(1){
		UART_send_stringL("Hello main 4");
		uint32_t i=0;
		for(i=0; i<1000000; ++i);
//		OS_delay(500);
	}
}



#define delay_timer	TIMER6
#define delay_timer_bit	BIT6
#define delay_timer_irq	TIMER6A_IRQn

void delay_timer_init(){

	SYSCTL->RCGCTIMER |= delay_timer_bit;
	delay_us(1);

	delay_timer->CTL =0;
	delay_timer->CFG = 0x0;
	delay_timer->TAMR = 0x1 | BIT4 ;	//count up, one shot
	delay_timer->IMR |= BIT0;
	__NVIC_EnableIRQ(delay_timer_irq);
	//__enable_irq();
	delay_timer->CTL |= BIT1 ;
}

#define delay_time(duration)	delay_timer->TAILR = (F_SysClk / 1000) * duration



uint32_t stack0[40];	//idle stack

void OS_onIdle(){
	while(1){
	//UART_send_stringL("Idle");
		__wfi();	//w8 for interrupts
	}
}


int main(void) {

	clock_setup_MO(F_SysClk/1000000UL);
	LEDs_EK_setup();
	UART_initialize_polling();

	OS_init(stack0, sizeof(stack0));

//	delay_timer_init();

//	delay_time(500);
//	delay_timer->CTL |= BIT0;


//	OSThread_Start(&blinky1, &main1, stack1, sizeof(stack1));
//	OSThread_Start(&blinky2, &main2, stack2, sizeof(stack2));
//	OSThread_Start(&blinky3, &main3, stack3, sizeof(stack3));
//	OSThread_Start(&UART,  &main4, stack4, sizeof(stack4));

	blinky1.OSThreadHandler = &main1;
	blinky2.OSThreadHandler = &main2;
	blinky3.OSThreadHandler = &main3;
	UART.OSThreadHandler = &main4;
	OS_SVC_threadCreate(&blinky1, stack1, sizeof(stack1), 1);
	OS_SVC_threadCreate(&blinky2, stack2, sizeof(stack2), 1);
	OS_SVC_threadCreate(&blinky3, stack3, sizeof(stack3), 3);
	OS_SVC_threadCreate(&UART, stack4, sizeof(stack4), 4);

	OS_run();
}




void TIMER6_Handler(){
	GPIOF_DATA(P0) ^= P0;
	delay_timer->CTL |= BIT0;

	int i=0;	//weird!!, the LED doesn't toggle without it


	delay_timer->ICR |= BIT0;

}


void GPIOJ_Handler(){
	GPIO_button->ICR |= P0 | P1;
}

//useless here, it's used in keil
void SystemInit(void){
	SCB->CPACR |= 0x00F00000;
}
