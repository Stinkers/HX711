# HX711
C/C++ library for using a Raspberry Pi 1 to communicate with the HX711 chip.

Due to the critical timing needed for communications with the HX711, this code will probably only work with a Raspberry Pi version 1.
A real time OS would be preferable to a Linux/Raspberry Pi - something like a PIC or Arduino would be ideal, as the timing can be carefully controlled. This code tries to mitigate the problems of a non-real time OS by assigning the program a high priority and disabling most interrupts at the  critical point. Even so, a few bodges are needed to avoid false readings.
