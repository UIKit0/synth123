TARGET = synth123.out
CFILES = main.c mcp4802.c uart.c
OBJECTS = $(CFILES:.c=.o)
OPTLEVEL=0
LDFLAGS = -mmcu=$(MCU) -Wl,-Map,$(TARGET).map
VPATH = ..
MCU = attiny4313
CC = avr-gcc

CFLAGS = -mmcu=$(MCU) -std=c99 -g \
         -O$(OPTLEVEL) \
         -Wall -Wextra -Wstrict-prototypes -Werror \
         -pedantic $(INC)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

$(OBJECTS): $(DEPS)

size: $(TARGET) $(OBJECTS)
	avr-size $(TARGET) $(OBJECTS)

program: $(TARGET)
	sudo avrdude -p t4313 -c dragon_isp -P usb -U $(TARGET)

gdbserver: $(TARGET)
	sudo avarice -g --erase --program --file $(TARGET) localhost:1212

gdb: $(TARGET)
	avr-gdb -x gdbinit

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).map

# Every non-.PHONY rule must update a file with the exact
# name of its target.  Make sure every command script touches
# the file "$@"-- not "../$@", or "$(notdir $@)", but exactly
# $@. That way you and GNU make always agree.
# http://mad-scientist.net/make/rules.html

