/*
 * semaphore.h
 *
 *  Created on: Jun 11, 2020
 *      Author: Diaa Eldeen
 */

#ifndef INCLUDE_SEMAPHORE_H_
#define INCLUDE_SEMAPHORE_H_


#include "thread.h"



typedef struct semaphore_t {

	uint32_t val;
	queue_t waitingQueue;

}semaphore_t;



void OS_semaphoreCreate(semaphore_t* pxSemaphore, uint32_t ui32InitialValue);
uint32_t OS_semaphoreTake(semaphore_t* pxMutex);
uint32_t OS_semaphoreGive(semaphore_t* pxMutex);



#endif /* INCLUDE_SEMAPHORE_H_ */
