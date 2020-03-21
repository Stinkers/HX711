# HX711
## C/C++ library for using a Raspberry Pi 1 to communicate with the HX711 chip.

Commonly used with strain gauges, the HX711 chip is a high resolution A/D with configurable inputs and a serial output. Multiple HX711s are supported.

**Due to the critical timing needed for communications with the HX711, this code will probably only work with a Raspberry Pi version 1.**

A real time OS would be preferable to a Linux/Raspberry Pi - something like a PIC or Arduino would be ideal, as the timing can be carefully controlled. For example, keeping the clock line high for longer than 60 microseconds sends the chip to sleep. That's microseconds, not milliseconds. An interrupt can take longer than that to service, resulting in the data line going high for the rest of the reading. A few bodges are needed to avoid false readings:

* A very crude filter that rejects readings that differ from the previous one by more than a threshold value.
* Some horrible, horrible timing cludges.
* Setting the priority of the process as high as possible.
* Turning off most interrupts in the readout loop.
* Averaging the readings over a period of time.

Even so, the occasional wonky reading gets through. Don't use this code in mission critical applications, or at all really. 

## Using the library

The hx711.h header file contains the constants and structure for each sensor. The constants are:

* HX711_GLITCHLEVEL : The threshold for consequetive readings to valid. Default is 200, i.e. the reading is rejected if it is 200 or more higher or lower than the previous one.
* HX711_TIMEOUT : How long to wait for the HX711 to respond. Default is 5 seconds.

These are used for all HX711 readings.

To be continued...