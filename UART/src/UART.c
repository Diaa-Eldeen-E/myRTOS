/*
 * UART.c
 *
 *  Created on: Jul 7, 2019
 *      Author: DiaaEldeen
 */


#include "UART.h"


void UART_initialize_polling()
{
	SYSCTL->RCGCUART  |= BIT0;	//enable clock to UART0
	SYSCTL->RCGCGPIO  |= BIT0;	//enable clock for portA (UART0 pins ->PA0,PA1)
	delay_us(1);

	GPIOA_AHB->DEN |= BIT0 | BIT1;
	GPIOA_AHB->AMSEL &= ~(BIT0 | BIT1);	//disable analog functions of the port pins 0,1
	GPIOA_AHB->AFSEL |= BIT0 | BIT1;	//enable alternate function for PA0, PA1 (Rx,Tx)
	GPIOA_AHB->PCTL  |= 0x00000011;	//each pin uses 1 digit in the hexadecimal

	UART0->CTL = 0;		//clear UART enable before changing the CTL register
	//BAUD RATE. a write operation to LCRH is required for this setting to take effect
	float BR = (float) F_SysClk / (16 * BAUD_RATE);
	uint32_t IBR = (uint32_t) BR;
	UART0->IBRD = IBR;
	UART0->FBRD	= (uint32_t) ((float)( BR - IBR) * 64 + 0.5);

	UART0->LCRH	|= BIT5 | BIT6;		//1 stop bit, 8 bits char, no parity
	UART0->CC =0;	//use system clock as clock source

	UART0->CTL	|= BIT0 | BIT8 | BIT9;	//UARTEN, TXE, RXE -> set those after config
	delay_us(5);
}

void UART_initialize_interrupts()
{
	SYSCTL->RCGCUART  |= BIT0;	//enable clock to UART0
	SYSCTL->RCGCGPIO  |= BIT0;	//enable clock for portA (UART0 pins ->PA0,PA1)
	delay_us(1);

	GPIOA_AHB->DEN |= BIT0 | BIT1;
	GPIOA_AHB->AMSEL &= ~(BIT0 | BIT1);	//disable analog functions of the port pins 0,1
	GPIOA_AHB->AFSEL |= BIT0 | BIT1;	//enable alternate function for PA0, PA1 (Rx,Tx)
	GPIOA_AHB->PCTL  |= 0x00000011;	//each pin uses 1 digit in the hexadecimal

	UART0->CTL = 0;		//clear UART enable before changing the CTL register

	//BAUD RATE. a write operation to LCRH is required for this setting to take effect
	float BR = (float) F_SysClk / (16 * BAUD_RATE);
	uint32_t IBR = (uint32_t) BR;
	UART0->IBRD = IBR;
	UART0->FBRD	= (uint32_t) ((float)( BR - IBR) * 64 + 0.5);

	UART0->LCRH	|= BIT5 | BIT6;		//1 stop bit, 8 bits char, no parity
	UART0->CC =0;	//use system clock as clock source

	UART0->IM  |= BIT4;	//enable receive interrupt

	UART0->CTL	|= BIT0 | BIT8 | BIT9;	//UARTEN, TXE, RXE -> set those after config

	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_SetPriority(UART0_IRQn, 3);	//set the int priority to 3
	delay_us(5);
}




void UART_send_char(unsigned char DATA)
{
	while( UART0->FR & BIT5 );		//polling until TX FIFO full goes low
	UART0->DR = DATA;
}


void UART_send_string(char* str)
{
	while(*str)	UART_send_char(*str++);
}


void UART_send_stringL(char* str)
{
	while(*str)	UART_send_char(*str++);
	UART_send_string(newLine);
}


void UART_send_intL(int32_t number)
{
	UART_send_int(number);
	UART_send_string(newLine);
}


void UART_send_uintL(uint32_t number)
{
	UART_send_int(number);
	UART_send_string(newLine);
}


void UART_send_int(int32_t number)
{
	char intNum_String [12];
	itoa(number,intNum_String,10);
	UART_send_string(intNum_String);
}

void UART_send_uint(uint32_t number)
{
	char intNum_String [11];
	utoa(number,intNum_String,10);
	UART_send_string(intNum_String);
}


//max precision is 6, max digits including (.) is 11
void UART_send_float(double number , uint8_t precision)
{
	char doubleNum_String [12];
	ftoa(number, precision ,doubleNum_String);
	UART_send_string(doubleNum_String);
}



uint32_t UART_receive()
{
	while( UART0->FR & BIT4 );	//polling until RX FIFO empty goes low
	return UART0->DR;
}

char* UART_receive_line(){

	uint8_t strSize = 8;
	char* str = malloc(strSize);
	char receivedChar =0;

	int8_t i =0;
	for (i =0; i<250; i++) {

		//double allocated space  whenever current space is full
		if( (i+2) >= strSize ) {
			strSize *=2;
			str = realloc(str, strSize);
		}
		receivedChar = UART_receive();

		if (receivedChar =='\r' || receivedChar == '\n'){
			str[i] = '\0';
			break;
		}

		str[i] = receivedChar;
	}

	return str;
}


uint32_t UART_receive_uint(){

	char* str = UART_receive_line();
	return atol(str);
}

void UART_send_packedBCD(unsigned char DATA)
{
	UART_send_char('0'+(DATA>>4) ) ;
	UART_send_char('0'+(DATA & 0x0f) )  ;
}
