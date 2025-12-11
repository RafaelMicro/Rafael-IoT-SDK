/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_otp.h"
#include "hosal_uart.h"
#include "hosal_trng.h"
#include "uart_stdio.h"
#include "app_hooks.h"

#define BUF_SIZE       256

void printf_otp(uint32_t id, uint32_t *outbuf, uint32_t length)
{
    uint32_t i = 0;

    printf("\r\n==========================================================================\r\n");

    if (id == 0)
    {
        printf("                               UID");
    }
    else if (id == 1)
    {
        printf("                              OTP1");
    }
    else if (id == 2)
    {
        printf("                              OTP2");
    }
    else if (id == 3)
    {
        printf("                              RAND");
    }    
    else
    {
        printf("   ");
    }


    printf("\r\n==========================================================================\r\n");
    for (i = 0; i < 8; i++)
    {
        printf("   %X     ", i);
    }
    printf("\r\n==========================================================================\r\n");

    for (i = 0; i < length; i++)
    {
        if ((i % 8 == 0) && i != 0)
        {
            printf("\r\n");
        }
        if ((i % 8 == 0))
        {
            printf("%2X ", ((i / 8) << 3));
        }
        printf("%08X ", outbuf[i] );
    }
    printf("\r\n===========================================================================\r\n");
    printf("\r\n");
}

int main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

	uint32_t buf[256];
    uint32_t pufs_version = 0;
    uint8_t getinptchar;
    printf("\r\n");
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal otp  puf trng read demo\r\n");
    printf("----------------------------------------------------------------\r\n");
   

    while (1) {

    printf("Input Number(0~5)  :");
    int counter=0;
    getinptchar = 0;

    do{
        
        counter = uart_stdio_read(&getinptchar, 1);

        if(counter!=0)
        {
            if(getinptchar=='0' ||  getinptchar=='1' || getinptchar=='2' || getinptchar=='3' || getinptchar=='4') {
                    break;
            }
            else{
                counter = 0;
                getinptchar = 0x00;
            }
        }
    }while(counter==0);


        switch(getinptchar)
        {
            case 0x30:
            hosal_otp_ioctrl(HOSAL_PUF_READ_VERSION,0,&pufs_version,1);
            printf(" version :%.8X\r\n", pufs_version);
            break;

            case 0x31:
            hosal_otp_ioctrl(HOSAL_PUF_READ_UID, 0, buf, 32);  
            printf_otp(0, buf, 32);
            break;

            case 0x32:
            hosal_otp_ioctrl(HOSAL_PUF_GET_RAND,0,buf,32);  
            printf_otp(3, buf, 32);	
            break;

            case 0x33:
            hosal_otp_ioctrl(HOSAL_PUF_READ_OTP1,0,buf,256);  
            printf_otp(1, buf, 256);
            break;

            case 0x34:
            hosal_otp_ioctrl(HOSAL_PUF_READ_OTP2,0,buf,256);  
            printf_otp(2, buf, 256);
            break;

            default:
                break;
        }
    
    
    }
    
   
}


