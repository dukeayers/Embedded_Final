#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#include "spi.h"
#include "sd.h"

void start_program();
void process_command(unsigned char in);
void init_sd(); 
void read_block();
void write_block();

int main()
{
    uart_init(); // initialize uart
    spi_init(); // initialize spi
    unsigned char in;
    // wait for uart to receive something before beginning
    start_program();
    for(;;)
    {
        printf(">> ");
        scanf("%c", &in);
        printf("\n");
        process_command(in);
    }
}

void start_program()
{
    char buf;
    scanf("%c", &buf);
    printf("\r");
}

void process_command(unsigned char in)
{
    switch(in)
    {
        case '1':
            init_sd();
            break;
        case '2':
            read_block();
            break;
        case '3':
            write_block();
            break;
        default:
            puts("Invalid Command");
    }
}

void init_sd()
{
    puts("Initializing SD Card");
    Byte result = sd_init();
    if(result)
        puts("SD Card initialized");
    else
        puts("Failed to initialize sd card");

}

void read_block()
{
    unsigned char buffer[512];
    printf("Address: ");
    scanf("%s", buffer);
    printf("\n");
    unsigned long addr = atoi((char*)buffer);
    Byte result = sd_read_block(addr, buffer);
    if(!result)
        result = sd_read_block(addr, buffer);
    if(!result)
        puts("failed to read block");
    getchar();
    fflush(stdin); // clean stdin
}

void write_block()
{
    unsigned char buffer[BLOCKLEN];
    printf("Address: ");
    scanf("%s", buffer);
    puts("");
    unsigned long addr = atoi((char*)buffer);
    printf("Value: ");
    scanf("%s", buffer);
    Byte i = atoi((char*)buffer);
    int j;
    for(j = 0; j < BLOCKLEN; j++)
    {
        buffer[j] = i;
    }
    Byte result = sd_write_block(addr, buffer);
    if(!result)
        result = sd_write_block(addr, buffer);
    if(!result)
        puts("failed to write block");
    getchar();
}
