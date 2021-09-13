Overview
========
This document explains the freertos_tickless example. It shows how the CPU enters the sleep mode and then
it is woken up either by expired time delay using low power timer module or by external interrupt caused by a
user defined button.


Toolchain supported
===================
- MCUXpresso  11.3.0
- IAR embedded Workbench  8.50.6

Hardware requirements
=====================
- Mini USB cable
- OM15076-3 Carrier Board
- OM15082-2 Board plugged on the Carrier Board
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
3.  Plug the OM15082-2 Expansion Board onto the Carrier Board.
4.  Download the program to the target board.
5.  Reset the SoC and run the project.

Running the demo
================
After flashing the example to the board the console will start printing the number of tick count periodically
when the CPU is running.
To wake up the CPU by external interrupt, press the button, that is specified at the beginning of the
example (SWx where x is the number of the user defined button). When the button is pressed, the console prints
out the "CPU woken up by external interrupt" message.

Example output:

Tickless Demo example
Press SWx to wake up the CPU

Tick count :
0
5000
10000
CPU woken up by external interrupt
15000
20000
25000

Explanation of the example
The example application prints the actual tick count number every time after the specified
delay. When the vTaskDelay() is called, the CPU enters the sleep mode for that defined period
of time.

While the CPU is in sleep mode and the user defined button is pressed, the CPU is woken up
by the external interrupt and continues to sleep after the interrupt is handled.
The period of time delay is not changed after the external interrupt occurs.

Customization options
=====================

