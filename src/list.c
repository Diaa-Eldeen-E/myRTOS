/*
 * list.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


#include "list.h"

//insert without changing index !!! !!!
 void listInsertItemLast(list_t* list,OSThread_t* OSThread){

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
//		 list->ptIndex = OSThread;	//change index ??

	 }
	 list->ui32NoOfItems++;


}

void listInsertItem(list_t* list, OSThread_t* OSThread){

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

//void __error__(char *pcFilename, uint32_t ui32Line);
//void __error__(char *pcFilename, uint32_t ui32Line)
//{
//    while(1);
//}

OSThread_t* listGetItem(list_t* list, OSThread_t* OSThread){

	//check the no of OSThreads > 0
	if(list->ui32NoOfItems < 1)
		while(1);

	OSThread->ptNext->ptPrev = OSThread->ptPrev;
	OSThread->ptPrev->ptNext = OSThread->ptNext;

	list->ptIndex = list->tHead.ptNext;

	list->ui32NoOfItems--;

	return OSThread;
}

void listInit(list_t* list){


	list->ui32NoOfItems = 0;	//convention?

	list->ptIndex = &list->tHead;


}







//
//void listInsertItemLast(list_t* list, listItem_t* item){
//
//	list->liHead.pliPrev->pliNext = item;
//	list->liHead.pliPrev = item;
//
//	list->ui32NoOfItems++;
//
//
//}
//
//void listInsertItem(list_t* list, listItem_t* item){
//
//	if(list->ui32NoOfItems < 1){
//
//		list->liHead.pliNext = item;
//		list->liHead.pliPrev = item;
//		list->pliIndex = item;
//		item->pliNext = &list->liHead;
//		item->pliPrev = &list->liHead;
//
//	}
//	else{
//		list->pliIndex->pliNext->pliPrev = item;
//
//		item->pliNext = list->pliIndex->pliNext;
//		item->pliPrev = list->pliIndex;
//
//		list->pliIndex->pliNext = item;
//		list->pliIndex = item;	//change index ??
//
//	}
//	list->ui32NoOfItems++;
//
//
//}
//
//listItem_t* listGetItem(list_t* list, listItem_t* item){
//
//	//check the no of items > 0
//	item->pliNext->pliPrev = item->pliPrev;
//	item->pliPrev = item->pliNext;
//
//	list->ui32NoOfItems--;
//
//	return item;
//}
//
//void listInit(list_t* list){
//
//
//	list->ui32NoOfItems = 0;	//convention?
//
//	list->pliIndex = &list->liHead;
//
//
//
//}
