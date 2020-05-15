/*
 * MRTOS.h
 *
 *  Created on: Oct 9, 2019
 *      Author: DiaaEldeen
 */

#ifndef MRTOS_H_
#define MRTOS_H_

#include "handy.h"
#include "assert.h"
#include "math.h"


typedef struct {
	void* sp;
	uint32_t timeout;

}OSThread;


extern OSThread blinky1;
extern OSThread blinky2;

typedef void (*OSThreadHandler)();

extern void OS_onIdle();
void OS_delay(uint32_t t);
void OS_run();

//must be called when interrupts are disabled
void OS_sched();
void OS_init(void* stkSto, uint32_t stkSize);
void OSThread_Start(
		OSThread* me,
		OSThreadHandler thread_handler,
		void* sp, uint32_t stkSize);



#endif /* MRTOS_H_ */
