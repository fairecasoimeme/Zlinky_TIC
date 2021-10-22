Overview
========
This example demonstrates how to use the NTAG component in the context of a wireless application. 
It has been designed to be easily portable and integrated in a Zigbee, Ble or Thread application.
The ntag_demo demonstrates the following features:
-	Low power: the device remains in power down if there is no NFC device detected in the RF field. 
    Once in the field, the device wakes up and remains awake until the NFC device is out of the RF field.
-	NTAG i2C eeprom access: at the first boot the device will register, in the NTAG eeprom, a NDEF smart poster containing an URI link to access the NXP website. 
    A phone running the application “NXP NFC Tag Info” can be used to read the NTAG data.
-	NTAG SRAM usage: the goal is to show how to use the SRAM to transfer some data from a NFC device to the JN/QN/K32W platform. 
    For that a simple protocol has been developed in a “client”/“server” context to be able to send/receive commands. 
    To run this demo a NFC reader supporting this protocol has to be used. 
    As an NFC reader, the nci_demo can be used on JN/QN/K32W platform with the OM5578/PN7150S extension board that can be plugged on top of the OM15076-3 Carrier Board. 

Toolchain supported
===================
- MCUXpresso IDE

Hardware requirements
=====================
To run the NTAG I2C eeprom access demo the following hardware is required:
- 1 Mini USB cable
- 1 OM15076-3 Carrier Board (DK6 board)
- 1 JN518x (T) Module or 1 QN9090/30 (T) Module or 1 K32W061 Module to be plugged on the Carrier Board

In addition to that, to run the NTAG SRAM data transfert demo, the following hardware is required:
- 1 Mini USB cable
- 1 OM15076-3 Carrier Board (DK6 board)
- 1 OM5578/PN7150S (NFC reader) extension board to be plugged on the OM15076-3 Carrier Board
- 1 JN/QN/K32W Module to be plugged on the Carrier Board

Board settings
==============
No special board setting.

Prepare Demos (the following steps should be done for the ntag_demo and also for the nci_demo to show the SRAM data transfert demo)
================
1.  Connect a mini USB cable between the PC host and the USB port on the board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board. 
It is recommended to use DK6Programmer to flash the board instead of using the MCUXpresso IDE.
4.  Reset the SoC and run the project.

Running the demo
================
1. NTAG I2C eeprom access demo:
After flashing the ntag_demo, the device will enter in power down if there is no NFC reader in the field.
A message is printed to the serial terminal when an NFC reader is detected in the field.
To read the content of the NTAG eeprom, a phone (supporting NFC) running the application “NXP NFC Tag Info” can be used.
After reading the content of the NTAG eeprom with a phone, the phone application should display a NDEF smart poster containing an URI link to access the NXP website.

2. NTAG SRAM data transfert demo:
Both the nci_demo and the ntag_demo demos have to be flashed in two boards.
The board running the NTAG demo should be placed in the NFC field of the board running the nci_demo.
Once in the NFC field, the "client" (the nci_demo) will send a TLV command through the SRAM.
The "server" (the ntag_demo) will poll and read the sram content until having received a full TLV message.
After processing a command the "server" (the ntag_demo) will send a TLV response to the "client" (the nci_demo) through the SRAM.
Two commands and two responses are exchanged between the nci_demo device and the ntag_demo device:
    - Get device name command
    - Put provisionning data
The follow logs are printed in serial terminals:
    - ntag_demo logs:
    sleeping
    wake-up from field detect
    [APP] Get Device Name request content [ ]
    [APP] Put provisionning data request content [...]
    sleeping
    - nci_demo logs:
    [APP] Get Device Name response content [n t a g _ i 2 c]
    [APP] Put Provisioning data response content [OK]
