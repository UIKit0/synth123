#include <stdlib.h>
#include <stdint.h>

#include <avr/io.h>
#include "mcp4802.h"
#include "uart.h"

int main(void)
{
    volatile uint16_t i;

    /* BLINK LED FOR A SHORT TIME AT RESET. */
    /* PD0 is output. */
    DDRD = 0x02;
    /* Write data on port. */
    PORTD = 0x00;
    i = 0;
    do {
        i--;
    } while(i);
    PORTD = 0x02;

    dac_init();
    uart_init();
    while(1) {
        int16_t data;
        data = uart_receive();
        if (-1 < data) {
            //dac_write(data, DAC_A);
            dac_raw(data);
        }
    }
    return 0;
}

