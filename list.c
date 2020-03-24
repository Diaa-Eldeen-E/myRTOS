/*
 * list.c
 *
 *  Created on: Mar 24, 2020
 *      Author: DiaaEldeen
 */


#include "list.h"





void listInsertItemLast(list_t* list, listItem_t* item){

	list->pliHead->pliPrev->pliNext = item;
	list->pliHead->pliPrev = item;

	list->ui32NoOfItems++;


}

void listInsertItem(list_t* list, listItem_t* item){

	list->pliIndex->pliNext->pliPrev = item;
	list->pliIndex->pliNext = item;
	list->pliIndex = item;

	list->ui32NoOfItems++;


}

listItem_t* listGetItem(list_t* list, listItem_t* item){

	item->pliNext->pliPrev = item->pliPrev;
	item->pliPrev = item->pliNext;

	list->ui32NoOfItems--;

	return item;
}

void listInit(list_t* list){

	struct listItem_t* dummyItem;
	dummyItem->pliNext = dummyItem;
	dummyItem->pliPrev = dummyItem;

	list->ui32NoOfItems = 0;	//convention?

	list->pliIndex = dummyItem;
	list->pliHead = dummyItem;



}
