PROJECTNAME = synth123
TARGET = $(PROJECTNAME).out
DUMPTARGET = $(PROJECTNAME).s

OPTLEVEL = 2
CFILES = main.c mcp4802.c uart.c

OBJECTS = $(CFILES:.c=.o)
LST = $(filter %.lst, $(OBJECTS:.o=.lst))
LDFLAGS = -mmcu=$(MCU) -Wl,-Map,$(TARGET).map -Wl,--gc-sections 
MCU = attiny4313
CC = avr-gcc
OBJDUMP = avr-objdump
SIZE = avr-size

CFLAGS =	-mmcu=$(MCU) -std=c99 -g \
         	-O$(OPTLEVEL) \
         	-Wall -Wextra -Wstrict-prototypes -Werror \
         	-pedantic $(INC) \
			-fpeel-loops \
			-fdata-sections -ffunction-sections \
		 	-Wa,-ahlms=$(firstword $(filter %.lst, $(<:.c=.lst)))

.PHONY: d s program gdbserver gdb clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

$(OBJECTS): $(DEPS)

d: $(DUMPTARGET) s

#size: $(TARGET) $(OBJECTS)
#	$(SIZE) $(TARGET) $(OBJECTS)

$(DUMPTARGET): $(TARGET) 
	$(OBJDUMP) -S  $< > $@

s: $(TARGET)
#	$(OBJDUMP) -h $(TARGET)
	$(SIZE) $(OBJECTS) $(TARGET)

program: $(TARGET)
	sudo avrdude -P usb -c dragon_isp -b 2000000 -p t4313 -U $(TARGET)

gdbserver: $(TARGET)
	sudo avarice -g --erase --program --file $(TARGET) localhost:1212

gdb: $(TARGET)
	avr-gdb -x gdbinit

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).map $(DUMPTARGET) $(LST)

# Every non-.PHONY rule must update a file with the exact
# name of its target.  Make sure every command script touches
# the file "$@"-- not "../$@", or "$(notdir $@)", but exactly
# $@. That way you and GNU make always agree.
# http://mad-scientist.net/make/rules.html

