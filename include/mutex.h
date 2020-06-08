/*
 * mutex.h
 *
 *  Created on: Jun 8, 2020
 *      Author: Diaa Eldeen
 */

#ifndef INCLUDE_MUTEX_H_
#define INCLUDE_MUTEX_H_

#include "thread.h"

typedef struct mutex_t {

	uint32_t val;
	queue_t waitingQueue;

}mutex_t;



void OS_mutexCreate(mutex_t* pxMutex);
uint32_t OS_mutexLock(mutex_t* pxMutex);
uint32_t OS_mutexRelease(mutex_t* pxMutex);


#endif /* INCLUDE_MUTEX_H_ */
