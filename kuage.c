#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "rpio.h"
#include "hx711.h"

int main (int arcgc, char **argv);
int hx711 (int channel);

int main (int arcgc, char **argv)
{
    int i = 1000;
    
    struct hx711 channel1 = 
    {
        3, 15,      // pd_sck, dout GPIO pins
        MODE_A128,  // channel and gain mode
        10,          // Average over x seconds of samples
        0, 0, 0, 0, 0   // Ballast
    };
    
    setup_io();     // Set up GPIO pins

    hx711_setup(&channel1);     // Initialise the interface

    int zero = hx711_zero (&channel1, 10);  // Get a zero reading
    printf ("zero: %d\n", zero);

    while (i--) {
        while (!hx711_read(&channel1));     // Wait for a valid reading
        printf ("read: %d\n", channel1.average - channel1.zero);
     }

    hx711_sleep(&channel1);
}