#ifndef _SD_H_
#define _SD_H_
#define F_CPU 1000000
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
void SPI_init();
unsigned char spi_rxtx(unsigned char ch);
void USARTWriteChar(char data);
void uwrite_hex(unsigned char n);
char USARTReadChar();
void SD_command(unsigned char cmd, unsigned long arg, unsigned char crc);
void sd_command(unsigned char cmd, unsigned long arg, unsigned char crc);
void sd_init();
void sd_reinit();
void sd_read();
void sd_write(unsigned char value);
#endif
