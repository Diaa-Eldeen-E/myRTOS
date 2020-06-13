/*
 * mailbox.c
 *
 *  Created on: Jun 12, 2020
 *      Author: Diaa Eldeen
 */


#include "mailbox.h"


extern volatile queue_t readyQueues[PRIORITY_LEVELS];
extern OSThread_t* volatile pxRunning;


void OS_mailboxCreate(mailbox_t* pxMailbox, uint32_t* pBuffer,
				uint32_t uiSize, uint32_t uiMsgSize) {

	ASSERT_TRUE(pxMailbox != NULL && pBuffer != NULL);
	ASSERT_TRUE(uiSize > 0 && uiMsgSize > 0);

	pxMailbox->pStart = pBuffer;
	pxMailbox->pEnd = (uint32_t*) ((uint32_t) pBuffer + uiSize + uiMsgSize);
	pxMailbox->bufferLength = uiSize/uiMsgSize;
	pxMailbox->pReadIdx = pBuffer;
	pxMailbox->pWriteIdx = pBuffer;
	pxMailbox->msgSize = uiMsgSize;
	pxMailbox->noOfMsgs = 0;

	OS_queueInit(&pxMailbox->waitingQueue);

}


uint32_t OS_mailboxWrite(mailbox_t* pxMailbox, uint32_t* pMsg) {

	ASSERT_TRUE(pxMailbox != NULL && pMsg != NULL);

	if(pxMailbox->noOfMsgs < pxMailbox->bufferLength) {	// buffer is available

		memcpy(pxMailbox->pWriteIdx, pMsg, pxMailbox->msgSize);

		pxMailbox->pWriteIdx = (uint32_t*) ((uint32_t) pxMailbox->pWriteIdx + pxMailbox->msgSize);

		if(pxMailbox->pWriteIdx == pxMailbox->pEnd)
			pxMailbox->pWriteIdx = pxMailbox->pStart;

		pxMailbox->noOfMsgs++;

		// If some threads was waiting to read, make one of them ready
		if(pxMailbox->waitingQueue.ui32NoOfItems && pxMailbox->noOfMsgs == 1) {

			OSThread_t* pxThread = pxMailbox->waitingQueue.xHead.pxNext;
			OS_queuePushThread(&readyQueues[pxThread->ui32Priority], \
					OS_queuePopThread(&pxMailbox->waitingQueue, pxThread));

			// If the unblocked thread has a higher priority than the running one, yield
			if(pxThread->ui32Priority < pxRunning->ui32Priority) {
				OS_queuePushThread(&readyQueues[pxRunning->ui32Priority], pxRunning);
				OS_yield();
			}
		}

	}
	else {	// buffer is full

		// Put it in the mailbox waiting list then force a context switch
		OS_queuePushThread(&pxMailbox->waitingQueue, pxRunning);
		OS_yield();

		return 2;	// The function repeats the mailbox write SVCall
	}

	return 0;
}



uint32_t OS_mailboxRead(mailbox_t* pxMailbox, uint32_t* pMsg) {

	ASSERT_TRUE(pxMailbox != NULL && pMsg != NULL);


	if(pxMailbox->noOfMsgs > 0) {	// buffer not empty

		memcpy(pMsg, pxMailbox->pReadIdx, pxMailbox->msgSize);

		pxMailbox->pReadIdx = (uint32_t*) ((uint32_t) pxMailbox->pReadIdx + pxMailbox->msgSize);

		if(pxMailbox->pReadIdx == pxMailbox->pEnd)
			pxMailbox->pReadIdx = pxMailbox->pStart;

		pxMailbox->noOfMsgs--;


		// If some threads was waiting to write, make one of them ready
		if(pxMailbox->waitingQueue.ui32NoOfItems && pxMailbox->noOfMsgs == (pxMailbox->bufferLength -1)) {

			OSThread_t* pxThread = pxMailbox->waitingQueue.xHead.pxNext;
			OS_queuePushThread(&readyQueues[pxThread->ui32Priority], \
					OS_queuePopThread(&pxMailbox->waitingQueue, pxThread));

			// If the unblocked thread has a higher priority than the running one, yield
			if(pxThread->ui32Priority < pxRunning->ui32Priority) {
				OS_queuePushThread(&readyQueues[pxRunning->ui32Priority], pxRunning);
				OS_yield();
			}
		}

	}
	else {	// buffer is empty

		// Put it in the mailbox waiting list then force a context switch
		OS_queuePushThread(&pxMailbox->waitingQueue, pxRunning);
		OS_yield();

		return 2;	// The function repeats the mailbox write SVCall
	}

	return 0;
}
