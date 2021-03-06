
/*********************************************************************
 *
 *     libraries for use with the Microchip USB C18 Firmware
 *
 *********************************************************************
 * FileName:        userlib.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        Microchip C18 2.30.01+
 * Author:          Jeanne Pindar.
 ********************************************************************/

/* must have C:\Program Files (x86)\Microchip\mplabc18\v3.46\h\ in the include path*/
#include <p18cxxx.h>
#include <usart.h>
#include <stdio.h>
#include <string.h>
#include <delays.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "io_cfg.h"             // I/O pin mapping
#include "projdefs.h"
#include "mxmc18\tick.h"    // Modtronix library
#include "user\userlib.h"

#define FOSC 20000000
#pragma udata
char input_buffer[64];
char output_buffer[64];

#pragma code
void InitializeUSART(void)
{
    TRISCbits.TRISC7=1; // RX
    TRISCbits.TRISC6=0; // TX
    SPBRG = 0x71;
    SPBRGH = 0x02;      // 0x0271 for 48MHz -> 19200 baud
    TXSTA = 0x24;       // TX enable BRGH=1
    RCSTA = 0x90;       // continuous RX
    BAUDCON = 0x08;     // BRG16 = 1
}

void delay_ms(uint16 t)
{
    uint16 i;

    if (t == 0)
        return;
    //seconds per instruction = (1/FOSC)*4
    //instructions per second =  (FOSC/4)
    //instructions per msec = (FOSC/4) / 1000
    // for 20 MHZ, 5000 instructions / msec
    //instructions = t * ((FOSC/4)/1000);
    for (i=0; i<t; i++)
    {
        ClrWdt();
        Delay1KTCYx(5);
    }
}

void delay_us(uint16 t)
{
    uint16 i;

    if (t == 0)
        return;
    //seconds per instruction = (1/FOSC)*4
    //instructions per second =  (FOSC/4)
    //instructions per usec = (FOSC/4) / 1000000
    // for 20 MHZ, 5 instructions / usec
    //instructions = t * ((FOSC/4)/1000000);
    ClrWdt();
    for (i=0; i<t; i++)
    {
        Delay1TCY(); Delay1TCY();
        Delay1TCY(); Delay1TCY();Delay1TCY();
    }
}

//this apparently works. addresses start at 1, do not use address 0
unsigned char eeprom_read_byte(unsigned char addr)
{
    unsigned char data;
    ClrWdt();
    EECON1bits.CFGS=0; //EEPROM not config bits
    EECON1bits.EEPGD=0;
    EEADR=addr;
    INTCONbits.GIEH = 0; //Stop INTs when reading (per B3 Errata)
    EECON1bits.RD=1;
    data = EEDATA;
    INTCONbits.GIEH = 1; //Re-Enable Ints
    return data;
}

void eeprom_write_byte(char address,char data)
{
    unsigned char i=INTCONbits.GIEH;
    INTCONbits.GIEH = 0; //disable interrupts
    EEADR=address;
    EEDATA=data;
    EECON1bits.EEPGD=0;//Point to DATA Memory
    EECON1bits.CFGS=0;
    EECON1bits.WREN=1;/*enables writes to the eeprom*/
    EECON2=0x55;
    EECON2=0xAA;
    EECON1bits.WR=1;/*write data to the eeprom*/
    do{
    ClrWdt();
    } while (EECON1bits.WR); //Wait for EEPROM write
    EECON1bits.WREN=0; //No more write
    INTCONbits.GIEH = i; //restore interrupts
}

void eeprom_write_block(void *ptr, unsigned short addr, unsigned char len)
{
    unsigned char *data = ptr;

    while (len--) {
        eeprom_write_byte(addr++, *data++);
    }
}

void eeprom_write_string(char *s, unsigned char addr)
{
   unsigned int i = 0;

   for (i = 0; s[i] != 0x00; i++)
   {
     eeprom_write_byte(addr + i,s[i]);
   }
   eeprom_write_byte(++addr, 0x00);
}

void eeprom_read_string(char* s, unsigned int addr)
{
   //unsigned int len = 4;
   unsigned int i;
   char* b;

   b = s;
   *s = eeprom_read_byte(addr++);
   while( *s!=0x00)
   {
     s++;
     *s = eeprom_read_byte(addr++);

   }
   //*ptr = 0x00;
   s = b;
  // return s;
}

void doAtIntervals(void)
{
    static uint16 tmr1sec = 0;

    //Enter every 60ms
    if (tick16GetDiff(tmr1sec) > 1000) {
        tmr1sec += 1000;

        if(mUSBUSARTIsTxTrfReady())
        {
            putrsUSBUSART("\rHello World\r\n");
        }
    }
}

void ioStateMachine(void)
{
    static byte bytesRead;
    char msg[20];
    char msg2[20];
    char foo;

    static byte smEx2State = 0;
    // State Machine states
    typedef enum SM_CMD
    {
        EX2_WAIT_FOR_INPUT,     // Wait for input from user via virtual serial port
        EX2_WAIT_TX_READY       // Wait for driver ready to accept a new string to write to the USB
    } SM_CMD;

    switch (smEx2State) {
        case EX2_WAIT_FOR_INPUT:
            //Check if any data was received via the virtual serial port
            if(bytesRead = getsUSBUSART(input_buffer, 8)) {
                smEx2State = EX2_WAIT_TX_READY;
            }
        break;
        case EX2_WAIT_TX_READY:
            //Write "Received: ", followed by data received, to virtual serial port
            if(mUSBUSARTIsTxTrfReady())
            {
                //putrsUSBUSART(msg);//for pgm memory (rom) only

                /*
                output_buffer[0] = '\r';
                output_buffer[1] = 'R';
                output_buffer[2] = 'e';
                output_buffer[3] = 'c';
                output_buffer[4] = 'e';
                output_buffer[5] = 'i';
                output_buffer[6] = 'v';
                output_buffer[7] = 'e';
                output_buffer[8] = 'd';
                output_buffer[9] = ':';
                output_buffer[10] = ' ';
                output_buffer[11] = input_buffer[0];
                output_buffer[12] = '\r';
                output_buffer[13] = '\n';
                output_buffer[14] = '\0';
                putsUSBUSART(output_buffer);
                */
                /*
                 * sprintf gives a warning 2066  type qualifier mismatch in assignment
                 * which the manual describes as:
                 * Pointer assignment where the source and destination pointers point to
                 * objects of compatible type, but the source pointer points to an object which
                 * is const or volatile qualified and the destination pointer does not.
                 *
                 * Well, yeah.
                 */

                //output_buffer is in udata, this works
                //sprintf(output_buffer, "got a <%c> \r\n\0",input_buffer[0]);
                //putsUSBUSART(output_buffer);

                //this works if msg is a local variable
                //sprintf(msg, "got a <%c> \r\n\0",input_buffer[0]);
                //putsUSBUSART(msg);

               // eeprom_write_byte(2,42);
               // foo = eeprom_read_byte(2);
               // sprintf(msg, "read <%u> \r\n\0",foo);
               // putsUSBUSART(msg);

               // sprintf(msg, "ABC123\r\n");
               // eeprom_write_block(msg, 4, strlen(msg));
               // eeprom_read_string(msg2, 4);
               // putsUSBUSART(msg2);

                //Back to wait for input state
                smEx2State = EX2_WAIT_FOR_INPUT;
            }
        break;
    }
}

