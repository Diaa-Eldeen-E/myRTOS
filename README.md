
[![Build Status](https://api.travis-ci.com/Diaa-Eldeen-E/myRTOS.svg?branch=master)](https://app.travis-ci.com/github/Diaa-Eldeen-E/myRTOS/)

# This is a real time operating system implementation
## A practical project for learning the RTOS from the inside

## It has the following features:
- Priority preemptive multi-level queue with round robin scheduling algorithm.
- Context switching supports many modes of operation.
- Uses supervisor system calls to isolate the OS layer.
- Two modes of operation with separate stack pointers for each mode:
	- Kernel mode:
		- Privileged access.
		- OS routines run in kernel mode.
	- Thread mode:
		- Non-Privileged access.
		- User routines run in thread mode.
- Supports mutex and semaphore synchronization.
- Supports mailbox for threads communication.
- A config header file to control the RTOS e.g. FPU_ENABLED, PRIORTY_LEVELS and Tick period.
- Provides an OS_delay function for periodic routines.
- Depends on static allocation.
- Provides an idle task hook.


#### RTOS CPU overhead < 0.8%	(CPU running at 80 MHz)