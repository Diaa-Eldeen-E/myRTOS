/*
 * list.h
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */

#ifndef LIST_H_
#define LIST_H_

#include "handy.h"


typedef void (*OSThreadHandler_t)();


typedef struct OSThread_t{
	uint32_t ui32ThreadID;
	uint32_t* sp;
	OSThreadHandler_t OSThreadHandler;
	uint32_t ui32TimeOut;
	uint32_t ui32Priority;

	struct OSThread_t* ptNext;
	struct OSThread_t* ptPrev;

}OSThread_t;


typedef struct list_t{
	uint32_t ui32NoOfItems;

	OSThread_t* ptIndex;
	OSThread_t tHead;

}list_t;





//typedef struct listItem_t{
//	uint32_t ui32ThreadID;
//
//	struct listItem_t* pliNext;
//	struct listItem_t* pliPrev;
//	//function pointer
//	//item list
//	//timeout
//	//priority
//
//}listItem_t;
//
//typedef struct list_t{
//	uint32_t ui32NoOfItems;
//
//	listItem_t* pliIndex;
//	listItem_t liHead;
//
//	//function pointer
//	//item list
//	//timeout
//	//priority
//
//}list_t;





void listInit(list_t* list);
void listInsertItemLast(list_t* list, OSThread_t* item);
void listInsertItem(list_t* list, OSThread_t* item);
OSThread_t* listGetItem(list_t* list, OSThread_t* item);


#endif /* LIST_H_ */
