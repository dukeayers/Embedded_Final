all:
	avr-gcc -mmcu=atmega644 -O -c main.c -o main.o
	avr-gcc -mmcu=atmega644 -O -c ../atmega644/WithStdio/uart.c -o uart.o
	avr-gcc -mmcu=atmega644 -O -c sd.c -o sd.o
	avr-gcc -mmcu=atmega644 -O uart.o main.o sd.o -o main
	rm main.o
	rm uart.o
	rm sd.o

install:
	avr-objcopy -j .text -j .data -O ihex main main.hex
	avrdude -c usbtiny -p atmega644 -U flash:w:main.hex

