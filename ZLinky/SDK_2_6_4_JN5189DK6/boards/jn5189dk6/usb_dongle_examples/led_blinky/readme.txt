Overview
========
The LED Blinky demo application provides a sanity check for the new SDK build environments and board bring up. The LED Blinky demo 
uses the systick interrupt to realize the function of timing delay. The example takes turns to shine the LED. The purpose of this 
demo is to provide a simple project for debugging and further development.

Toolchain supported
===================
- IAR embedded Workbench  8.50.6
- MCUXpresso  11.3.0

Hardware requirements
=====================
- PCB2459-2 JN5189 USB DONGLE
- Personal Computer

Board settings
==============
No special settings are required.

Prepare the Demo
================
1.  Plug the PCB2459-2 JN5189 USB DONGLE onto the Personal Computer.
2.  Download the program to the target board. run "DK6Programmer.exe -s COMx -p led_blinky.bin" command.
3.  Rebuild the debugger to generate new led_blinky.bin file for new program.

Running the demo
================
When the demo program is executed, the LED D1 and D2 will start flashing alternately.

Customization options
=====================

