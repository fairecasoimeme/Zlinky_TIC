Overview
========
This example shows how to use SDK drivers to use the Frequency Measure feature of SYSCON module on LPC devices.
It shows how to measure a target frequency using a (faster) reference frequency. The example uses the internal main clock as the reference frequency to measure the frequencies of the RTC, watchdog oscillator, and internal RC oscillator.

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
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
The log below shows example output of the frequency measure demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Capture source: FRO 1MHz oscillator (Xtal 32Mhz reference), reference frequency = 32000000 Hz
Computed frequency value = 999268 Hz
Expected frequency value = 1000000 Hz

Capture source: XTAL 32K oscillator (Xtal 32Mhz reference), reference frequency = 32000000 Hz
Computed frequency value = 32775 Hz
Expected frequency value = 32768 Hz

Capture source: FRO 32K oscillator (Xtal 32Mhz reference), reference frequency = 32000000 Hz
Computed frequency value = 32775 Hz
Expected frequency value = 32768 Hz
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================

