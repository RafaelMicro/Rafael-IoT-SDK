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
#include "uart_stdio.h"

#define BUF_SIZE       256

uint32_t rx_finish, rx_index,otp1_index;
uint8_t  sendbuf[10], recvbuf[10];
hosal_uart_dma_cfg_t uart1_dam_rx;
hosal_uart_dev_t uart1_dev;

void printf_out(uint32_t id, uint32_t *outbuf, uint32_t length)
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
    

    uint32_t  i,j,status,wr_data;
	uint32_t buf[BUF_SIZE];
    uint8_t  getinptchar;
    uart_stdio_init();
    printf("\r\n");
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal one time program write demo\r\n");
    printf("----------------------------------------------------------------\r\n");

    memset(recvbuf,0xFF,sizeof(recvbuf));

    for (otp1_index = 0; otp1_index < 256; otp1_index += 1)
    {
        if (otp1_index != 0 && (otp1_index % 16) == 0)
        {
            printf("\r\n");
        }
        
        status = hosal_otp_ioctrl(HOSAL_PUF_GET_LOCK_OTP1,(otp1_index*4),NULL,1);

        if (status == 0)
        {
            printf("OTP[%.4d] lock = %d ", otp1_index, status);
            break;
        }
    }    

    printf("\r\n");
    printf("----------------------------------------------------------------\r\n");
    printf("Write one time only\r\n");
    printf("Write otp1 test input Y :\r\n");
    printf("----------------------------------------------------------------\r\n"); 

    int counter = 0;

    do{
        
        counter = uart_stdio_read(&getinptchar, 1);

        if(counter!=0)
        {
            if(getinptchar=='Y' || getinptchar=='y' || getinptchar=='N' || getinptchar=='n') {
                    break;
            }
            else{
                counter = 0;
                getinptchar = 0x00;
            }
        }
    }while(counter==0);


    if(getinptchar=='Y' || getinptchar=='y') {

      wr_data = 0x22222222;
      hosal_otp_ioctrl(HOSAL_PUF_WRITE_OTP1,otp1_index,&wr_data,1);
      hosal_otp_ioctrl(HOSAL_PUF_SET_LOCK_OTP1,otp1_index,NULL,0); 
      printf("\r\nOTP[%.4d] lock = %d \r\n",(otp1_index),hosal_otp_ioctrl(HOSAL_PUF_GET_LOCK_OTP1,(otp1_index*4),NULL,1)); 
      hosal_otp_ioctrl(HOSAL_PUF_READ_OTP1,0,buf,256);  
      printf_out(1,buf,256); 
         
    }
   
	printf("\r\n");    

    while (1) {;}
}

