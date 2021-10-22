Overview
========
This application represents the "Thread router eligible device" of the OpenThread Stack Demo Applications.
A Router Eligible Device is a node which initially joins the network as an End Device, but can adaptively 
become a mesh Router. Such a device may also have capabilities to initialize, create, and bootstrap 
a new Thread Network for the user or a management entity.
For more information please refer to the "Open Thread Demo Applications User Guide.pdf" document.

Toolchain supported
===================
- IAR embedded Workbench 8.50.6
- MCUXpresso IDE 11.3.0

Hardware requirements
=====================
- Mini USB cable
- JN5189 or K32W041(041A/041AM)/061 Mezannine board with DK6 carrier board and OM15082 Expansion board
- Personal Computer

Board settings
==============
No special board setting.

Prepare the Demo
================
1.  Using a mini USB cable, connect the PC host and the OpenSDA USB port marked TARGET on the DK6 board to use
the supported toolchains or connect the PC host and the USB port marked FTDI USB to use NXP Test Tool Firmware loader.
2.  Download the program to the target board.
3.  Press the reset button, then any other switch button on your board to begin running the demo.

Running the demo
================
Follow the instructions from the "Open Thread Demo Applications User Guide.pdf".