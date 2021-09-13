Overview
========
The WWDT Example project is to demonstrate usage of the KSDK wdog driver.
In this example,quick test to show user how to feed watch dog.
User need to feed the watch dog in time before it warning or timeout interrupt.
The WINDOW register determines the highest TV value allowed when a watchdog feed is
performed. 

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
The following lines are printed to the serial terminal when the demo program is executed.

--- Time out reset test start ---
Watchdog reset occurred

--- Window mode refresh test start ---
 WDOG has been refreshed!
 WDOG has been refreshed!
 WDOG has been refreshed!
 WDOG has been refreshed!
 WDOG has been refreshed!
 WDOG has been refreshed!
 WDOG has been refreshed!
 WDOG has been refreshed!
 WDOG has been refreshed!
 WDOG has been refreshed!

 
Customization options
=====================

