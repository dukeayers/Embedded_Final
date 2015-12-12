#define DBUG     1
 #define NODBUG    0
 
 
 struct command_fields {
     unsigned         end_bit:1;            // always 1
     unsigned         crc:7;                // CRC
     unsigned long    argument;            // 32 bit argument
     unsigned          index:6;            // command index
     unsigned          transmitter_bit: 1;    // always 1
     unsigned          start_bit: 1;        // always 0
 };    
 struct command_bytes {
     unsigned char    crc_w_stop;            // crc with stop bit
     unsigned char    do_not_send;        // filler to account for compiler TODO: Why?
     unsigned char    arg_lsb;            // least significant argument byte
     unsigned char    arg_3;                
     unsigned char    arg_2;            
     unsigned char    arg_msb;            // most sign byte of arb
     unsigned char    cmd_w_starttx;        // command with start and tx bits
 };    
 union sd_command {
     struct command_fields fields;
     unsigned char bytewise [7];
     struct command_bytes bytes;
 };
 
 #define GO_IDLE_STATE         0             // CMD0 - resets card
 #define SEND_OP_COND        1            // CMD1 - send operating condition
 #define SEND_IF_COND        8            // CMD8 - send interface condition
 #define SEND_CSD            9            // CMD9 - send card status
 #define SET_BLOCKLEN        16            // CMD16 - set blocklength
 #define WRITE_SINGLE_BLOCK    24            // CMD24 - write (single) block
 #define APP_CMD                55            // CMD55 - next command is an application specific command, not standard
 #define READ_OCR            58            // CMD58 - read the ocr register
 #define CRC_ON_OFF            59            // CMD59 - turns crc on or off
 
 #define SD_STATUS            13            // ACMD13 - send sd card status
 #define SD_SEND_OP_COND        41            // ACMD41 - send operating condition
 
 #define READ_SINGLE_BLOCK    17            // CMD17 - read single block
 
 
 #define START_BLOCK            0xFE        // used to indicate the start of a data block
 #define MMC_FLOATING_BUS     0xFF        // Floating bus condition(?)
 
 // Summary: The format of an R1 type response
 // Description: This union represents different ways to access an SD card R1 type response packet.
 typedef union
 {
     unsigned char     byte;                 // Byte-wise access
     // This structure allows bitwise access of the response
     struct
     {
         unsigned     IN_IDLE_STATE:1;       // Card is in idle state
         unsigned     ERASE_RESET:1;         // Erase reset flag
         unsigned     ILLEGAL_CMD:1;         // Illegal command flag
         unsigned     CRC_ERR:1;             // CRC error flag
         unsigned     ERASE_SEQ_ERR:1;       // Erase sequence error flag
         unsigned     ADDRESS_ERR:1;         // Address error flag
         unsigned     PARAM_ERR:1;           // Parameter flag   
         unsigned     B7:1;                  // Unused bit 7
     }bits;
 } SD_RESPONSE;
 
 typedef struct {                        // struct to describe sd card
     unsigned         ok:1;        // card is initialized or status is OK
     unsigned         wp:1;        // card is write protected
     unsigned         cd:1;        // card is detected
     unsigned         hc:1;        // card is high capacity
     unsigned         v1:1;        // card is version 1
     unsigned         no_idle:1;    // card did not GO_STATE_IDLE
     unsigned         no_opc:1;     // send_op_cond timed out
     unsigned         no_ocr:1;    // send_ocr command timed out
     unsigned         no_pup:1;    // card not powered up after init request
     unsigned         no_csd:1;     // send_CSD command timed out
     unsigned        no_w:1;        // write did not complete (timed out)
     unsigned         csd_err:1;     // card did not issue 0xFE at start of CSD data
     unsigned        w_err:1;    // write error (command not accepted)
     unsigned         r_err:1;    // read error
     unsigned        d_err:1;    // data error (data not accepted)
     unsigned        pad:2;
     unsigned int    timeout;    // timeout value when ops timeout
     unsigned long    size;        // card size in 512kbyte blocks (ie actual capacity is 1000 * m_size / 2 bytes)
 } SD_CARD;
 
 // Prototypes
 unsigned long write_sd_sector (SD_CARD *sd_card, unsigned long address, unsigned char *buffer);
 SD_RESPONSE tx_cmd(unsigned char command_index, unsigned long command_argument, char extra_bytes, unsigned char *location, unsigned char debug);
 SD_CARD init_sdcard (void); 
 
 #endif
 /*****************************************************************************
  * EOF
  *****************************************************************************/
 
 
