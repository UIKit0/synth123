/* Represent a MIDI message. data1 and data2 has different
meaning depent on status.*/

struct midi_msg {
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
};

/* Initialize internal midi structures. This function must be
called before any other function in this module. It can also
be called at any time to reset the midi module. */
void midi_init(void);

/* Get user midi message.  The caller owns the data pointed
to by midi_msg until next call of midi_msg_get(). NULL is
returned if there is no unread midi message available. */
struct midi_msg *midi_msg_get(void);

uint8_t midi_msg_is_note_off(struct midi_msg *msg);
uint8_t midi_msg_is_note_on(struct midi_msg *msg);
uint8_t midi_msg_is_control(struct midi_msg *msg);
uint8_t midi_msg_is_program_change(struct midi_msg *msg);
uint8_t midi_msg_is_mode(struct midi_msg *msg);

