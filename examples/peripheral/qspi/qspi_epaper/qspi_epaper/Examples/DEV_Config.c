/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/*****************************************************************************
* | File        :   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* | This version:   V2.0
* | Date        :   2018-10-30
* | Info        :
# ******************************************************************************
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "mcu.h"
#include "hosal_gpio.h"
#include "hosal_qspi.h"
#include "DEV_Config.h"



void DEV_SPI_WriteByte(UBYTE value, UBYTE DC)
{
    uint8_t   write_buf[2],read_buf[2];
    hosal_qspi_dev_t* spi_epd = hosal_qspi_handle_get(0);

    spi_epd->rx_buf = read_buf;
    spi_epd->tx_buf = write_buf;

    
    write_buf[0] = 0;
    write_buf[0] = value;
    write_buf[1] = 0;
    write_buf[1] = DC;

    hosal_qspi_transfer_pio_epd(spi_epd, spi_epd->tx_buf,spi_epd->rx_buf,2,1000);

}

void DEV_SPI_Write9BIT(UBYTE value, UBYTE DC)
{
    uint8_t   write_buf[2],read_buf[2];

    hosal_qspi_dev_t* spi_epd = hosal_qspi_handle_get(0);

    spi_epd->rx_buf = read_buf;
    spi_epd->tx_buf = write_buf;

    write_buf[0] = 0;
    write_buf[0] = value;
    write_buf[1] = 0;
    write_buf[1] = DC;

   hosal_qspi_transfer_pio_epd(spi_epd, spi_epd->tx_buf,spi_epd->rx_buf,2,1000);


}

int DEV_Module_Init(void)
{
    hosal_gpio_pin_clear(EPD_DC_PIN);
    hosal_gpio_pin_set(EPD_RST_PIN);
    return 0;
}

void DEV_Module_Exit(void)
{
    hosal_gpio_pin_clear(EPD_DC_PIN);

    //close 5V
    hosal_gpio_pin_clear(EPD_RST_PIN);
}

