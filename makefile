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

