
#MRS rd, spec reg	move from special register to register
#MSR	spec reg rd	move from register to special register

#ISB	Instruction sync barrier
#DMB	data memory barrier
#DSB	data sync barrier
	#.text

.thumb
.syntax unified

		.global	ptRunning
		.global ptNext
		.global ptNextAddr
		.global	lrTemp
ptRunningAddr:	.word	ptRunning
ptNextAddr:		.word	ptNext
lrTempAddr:		.word	lrTemp


		.global	OS_SVC_threadCreate
		.global SVC_HandlerMain
		.global	SVC_Handler
		.global	PendSV_Handler
		.global	OS_SVC_run
		
OS_SVC_run:
		svc	#0
infLoop:
		b infLoop
		#bx	lr
		
OS_SVC_threadCreate:
		svc	#1
		bx	lr
		

SVC_Handler:
		TST	lr,	#4	@ lr bitwise-& bit 2	@check the bit for the msp or psp
		IT	EQ
		MRSEQ	r0,	msp	@put the msp or psp in the r0 (first argument)
		IT	NE
		MRSNE	r0,	psp
		#IT EQ	#zero flag
		#MRS	r0,	msp
		#IT NE
		#MRS r0,	psp
		
		#push	{lr}
		ldr	r1,	lrTempAddr
		str	lr,	[r1]
		BL	SVC_HandlerMain	@call SVC_HandlerMain
		ldr r1,	lrTempAddr
		ldr	lr,	[r1]
		#pop	{lr}
		BX lr


		
#see LR and check for msp or psp


		
PendSV_Handler:
		cpsid	 i
		ldr	r0,	ptRunningAddr
		ldr	r0,	[r0]
		
		cbz r0,	LoadNext	@if runningThread = NULL, skip to LoadNext
	
		mrs	r1,	psp		@load psp to r1
		tst	lr,	#0x10	@check fpu from lr	bit 4
		it	eq
		vstmdbeq	r1!, {s16-s31}
		# use store multiple to store the registers inside the psp after loading it into r1
		mov	r2,	lr
		mrs	r3,	control
		stmdb r1!, {r2-r11}
		#save current sp in current thread
		
		#ldr	r0,	ptRunningAddr
		#ldr	r0,	[r0]
		str	r1,	[r0, #4]

LoadNext:
		ldr	r1,	ptRunningAddr
		
		#load sp from nextThread struct
		ldr	r0,	ptNextAddr
		ldr r0,	[r0]
		str r0,	[r1]		@ptRunningAddr = ptNext
		ldr	r0,	[r0, #4]	@2nd struct member
		
		
		# -------------------------
		#mov	sp,	r0	
		
		#restore new context
		ldmia r0!, {r2-r11}		@lr, control, r3-r11
		mov	lr,	r2
		msr	control, r3
		isb
		tst	lr,	#0x10		@check fpu in lr
		it	eq				@if fpu
		vldmiaeq	r0!, {s16-s31}	@restore fpu context
		
		#pop	{r4-r5}
		#pop	{r4-r11}
		msr	psp, r0		@move the r0 to psp using msr 
		cpsie	 i
		bx lr



