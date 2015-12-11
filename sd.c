#include "sd.h"
//Module added for project 3
//Defines used in sd spi communcation taken from
//Taken from 
//http://codeandlife.com/2012/04/25/simple-fat-and-sd-tutorial-part-3/
#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define MOSI (1<<PB5)
#define MISO (1<<PB6)
#define SCK (1<<PB7)

#define CS_DDR DDRB
#define CS (1<<PB4)
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)

//Defines to prevent hard coding 
#define RESPONSE 8
#define STUFFING 0x00
#define BLOCK 512
#define FF 0xFF

// Taken from 
// http://www.openbeacon.org/git-view/openbeacon/plain/firmware/at91sam7/openbeacon-openpicc2/application/dosfs/sdcard.c?id=v0.0.6
// Deleted some things not used
/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define CMD41	(0x40+41)
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

//Taken from http://codeandlife.com/2012/04/25/simple-fat-and-sd-tutorial-part-3/
// Similar to provided spi initalization
void SPI_init() {
	CS_DDR |= CS; // SD card circuit select as output
	SPI_DDR |= MOSI + SCK; // MOSI and SCK as outputs
	SPI_PORT |= MISO; // pullup in MISO, might not be needed
	
	// Enable SPI, master, set clock rate fck/128
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
}

unsigned char spi_rxtx(unsigned char ch) {
	SPDR = ch;
	while(!(SPSR & (1<<SPIF))) {}
	return SPDR;
}

void USARTWriteChar(char data) { // blocking
	while(!(UCSR0A & (1<<UDRE0))) {}
	UDR0=data;
}
void uwrite_hex(unsigned char n) {
	if(((n>>4) & 15) < 10)
	USARTWriteChar('0' + ((n>>4)&15));
	else
	USARTWriteChar('A' + ((n>>4)&15) - 10);
	n <<= 4;
	if(((n>>4) & 15) < 10)
	USARTWriteChar('0' + ((n>>4)&15));
	else
	USARTWriteChar('A' + ((n>>4)&15) - 10);
}
char USARTReadChar() { // blocking
	while(!(UCSR0A & (1<<RXC0))) {}
	return UDR0;
}
// End of unmodifed code from
//Taken from http://codeandlife.com/2012/04/25/simple-fat-and-sd-tutorial-part-3/

//This is a modification of SD command code 
//Taken from http://codeandlife.com/2012/04/25/simple-fat-and-sd-tutorial-part-3/
//It incorporates above uart functions as well as the uart module
// This function is used to send commands over spi to the sd card 
void SD_command(unsigned char cmd, unsigned long arg, unsigned char crc) {

	//Declare and set characters used in responses	
	unsigned char read = RESPONSE; 
	unsigned char i, buffer[RESPONSE];

	//Infor for display of command and the writes the hex value	
	printf("Sending Command: ");
	uwrite_hex(cmd);
	
	//Set chip select for spi communication
	CS_ENABLE();
	
	//Sends comman byte
	spi_rxtx(cmd);
	printf(" ");
	
	//Sends arguments/stuffing
	uwrite_hex(arg>>24);
	printf(" ");
	spi_rxtx(arg>>24);
	uwrite_hex(arg>>16);
	printf(" ");
	spi_rxtx(arg>>16);
	uwrite_hex(arg>>8);
	printf(" ");
	spi_rxtx(arg>>8);
	uwrite_hex(arg);
	printf(" ");
	spi_rxtx(arg);
	
	//Sends the CRC
	uwrite_hex(crc);
	printf("\r\n");
	spi_rxtx(crc);

	//Reads the response into a buffer
	for(i=0; i<read; i++)
		buffer[i] = spi_rxtx(FF);
	//Disable chip select
	CS_DISABLE();
	
	//Prints out the response received
	printf("Response:");	
	for(i=0; i<read; i++) {
		printf(" ");
		uwrite_hex(buffer[i]);
	}	
	printf("\r\n");
}

//This is simply the above function implemented without printed out information
// Used to reinitialize after a write 
void sd_command(unsigned char cmd, unsigned long arg, unsigned char crc) {
	
	unsigned char read = RESPONSE;
	unsigned char i, buffer[8];
		
	CS_ENABLE();
	
	spi_rxtx(cmd);
	spi_rxtx(arg>>24);
	spi_rxtx(arg>>16);	
	spi_rxtx(arg>>8);	
	spi_rxtx(arg);
	spi_rxtx(crc);

	for(i=0; i<read; i++)
		buffer[i] = spi_rxtx(FF);
	
	CS_DISABLE();
}

// Function that initalizes the sd card 
void sd_init(){
	// declaration of counter variable
	char i;
	//Initialize local spi function
	SPI_init();
	// Disable Chip select
	CS_DISABLE();
	//Wake up the SD card
	for(i=0; i<10; i++) // idle for 1 bytes / 80 clocks
		spi_rxtx(FF);

	// Send command 0 to get SD talking with standard crc
	SD_command(CMD0, 0x00000000, 0x95);

	//Sends command 8 with standard crc
	SD_command(CMD8, 0x000001AA, 0x87);
	_delay_ms(100);

	// Send command equivalent of ACMD41
	// Delay added as SD card can take up to a second to respond				
	for(i=0;i<3;i++){
		SD_command(CMD55, 0x40000000, FF);
		SD_command(CMD41, 0x40000000, FF);
		_delay_ms(200);
				}

	//Send command 58 and the set block length
	SD_command(CMD58, STUFFING, FF);
	SD_command(CMD16, BLOCK, FF);

}

//Used to silently reinitialize the sd card after a write
void sd_reinit(){
	char i;
	SPI_init();
	CS_DISABLE();
	for(i=0; i<10; i++) // idle for 1 bytes / 80 clocks
	spi_rxtx(FF);

	sd_command(CMD0, STUFFING, 0x95);
	
	sd_command(CMD8, 0x000001AA, 0x87);
	_delay_ms(100);
	
	for(i=0;i<3;i++){
		sd_command(CMD55, 0x40000000, FF);
		sd_command(CMD41, 0x40000000, FF);
		_delay_ms(200);
	}

	sd_command(CMD58, STUFFING, FF);

	sd_command(CMD16, BLOCK, FF);

}

//Reads the bytes of the SD cards first sector
void sd_read(){
	
	// creates variable declarations and counters
	unsigned long address = 512;
	unsigned short i;

	// Sends the command for read then prints it out formatted as the 
	// initialization function
	printf("Sending Command: ");
	uwrite_hex(CMD17);
	CS_ENABLE();
	
	spi_rxtx(CMD17);
	printf(" ");
	
	uwrite_hex(address>>24);
	printf(" ");
	spi_rxtx(address>>24);
	uwrite_hex(address>>16);
	printf(" ");
	
	spi_rxtx(address>>16);
	
	uwrite_hex(address>>8);
	printf(" ");
	spi_rxtx(address >> 8);
	
	uwrite_hex(address);
	printf(" ");
	spi_rxtx(address);
	
	uwrite_hex(FF);
	printf("\r\n");
	spi_rxtx(FF);
	
	//Prints out the response
	printf("Response: ");
	
	unsigned char received = 0x00;
	while(received!=0xFE){
		received=spi_rxtx(FF);
		uwrite_hex(received);
		printf(" ");
	}

	//Prints out the bytes read from the sd card in hex	
	printf("\r\nHere is the read block:\r\n");
	
	i=0;
	while(i<BLOCK)
	{
		uwrite_hex(spi_rxtx(FF));
		i++;
		printf(" ");
	}
	CS_DISABLE();
	printf("\r\n");
}

//Perform the write operation
void sd_write(unsigned char value){
	
	unsigned char response;
	CS_ENABLE();
	//Sends command to start write
	spi_rxtx(CMD24);
	//dummy data
	spi_rxtx(STUFFING);
	spi_rxtx(STUFFING);
	spi_rxtx(STUFFING);
	spi_rxtx(STUFFING);
	spi_rxtx(STUFFING);

	spi_rxtx(FF);
	
	//response= spi_rxtx(FF);
	
	
	// send the data to be written to sd 
        int i;
	for(i=0;i<BLOCK;i++){
		response = spi_rxtx(value);
	}
	// Disable chip select	
	CS_DISABLE();
	
	// re initialize sd		
	sd_reinit();	
}
