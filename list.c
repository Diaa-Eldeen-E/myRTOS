/*
 * list.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


#include "list.h"





void listInsertItemLast(list_t* list, listItem_t* item){

	list->liHead.pliPrev->pliNext = item;
	list->liHead.pliPrev = item;

	list->ui32NoOfItems++;


}

void listInsertItem(list_t* list, listItem_t* item){

	if(list->ui32NoOfItems < 1){

		list->liHead.pliNext = item;
		list->liHead.pliPrev = item;
		list->pliIndex = item;
		item->pliNext = &list->liHead;
		item->pliPrev = &list->liHead;

	}
	else{
		list->pliIndex->pliNext->pliPrev = item;

		item->pliNext = list->pliIndex->pliNext;
		item->pliPrev = list->pliIndex;

		list->pliIndex->pliNext = item;
		list->pliIndex = item;	//change index ??

	}
	list->ui32NoOfItems++;


}

listItem_t* listGetItem(list_t* list, listItem_t* item){

	//check the no of items > 0
	item->pliNext->pliPrev = item->pliPrev;
	item->pliPrev = item->pliNext;

	list->ui32NoOfItems--;

	return item;
}

void listInit(list_t* list){


	list->ui32NoOfItems = 0;	//convention?

	list->pliIndex = &list->liHead;



}
