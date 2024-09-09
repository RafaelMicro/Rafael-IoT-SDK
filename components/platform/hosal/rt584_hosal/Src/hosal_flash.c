/**
 * \file            hosal_flash.c
 * \brief           Hosal Flash driver file
 */

/*
 * Copyright (c) 2024 Rafale Micro
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of library_name.
 *
 * Author:         ives.lee 
 */
#include "stdio.h"
#include <stdint.h>
#include "mcu.h"
#include "flashctl.h"
#include "hosal_flash.h"


int hosal_flash_enable_qe(void) {
    uint32_t  status = STATUS_ERROR;

    flash_enable_qe();

    return (int)status;
}


int hosal_flash_init(void) {
    uint32_t  status = STATUS_ERROR;

    return (int)status;
}


int hosal_flash_read(int ctl, uint32_t address, uint8_t *buf) {

    uint32_t  status = 0, buf_address;

    switch (ctl) {

    case HOSAL_FLASH_READ_BYTE:

        buf[0] = flash_read_byte(address);
        break;


    case HOSAL_FLASH_READ_PAGE:

        status = flash_read_page_syncmode((uint32_t)buf, address);
        break;


    case HOSAL_FLASH_SECURITY_READ:

        status = flash_read_sec_register((uint32_t)buf, address);
        break;

    default :
        return -1;

    }
    return (int)status;
}


int hosal_flash_write(int ctl, uint32_t address, uint8_t *buf) {

    uint32_t  status = STATUS_ERROR;

    switch (ctl) {

    case HOSAL_FLASH_WRITE_BYTE:

        status = flash_write_byte(address, buf[0]);

        break;

    case HOSAL_FLASH_WRITE_PAGE:

        status = flash_write_page((uint32_t)buf, address);

        break;

    case HOSAL_FLASH_SECURITY_WRITE:

        status = flash_write_sec_register(address, (uint32_t)buf);

        break;
    default :
        return -1;

    }
    
    return (int)status;
}


int hosal_flash_erase(int ctl, uint32_t address ) {
    uint32_t  status = STATUS_ERROR;

    switch (ctl) {


    case HOSAL_FLASH_ERASE_PAGE:        //Only Support 512k flash

        status =  flash_erase(FLASH_ERASE_PAGE, address);

        break;

    case HOSAL_FLASH_ERASE_SECTOR:

        status = flash_erase(FLASH_ERASE_SECTOR, address);

        break;


    case HOSAL_FLASH_ERASE_32K_SECTOR:

        status = flash_erase(FLASH_ERASE_32K, address);

        break;


    case HOSAL_FLASH_ERASE_64K_SECTOR:

        status = flash_erase(FLASH_ERASE_64K, address);

        break;


    case HOSAL_FLASH_ERASE_CHIP:


        break;
    default :
        return -1;

    }

    return (int)status;
}


int hosal_flash_ioctrl(int ctl, uint32_t address, uint8_t *buf) {
    uint32_t  status = STATUS_ERROR;

    switch (ctl) {

    case HOSAL_FLASH_READ_ID:

        status = flash_get_unique_id((uint32_t)buf, 32);

        break;

    case HOSAL_FLASH_ENABLE_SUSPEND:

        flash_enable_suspend();

        status = STATUS_SUCCESS;
        break;

    case HOSAL_FLASH_DISABLE_SUSPEND:

        flash_disable_suspend();

        status = STATUS_SUCCESS;
        break;

    case HOSAL_FLASH_CACHE:

        flush_cache();

        status = STATUS_SUCCESS;

        break;
    default :
        return -1;

    }

    return (int)status;
}
