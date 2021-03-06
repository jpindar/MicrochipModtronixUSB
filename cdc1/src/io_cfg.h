/*********************************************************************
 *
 *                Microchip USB C18 Firmware Version 1.0
 *
 *********************************************************************
 * FileName:        io_cfg.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * David Hosken         06/15/07     Modified for Modtronix SBC44UC board
 * Rawin Rojvanit       7/21/04     Original.
 ********************************************************************/

#ifndef IO_CFG_H
#define IO_CFG_H

/** I N C L U D E S *************************************************/
#include "autofiles\usbcfg.h"

/** T R I S *********************************************************/
#define INPUT_PIN           1
#define OUTPUT_PIN          0

/** U S B ***********************************************************/
#if defined(SBC44UC_BOARD)
    /* SBC44UC uses RD3 for USB BUS Sense */
    #define tris_usb_bus_sense  TRISDbits.TRISD3    // Input
#else
    #define tris_usb_bus_sense  TRISAbits.TRISA1    // Input
#endif

#if defined(USE_USB_BUS_SENSE_IO)
    /* SBC44UC uses RD3 for USB BUS Sense */
    #if defined(SBC44UC_BOARD)
        #define usb_bus_sense       PORTDbits.RD3
    #else
        #define usb_bus_sense       PORTAbits.RA1
    #endif
#else
    #define usb_bus_sense       1
#endif


#if defined(SBC44UC_BOARD)
    /* SBC44UC uses RD2 for USB Self power sense */
    #define tris_self_power     TRISDbits.TRISD2    // Input
#else
    #define tris_self_power     TRISAbits.TRISA2    // Input
#endif

#if defined(USE_SELF_POWER_SENSE_IO)
    /* SBC44UC uses RD2 for USB Self power sense */
    #if defined(SBC44UC_BOARD)
        #define self_power          PORTDbits.RD2
    #else
        #define self_power          PORTAbits.RA2
    #endif
#else
    #define self_power          1
#endif

/** I/O ***********************************************************/
    #define mInitRA1()      LATA &= 0x02; TRISA &= ~0x02; //make RA1 outputs
    #define mInitRA3()      LATA &= 0x08; TRISA &= ~0x08; //make RA3 outputs

/** L E D ***********************************************************/
/* The SBC44UC only has one LED on RB6, configure it as an output */
#if defined(SBC44UC_BOARD)
    #define mInitAllLEDs()      LATB &= 0x40; TRISB &= ~0x40;
#else
    #define mInitAllLEDs()      LATD &= 0xF0; TRISD &= 0xF0;
#endif

#if defined(SBC44UC_BOARD)
    #define mLED_1              LATBbits.LATB6
    //Modtronix changed! The SBC44UC only has one LED on B6, use D4 to D6 for additional LEDs
    #define mLED_2              LATDbits.LATD4
    #define mLED_3              LATDbits.LATD5
    #define mLED_4              LATDbits.LATD6
#else
    #define mLED_1              LATDbits.LATD0
    #define mLED_2              LATDbits.LATD1
    #define mLED_3              LATDbits.LATD2
    #define mLED_4              LATDbits.LATD3
#endif

#define mLED_1_On()         mLED_1 = 1;
#define mLED_2_On()         mLED_2 = 1;
#define mLED_3_On()         mLED_3 = 1;
#define mLED_4_On()         mLED_4 = 1;

#define mLED_1_Off()        mLED_1 = 0;
#define mLED_2_Off()        mLED_2 = 0;
#define mLED_3_Off()        mLED_3 = 0;
#define mLED_4_Off()        mLED_4 = 0;

#define mLED_1_Toggle()     mLED_1 = !mLED_1;
#define mLED_2_Toggle()     mLED_2 = !mLED_2;
#define mLED_3_Toggle()     mLED_3 = !mLED_3;
#define mLED_4_Toggle()     mLED_4 = !mLED_4;

/** S W I T C H *****************************************************/
#define mInitAllSwitches()  TRISBbits.TRISB4=1;TRISBbits.TRISB5=1;
#define mInitSwitch2()      TRISBbits.TRISB4=1;
#define mInitSwitch3()      TRISBbits.TRISB5=1;
#define sw2                 PORTBbits.RB4
#define sw3                 PORTBbits.RB5

/** P O T ***********************************************************/
#define mInitPOT()          TRISAbits.TRISA0=1;ADCON0=0x01;ADCON2=0x3C;

/** S P I : Chip Select Lines ***************************************/
#define tris_cs_temp_sensor TRISBbits.TRISB2    // Output
#define cs_temp_sensor      LATBbits.LATB2

#define tris_cs_sdmmc       TRISBbits.TRISB3    // Output
#define cs_sdmmc            LATBbits.LATB3

/** S D M M C *******************************************************/
#define TRIS_CARD_DETECT    TRISBbits.TRISB4    // Input
#define CARD_DETECT         PORTBbits.RB4

#define TRIS_WRITE_DETECT   TRISAbits.TRISA4    // Input
#define WRITE_DETECT        PORTAbits.RA4

#endif //IO_CFG_H
