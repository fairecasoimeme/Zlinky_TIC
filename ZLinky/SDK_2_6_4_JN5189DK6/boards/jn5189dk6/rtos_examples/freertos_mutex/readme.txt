Overview
========
This document explains the freertos_mutex example. It shows how mutex manage access to common
resource (terminal output).

The example application creates two identical instances of write_task. Each task will lock the mutex
before printing and unlock it after printing to ensure that the outputs from tasks are not mixed
together.

The test_task accept output message during creation as function parameter. Output message have two
parts. If xMutex is unlocked, the write_task_1 acquire xMutex and print first part of message. Then
rescheduling is performed. In this moment scheduler check if some other task could run, but second
task write_task_2 is blocked because xMutex is already locked by first write task. The first
write_task_1 continue from last point by printing of second message part. Finally the xMutex is
unlocked and second instance of write_task_2 is executed.




Toolchain supported
===================
- IAR embedded Workbench  8.50.6
- MCUXpresso  11.3.0

Hardware requirements
=====================
- Mini USB cable
- OM15076-3 Carrier Board
- JN518x Module plugged on the Carrier Board
- Personal Computer

Board settings
==============
No special settings are required.

Prepare the Demo
================
1.  Connect a mini USB cable between the PC host and the USB port (J15) on the board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Reset the SoC and run the project.

Running the demo
================
After the board is flashed the Tera Term will start periodically printing strings synchronized by
mutex.

Example output:
"ABCD | EFGH"
"1234 | 5678"
"ABCD | EFGH"
"1234 | 5678"

Customization options
=====================

