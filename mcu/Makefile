# avr-gcc to complie and link
# avr-libc for C libraries
# avrdude to upload

CC = avr-gcc
LD = $(CC)
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

TARGET = obj/mcu

override CFLAGS = -c -O -g -c -ffreestanding -funsigned-char -funsigned-bitfields -fshort-enums -fpack-struct -mmcu=$(MCU_TARGET)
# -g : avr-gdb
# -ffreestanding : turn off builting functions

override LDFLAGS = -Wl,-Map,$(TARGET).map

MCU_TARGET = atmega328p
UPFLAGS = -p m328p

LIB = 
INC = 
HEADERS = 
OBJS = obj/mcu.o
SRCS = src/mcu.c

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ $< $(INC) $(LIB)

all: $(TARGET).elf text eeprom


$(TARGET).elf:  $(OBJS) $(HEADERS)
	$(CC) $(CDFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(INC) $(LIB)


# Rules for building the .text rom images
text: lst hex bin srec

lst: $(TARGET).lst
hex:  $(TARGET).hex
bin:  $(TARGET).bin
srec: $(TARGET).srec

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images

eeprom: ehex ebin esrec

ehex:  $(TARGET)_eeprom.hex
ebin:  $(TARGET)_eeprom.bin
esrec: $(TARGET)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
        || { echo empty $@ not generated; exit 0; }

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ \
        || { echo empty $@ not generated; exit 0; }

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
        || { echo empty $@ not generated; exit 0; }
 
clean:
	rm obj/*

.phony: all clean lst hex bin srec ehex ebin esrec text eeprom
