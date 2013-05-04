#include <stdlib.h>
#include <stdint.h>

#include <avr/io.h>
#include "mcp4802.h"

int main(void)
{
    volatile uint16_t i;

    /* PD0 is output. */
    DDRD = 1;
    /* Write data on port. */
    PORTD = 0x00;
    i = 0;
    do {
        i--;
    } while(i);
    PORTD = 0x01;

    dac_init();
    while(1) {
        /* Output pattern 1001 1100. */
        dac_write_fast(0x5C, DAC_A);
        /* Output pattern 1010 1001. */
        dac_write_fast(0x69, DAC_B);
    }
    return 0;
}

