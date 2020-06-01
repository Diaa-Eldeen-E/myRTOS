/*
 * ERTOS.h
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */

#ifndef ERTOS_H_
#define ERTOS_H_



#include "ERTOS_config.h"
#include "list.h"




extern void OS_SVC_delay();
extern void OS_SVC_yield();
extern void OS_SVC_run();
extern void OS_SVC_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty);

void OS_delay(uint32_t ui32Ticks);
void OS_run();
void OS_tick();

void OS_sched();
//void OS_threadCreate(OSThread_t* me, uint32_t* sp, uint32_t ui32StkSize, uint32_t ui32Priorty);

void OS_init();

#endif /* ERTOS_H_ */
