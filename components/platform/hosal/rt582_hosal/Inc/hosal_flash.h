/**
 * \file            hosal_flash.h
 * \brief           hosal_flash include file
 */

/*
 * Copyright (c) 2024 Rafael Micro
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
 * Author:  ives.lee 
 */
#ifndef HOSAL_FLASH_H
#define HOSAL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief flash read command
 * 
 */
#define HOSAL_FLASH_READ_BYTE     0x00
#define HOSAL_FLASH_READ_PAGE     0x01
#define HOSAL_FLASH_SECURITY_READ 0x02
#define HOSAL_FLASH_READ_STATUS   0x03
/**
* \brief flash write command
 * 
 */
#define HOSAL_FLASH_WRITE_BYTE     0x10
#define HOSAL_FLASH_WRITE_PAGE     0x11
#define HOSAL_FLASH_SECURITY_WRITE 0x12
#define HOSAL_FLASH_WRITE_STATUS   0x13
/**
 * \brief flash erase command 
 * 
 */
#define HOSAL_FLASH_ERASE_PAGE          0x20
#define HOSAL_FLASH_ERASE_SECTOR        0x21
#define HOSAL_FLASH_ERASE_32K_SECTOR    0x22
#define HOSAL_FLASH_ERASE_64K_SECTOR    0x23
#define HOSAL_FLASH_ERASE_CHIP          0x24
#define HOSAL_FLASH_ERASE_SECURITY_PAGE 0x25
/**
 * \brief flash io control command 
 * 
 */
#define HOSAL_FLASH_READ_ID         0x30
#define HOSAL_FLASH_ENABLE_SUSPEND  0x31
#define HOSAL_FLASH_DISABLE_SUSPEND 0x32
#define HOSAL_FLASH_CACHE           0x33

/**
 * \brief               flash conifg struct table
 *                      This structure is used to calculate all point
 *                      related stuff
 */
typedef struct {
    uint32_t address;     /*!< flash address */
    uint32_t length;      /*!< flash data length  */
    uint32_t buf_address; /*!< flash data buffer address  */
} hosal_flash_config_t;

/**
 * \brief           enable flash
 * \param[in]       NONE
 * \return           function status
 */
int hosal_flash_enable_qe(void);
/**
 * \brief           flash initinal
 * \param[in]       NONE
 * \return           function status
 */
int hosal_flash_init(void);
/**
 * \brief           flash io control function
 * \param[in]       ctl: control command
 * \param[in]       address: flash address
 * \param[in]       buf: flash buffer address
 * \return          function status
 */
int hosal_flash_read(int ctl, uint32_t address, uint8_t* buf);
/**
 * \brief           flash io control function
 * \param[in]       ctl: control command
 * \param[in]       address: flash address
 * \param[in]       buf: flash buffer address
 * \return          function status
 */
int hosal_flash_write(int ctl, uint32_t address, uint8_t* buf);
/**
 * \brief           flash io control function
 * \param[in]       ctl: control command
 * \param[in]       address: flash address
 * \return          function status
 */
int hosal_flash_erase(int ctl, uint32_t address);
/**
 * \brief           flash io control function
 * \param[in]       ctl: control command
 * \param[in]       address: flash address
 * \param[in]       buf: flash buffer address
 * \return          function status
 */
int hosal_flash_ioctrl(int ctl, uint32_t address, uint8_t* buf);

#ifdef __cplusplus
}
#endif

#endif
