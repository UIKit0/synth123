#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>

#include "debug.h"
#include "uart.h"
#include "midi.h"
#include "util.h"

/* Double buffer so that we can give the user one midi_msg and work on the
other.*/
static struct midi_msg msgs[2];
static struct midi_msg *msg_driver;
static struct midi_msg *msg_user;

/* One running status is stored per channel. It is updated
when a MIDI message of type "channel message" arrives. */
static uint8_t running_status;


static uint8_t is_status(uint8_t target)
{
    return 0x80 & target;
}

static uint8_t is_data(uint8_t target)
{
    return !is_status(target);
}

static uint8_t is_channel(uint8_t status)
{
    return ((0x80 <= status) && (status <= 0xef));
}

static uint8_t is_sysex(uint8_t status)
{
    return (status == 0xf0);
}

static uint8_t is_system_common(uint8_t status)
{
    return ((0xf1 <= status) && (status <= 0xf7));
}

static uint8_t is_system_real_time(uint8_t status)
{
    return (0xf8 <= status);
}

/* This works for channel messages. */
static uint8_t ndata(uint8_t token)
{
    switch (token & 0xf0) {
        case 0x80:
        case 0x90:
        case 0xa0:
        case 0xb0:
        case 0xe0:
            return 2;
    }
    return 1;
}

/* parse: emit channel message.  The function aims to be
robust and comply with the "MIDI 1.0 Detailed Specification,
document version 4.2". A non-zero value is returned when a
midi message is ready. */
static uint8_t parse(uint8_t token)
{
    static uint8_t index;
    static uint8_t index_end;

    if (is_data(token)) {
        if (is_channel(running_status)) {
            /* Install this data byte. */
            if (!index) {
                msg_driver->data1 = token;
            } else {
                msg_driver->data2 = token;
            }
            dprint("data%d <- 0x%x\n", index, token);
            /* Swap between data1 and data2. */
            if (index == index_end) {
                /* We are done with this message. Install running status. */
                msg_driver->status = running_status;
                /* The data bytes are already there. */
                index = 0;
                return 1;
            } else {
                /* Increase index. */
                index = 1;
            }
        }
        /* Drop SYSEX/SYSCOM data */

    } else if (is_channel(token)) {
        running_status = token;
        /* Start building a new channel message. */
        index = 0;
        index_end = ndata(token);
        dprint("Channel message 0x%x (%d data bytes).\n",
          token, index_end);
    } else if (is_system_real_time(token)) {
        dprint("Real-time message 0x%x\n", token);
        /* IGNORE. Consider moving it to a higher priority if used. */
    } else if (is_system_common(token) ||
      is_sysex(token)) {
        /* Forget running status. */
        running_status = 0;
    } else {
        dprint("Token was not recognized: 0x%x\n", token);
    }
    return 0;
}


void midi_init(void)
{
    msg_driver = msgs;
    running_status = 0;
}

/* Fetch bytes from uart and break when the buffer is empty or
when a MIDI message is found. The parse() function is called
for every byte. */
struct midi_msg *midi_msg_get(void) {
    struct midi_msg *m;
    int16_t token;

    token = uart_receive();
    while(-1 < token) {
        if (parse(token)) {
            /* Update buffer pointers. */
            m = msg_driver;
            msg_driver = msg_user;
            msg_user = m;
            return m;
        }
        token = uart_receive();
    }
    return NULL;
}

uint8_t midi_msg_is_note_off(struct midi_msg *m)
{
    /* Either "note off" message or "note on" with zero velocity. */
    return (0x80 == (m->status & 0xf0))
      || ((0x90 == (m->status & 0xf0)) && (m->data2 == 0x00));
}

uint8_t midi_msg_is_note_on(struct midi_msg *m)
{
    return (0x90 == (m->status & 0xf0)) && (m->data2 != 0x00);
}

uint8_t midi_msg_is_control(struct midi_msg *m)
{
    return (0xb0 == (m->status & 0xf0)) && (m->data1 <= 119);
}

uint8_t midi_msg_is_mode(struct midi_msg *m)
{
    return (0xb0 == (m->status & 0xf0)) && (120 <= m->data1);
}

uint8_t midi_msg_is_program_change(struct midi_msg *m)
{
    return (0xc0 == (m->status & 0xf0));
}

