/*
 * handy.c
 *
 *  Created on: Jun 27, 2019
 *      Author: DiaaEldeen
 */


#include "handy.h"

void clock_setup_MO(uint8_t freq){

	SYSCTL->MOSCCTL = 0x10;	//settings for connected MOSC
	while( !(SYSCTL->RIS & BIT8) );	//w8 until the MOSC  stabilize
	if(freq == 0 || freq > 96 ) while(1);	//96*5 = 480 (max Fvco)

	//memory timing setting
	if(freq <= 16)						SYSCTL->MEMTIM0 = 0x00300030;
	else if (freq > 16 && freq <=40)	SYSCTL->MEMTIM0 = 0x00910091;
	else if (freq > 40 && freq <=60) 	SYSCTL->MEMTIM0 = 0x00d200d2;
	else if (freq > 60 && freq <=80)	SYSCTL->MEMTIM0 = 0x01130113;
	else if (freq > 80 && freq <=100)	SYSCTL->MEMTIM0 = 0x01540154;
	else if (freq > 100 && freq <=120)	SYSCTL->MEMTIM0 = 0x01950195;

	//PLL setting
	SYSCTL->PLLFREQ0 &=~BIT23;	//power down PLL
	SYSCTL->PLLFREQ1 = 0x0004; // Q=0, N=4  (divide MOSC by 5)

	if (freq == 1) {
		freq = 6;
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (5MHz * freq)
		SYSCTL->RSCLKCFG = 0xd300001d | (0x3<<24); 	//divide VCO by 30
	}else if (freq == 2) {
		freq = 6;
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (5MHz * freq)
		SYSCTL->RSCLKCFG = 0xd300000e | (0x3<<24); 	//divide VCO by 15
	}else if (freq == 3) {
		freq = 6;
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (5MHz * freq)
		SYSCTL->RSCLKCFG = 0xd3000009 | (0x3<<24); 	//divide VCO by 10
	}else if (freq == 4) {
		freq = 12;
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (5MHz * freq)
		SYSCTL->RSCLKCFG = 0xd300000e | (0x3<<24); 	//divide VCO by 15
	}
	else {
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (5MHz * freq)
		SYSCTL->RSCLKCFG = 0xd3000004 | (0x3<<24); 	////MEMTIM & PLLFREQ update, use PLL, MOSC, divide VCO by 5
	}

	SYSCTL->PLLFREQ0 |= BIT23;	//power up the PLL
	while ( !(SYSCTL->PLLSTAT & BIT0));	//w8 until it locks
}


void clock_setup_PIO(uint8_t freq){

	SYSCTL->MOSCCTL = 0x10;	//settings for connected MOSC
	if(freq == 0 || freq > 120 ) while(1);

	//memory timing setting
	if(freq <= 16)						SYSCTL->MEMTIM0 = 0x00300030;
	else if (freq > 16 && freq <=40)	SYSCTL->MEMTIM0 = 0x00910091;
	else if (freq > 40 && freq <=60) 	SYSCTL->MEMTIM0 = 0x00d200d2;
	else if (freq > 60 && freq <=80)	SYSCTL->MEMTIM0 = 0x01130113;
	else if (freq > 80 && freq <=100)	SYSCTL->MEMTIM0 = 0x01540154;
	else if (freq > 100 && freq <=120)	SYSCTL->MEMTIM0 = 0x01950195;

	//PLL setting
	SYSCTL->PLLFREQ0 &=~BIT23;	//power down PLL
	SYSCTL->PLLFREQ1 = 0x0003; // Q=0, N=3  (divide PIOSC by 4)

	if (freq == 1) {
		freq = 6;
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (4MHz * freq)
		SYSCTL->RSCLKCFG = 0xd3000017; 	//divide VCO by 24
	}else if (freq == 2) {
		freq = 6;
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (4MHz * freq)
		SYSCTL->RSCLKCFG = 0xd300000b; 	//divide VCO by 12
	}else if (freq == 3) {
		freq = 6;
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (4MHz * freq)
		SYSCTL->RSCLKCFG = 0xd3000007; 	//divide VCO by 8
	}else {
		SYSCTL->PLLFREQ0 |= freq;	//multiplier (4MHz * freq)
		SYSCTL->RSCLKCFG = 0xd3000003; //MEMTIM & PLLFREQ update, use PLL, MOSC, divide VCO by 4
	}

	SYSCTL->PLLFREQ0 |= BIT23;	//power up the PLL
	while ( !(SYSCTL->PLLSTAT & BIT0));	//w8 until it locks
}


void blink_EK_LED(){

	LEDs_EK_setup();

	while(1){
	LED1_ON
	delay_ms(500);
	LED1_OFF
	delay_ms(500);

	}
}

void LEDs_EK_setup()
{

	SYSCTL->RCGCGPIO |= PORTF | PORTN;
	delay_us(1);

	//PORTF LEDS
	GPIOF_AHB->DEN |= P0 | P4;
	GPIOF_AHB->DIR |= P0 | P4;

	//PORTN LEDS
	GPION->DEN |= P0 | P1;
	GPION->DIR |= P0 | P1;
}


void buttons_EK_setup(){
	SYSCTL->RCGCGPIO |= PORTJ;
	delay_us(1);

	GPIO_button->DEN |= P0 | P1;
	GPIO_button->DIR &= ~(P0 | P1);
	GPIO_button->PUR |= P0 | P1;

	//interrupt setup
	GPIO_button->IM &= ~(0xff);		//disable interrupts for safe configuration
	GPIO_button->IS &= ~(P0 | P1);	//edge detection
	GPIO_button->IBE &= ~(P0 | P1);	//single edge
	GPIO_button->IEV &= ~(P0 | P1);	//falling edge
	GPIO_button->ICR |= 0xff;			//clear interrupt flags for the port
	GPIO_button->IM |= P0 | P1;		//enable interrupts
	NVIC_EnableIRQ(GPIOJ_IRQn);		//enable interrupt in NVIC
}

// this function reverses the order of a number of bytes (length) starting from the address (src)
char* reverse(char* src,uint8_t length)
{
	char temp=0,i;
	for (i=0;i<length/2;i++)
		{
			temp = *(src+i);
			*(src+i) = *(src+length-i-1);
			*(src+length-i-1) = temp;
		}
	return src;
}



// this function converts a 4-byte signed integer (num) to
//its ASCI char array (str) in a numeric system (base)
void itoa(int32_t num, char* str, uint32_t base)
{
	uint8_t counter= 0;    //this counts the number of digits
    uint8_t sign =0;

    if (num<0 && base==10) { sign =1; num*=-1; }

    do
    {
    	if (num%base > 9) //for base 16
    		*(str+counter++) = (num%base) - 10 + 'A';  //transform the numbers (10,11,12,13,14,15) to (A,B,C,D,E,F)

    	else
    		*(str+counter++) = num % base + '0' ;   //add the '0' to convert the digit into ASCI
    	num /=base;
    }
    while(num>0);

    if (sign==1) *(str+counter++)= '-';
    reverse(str,counter);
    *(str+counter)= 0;   //null character
}



// this function converts a 4-byte unsigned integer (num) to
//its ASCI char array (str) in a numeric system (base)
void utoa(uint32_t num, char* str, uint32_t base)
{
	uint8_t counter= 0;    //this counts the number of digits

    do
    {
		if (num%base > 9) //for base 16
    		*(str+counter) = (num%base) - 10 + 'A';  //transform the numbers (10,11,12,13,14,15) to (A,B,C,D,E,F)

        else
			*(str+counter) = num % base + '0' ;   //add the '0' to convert the digit into ASCI
		num /=base;
        counter++;
    }
    while(num>0);

    reverse(str,counter);
    *(str+counter)= 0;   //null character
}


//this function converts float number to an to its ASCI char array, the float can hold nearly 8 digits at max
//more digits will cause lack of precision, so the string passed size typically should be 10
void ftoa (float num,uint8_t precision,char *str)  //requires 2048 stack size
{
	switch (precision){
	case 1:
		sprintf(str, "%.1f", num);
		break;
	case 2:
		sprintf(str, "%.2f", num);
		break;
	case 3:
		sprintf(str, "%.3f", num);
		break;
	case 4:
		sprintf(str, "%.4f", num);
		break;
	case 5:
		sprintf(str, "%.5f", num);
		break;
	case 6:
		sprintf(str, "%.6f", num);
		break;
	default:
		str[0]='0'; str[1]= '\0';
		break;
	}
}


void delay_ms(uint_fast16_t ms)
{
	SYSCTL->RCGCTIMER |= TIMER_delay_bit;
	++ms;--ms;	//some time to stabilize the clock

	TIMER_delay->CTL &= ~BIT0;	//clear the timer enable before any config
	TIMER_delay->CFG = 0x04;	//16 bit timer configuration
	TIMER_delay->TAMR = 0x2;	//count down, periodic
	TIMER_delay->CTL = BIT1 ;	//stall enable,
	TIMER_delay->TAPR = 2-1;	//prescaler
	TIMER_delay->CC &= ~(BIT0);	//system clock, set it to 1 for RTC mode (RTC alt clock)
	TIMER_delay->TAILR = ms_LOAD_VALUE;	//load value
	TIMER_delay->ICR |= BIT0;
	TIMER_delay->CTL |= BIT0;	//enable timer after all configuration

	while(ms--)
	{
		while ( !(TIMER_delay->RIS & BIT0) );
		TIMER_delay->ICR |= BIT0;
	}
}


void delay_us(uint_fast16_t us)
{
	SYSCTL->RCGCTIMER |= TIMER_delay_bit;
	++us;--us;	//some time to stabilize the clock

	TIMER_delay->CTL &= ~BIT0;	//clear the timer enable before any config
	TIMER_delay->CFG = 0x04;	//16 bit timer configuration
	TIMER_delay->TAMR = 0x2;	//count down, periodic
	TIMER_delay->CTL = BIT1 ;	//stall enable,
	TIMER_delay->CC &= ~(BIT0);	//system clock, set it to 1 for RTC mode (RTC alt clock)
	TIMER_delay->TAILR = us_LOAD_VALUE;	//load value
	TIMER_delay->ICR |= BIT0;
	TIMER_delay->CTL |= BIT0;	//enable timer after all configuration

	while(us--)
	{
		while ( !(TIMER_delay->RIS & BIT0) );
		TIMER_delay->ICR |= BIT0;
	}
	TIMER_delay->CTL =0;	//disable the timer
}
