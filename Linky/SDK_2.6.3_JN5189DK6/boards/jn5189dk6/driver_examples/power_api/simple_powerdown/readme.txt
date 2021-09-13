Overview
========
This Power demo application allows each of the various low mode modes to be selected, using a simple user interface via the serial port.
The example can test below sleep modes (Deep sleep is not implemented with new API):
1 - PM_POWER_DOWN
2 - PM_DEEP_DOWN

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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Power API demo V1.0
Use power measurement view in MCUXpresso
Reset Cause :    POWER ON RESET
   BROWN OUT DETECTORS
   SYSTEM RESET REQUEST (from CPU)
   WATCHDOG TIMER
   WAKE UP I/O (from DEEP POWER DOWN)
   :: WARNING :: multiple reset causes detected.
SystemCoreClock=12000000
Select Mode
0 - PM_POWER_DOWN
1 - PM_DEEP_DOWN
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================

