/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "hosal_timer.h"
#include "hosal_i2c_master.h"
#include "hosal_i2c_slave.h"
#include "hosal_status.h"
#include "task.h"
#include "app_hooks.h"
#include "uart_stdio.h"


/**
 * \brief           I2C Slave storage start address
 */
#define  BASE_ADDRESS  0x30002000

/**
 * \brief           I2C Slave 
 */
#define  I2C_SLAVE_ERROR_REQUEST    -2
#define  I2C_SLAVE_UNINIT           -1
#define  I2C_SLAVE_LEGAL_ADDR        2

/**
 * \brief           I2C master port 
 */
#define I2C_MASTER 0

/**
 * \brief           I2C master pin 
 */
#define I2CM0_SCL      22
#define I2CM0_SDA      23

/**
 * \brief           I2C slave pin 
 */
#define I2C_SLAVE_SCL   28
#define I2C_SLAVE_SDA   29

/**
 * \brief           I2C test data size
 */
#define TEST_BLOCK_SIZE  320
#define TEST_BLOCK_SIZE1 (TEST_BLOCK_SIZE + 40)

volatile uint8_t transfer_done;
volatile uint32_t  i2c_status;

static uint8_t *operation_addr;

static int8_t read_count = I2C_SLAVE_UNINIT;

/* because we simulation i2c flash, it has two bytes register address. */
static uint16_t i2c_register_addr = 0;
uint8_t i2c_register_addr_high = 0, i2c_register_addr_low = 0;

void init_default_pin_mux(void) {
    /* set SCL, SDA as open drain mode */
    hosal_enable_pin_opendrain(I2CM0_SCL);
    hosal_enable_pin_opendrain(I2CM0_SDA);

    /* set SCL, SDA I2C mode */
    hosal_pin_set_mode(I2CM0_SCL, HOSAL_MODE_I2CM0_SCL);
    hosal_pin_set_mode(I2CM0_SDA, HOSAL_MODE_I2CM0_SDA);

    hosal_pin_enable_schmitt(I2CM0_SCL);
    hosal_pin_enable_schmitt(I2CM0_SDA);
    hosal_pin_enable_filter(I2CM0_SCL);
    hosal_pin_enable_filter(I2CM0_SDA);

    /* I2C slave pin set */
    hosal_pin_set_mode(I2C_SLAVE_SCL, HOSAL_MODE_I2CS_SCL);
    hosal_pin_set_mode(I2C_SLAVE_SDA, HOSAL_MODE_I2CS_SDA);

    /* set SCL, SDA as open drain mode */
    hosal_enable_pin_opendrain(I2C_SLAVE_SCL);
    hosal_enable_pin_opendrain(I2C_SLAVE_SDA);

    hosal_pin_set_pullopt(I2C_SLAVE_SCL, HOSAL_PULL_NONE);
    hosal_pin_set_pullopt(I2C_SLAVE_SDA, HOSAL_PULL_NONE);

    hosal_pin_enable_schmitt(I2C_SLAVE_SCL);
    hosal_pin_enable_schmitt(I2C_SLAVE_SDA);
    hosal_pin_enable_filter(I2C_SLAVE_SCL);
    hosal_pin_enable_filter(I2C_SLAVE_SDA);

    hosal_i2c_preinit(I2C_MASTER);

    return;
}

void i2c_finish(uint32_t status) {
    i2c_status = status;
    transfer_done = TRUE;
}

void i2c_slave_user_cb(uint32_t status) {
    if (status & HOSAL_I2C_SLAVE_STATUS_STOP) {
        /* we don't care stop condition.
         * If support multitask --- maybe we should signal some event to
         * notify the request.
         */
    } else if (status & HOSAL_I2C_SLAVE_STATUS_ADDR_MATCH) {
        if (status & HOSAL_I2C_SLAVE_STATUS_WRITE) {
            /* master send a new write request */
            read_count = 0;
            i2c_register_addr = 0;
        } else {
            /* 
             * master send a new read request
             * we should write one byte to i2c master
             */
            if (read_count < I2C_SLAVE_LEGAL_ADDR) {
                /*
                 * host send illegal register address (less than 2 bytes)-- so return dummy
                 * or host send read request without write request
                 */
                hosal_i2c_slave_write_byte(0x00);
                read_count = I2C_SLAVE_ERROR_REQUEST;
            } else {
                hosal_i2c_slave_write_byte(*operation_addr++);
                /* 
                 * because this read request does NOT require read_count anymore
                 * so we reset read_count to -1 to avoid next read request without
                 * a new write requset.
                 */
                read_count = I2C_SLAVE_UNINIT;
            }
        }
    } else if (status & HOSAL_I2C_SLAVE_STATUS_DATA_READY) {
        /* master write data to slave */
        if (status & HOSAL_I2C_SLAVE_STATUS_WRITE) {
            /* master send a write request */
            if (read_count == 0) {
                /* high address of uint16_t */
                hosal_i2c_slave_read_byte(&i2c_register_addr_high);
                i2c_register_addr = i2c_register_addr_high << 8;
                read_count++;
            } else if (read_count == 1) {
                /* low address of uint16_t */
                hosal_i2c_slave_read_byte(&i2c_register_addr_low);
                i2c_register_addr += i2c_register_addr_low;

                /* 
                 * We should check i2c_register_addr range
                 * if the size is too large... maybe we should return dummy data or round off data
                 * But 16 bits data is just 64KB, the same as this project assumption.
                 * If the system support small memory, we may round off the i2c_register_addr like
                 *  i2c_register_addr = i2c_register_addr & 0xFFF;  (for 4KB)
                 */

                operation_addr = (uint8_t *) (BASE_ADDRESS + i2c_register_addr);
                read_count++;
            } else {
                /* host write data to slave */
                hosal_i2c_slave_read_byte(operation_addr++);
                //printf("write:%x,data:%x\n",operation_addr,I2c_Slave_Read_Byte());
            }
        } else {
            /*
             * master should send a write request before read request
             * we should write one byte to i2c master
             */
            if ((read_count == I2C_SLAVE_LEGAL_ADDR) || (read_count == I2C_SLAVE_ERROR_REQUEST)) {
                /* 
                 * read_count should not be I2C_SLAVE_LEGAL_ADDR or I2C_SLAVE_ERROR_REQUEST in FSM
                 * so we return dummy back to master.
                 */
                hosal_i2c_slave_write_byte(0x00);
            } else {
                /*TODO: we should do some address range check...*/
                hosal_i2c_slave_write_byte(*operation_addr++);
            }
        }
    } else if (status & HOSAL_I2C_SLAVE_STATUS_ERROR) {
        puts("!!!i2c slave fatal error!!!!\r\n");
    }

    return;
}

void i2c_slave_init(void) {
    hosal_i2c_slave_mode_t   slave_cfg;
    uint32_t           status;

    operation_addr = pvPortMalloc(400);

    slave_cfg.i2c_slave_cb_func = i2c_slave_user_cb;

    slave_cfg.i2c_bus_timeout_enable = 0;
    slave_cfg.i2c_bus_timeout = 0;
    slave_cfg.i2c_slave_addr = 0x52  ;      /*7bit only.*/

    status = hosal_i2c_slave_open(&slave_cfg);
    
    if (status != STATUS_SUCCESS) {
        puts("check what wrong\r\n");
        while (1);
    }

    NVIC_EnableIRQ(I2C_Slave_IRQn);
}


void i2c_master_slaver_read_write(void) {
    hosal_i2c_master_mode_t master_dev_cfg;
    uint32_t   status;
    uint32_t   i, temp, read_bytes,  write_offset, write_offset_index;

    static uint8_t data[TEST_BLOCK_SIZE1] = {0x55, 0xAA, 0xC4, 0x37, 0x13, 
                                             0x46, 0xC9, 0xF3, 0x18, 0x2F, 0x38};
    static uint8_t rdata[TEST_BLOCK_SIZE1];

    i2c_slave_init();
    NVIC_EnableIRQ(I2C_Slave_IRQn);

    for (i = 0; i < TEST_BLOCK_SIZE1; i++) {
        data[i] = i & 0xFF;
    }

    i = 0;
    hosal_i2c_init(I2C_MASTER, I2C_CLOCK_400K);
    NVIC_EnableIRQ(I2C_Master0_IRQn);
    printf("i2c write / read bytes \r\n");
    while ( 1 ) {
        write_offset = 0;
        puts(".");
        i ++;
        for (temp = 1; temp <= TEST_BLOCK_SIZE; temp++) {
            uint16_t read_i = 0;

            transfer_done = FALSE;

            master_dev_cfg.dev_addr = 0x52;
            master_dev_cfg.reg_addr = 0x00 + (0x10 * i);
            master_dev_cfg.bFlag_16bits = 0x01;
            master_dev_cfg.i2c_usr_isr = i2c_finish;

            status = hosal_i2c_write(I2C_MASTER, &master_dev_cfg, data, temp);
            while (transfer_done == FALSE) {}
            if (i2c_status == HOSAL_I2C_STATUS_ERR_NOACK) {
                printf("Why WRITE I2C NO ACK ERROR? %x\r\n", i2c_status);
                while (1);
            }

            memset(rdata, 0, TEST_BLOCK_SIZE);

            transfer_done = FALSE;
            status = hosal_i2c_read(I2C_MASTER, &master_dev_cfg, rdata, temp);

            while (transfer_done == FALSE) {}

            if (i2c_status == HOSAL_I2C_STATUS_ERR_NOACK) {
                printf("Why READ I2C NO ACK ERROR? %x\r\n", i2c_status);
                while (1);
            }

            if (memcmp(rdata, data, temp) != 0) {
                printf("read write data is mismatch\r\n");
                while (1);
            }

            /* for debug */
            #if 0
            for (read_i = 0; read_i < temp; read_i++)
            {
                printf("%.2x ", rdata[read_i]);
                if ( (read_i % 16) == 15)
                {
                    printf("\r\n");
                }
            }
            printf("\r\n");
            printf("\r\n");
            #endif 

            /*compare boundary*/
            if (rdata[temp] != 0) {
                printf("read boundary corruption \r\n");
                while (1);
            }
        }
    }
}


int main(void) {
    uint32_t  i, random_value;
    uint32_t   *ptr;

    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    init_default_pin_mux();
    
    printf("/************************************/\r\n");
    printf("/*******Start I2C Master Slave*******/\r\n");
    printf("/************************************/\r\n");
    printf("Please connect I2C SCL -- GPIO22 and GPIO28 \r\n");
    printf("Please connect I2C SDA -- GPIO23 abd GPIO29 \r\n");

    printf("Please notice: in FPGA I2C is 1.8V \r\n");
    printf("Please do NOT connect 3.3V host \r\n");

    ptr = (uint32_t*)BASE_ADDRESS;

    /*clear 2KB to zero*/
    for (i = 0; i < 512; i++) {
        *ptr++ = 0;
    }

    /*test I2C master*/
    i2c_master_slaver_read_write();

    while (1);

}
