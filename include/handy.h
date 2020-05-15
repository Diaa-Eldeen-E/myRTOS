/*
 * handy.h
 *
 *  Created on: Jun 27, 2019
 *      Author: DiaaEldeen
 */

#ifndef HANDY_H_
#define HANDY_H_

#include "TM4C1294NCPDT.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//no port I or O
//k -> Q normal
//A -> J AHB

#define LED1_ON		GPION_DATA(P1) = P1;
#define LED1_OFF	GPION_DATA(P1) = 0;
#define LED2_ON		GPION_DATA(P0) = P0;
#define LED2_OFF	GPION_DATA(P0) = 0;
#define LED3_ON		GPIOF_DATA(P4) = P4;
#define LED3_OFF	GPIOF_DATA(P4) = 0;
#define LED4_ON		GPIOF_DATA(P0) = P0;
#define LED4_OFF	GPIOF_DATA(P0) = 0;

#define GPIO_button		GPIOJ_AHB

#define TIMER_delay_bit	BIT7
#define TIMER_delay		TIMER7

extern uint32_t F_SysClk;

#define ms_LOAD_VALUE	F_SysClk / 2000		//2 for prescaler, and 1000 for ms
#define us_LOAD_VALUE	F_SysClk / 1000000


void clock_setup_MO(uint8_t freq);
void clock_setup_PIO(uint8_t freq);

void blink_EK_LED();
void LEDs_EK_setup();
void buttons_EK_setup();

char* reverse(char* src,uint8_t length);
void itoa(int32_t num, char* str, uint32_t base);
void utoa(uint32_t num, char* str, uint32_t base);
void ftoa (float num,uint8_t precision,char *str);	//requires 2048 stack size
void delay_ms(uint_fast16_t n);
void delay_us(uint_fast16_t n);


#endif /* HANDY_H_ */




/*

  void SystemInit(void)
{
	// Grant coprocessor access
	// This is required since TM4C123G has a floating point coprocessor
	//NVIC_CPAC_R |= 0x00F00000;
	SCB->CPACR |= 0x00F00000;
}

 */
