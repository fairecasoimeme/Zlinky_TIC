Overview
========
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This demo explains how built in HWVAD( HW voice activity detector) can be used to
wake up the device from sleep mode

Toolchain supported
===================
- IAR embedded Workbench  8.50.6
- MCUXpresso  11.3.0

Hardware requirements
=====================
- Mini USB cable
- OM15076-3 Carrier Board
- OM15081-1 Board plugged on the Carrier Board
- JN518x Module plugged on the Carrier Board
- Personal Computer


Board settings
==============
No special settings are required.
Note: Only DMIC_1 is enabled in the example.

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

Running the demo
================
1.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

The following lines are printed to the serial terminal when the demo program is executed.

Configure DMIC

2. This example wakes up microcontroller on any voice activity.
   Speak on DMIC,you can see blinking Red LED(DS3). Also serial message "Going into sleep" and "Just woke up" will be transmitted.

Customization options
=====================

