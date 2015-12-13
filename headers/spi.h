#ifndef __SPI_H__
#define __SPI_H__
#include <avr/io.h>
#include <avr/interrupt.h>
#define SS 4
#define MOSI 5
#define SCK 7

// wrappers for send and receive
#define spi_send(data) spi_rxtx(data)
#define spi_receive() spi_rxtx(0xFF)

void spi_init();
unsigned char spi_rxtx(unsigned char data);
#endif
