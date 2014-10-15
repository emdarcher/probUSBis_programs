
MCU=attiny85

F_CPU=16500000

## This is where your main() routine lives 
MAIN = main.c

## If you've split your program into multiple files, 
## include the additional .c source (in same directory) here 
## (and include the .h files in your foo.c)
LOCAL_SOURCE = 

## Here you can link to one more directory (and multiple .c files)
EXTRA_SOURCE_DIR = ./libs/
EXTRA_SOURCE_FILES =

## Defined programs / locations
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

MICRONUCLEUS = micronucleus

## Compilation options, type man avr-gcc if you're curious.
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU)UL -DBAUD=$(BAUD) -Os -I. -I$(EXTRA_SOURCE_DIR)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -g -ggdb
CFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--relax
CFLAGS += -std=gnu99
## CFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## CFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf

## Lump target and extra source files together
TARGET = $(strip $(basename $(MAIN)))
SRC = $(TARGET).c
EXTRA_SOURCE = $(addprefix $(EXTRA_SOURCE_DIR), $(EXTRA_SOURCE_FILES))
SRC += $(EXTRA_SOURCE) 
SRC += $(LOCAL_SOURCE) 

## List of all header files
HEADERS = $(SRC:.c=.h) 

## For every .c file, compile an .o object file
OBJ = $(SRC:.c=.o) 

## Generic Makefile targets.  (Only .hex file is necessary)
all: $(TARGET).hex

%.hex: %.elf
	$(OBJCOPY) -R .eeprom -O ihex $< $@

%.elf: $(SRC)
	$(CC) $(CFLAGS) $(SRC) --output $@ 

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ 

debug:
	@echo
	@echo "Source files:"   $(SRC)
	@echo "MCU, F_CPU, BAUD:"  $(MCU), $(F_CPU), $(BAUD)
	@echo	

# Optionally create listing file from .elf
# This creates approximate assembly-language equivalent of your code.
# Useful for debugging time-sensitive bits, 
# or making sure the compiler does what you want.
disassemble: $(TARGET).lst

disasm: disassemble

eeprom: $(TARGET).eeprom

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

# Optionally show how big the resulting program is 
size:  $(TARGET).elf
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom

squeaky_clean:
	rm -f *.elf *.hex *.obj *.o *.d *.eep *.lst *.lss *.sym *.map *~



#micronucleus stuff

upload: $(TARGET).hex
	$(MICRONUCLEUS) $<


