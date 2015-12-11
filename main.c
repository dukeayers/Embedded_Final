#include <avr/io.h>
#include "../atmega644/Serial/uart.h"
#include "sd.h"
#include <stdio.h>

unsigned char started = 0;
void start_program();
void print_help();
void invalid_command();
void process_command(char);
void read_block_helper();
char input;

int main()
{
    uart_init();
    SPI_init();
    DDRB = 0xFF;
    PORTB = 0xFF;
    while(1)
    {
        start_program(); // loop until something is received from uart
        print(">> ");
        scanf("%c", &input);
        uart_transmit('\n');
        process_command(input);
    }
}

void process_command(char in) {
    int i = input - '0';
    switch(i) {
        case 0:
            print_help();
            break;
        case 1:
            sd_init();
            break;
        case 2:
            read_block_helper();
            break;
        default:
            invalid_command();
    }
}

void print_help() {
    puts("Command List:");
    puts("0: Print this list");
    puts("1: Initialize sd card");
    puts("2: Read Block");
}

void invalid_command() {
    puts("Invalid command");
}


// Wait for input and set started to true when something is received
void start_program()
{
    if(!started) {
        uart_receive(); // wait for input
        started = 1; // set started to true when done
        print_help();
    }
}

void read_block_helper()
{
    sd_read();
}
