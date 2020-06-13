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
#include "semaphore.h"
#include "mailbox.h"

#include "handy.h"



extern void OS_SVC_mailboxCreate(mailbox_t* pxMailbox, uint32_t* pStart,  \
		uint32_t uiSize, uint32_t uiMsgSize);
extern void OS_SVC_mailboxWrite(mailbox_t* pxMailbox, uint32_t* pMsg);
extern void OS_SVC_mailboxRead(mailbox_t* pxMailbox, uint32_t* pMsg);

extern void OS_SVC_semaphoreCreate(mutex_t* pxMutex, uint32_t ui32InitialValue);
extern void OS_SVC_semaphoreTake(mutex_t* pxMutex);
extern void OS_SVC_semaphoreGive(mutex_t* pxMutex);

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
