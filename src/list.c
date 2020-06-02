/*
 * list.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


// This file contains the linked lists (Thread lists) API


#include "list.h"



// insert without changing the index pointer (The next ready thread)
 void listInsertItemLast(list_t* list, OSThread_t* OSThread) {

	 ASSERT_TRUE(list != NULL && OSThread != NULL);

	 if(list->ui32NoOfItems < 1){

		 list->tHead.ptNext = OSThread;
		 list->tHead.ptPrev = OSThread;
		 list->ptIndex = OSThread;
		 OSThread->ptNext = &list->tHead;
		 OSThread->ptPrev = &list->tHead;

	 }
	 else{
		 list->ptIndex->ptNext->ptPrev = OSThread;

		 OSThread->ptNext = list->ptIndex->ptNext;
		 OSThread->ptPrev = list->ptIndex;

		 list->ptIndex->ptNext = OSThread;
	 }

	 list->ui32NoOfItems++;
}

 // Insert an item and change the index pointer to point to it (To be picked next when we call listGetItem)
void listInsertItem(list_t* list, OSThread_t* OSThread){

	ASSERT_TRUE(list != NULL && OSThread != NULL);

	if(list->ui32NoOfItems < 1){

		list->tHead.ptNext = OSThread;
		list->tHead.ptPrev = OSThread;
		list->ptIndex = OSThread;
		OSThread->ptNext = &list->tHead;
		OSThread->ptPrev = &list->tHead;

	}
	else{
		list->ptIndex->ptNext->ptPrev = OSThread;

		OSThread->ptNext = list->ptIndex->ptNext;
		OSThread->ptPrev = list->ptIndex;

		list->ptIndex->ptNext = OSThread;
		list->ptIndex = OSThread;	//change index ??

	}
	list->ui32NoOfItems++;


}


OSThread_t* listGetItem(list_t* list, OSThread_t* OSThread){

	ASSERT_TRUE(list != NULL && OSThread != NULL);
	ASSERT_TRUE(list->ui32NoOfItems >= 1);

	OSThread->ptNext->ptPrev = OSThread->ptPrev;
	OSThread->ptPrev->ptNext = OSThread->ptNext;

	list->ptIndex = list->tHead.ptNext;

	list->ui32NoOfItems--;

	return OSThread;
}



void listInit(list_t* list){

	ASSERT_TRUE(list != NULL);

	list->ui32NoOfItems = 0;
	list->ptIndex = &list->tHead;
}

