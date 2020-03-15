//
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//  Revised: 15-Feb-2013


// Access from ARM Running Linux

#define BCM2708_PERI_BASE        0x20000000    // Raspberry Pi 1
// #define BCM2708_PERI_BASE        0x3f000000  Raspberry Pi 2-4
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define INT_BASE 0x2000B000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/mman.h>
#include <unistd.h>
#include "rpio.h"

//
// Set up a memory regions to access GPIO
//
void setup_io()
{
    if (getuid()) {
        printf ("This program needs to be run as root.\n");
        exit (1);
    }
    
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );
    
    int_map = mmap(
        NULL,
        4096,
        PROT_READ|PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        INT_BASE
    );
 
    close(mem_fd); //No need to keep mem_fd open after mmap

    if (int_map == MAP_FAILED) {
        printf ("int mmap error %d\n", (int)int_map);
        exit (-1);
    }

   if (gpio_map == MAP_FAILED) {
      printf("gpio mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

    // Always use volatile pointer!
    gpio = (volatile unsigned *)gpio_map;
    intrupt = (volatile unsigned *)int_map;

} // setup_io

// https://www.raspberrypi.org/forums/viewtopic.php?t=52393
/******************** INTERRUPTS *************

Is this safe?
Dunno, but it works

interrupts(0)   disable interrupts
interrupts(1)   re-enable interrupts

return 0 = OK
       1 = error with message print

Uses intrupt pointer set by setup()
Does not disable FIQ which seems to
cause a system crash
Avoid calling immediately after keyboard input
or key strokes will not be dealt with properly

*******************************************/

int interrupts(int flag)
  {
  static unsigned int sav132 = 0;
  static unsigned int sav133 = 0;
  static unsigned int sav134 = 0;

  if(flag == 0)    // disable
    {
    if(sav132 != 0)
      {
      // Interrupts already disabled so avoid printf
      return(1);
      }

    if( (*(intrupt+128) | *(intrupt+129) | *(intrupt+130)) != 0)
      {
//      printf("Pending interrupts\n");  // may be OK but probably
      return(1);                       // better to wait for the
      }                                // pending interrupts to
                                       // clear

    sav134 = *(intrupt+134);
    *(intrupt+137) = sav134;
    sav132 = *(intrupt+132);  // save current interrupts
    *(intrupt+135) = sav132;  // disable active interrupts
    sav133 = *(intrupt+133);
    *(intrupt+136) = sav133;
    }
  else            // flag = 1 enable
    {
    if(sav132 == 0)
      {
 //     printf("Interrupts not disabled\n");
      return(1);
      }

    *(intrupt+132) = sav132;    // restore saved interrupts
    *(intrupt+133) = sav133;
    *(intrupt+134) = sav134;
    sav132 = 0;                 // indicates interrupts enabled
    }
  return(0);
  }

void set_priority() {
    struct sched_param sched;

    memset (&sched, 0, sizeof(sched));

    sched.sched_priority = 10;
//   if (sched_setscheduler (0, SCHED_FIFO, &sched)) printf ("Warning, unable to set priority\n");
}