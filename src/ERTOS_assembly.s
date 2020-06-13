

.text	@ This is the default any way
.thumb
.syntax unified		@ thumb-2 mixed instructions


		.global	OS_SVC_threadCreate
		.global SVC_HandlerMain
		.global	SVC_Handler
		.global	PendSV_Handler
		.global	OS_SVC_delay
		.global	OS_SVC_yield
		.global OS_SVC_mutexCreate
		.global OS_SVC_mutexLock
		.global OS_SVC_mutexRelease	
		.global OS_SVC_semaphoreCreate
		.global OS_SVC_semaphoreTake
		.global OS_SVC_semaphoreGive
		.global OS_SVC_mailboxCreate
		.global OS_SVC_mailboxWrite
		.global OS_SVC_mailboxRead		
		
		
		.global	pxPrev
		.global	pxRunning
		.global pxNext

pxPrevAddr:	.word	pxPrev
pxRunningAddr:	.word	pxRunning
pxNextAddr:		.word	pxNext



OS_SVC_threadCreate:
		ldr r12, [sp]	@ Pass the fifth argument through r12
		push {lr}
		svc	#1
		pop {lr}
		bx	lr
		

OS_SVC_delay:
		svc #2
		bx lr


OS_SVC_yield:
		svc #3
		bx lr


OS_SVC_mutexCreate:
		svc #10
		bx lr
		
		
OS_SVC_mutexLock:
		svc #11
		bx lr
		
		
OS_SVC_mutexRelease:
		svc #12
		bx lr
		
		
OS_SVC_semaphoreCreate:
		svc #13
		bx lr
		
		
OS_SVC_semaphoreTake:
		svc #14
		bx lr
		
		
OS_SVC_semaphoreGive:
		svc #15
		bx lr
		
		
OS_SVC_mailboxCreate:
		svc #16
		bx lr
		
		
OS_SVC_mailboxWrite:
		svc #17
		bx lr
		
		
OS_SVC_mailboxRead:
		svc #18
		bx lr
		
		


@ It causes error "Invalid state (arm state)" when we don't use the following line
@ It sets the symbol name as a function symbol in the symbol entry table
.type SVC_Handler, %function	
SVC_Handler:
		@ put sp in r0 (passing it as the first argument to SVC_HandlerMain)
		TST	lr,	#4	@ lr bitwise-& bit 2	@ check the bit for the msp or psp
		IT	EQ		@ if msp
		MRSEQ	r0,	msp	
		IT	NE
		MRSNE	r0,	psp
		
		push {lr}
		BL	SVC_HandlerMain	@ call SVC_HandlerMain
		
		pop {lr}
		BX lr



.type PendSV_Handler, %function
PendSV_Handler:
		cpsid	 i		@ Disable interrupts
		
		@ Save the current context
		ldr	r0,	pxRunningAddr
		ldr	r0,	[r0]
		
		@ pxPrev = pxRunning
		ldr r1, pxPrevAddr
		str r0, [r1]
		
		cbz r0,	LoadNext	@ if runningThread = NULL, skip to LoadNext
	
		mrs	r1,	psp		@ load psp to r1
		tst	lr,	#0x10	@ check fpu from lr	bit 4
		it	eq			@ if FPU
		vstmdbeq	r1!, {s16-s31}	@ Save FPU context
		
		mov	r2,	lr		@ include lr in the context
		mrs	r3,	control	@ include control in the context
		stmdb r1!, {r2-r11}	@ Save the context
	
		
		@ Uncomment the following lines if r0 value changed 
		@ ldr	r0,	pxRunningAddr	
		@ ldr	r0,	[r0]
		str	r1,	[r0, #4]	@ save current sp in the current running thread

LoadNext:
		ldr	r1,	pxRunningAddr
		
		@ load sp from nextThread struct
		ldr	r0,	pxNextAddr
		ldr r0,	[r0]
		str r0,	[r1]		@ pxRunning = pxNext
		ldr	r0,	[r0, #4]	@ 2nd struct member (sp)
		
		@ restore new context
		ldmia r0!, {r2-r11}		@ lr, control, r3-r11
		mov	lr,	r2
		msr	control, r3
		isb
		tst	lr,	#0x10		@ check fpu in lr
		it	eq				@ if fpu
		vldmiaeq	r0!, {s16-s31}	@ restore fpu context
		
		msr	psp, r0		@ move the r0 to psp using msr 
		cpsie	 i		@ Enable interrupts
		bx lr


@ Quick reference
@ MRS rd, spec reg	move from special register to register
@ MSR	spec reg rd	move from register to special register

@ ISB	Instruction sync barrier
@ DMB	data memory barrier
@ DSB	data sync barrier

