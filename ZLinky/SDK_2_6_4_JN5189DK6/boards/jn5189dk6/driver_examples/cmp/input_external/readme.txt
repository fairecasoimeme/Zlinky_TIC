Overview
========
The cmp input external example demonstrates the comparator's functionality with two external voltage source, the example detect high level interrupt and lower level interrupt.

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
Connect CN4_5 and CN4_6 to external voltage source separately, keep CN4_5 at 1.0v.

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
The following message shows in the terminal if the example runs successfully with adjust the external voltage source CN4_6 voltage to a value higher than 1.0v firstly, then adjust CN4_6 to a value lower than 1.0V.
Note: 1.0v is just a reference voltage, user can adjust the reference voltage and keep it static.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CMP input external driver example
Input source 0 voltage range higher than source 1
Input source 0 voltage range lower than source 1
CMP input external driver example finish
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================

