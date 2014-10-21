MCU=attiny85
TARGET=main
ASRC=$(TARGET).asm

AS=avra
MICRONUCLEUS=micronucleus

all:
	$(AS) $(ASRC)

upload: all
	$(MICRONUCLEUS) $(TARGET).hex

clean:
	rm *.hex *.obj *.cof
