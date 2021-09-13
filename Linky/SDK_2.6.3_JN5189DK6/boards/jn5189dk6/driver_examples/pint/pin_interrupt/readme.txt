Overview
========
This example shows how to use SDK drivers to use the Pin interrupt & pattern match peripheral.

Toolchain supported
===================
- IAR embedded Workbench  8.50.6
- MCUXpresso  11.3.0

Hardware requirements
=====================
- Mini USB cable
- OM15076-3 Carrier Board
- OM15082-2 Expansion Board
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
PINT Pin interrupt example

PINT Pin Interrupt events are configured

Press corresponding switches to generate events
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This example configures:
"Pin Interrupt 0" to be invoked when SW4 switch on OM15082-2 Expansion Board is pressed by the user.
The interrupt callback prints
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PINT Pin Interrupt 0 event detected
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"Pin Interrupt 1" to be invoked when SW3 on OM15082-2 Expansion Board is pressed by the user.
The interrupt callback prints
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PINT Pin Interrupt 1 event detected
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================

