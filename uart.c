#include "synth123.h"

#include <avr/io.h>
#include <util/setbaud.h>
#include <stdint.h>
#include "uart.h"

void uart_init(void)
{
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
    /* Clear the Receive Error Flags. */
#if USE_2X
    UCSRA = _BV(U2X);
#else
    UCSRA = 0;
#endif
    UCSRB = _BV(RXEN);
    UCSRC = _BV(UCSZ1) | _BV(UCSZ0);
}

int16_t uart_receive(void)
{
    uint8_t data;
    uint8_t status;

    /* The Receive Complete flag indicates presence of unread
    data in the receive buffer. */
    if (UCSRA & _BV(RXC)) {
        /* Status must be read before data register UDR,
        since reading it changes buffers. */
        status = UCSRA;
        data = UDR;
        /* FE, DOR and UPE indicate reception error. */
        if (!(status & (_BV(FE) | _BV(DOR) | _BV(UPE)))) {
            return data;
        }
    }
    /* The receive buffer was empty or an error occured. */
    return -1;
}

