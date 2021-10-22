Overview
========
The pwm_output project is a simple demonstration program of the SDK PWM driver.
The pulse width modulator (PWM) module contains PWM channels, each of which is set up to output different signals.
This PWM module can generate various switching patterns, including highly sophisticated waveforms.

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
5.  Reset the SoC and run the project.

Running the demo
================
The following message shows in the terminal if the example runs successfully.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PWM driver example. Connect scope to Port 0 Pin 3 to see output
or look at LED DS3/D3 on carrier/sensor boards.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================

