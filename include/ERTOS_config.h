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
#define TIME_SLOT	2



#endif /* ERTOS_CONFIG_H_ */
