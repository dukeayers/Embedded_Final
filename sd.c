#include "sd.h"
void sd_clock_sync()
{
    PORTB |= _BV(SS); // MOSI and SS logic 1
    int i;
    for(i = 0; i < 15; i++)
    {
        spi_send(DUMMY);
    }
    PORTB ^= _BV(SS);
}

Byte sd_init()
{
    // SYNC Clocks
    sd_clock_sync();
    Byte buffer[5];
    buffer[0] = 0;

    // SEND CMD 0
    puts("Sending CMD0");
    Byte i = 0;
    // attempt CMD0 up to 5 times
    while(buffer[0] != 0x01 && i < 5)
    {
        sd_send_command(GO_IDLE_STATE, 0x00, GO_IDLE_STATE_CRC);
        sd_get_response(buffer);
        i++;
    }
    // If unsuccessful then quit
    if(buffer[0] != 1)
        return 0;

    /*
     *SEND CMD 8
     */
    Byte sent58 = 0; // for some process flow later
    puts("Sending CMD8");
    sd_send_command(SEND_IF_COND, SEND_IF_COND_ARG, SEND_IF_COND_CRC);
    sd_get_response(buffer);
    if(buffer[0] & _BV(2))
    {
        // older sd card, skip to cmd58
        puts("Sending CMD58");
        sd_send_command(READ_OCR, STUFF, READ_OCR_CRC);
        sd_get_response(buffer);
        sent58 = 1;
        // TODO check voltage range
    }
    else if(buffer[3] == 0)
    {
        puts("Unusable SD Card");
        return 0;
    }

    i = 0;
    Byte j = 0;
    while(i < 5)
    {
        while(1)
        {
            puts("Sending CMD55");
            sd_send_command(APP_CMD, STUFF, DUMMY);
            sd_get_response(buffer);
            if(buffer[0] == 0x01)
                break;
            j++;
            if(j > 5)
            {
                puts("SD Card stopped responding");
                return 0;
            }
        }
        // determine if the arg should have HCS or not
        long arg = sent58 ? 0 : SD_SEND_OP_COND_ARG;
        puts("Sending ACMD41");
        sd_send_command(SD_SEND_OP_COND, arg, DUMMY);
        sd_get_response(buffer);
        if(buffer[0] == 0x01)
            break;
        i++;
    }
    if(i >= 5)
    {
        puts("SD Card timed out");
        return 0;
    }

    if(!sent58)
    {
        puts("Sending CMD58");
        sd_send_command(READ_OCR, STUFF, READ_OCR_CRC);
        sd_get_response(buffer);
        if(buffer[4] & _BV(1))
            puts("High Capacity");
        else
            puts("Standard Capacity");
    }

    sd_send_command(SET_BLOCKLEN, BLOCKLEN, DUMMY); 
    sd_get_response(buffer);
    return 1;
}


void sd_send_command(Byte cmd, unsigned long arg, Byte crc)
{
    printf("Sending : %02X %02X %02X %02X %02X %02X\n", 
            cmd, 
            (Byte) (arg >> BYTE(3)), 
            (Byte) (arg >> BYTE(2)), 
            (Byte) (arg >> BYTE(1)), 
            (Byte) arg, 
            crc);

    spi_send(cmd); // send the command byte
    spi_send(arg >> BYTE(3)); // send the first byte of the arg
    spi_send(arg >> BYTE(2)); // second byte
    spi_send(arg >> BYTE(1)); // third byte
    spi_send(arg);            // last byte
    spi_send(crc);
}

void sd_get_response(Byte *buffer)
{
    // skip the first byte
    // it shouldn't have the response
    sd_receive_byte(); 
    buffer[0] = sd_receive_byte();
    buffer[1] = sd_receive_byte();
    buffer[2] = sd_receive_byte();
    buffer[3] = sd_receive_byte();
    buffer[4] = sd_receive_byte();
    printf("Response: %02X %02X %02X %02X %02X\n", 
            buffer[0],
            buffer[1],
            buffer[2],
            buffer[3],
            buffer[4]);
}

Byte sd_read_block(unsigned long address, Byte *buffer)
{
    sd_send_command(READ_SINGLE_BLOCK, address, DUMMY);
    int i;
    for(i = 0; i < 20; i++)
    {
        buffer[0] = sd_receive_byte();
        if(buffer[0] == 0xFE) // data start token received
            break;
    }
    if(buffer[0] != 0xFE)
    {
        return 0;
    }
    // get the data from the block
    for(i = 0; i < BLOCKLEN; i++) 
        buffer[i] = sd_receive_byte();
    // print block data
    unsigned int max = BLOCKLEN/16;
    for(i = 0; i < max; i++)
    {
        printf("%2d: ", i);
        print_bytes(buffer, i*16, i*16+16);
        puts("");
    }
    unsigned char k = spi_receive();
    unsigned char j = spi_receive();
    printf("CRC: %02X %02X\n", k, j);
    return 1;
}

Byte sd_write_block(unsigned long address, Byte *buffer)
{
    sd_send_command(WRITE_BLOCK, address, DUMMY);   
    int i;
    for(i = 0; i < 20; i++)
    {
        buffer[0] = sd_receive_byte();
        printf("%02X ", buffer[0]);
        if(buffer[0] == 0x01 || buffer[0] == 0x00)
            break;
    }
    if(buffer[0] != 0x01 && buffer[0] != 0x00)
    {
        return 0;
    }
    int j;
    puts("Sending data");
    spi_send(START_TOKEN);
    for(j = 0; j < BLOCKLEN+1; j++)
    {
        spi_send(buffer[j]);
    }
    Byte response_buffer[5];
    sd_get_response(response_buffer);
    sd_get_response(response_buffer);
    return 1;
}

void print_bytes(Byte *buffer, unsigned long start, unsigned long end)
{
    unsigned long tmp;
    for(tmp = start; tmp < end; tmp++)
    {
        printf("%02X ", buffer[tmp]);
    }
}
