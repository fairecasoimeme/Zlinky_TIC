Overview
========
This example shows how to use SDK drivers to use the Group GPIO input interrupt peripheral.

Toolchain supported
===================
- IAR embedded Workbench  8.50.6
- MCUXpresso  11.3.0

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
5.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
The following lines are printed to the serial terminal when the demo program is executed.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Group GPIO input interrupt example

GINT0 event is configured

Press corresponding switch to generate event
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This example configures "Group GPIO interrupt 0" to be invoked when SW4 switch is pressed by the user.
The interrupt callback prints "GINT0 event detected" message on the serial terminal.

Customization options
=====================

