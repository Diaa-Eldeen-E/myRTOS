
;MRS rd, spec reg	move from special register to register
;MSR	spec reg rd	move from register to special register

;ISB	Instruction sync barrier
;DMB	data memory barrier
;DSB	data sync barrier
	;.text

		.global	ptRunning
		.global ptNext
ptRunningAddr:	.word	ptRunning
ptNextAddr:		.word	ptNext


		.global	OS_SVC_threadCreate
		.global SVC_HandlerMain
		.global	SVC_Handler
		.global	PendSV_Handler
		
OS_SVC_threadCreate:
		svc	#1
		bx	lr
		

SVC_Handler:
		TST	lr,	#4	; lr bitwise-& bit 2	(;check the bit for the msp or psp)
		IT	EQ
		MRSEQ	r0,	msp	;put the msp or psp in the r0 (first argument)
		IT	NE
		MRSNE	r0,	psp
		;IT EQ	;zero flag
		;MRS	r0,	msp
		;IT NE
		;MRS r0,	psp
		
		push	{lr}
		BL	SVC_HandlerMain	;call SVC_HandlerMain
		pop	{lr}
		BX lr


		


		
PendSV_Handler:
		cpsid	 i
		ldr	r0,	ptRunningAddr
		ldr	r0,	[r0]
		cbz r0,	LoadNext
		;save current context if running thread != NULL
		push	{r4-r11}
		push	{r4-r6}
		;save current sp in current thread
		
		;ldr	r0,	ptRunningAddr
		;ldr	r0,	[r0]
		str	sp,	[r0, #4]

LoadNext:
		ldr	r1,	ptRunningAddr
		
		;change sp
		ldr	r0,	ptNextAddr
		ldr r0,	[r0]
		str r0,	[r1]	; ptRunningAddr = ptNext
		ldr	r0,	[r0, #4]	;2nd struct member
		mov	sp,	r0
		;restore new context
		pop	{r4-r6}
		pop	{r4-r11}
		cpsie	 i
		bx lr


