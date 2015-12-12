
 unsigned char SPIc(unsigned char c)
 // exchange data between SPI master and slave
 // sends char c and returns received data
 {
     //while(MySPITBF);         // Wait for tx buffer to clear
     MySPIBUF = c;            // send character
     while(MySPIRBF ==0 );    // Wait for rx buffer to fill
     return MySPIBUF;        // return received character
 }
 
 unsigned char SPIcdebug(unsigned char c)
 // exchange data between SPI master and slave
 // sends char c and returns received data
 // DEBUG version loads sent and received data in sd_spi_data array
 {
     extern int    *arrayp; // pointer to next location in sd_spi_data array
     unsigned char c2;
     //while(MySPITBF);     // Wait for tx buffer to clear
     MySPIBUF = c;        // send character
     while(MySPIRBF == 0);    // Wait for RBF, tx and rx is complete
     c2 = MySPIBUF;        // store received character
     *(arrayp++) = (int)(c<<8)^(int)(c2&0xFF);        // put pair in array
     return c2;            // return stored character
 }
 
