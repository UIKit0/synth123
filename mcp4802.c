#include <stdint.h>
#include <avr/io.h>

#include "mcp4802.h"

/* USI definitions not in the avr-libc files */
#define USCK_BIT    SCK_BIT

/* Definitions for mapping MCP4802 to USI interface */
#define CSn_PORT    PORTB
#define CSn_BIT     DI_BIT

static void dac_select(void)
{
    CSn_PORT &= ~(_BV(CSn_BIT));
}

static void dac_deselect(void)
{
    CSn_PORT |= _BV(CSn_BIT);
}

void dac_init(void)
{
    /* Enable output by setting corresponding DDR bits. */
    DDRB |= (_BV(USCK_BIT) | _BV(DO_BIT) | _BV(CSn_BIT));
    dac_deselect();
}

void dac_write(uint8_t data, uint8_t config) {
    /* Bit7 is sent first. */
    /* (/A)/B, --, /GA, /SHDN, D7, D6, D5, D4 */
    USIDR = config | data >> 4;
    /* Clear USI Counter Overflow Flag and USI counter value. */
    USISR = _BV(USIOIF);
    dac_select();

    /* USIOIF is set in USISR when transfer is complete. */
    while(0 == (_BV(USIOIF) & USISR)) {
        /* 3W mode, positive edge clock, count at USITC strobe,
        toggle USCK. External clock mode 0: USICS0 = 0. DO is
        changed at negative clock edge. */
        USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK) |
          _BV(USITC);
    }

    /* D3, D2, D1, D0, x, x, x, x */
    USIDR = data << 4;
    USISR = _BV(USIOIF);

    while(0 == (_BV(USIOIF) & USISR)) {
        USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK) |
          _BV(USITC);
    }
    dac_deselect();
}

/*
 * Compare the CPU time needed for manually toggling USCK vs using
 *  Counter0
 *  USICR: 2
 *  Enter ISR: 4
 *  Save r0, r1: 2
 *  SREG: 1
 *  Save reg: 1 ???
 *  USICR: 2
 *  Restore reg: 1
 *  SREG: 2
 *  Restore r0, r1: 2
 *  RTI: 4
 *
 *  Software solution: 2 + 16 + 16 = 34
 *
 *  variant:
 *  At sample time interrupt (or poll timer?):
 *      Start transfer of first octet and enable USI interrupt
 *      Clock source should be "Timer/Counter0 Compare Match"
 *  Calculate sample
 *  At USI interrupt:
 *      Start transfer of second octet and disable interrupt.
 *  Continue calculate sample.
*/
