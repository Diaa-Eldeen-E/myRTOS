/*
 * main.c
 */



#include "handy.h"
#include "UART.h"
#include "MRTOS.h"



uint32_t F_SysClk = 80000000;


volatile uint32_t sysTicks =0;






uint32_t stack1[40];
OSThread blinky1;
void main1(){

	while(1){
		LED1_ON
		delay_ms(500);
		LED1_OFF
		delay_ms(500);
	}
}


uint32_t stack2[40];
OSThread blinky2;
void main2(){
	while(1){
		LED2_ON
		delay_ms(1000);
		LED2_OFF
		delay_ms(1000);
	}
}

uint32_t stack3[40];
OSThread blinky3;
void main3(){
	while(1){
		LED3_ON
		delay_ms(250);
		LED3_OFF
		delay_ms(250);
	}
}

uint32_t stack4[40];
OSThread UART;
void main4(){
	while(1){
		UART_send_stringL("Hello");
		delay_ms(500);
	}
}



int main(void) {

	clock_setup(80);
	LEDs_EK_setup();
	UART_initialize_polling();

	OS_init();



	OSThread_Start(&blinky1, &main1, stack1, sizeof(stack1));
	OSThread_Start(&blinky2, &main2, stack2, sizeof(stack2));
	OSThread_Start(&blinky3, &main3, stack3, sizeof(stack3));
	OSThread_Start(&UART, &main4, stack4, sizeof(stack4));

	OS_run();
}

void TIMER6_Handler(){
	TIMER6->ICR |= BIT0;
}


void GPIOJ_Handler(){
	GPIO_button->ICR |= P0 | P1;
}


void SystemInit(void){
	SCB->CPACR |= 0x00F00000;
}
