Overview
========
The cmp input internal example demonstrates the comparator's functionality with one external voltage source and one internal voltage reference source, the example detect rising edge interrupt and falling edge interrupt.

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
Connect CN4_6 to external voltage source.

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
The following message shows in the terminal if the example runs successfully with adjust the external voltage higher than 0.8v firstly, then adjust external voltage to a value lower than 0.8V(0.8V is the internal reference voltage).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CMP input internal driver example
External source voltage range higher than internal source 
External source voltage range lower than internal source 
CMP input internal driver example finish
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================

