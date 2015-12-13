#ifndef __SD_H__
#define __SD_H__
#include "spi.h"
#include <stdio.h>
#define DUMMY 0xFF
#define STUFF 0xFFFFFFFF
#define BYTE(n) 8*n
#define CMD(n) 0x40 | n
#define GO_IDLE_STATE CMD(0)
#define GO_IDLE_STATE_CRC 0x95 
#define SEND_IF_COND CMD(8)
#define SEND_IF_COND_ARG 0x1AA
#define SEND_IF_COND_CRC 0x87
#define SD_SEND_OP_COND CMD(41)
#define SD_SEND_OP_COND_ARG 0x40000000
#define APP_CMD CMD(55)
#define READ_OCR CMD(58)
#define READ_OCR_CRC 0x75
#define READ_SINGLE_BLOCK CMD(17)
#define WRITE_BLOCK CMD(24)
#define SET_BLOCKLEN CMD(16)
#define BLOCKLEN 512
#define START_TOKEN 0xFE

typedef unsigned char Byte;
void sd_clock_sync();
Byte sd_init();
void sd_send_command(Byte cmd, unsigned long arg, Byte crc);
void sd_get_response(Byte *);
#define sd_receive_byte() spi_receive()
#define sd_get_r1_response(var) spi_receive(); var = spi_receive()
Byte sd_read_block(unsigned long address, Byte *buffer);
Byte sd_write_block(unsigned long address, Byte *buffer);
void print_bytes(Byte *buffer, unsigned long start, unsigned long end);
#endif
