
;MRS rd, spec reg	move from special register to register
;MSR	spec reg rd	move from register to special register

;ISB	Instruction sync barrier
;DMB	data memory barrier
;DSB	data sync barrier
	;.text

		.global	OS_SVC_threadCreate
		.global SVC_HandlerMain
		.global	SVC_Handler
		.global	PendSV_Handler


OS_SVC_threadCreate:
		svc	#1
		bx	lr
		

SVC_Handler:
		TST	lr,	#4	; lr bitwise-and bit 2
		IT	EQ
		MRSEQ	r0,	msp
		IT	NE
		MRSNE	r0,	psp
		;IT EQ	;zero flag
		;MRS	r0,	msp

		;IT NE
		;MRS r0,	psp
		;check the bit for the msp or psp
		;put the msp or psp in the r0 (first argument)
		push	{lr}
		
		BL	SVC_HandlerMain
		;call SVC_HandlerMain

		pop	{lr}
		BX lr





PendSV_Handler:
		
		;save current context
		;restore new context




