#include <stdlib.h>
#include <stdint.h>

#include <avr/io.h>
#include "mcp4802.h"
#include "uart.h"
#include "midi.h"

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
    midi_init();

    struct midi_msg *m;
    while(1) {
        m = midi_msg_get();
        if (NULL != m) {
            if (midi_msg_is_note_on(m)) {
                dac_write(m->data1, DAC_A);
            }
        }
    }
}

