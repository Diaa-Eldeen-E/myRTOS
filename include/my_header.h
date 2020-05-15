/*
 * my_header.h
 *
 *  Created on: Jul 8, 2019
 *      Author: DiaaEldeen
 */

#ifndef MY_HEADER_H_
#define MY_HEADER_H_



//masking GPIO data registers

#define GPIOA_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOA_AHB_BASE + (mask<<2)) ) )
#define GPIOB_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOB_AHB_BASE + (mask<<2)) ) )
#define GPIOC_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOC_AHB_BASE + (mask<<2)) ) )
#define GPIOD_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOD_AHB_BASE + (mask<<2)) ) )
#define GPIOE_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOE_AHB_BASE + (mask<<2)) ) )
#define GPIOF_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOF_AHB_BASE + (mask<<2)) ) )
#define GPIOG_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOG_AHB_BASE + (mask<<2)) ) )
#define GPIOH_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOH_AHB_BASE + (mask<<2)) ) )
#define GPIOJ_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOJ_AHB_BASE + (mask<<2)) ) )
#define GPIOK_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOK_BASE + (mask<<2)) ) )
#define GPIOL_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOL_BASE + (mask<<2)) ) )
#define GPIOM_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOM_BASE + (mask<<2)) ) )
#define GPION_DATA(mask)		 (* ((volatile uint32_t*)		(GPION_BASE + (mask<<2)) ) )
#define GPIOP_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOP_BASE + (mask<<2)) ) )
#define GPIOQ_DATA(mask)		 (* ((volatile uint32_t*)		(GPIOQ_BASE + (mask<<2)) ) )



#define BIT0              0x00000001
#define BIT1              0x00000002
#define BIT2              0x00000004
#define BIT3              0x00000008
#define BIT4              0x00000010
#define BIT5              0x00000020
#define BIT6              0x00000040
#define BIT7              0x00000080
#define BIT8              0x00000100
#define BIT9              0x00000200
#define BIT10             0x00000400
#define BIT11             0x00000800
#define BIT12             0x00001000
#define BIT13             0x00002000
#define BIT14             0x00004000
#define BIT15             0x00008000
#define BIT16             0x00010000
#define BIT17             0x00020000
#define BIT18             0x00040000
#define BIT19             0x00080000
#define BIT20             0x00100000
#define BIT21             0x00200000
#define BIT22             0x00400000
#define BIT23             0x00800000
#define BIT24             0x01000000
#define BIT25             0x02000000
#define BIT26             0x04000000
#define BIT27             0x08000000
#define BIT28             0x10000000
#define BIT29             0x20000000
#define BIT30             0x40000000
#define BIT31             0x80000000



#define P0		BIT0
#define P1		BIT1
#define P2		BIT2
#define P3		BIT3
#define P4		BIT4
#define P5		BIT5
#define P6		BIT6
#define P7		BIT7


//for SYSCTL->RCGCGPIO settings
#define PORTA		BIT0
#define PORTB		BIT1
#define PORTC		BIT2
#define PORTD		BIT3
#define PORTE		BIT4
#define PORTF		BIT5
#define PORTG		BIT6
#define PORTH		BIT7
#define PORTJ		BIT8
#define PORTK		BIT9
#define PORTL		BIT10
#define PORTM		BIT11
#define PORTN		BIT12
#define PORTP		BIT13
#define PORTQ		BIT14


#endif /* MY_HEADER_H_ */
