/* Communication with the HX711 library */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include "hx711.h"
#include "rpio.h"

void hx711_setup (struct hx711 *channel)
{    
    INP_GPIO(channel->dout);
    GPIO_PULL = 0;
    usleep (100);
    GPIO_PULLCLK0 = 0x0fffffff;
    usleep (100);
    GPIO_PULL = 0;
    usleep (100);
    GPIO_PULLCLK0 = 0;
    INP_GPIO(channel->pd_sck);     // Needs to be set to read before write.
    OUT_GPIO(channel->pd_sck);
 
    GPIO_SET = 1<<channel->pd_sck;  // Reset the HX711
    usleep(60);
    GPIO_CLR = 1<<channel->pd_sck;
    usleep (60);
   
    set_priority();

    channel->avgcount = 0;
    channel->average = 0;

    channel->last = hx711_zero(channel, 1); // Initial reading
}

int hx711_zero(struct hx711 *channel, int count)
{
    long avgz = 0;
    int thisread, i = 0;

    int avgtime = channel->avgtime;
    channel->avgtime = 0;

    for (i = 0; i < count; i++){
        while (!hx711_read(channel));
        avgz += channel->last;
        }

    avgz /= count;
    channel->zero = avgz;
    channel->avgtime = avgtime;
    channel->average = avgz;
    channel->avgcount = 0;
    return avgz;
}

/* Read the HX711
    int hx711_read (struct hx711 *channel)

    Returns 1 for a valid read of the HX711, otherwise 0.
    Calling with channel.avgtime set to 0 resets the averaginig.
*/
int hx711_read(struct hx711 *channel)
{
    int readvalue, valid = 0;
    char bitset = 0;
    int delaya = 0, delayb = 0;

    time_t start, now;
    time(&start);

    if (!channel->avgtime) {
        channel->avgcount = 0;
        time(&channel->avgstart);
    }

    GPIO_CLR = 1<<channel->pd_sck;     // Wake up the HX711

    while (GET_GPIO(channel->dout) == 1<<channel->dout) {
        time(&now);
        if (now - start > HX711_TIMEOUT) {
            printf ("Timed out waiting for HX711\n");
            exit (1);
        }
        usleep(10);
    }

    valid = 0;
    readvalue = 0;
    if (!interrupts(0)) {   // If sck is high for >60us, it goes to sleep. Can't service interrupts.

        for (int i = 0; i < channel->mode; i++){
            GPIO_SET = 1<<channel->pd_sck;
            GPIO_CLR = 1<<channel->pd_sck;
            delaya++;
            bitset = GET_GPIO(channel->dout)>>channel->dout;
            if (i < 24) {
                readvalue = readvalue<<1;
                if (bitset) readvalue++;
            }
        }
        interrupts(1);
        time(&now);

        if (abs(readvalue - channel->last) < HX711_GLITCHLEVEL) {
            channel->average = (channel->average * channel->avgcount + readvalue) / (channel->avgcount+1);
            valid = 1;
        }

        if (channel->avgstart + channel->avgtime > now) channel->avgcount++;
        channel->last = readvalue;       
    }
    
    usleep(100);    // Need a delay here for reasons... Nope. No idea. HX711 sensitive to timing.
    return valid;
}

void hx711_sleep (struct hx711 *channel)
{
    GPIO_SET = 1<<channel->pd_sck;
    usleep(60);
}