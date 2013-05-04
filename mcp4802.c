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

void dac_write_fast(uint8_t data, uint8_t config) {
    uint8_t i;

    dac_select();
    /* MSB is sent first. */
    /* (/A)/B, --, /GA, /SHDN, D7, D6, D5, D4 */
    USIDR = config | data >> 4;
    for(i = 0; i < 8; i++) {
        USICR = _BV(USIWM0) | _BV(USITC);
        USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
    }

    /* D3, D2, D1, D0, x, x, x, x */
    USIDR = data << 4;
    for(i = 0; i<8;i++) {
        USICR = _BV(USIWM0) | _BV(USITC);
        USICR = _BV(USIWM0) | _BV(USITC) | _BV(USICLK);
    }
    dac_deselect();
}

