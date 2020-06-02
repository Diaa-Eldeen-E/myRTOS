/*
 * ERTOS_config.h
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */

#ifndef ERTOS_CONFIG_H_
#define ERTOS_CONFIG_H_


#define PRIORITY_LEVELS		9
#define FPU_ENABLED	0


/* This is used to control the time slot given for each function in round robin scheduling
 * The number represents the number of ticks for each time slot
 */
#define TIME_SLOT	1


/* This is used to control the tick period or number of interrupts per second
 * This number represents the number of milli-seconds in the tick
 * Higher period for better performance and lower period for real time response
 * It also could be used for calculating delay periods
 */
#define TICK_PERIOD_MS	1


#endif /* ERTOS_CONFIG_H_ */
