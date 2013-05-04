#include <stdlib.h>
#include <stdint.h>

#include <avr/io.h>
#include "mcp4802.h"

int main(void)
{
    volatile unsigned long int i;

    /* PD0 is output. */
    DDRD = 1;
    /* Write data on port. */
    i = 0;

    PORTD = 0x00;
    while(i < 80000) {
        i++;
    }
    PORTD = 0x01;

    dac_init();
    while(1) {
        /* Output pattern 1010 1100. */
        dac_write(0x5C,
 DAC_A);
        /* Output pattern 1010 1001. */
        dac_write(0x59,
     DAC_B);
    }
    return 0;
}

