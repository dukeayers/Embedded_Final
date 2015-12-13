#include "spi.h"

void spi_init()
{ 
    // set mosi and sck as output pins
    DDRB = _BV(SS)|_BV(MOSI)|_BV(SCK);
    // enables spi, set master, FCPU 1/4
    SPCR = _BV(SPE)|_BV(MSTR); 
    sei();
}

// Send and receive a byte
unsigned char spi_rxtx(unsigned char data)
{
    SPDR = data;
    unsigned char in;
    while(!(SPSR & (1<< SPIF)));
    in = SPDR;
    return in;
}
