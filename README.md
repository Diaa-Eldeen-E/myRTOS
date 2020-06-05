
[![Build Status](https://travis-ci.com/diaa3007/myRTOS.svg?branch=master)](https://travis-ci.com/diaa3007/myRTOS/)

# This is a real time operating system implementation
## A practical project for learning the RTOS from the inside

## It has the following features:
- Priority preemptive multi-level queue with round robin scheduling algorithm.
- Context switching supports many modes of operation.
- Uses supervisor system calls to isolate the OS layer.
- Two modes of operation: thread mode and kernel mode
	- Separate stack pointer for each mode
	- Kernel mode has privileged access
	- Thread mode has un-privileged access
	- OS routines run in kernel mode 
	- User threads run in thread mode
- Supports mutex and semaphore (This is stil a work in progress).
- A config header file to control the RTOS e.g. FPU_ENABLED, PRIORTY_LEVELS and Tick period.
- Provides an OS_delay function for periodic routines
- Depends totaly on static allocation i.e. requires 0 heap space
- Provides an idle task hook.


#### RTOS CPU overhead < 0.8%	(CPU running at 80 MHz)