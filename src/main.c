/*
 *
 * main.c
 */



#include "handy.h"
#include "UART.h"
#include "ERTOS.h"


uint32_t F_SysClk = 80000000UL;


#define delay_timer	TIMER6
#define delay_timer_bit	BIT6
#define delay_timer_irq	TIMER6A_IRQn
void delay_timer_init();
#define delay_time(duration)	delay_timer->TAILR = (F_SysClk / 1000) * duration


mutex_t xMutUART;


// Thread 1
uint32_t stack1[80];
OSThread_t xblinky1;
void blinky1() {

	while(1) {

		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Hello main 1");
		OS_SVC_semaphoreGive(&xMutUART);
		LED1_ON
		OS_SVC_delay(500);
		LED1_OFF
		OS_SVC_delay(500);
	}
}

// Thread 2
uint32_t stack2[80];
OSThread_t xblinky2;
void blinky2() {

	while(1) {
		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Hello main 2");
		OS_SVC_semaphoreGive(&xMutUART);
		LED2_ON
		OS_SVC_delay(1000);
		LED2_OFF
		OS_SVC_delay(1000);
	}
}

// Thread 3
uint32_t stack3[80];
OSThread_t xblinky3;
void blinky3() {

	while(1) {

		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Hello main 3");
		OS_SVC_semaphoreGive(&xMutUART);

		LED3_ON
		OS_SVC_delay(250);
		LED3_OFF
		OS_SVC_delay(250);
	}
}

// Thread 4
uint32_t stack4[80];
OSThread_t xUART_Msg;
void UART_Msg() {

	while(1) {

		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Hello main 4");
		OS_SVC_semaphoreGive(&xMutUART);

		OS_SVC_delay(500);
	}
}


// Thread 5
uint32_t stack5[80];
OSThread_t xUART_Msg5;
void UART_Msg5() {

	while(1) {

		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Hello main 5");
		OS_SVC_semaphoreGive(&xMutUART);

		OS_SVC_delay(140);
	}
}


// Thread 6
uint32_t stack6[80];
OSThread_t xUART_Msg6;
void UART_Msg6() {

	while(1) {

		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Hello main 6");
		OS_SVC_semaphoreGive(&xMutUART);

		OS_SVC_delay(290);
	}
}

// Thread 7
uint32_t stack7[80];
OSThread_t xUART_Msg7;
void UART_Msg7() {

	while(1) {

		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Hello main 7");
		OS_SVC_semaphoreGive(&xMutUART);

		OS_SVC_delay(370);
	}
}

// Thread 8
uint32_t stack8[80];
OSThread_t xUART_Msg8;
void UART_Msg8() {

	while(1) {

		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Hello main 8");
		OS_SVC_semaphoreGive(&xMutUART);

		uint32_t i=0;
		for(i=0; i<1000000; ++i);
//		OS_SVC_delay(500);
	}
}


// Thread 9 - One shot
uint32_t stack9[80];
OSThread_t xUART_Msg9;
void UART_Msg9() {

		OS_SVC_semaphoreTake(&xMutUART);
		UART_send_stringL("Timer 6 Response");
		OS_SVC_semaphoreGive(&xMutUART);

		OS_SVC_yield();
}



void OS_test(uint32_t pri[]) {

	OS_SVC_threadCreate(&xblinky1, &blinky1, stack1, sizeof(stack1), pri[0]);
	OS_SVC_threadCreate(&xblinky2, &blinky2, stack2, sizeof(stack2), pri[1]);
	OS_SVC_threadCreate(&xblinky3, &blinky3, stack3, sizeof(stack3), pri[2]);
	OS_SVC_threadCreate(&xUART_Msg, &UART_Msg, stack4, sizeof(stack4), pri[3]);
	OS_SVC_threadCreate(&xUART_Msg5, &UART_Msg5, stack5, sizeof(stack5), pri[4]);
	OS_SVC_threadCreate(&xUART_Msg6, &UART_Msg6, stack6, sizeof(stack6), pri[5]);
	OS_SVC_threadCreate(&xUART_Msg7, &UART_Msg7, stack7, sizeof(stack7), pri[6]);
	OS_SVC_threadCreate(&xUART_Msg8, &UART_Msg8, stack8, sizeof(stack8), pri[7]);

	OS_run();
}

// Thread 0 (Idle thread)
void OS_onIdle(){


	while(1) {
		__WFI();	//w8 for interrupts
	}
}




// Program entry point
int main(void) {

	// Hardware initialization
	clock_setup_MOSC(F_SysClk/1000000UL);
	LEDs_EK_setup();
	UART_initialize_polling();
	UART_send_stringL("UART initialized successfully");

	delay_timer_init();
	delay_time(500);
	delay_timer->CTL |= BIT0;

	uint32_t stack0[80];	//idle stack

	OS_init(stack0, sizeof(stack0));

	uint32_t pri[] = {1, 1, 2, 2, 3, 3, 4, 5};

	OS_SVC_semaphoreCreate(&xMutUART, 1);

	OS_test(pri);

}




void delay_timer_init(){

	SYSCTL->RCGCTIMER |= delay_timer_bit;
	delay_us(1);

	delay_timer->CTL =0;	//Disable the timer before config
	delay_timer->CFG = 0x0;		//32-bit timer
	delay_timer->TAMR = 0x1 | BIT4 ;	// one shot, count up
	delay_timer->IMR |= BIT0;

	__NVIC_EnableIRQ(delay_timer_irq);
	__NVIC_SetPriority(delay_timer_irq, 1);
	//__enable_irq();
	delay_timer->CTL |= BIT1 ;	//Enable stall
}

// Used for testing
void TIMER6_Handler(){
	GPIOF_DATA(P0) ^= P0;
	delay_timer->CTL |= BIT0;

	// A solution for using mutex in interrupt but it's not guaranteed
//	if(__LDREXW(&xMutUART.val))
//		UART_send_stringL("Timer 6 Response");
//
//	__CLREX();	// Clear exclusion


	// A better solution for using the UART inside an ISR
	OS_SVC_threadCreate(&xUART_Msg9, &UART_Msg9, stack9, sizeof(stack9), 1);

	delay_timer->ICR |= BIT0;

}


void GPIOJ_Handler(){
	GPIO_button->ICR |= P0 | P1;
}

