/*
 * UART.h
 *
 *  Created on: Jul 7, 2019
 *      Author: DiaaEldeen
 */

#ifndef UART_H_
#define UART_H_

#include "handy.h"

#define newLine "\r\n"

#define BAUD_RATE	9600

void UART_initialize_polling();
void UART_initialize_interrupts();
void UART_send_char(unsigned char DATA);
void UART_send_string(char* str);
void UART_send_stringL(char* str);
void UART_send_intL(int32_t number);
void UART_send_uintL(uint32_t number);
void UART_send_int(int32_t number);
void UART_send_uint(uint32_t number);
void UART_send_float(double number , uint8_t precision);	//max precision is 6, max digits including (.) is 11
uint32_t UART_receive();
char* UART_receive_line();
uint32_t UART_receive_uint();
void UART_send_packedBCD(unsigned char DATA);



#endif /* UART_H_ */
