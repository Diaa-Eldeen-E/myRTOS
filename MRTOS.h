/*
 * MRTOS.h
 *
 *  Created on: Oct 9, 2019
 *      Author: DiaaEldeen
 */

#ifndef MRTOS_H_
#define MRTOS_H_

#include "handy.h"


typedef struct {
	void* sp;

}OSThread;


extern OSThread blinky1;
extern OSThread blinky2;

typedef void (*OSThreadHandler)();

//must be called when interrupts are disabled
void OS_sched();

void OS_init();
void OSThread_Start(
		OSThread* me,
		OSThreadHandler thread_handler,
		void* sp, uint32_t stkSize);



#endif /* MRTOS_H_ */
