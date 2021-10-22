Overview
========

The AES Example project is a demonstration program that uses the KSDK software to encrypt plain text
and decrypt it back using AES algorithm. CBC and GCM modes are demonstrated.


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
The log below shows example output of the AES example in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AES Peripheral Driver Example

AES CBC: encrypting using 128 bit key done successfully.
AES CBC: decrypting back done successfully.

AES CBC: encrypting using 192 bit key done successfully.
AES CBC: decrypting back done successfully.

AES CBC: encrypting using 256 bit key done successfully.
AES CBC: decrypting back done successfully.

AES GCM: encrypt done successfully.
AES GCM: decrypt done successfully.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================

