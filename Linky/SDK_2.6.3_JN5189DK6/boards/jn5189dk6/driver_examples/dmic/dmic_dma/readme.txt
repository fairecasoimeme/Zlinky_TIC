Overview
========
This example shows how to use DMA to transfer data from DMIC to memory.

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

Running the demo
================
1.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.
Whole buffer before and after data transfer will be displayed. All data before transfer is 0 and data in the 
buffer after transfer is dmic data which is now different. 
The following lines are printed to the serial terminal when the demo program is executed. 

Configure DMA
Buffer Data before transfer
 <data>
Transfer completed
Buffer Data after transfer
 <data>


2. This example shows how DMA can be used with DMIC to transfer data to memory.DMIC audio data can also be seen in g_rxBuffer.
Initially this buffer is initialized to zero.

Customization options
=====================

