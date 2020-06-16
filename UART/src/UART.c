/*
 * UART.c
 *
 *  Created on: Jul 7, 2019
 *      Author: DiaaEldeen
 */


#include "UART.h"


/* Initialize the UART 0 with baudrate of 9600, 1 stop bit, 8 bit word
 * no parity, no FIFO and no interrupts
 * Sending and receiving requires polling
 */
void UART_initialize_polling() {

	// Enabling the clock
	SYSCTL->RCGCUART  |= BIT0;	// Enable clock to UART0
	SYSCTL->RCGCGPIO  |= PORTA;	// Enable clock for portA (UART0 pins ->PA0,PA1)
	while( !(SYSCTL->PRUART & BIT0));	// W8 until UART is ready
	while( !(SYSCTL->PRGPIO & PORTA));	// W8 until PORTA is ready

	// Configure the PORTA UART pins
	GPIOA_AHB->DEN |= BIT0 | BIT1;		// Enable digital function
	GPIOA_AHB->AMSEL &= ~(BIT0 | BIT1);	// Disable analog function
	GPIOA_AHB->AFSEL |= BIT0 | BIT1;	// Enable alternate function
	GPIOA_AHB->PCTL  |= 0x00000011;	// Select alternate function for PA0, PA1 (Rx,Tx)

	// Configure the UART
	UART0->CTL = 0;		//clear UART enable before changing the CTL register

	/*BAUD RATE
	 * a write operation to LCRH is required for this setting to take effect
	 */
	float BR = (float) F_SysClk / (16 * BAUD_RATE);
	uint32_t IBR = (uint32_t) BR;
	UART0->IBRD = IBR;
	UART0->FBRD	= (uint32_t) ((float)( BR - IBR) * 64 + 0.5);

	UART0->LCRH	|= BIT5 | BIT6;		// 1 stop bit, 8 bits char, no parity
	UART0->CC =0;	// Use system clock as clock source

	UART0->CTL	|= BIT0 | BIT8 | BIT9;	// UARTEN, TXE, RXE -> set those after config
	delay_us(5);
}


/* Initialize the UART 0 with baudrate of 9600, 1 stop bit, 8 bit word
 * no parity and no FIFO
 * Sending requires polling but receiving could be done with interrupts
 */
void UART_initialize_interrupts() {

	// Enabling the clock
	SYSCTL->RCGCUART  |= BIT0;	// Enable clock to UART0
	SYSCTL->RCGCGPIO  |= PORTA;	// Enable clock for portA (UART0 pins ->PA0,PA1)
	while( !(SYSCTL->PRUART & BIT0));	// W8 until UART is ready
	while( !(SYSCTL->PRGPIO & PORTA));	// W8 until PORTA is ready

	// Configure the PORTA UART pins
	GPIOA_AHB->DEN |= BIT0 | BIT1;		// Enable digital function
	GPIOA_AHB->AMSEL &= ~(BIT0 | BIT1);	// Disable analog functions
	GPIOA_AHB->AFSEL |= BIT0 | BIT1;	// Enable alternate function
	GPIOA_AHB->PCTL  |= 0x00000011;	// Select alternate function for PA0, PA1 (Rx,Tx)

	// Configure the UART
	UART0->CTL = 0;		//clear UART enable before changing the CTL register

	/*BAUD RATE
	 * a write operation to LCRH is required for this setting to take effect
	 */
	float BR = (float) F_SysClk / (16 * BAUD_RATE);
	uint32_t IBR = (uint32_t) BR;
	UART0->IBRD = IBR;
	UART0->FBRD	= (uint32_t) ((float)( BR - IBR) * 64 + 0.5);

	UART0->LCRH	|= BIT5 | BIT6;		// 1 stop bit, 8 bits char, no parity
	UART0->CC =0;	// Use system clock as clock source

	UART0->IM  |= BIT4;	// Enable receive interrupt

	UART0->CTL	|= BIT0 | BIT8 | BIT9;	// UARTEN, TXE, RXE -> set those after config

	NVIC_EnableIRQ(UART0_IRQn);
	NVIC_SetPriority(UART0_IRQn, 3);	// Set the interrupt priority to 3
	delay_us(5);
}



// Send one character
void UART_send_char(unsigned char DATA) {

	while( UART0->FR & BIT5 );		//polling until TX FIFO full goes low

	UART0->DR = DATA;
}


// Send a string of characters (ends with null terminator)
void UART_send_string(char* str) {

	while(*str)	UART_send_char(*str++);
}


/* Send a string of characters (ends with null terminator)
 * and then end the line with (\r\n)
 */
void UART_send_stringL(char* str) {

	while(*str)	UART_send_char(*str++);

	UART_send_string(newLine);
}


/* Send a 4 bytes integer
 * and then end the line with (\r\n)
 */
void UART_send_intL(int32_t number) {

	UART_send_int(number);

	UART_send_string(newLine);
}


/* Send a 4 bytes unsigned integer
 * and then end the line with (\r\n)
 */
void UART_send_uintL(uint32_t number) {

	UART_send_int(number);

	UART_send_string(newLine);
}


// Send a 4 bytes integer
void UART_send_int(int32_t number) {

	char intNum_String [12];
	itoa(number,intNum_String,10);

	UART_send_string(intNum_String);
}


// Send a 4 bytes unsigned integer
void UART_send_uint(uint32_t number) {

	char intNum_String [11];
	utoa(number,intNum_String,10);

	UART_send_string(intNum_String);
}


/* Send a floating point number with a max precision of 6 and
 * max digits of 11 including (.)
 */
void UART_send_float(double number , uint8_t precision) {

	char doubleNum_String [12];
	ftoa(number, precision ,doubleNum_String);

	UART_send_string(doubleNum_String);
}


// Receive one character
uint32_t UART_receive() {

	while( UART0->FR & BIT4 );	//polling until RX FIFO empty goes low

	return UART0->DR;
}


// Receive a line (a string that ends with \r or \n but not both)
char* UART_receive_line() {

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


// Receive an unsigned integer number
uint32_t UART_receive_uint() {

	char* str = UART_receive_line();
	return atol(str);
}


void UART_send_packedBCD(unsigned char DATA) {

	UART_send_char('0'+(DATA>>4) ) ;
	UART_send_char('0'+(DATA & 0x0f) )  ;
}
