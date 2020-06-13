/*
 * mailbox.h
 *
 *  Created on: Jun 12, 2020
 *      Author: Diaa Eldeen
 */

#ifndef INCLUDE_MAILBOX_H_
#define INCLUDE_MAILBOX_H_


#include "thread.h"



typedef struct mailbox_t {

	uint32_t* pStart;
	uint32_t* pEnd;
	uint32_t bufferLength;
	uint32_t* pReadIdx;
	uint32_t* pWriteIdx;
	uint32_t noOfMsgs;
	uint32_t msgSize;
	queue_t waitingQueue;

}mailbox_t;



void OS_mailboxCreate(mailbox_t* pxMailbox, uint32_t* pStart,  \
				uint32_t uiSize, uint32_t uiMsgSize);
uint32_t OS_mailboxWrite(mailbox_t* pxMailbox, uint32_t* pMsg);
uint32_t OS_mailboxRead(mailbox_t* pxMailbox, uint32_t* pMsg);


#endif /* INCLUDE_MAILBOX_H_ */
