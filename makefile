DEVICE=atmega644
FILES=uart main spi sd
COMPILE=$(foreach file, $(FILES), avr-gcc -mmcu=$(DEVICE) -c $(file).c -Iheaders -o $(file).o; )
BUILD=avr-gcc -mmcu=$(DEVICE) $(foreach file, $(FILES), $(file).o ) -o main
build:
	$(COMPILE)
	$(BUILD)
install: build
	avr-objcopy -j .text -j .data -O ihex main main.hex
	avrdude -c usbtiny -p atmega644 -U flash:w:main.hex

oc_fuses:
	avrdude -c usbtiny -p atmega644 -U SUTCKSEL:w:EXTX0SC_8MHZ_XX_1KCK_4MS1:m -U hfuse:w:0x91:m -U efuse:w:0xFF:m -U lfuse:w:0xFE:m 
