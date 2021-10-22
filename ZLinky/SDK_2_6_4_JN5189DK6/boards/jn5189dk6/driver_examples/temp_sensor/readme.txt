Overview
========

The temp_sensor example shows how to use the temperature sensor.

In this example, the calibration data (adc and temperature sensor) are first loaded from flash and then the temperature is computed.
For reading the temperature, the software:
a.	Calls load_calibration_param_from_flash(). This step shall be done once prior doing any read of temperature.
If calibration data of either ADC or temperature sensor is not present, the function returns false and measurement of temperature cannot be done.
b.	Calls get_temperature(8, &temperature_in_128th_degree) to read the temperature. 8 is the number of sample to acquire.

Note that reported temperature is only valid if function returns true meaning that calibration data for ADC and temperature sensor are present in flash.


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
The following lines are printed to the serial terminal when the demo program is executed.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Temperature measurement example.

 Temperature in Celsius (average on 8 samples) = 23.5

...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================

