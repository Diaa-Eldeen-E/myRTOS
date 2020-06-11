/*
 * ERTOS.h
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */

#ifndef ERTOS_H_
#define ERTOS_H_




#include "ERTOS_config.h"
#include "thread.h"
#include "mutex.h"

#include "handy.h"


extern void OS_SVC_mutexCreate(mutex_t* pxMutex);
extern void OS_SVC_mutexLock(mutex_t* pxMutex);
extern void OS_SVC_mutexRelease(mutex_t* pxMutex);
extern void OS_SVC_delay();
extern void OS_SVC_yield();


void OS_delay(uint32_t ui32Ticks);
void OS_run();
void OS_tick();
void OS_init();

#endif /* ERTOS_H_ */
