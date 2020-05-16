
[![Build Status](https://travis-ci.com/diaa3007/myRTOS.svg?branch=master)](https://travis-ci.com/diaa3007/myRTOS/)

# This is a real time operating system implementation
## A practical project for learning the RTOS from the inside

## It has the following features:
- Priority preemptive with round robin scheduling algorithm.
- Context switching supports many modes of operation (FPU or non FPU, PSP or MSP).
- Supports supervisor system calls to isolate the OS layer.
- Provides an idle task defined by the user which gets executed when there isn't any ready task.
- Supports mutex and semaphore (This is stil in progress).
- A config header file to control the RTOS e.g. FPU_ENABLED, PRIORTY_LEVELS.

