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

* **HX711_GLITCHLEVEL** : The threshold for consequetive readings to valid. Default is 200, i.e. the reading is rejected if it is 200 or more higher or lower than the previous one.
* **HX711_TIMEOUT** : How long to wait for the HX711 to respond. Default is 5 seconds.

These are used for all HX711 readings.

### struct hx711

For each individual HX711 you read out from, there is a hx711 struct which contains settings and readings for each HX711. These are:

* **int pd_sck** : The GPIO number for the clock pin
* **int dout** : the GPIO number for the data out pin 
* **enum hx711_mode mode** : The mode for the chip -
  * MODE_B32 : Channel B, gain of 32
  * MODE_A64 : Channel A, gain of 64
  * MODE_A128 : Channel A, gain of 128
* **int avgtime** : Period in seconds to average over
* **int zero** : The zero reading - this is set by calling the hx711_zero function
* **int average** : The current averaged reading
* **int last** : The last single valid reading
* **int avgcount** : For internal use - initialise to 0
* **time_t** : For internal use - initialise to 0

### Functions

**void hx711_setup(struct hx711 \*channel);** 

Sets up the instance of **channel** and initialises the chip, setting the mode.

#### Example
```C
    struct hx711 channel_1 = {
        3, 15,      // pd_sck on GPIO 3, dout on GPIO 15
        MODE_A128,  // channel and gain mode
        10,          // Average over x seconds of samples
        0, 0, 0, 0, 0   // For internal use
    };

    hx711_setup(&channel_1);
```

**int hx711_zero(struct hx711 \*channel, int count);**

Sets the zero value for the HX711 by averaging over **count** samples. The value is stored in the **hx711** structure and returned by the function.

#### Example
```C
    printf("Zero value: %d\n", hx711_zero(&channel_1, 10));
```

