/*********************************************************************
 *
 *                Example 01 - Load-time Linking
 *
 *********************************************************************
 * FileName:        console.cpp
 * Dependencies:    None
 * Compiler:        Borland C++ Builder 6
 * Company:         Copyright (C) 2004 by Microchip Technology, Inc.
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
 * Rawin Rojvanit       11/19/04
 ********************************************************************/

//---------------------------------------------------------------------------

#pragma hdrstop

#include <stdio.h>
#include "windows.h"
#include "_mpusbapi.h"                  // MPUSBAPI Header File

//---------------------------------------------------------------------------
#pragma argsused

// Global Vars
char vid_pid[]= "vid_04d8&pid_000c";    // Default Demo Application Firmware
char out_pipe[]= "\\MCHP_EP1";
char in_pipe[]= "\\MCHP_EP1";

DWORD temp;

HANDLE myOutPipe;
HANDLE myInPipe;

//---------------------------------------------------------------------------
// Prototypes
void GetSummary(void);

//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    BOOLEAN bQuit;
    DWORD selection;
    bQuit = false;

    // Always a good idea to initialize the handles
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;

    printf("Microchip Technology Inc., 2004\r\n");
    printf("===============================\r\n");
    while(!bQuit)
    {
        printf("Select an option\r\n");
        printf("[1] Get MPUSBAPI Version\r\n");
        printf("[2] Summarize Instances\r\n");
        printf("[3] -\r\n");
        printf("[4] Quit\r\n>>");
        scanf("%d",&selection);

        switch(selection)
        {
            case 1:
                temp = MPUSBGetDLLVersion();
                printf("MPUSBAPI Version: %d.%d\r\n",HIWORD(temp),LOWORD(temp));
                break;
            case 2:
                GetSummary();
                break;
            case 3:
                // Place Holder
                break;
            case 4:
                bQuit = true;
                break;
            default:
                break;
        }// end switch

        fflush(stdin);printf("\r\n");
    }//end while

    // Always check to close all handles before exiting!
    if (myOutPipe != INVALID_HANDLE_VALUE) MPUSBClose(myOutPipe);
    if (myInPipe != INVALID_HANDLE_VALUE) MPUSBClose(myInPipe);
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;

    return 0;
}//end main

//---------------------------------------------------------------------------

void GetSummary(void)
{
    HANDLE tempPipe = INVALID_HANDLE_VALUE;
    DWORD count = 0;
    DWORD max_count;

    max_count = MPUSBGetDeviceCount(vid_pid);

    printf("\r\n%d device(s) with %s currently attached\r\n",max_count,vid_pid);

    // Note:
    // The total number of devices using the generic driver could be
    // bigger than max_count. They could have different vid & pid numbers.
    // This means if max_count is 2, the valid instance index do not
    // necessary have to be '0' and '1'.
    //
    // Below is a sample code for searching for all valid instance indexes.
    // MAX_NUM_MPUSB_DEV is defined in _mpusbapi.h
    
    count = 0;
    for(int i = 0; i < MAX_NUM_MPUSB_DEV; i++)
    {
        tempPipe = MPUSBOpen(i,vid_pid,NULL,MP_READ,0);
        if(tempPipe != INVALID_HANDLE_VALUE)
        {
            printf("Instance Index # %d\r\n",i);
            MPUSBClose(tempPipe);
            count++;
        }
        if(count == max_count) break;
    }//end for
    printf("\r\n");
}//end GetSummary

//---------------------------------------------------------------------------
