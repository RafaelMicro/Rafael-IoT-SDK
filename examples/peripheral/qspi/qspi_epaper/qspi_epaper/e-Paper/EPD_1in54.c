/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/*****************************************************************************
* | File        :   EPD_1in54.C
* | Author      :   Waveshare team
* | Function    :   1.54inch e-paper
* | Info        :
*----------------
* | This version:   V3.0
* | Date        :   2019-06-12
* | Info        :
* -----------------------------------------------------------------------------
* V3.0(2019-06-12):
* 1.Change:
*    lut_full_update[] => EPD_1IN54_lut_full_update[]
*    lut_partial_update[] => EPD_1IN54_lut_partial_update[]
*    EPD_Reset() => EPD_1IN54_Reset()
*    EPD_SendCommand() => EPD_1IN54_SendCommand()
*    EPD_SendData() => EPD_1IN54_SendData()
*    EPD_WaitUntilIdle() => EPD_1IN54_ReadBusy()
*    EPD_SetLut() => EPD_1IN54_SetLut()
*    EPD_SetWindow() => EPD_1IN54_SetWindow()
*    EPD_SetCursor() => EPD_1IN54_SetCursor()
*    EPD_TurnOnDisplay() => EPD_1IN54_TurnOnDisplay()
*    EPD_Init() => EPD_1IN54_Init()
*    EPD_Clear() => EPD_1IN54_Clear()
*    EPD_Display() => EPD_1IN54_Display()
*    EPD_Sleep() => EPD_1IN54_Sleep()
* 2.remove commands define:
*   #define PANEL_SETTING                               0x00
*   #define POWER_SETTING                               0x01
*   #define POWER_OFF                                   0x02
*   #define POWER_OFF_SEQUENCE_SETTING                  0x03
*   #define POWER_ON                                    0x04
*   #define POWER_ON_MEASURE                            0x05
*   #define BOOSTER_SOFT_START                          0x06
*   #define DEEP_SLEEP                                  0x07
*   #define DATA_START_TRANSMISSION_1                   0x10
*   #define DATA_STOP                                   0x11
*   #define DISPLAY_REFRESH                             0x12
*   #define DATA_START_TRANSMISSION_2                   0x13
*   #define PLL_CONTROL                                 0x30
*   #define TEMPERATURE_SENSOR_COMMAND                  0x40
*   #define TEMPERATURE_SENSOR_CALIBRATION              0x41
*   #define TEMPERATURE_SENSOR_WRITE                    0x42
*   #define TEMPERATURE_SENSOR_READ                     0x43
*   #define VCOM_AND_DATA_INTERVAL_SETTING              0x50
*   #define LOW_POWER_DETECTION                         0x51
*   #define TCON_SETTING                                0x60
*   #define TCON_RESOLUTION                             0x61
*   #define SOURCE_AND_GATE_START_SETTING               0x62
*   #define GET_STATUS                                  0x71
*   #define AUTO_MEASURE_VCOM                           0x80
*   #define VCOM_VALUE                                  0x81
*   #define VCM_DC_SETTING_REGISTER                     0x82
*   #define PROGRAM_MODE                                0xA0
*   #define ACTIVE_PROGRAM                              0xA1
*   #define READ_OTP_DATA                               0xA2
* -----------------------------------------------------------------------------
* V2.0(2018-10-30):
* 1.Remove:ImageBuff[EPD_1IN54_HEIGHT * EPD_1IN54_WIDTH / 8]
* 2.Change:EPD_Display(UBYTE *Image)
*   Need to pass parameters: pointer to cached data
* 3.Change:
*   EPD_RST -> EPD_RST_PIN
*   EPD_DC -> EPD_DC_PIN
*   EPD_CS -> EPD_CS_PIN
*   EPD_BUSY -> EPD_BUSY_PIN
#
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
#include "gpio.h"
#include "sysctrl.h"
#include "GUI_Paint.h"
#include "EPD_1in54.h"
//#include "Debug.h"


/* SSD1681 / 1.54" 200x200
 * ???? full / partial LUT ??
 * ??? panel ?? OTP LUT ??????
 */
static const unsigned char EPD_1IN54_lut_full_update[] = {
    0x80,0x60,0x40,0x00,0x00,0x00,0x00,0x00,
    0x10,0x60,0x20,0x00,0x00,0x00,0x00,0x00,
    0x80,0x60,0x40,0x00,0x00,0x00,0x00,0x00,
    0x10,0x60,0x20,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

static const unsigned char EPD_1IN54_lut_partial_update[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
};

static void EPD_1IN54_Reset(void)
{
    gpio_pin_set(EPD_RST_PIN);
    delay_ms(10);
    gpio_pin_clear(EPD_RST_PIN);
    delay_ms(10);
    gpio_pin_set(EPD_RST_PIN);
    delay_ms(20);
}

static void EPD_1IN54_SendCommand(UBYTE Cmd)
{
#if EPD_FOUR_BIT_MODE
    gpio_pin_clear(EPD_DC_PIN);
    DEV_SPI_WriteByte(Cmd,0);
#else
    DEV_SPI_Write9BIT(Cmd, 0);
#endif
}

static void EPD_1IN54_SendData(UBYTE Data)
{
#if EPD_FOUR_BIT_MODE
    gpio_pin_set(EPD_DC_PIN);
    DEV_SPI_WriteByte(Data,1);
#else
    DEV_SPI_Write9BIT(Data, 1);
#endif
}

void EPD_1IN54_ReadBusy(void)
{
    uint32_t timeout = 5000;
    uint32_t value = 0;

    //printf("e-Paper busy, active=%d\r\n", EPD_BUSY_ACTIVE_LEVEL);

    while (timeout--) {
        gpio_pin_get(EPD_BUSY_PIN, &value);
        //printf("busy=%d\r\n", value);

        if (value != EPD_BUSY_ACTIVE_LEVEL) {
            printf("e-Paper busy release\r\n");
            return;
        }

        delay_ms(1);
    }

    gpio_pin_get(EPD_BUSY_PIN, &value);
    printf("EPD busy timeout, busy=%d\r\n", value);
}

static void EPD_1IN54_SetLut(UBYTE Mode)
{
    UWORD i;
    const unsigned char *lut = NULL;
    UWORD lut_size = 0;

    if (Mode == EPD_1IN54_FULL) {
        lut = EPD_1IN54_lut_full_update;
        lut_size = sizeof(EPD_1IN54_lut_full_update);
    } else {
        lut = EPD_1IN54_lut_partial_update;
        lut_size = sizeof(EPD_1IN54_lut_partial_update);
    }

    EPD_1IN54_SendCommand(0x32);
    for (i = 0; i < lut_size; i++) {
        EPD_1IN54_SendData(lut[i]);
    }
}

static void EPD_1IN54_SetWindow(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    EPD_1IN54_SendCommand(0x44); /* SET_RAM_X_ADDRESS_START_END_POSITION */
    EPD_1IN54_SendData((Xstart >> 3) & 0xFF);
    EPD_1IN54_SendData((Xend   >> 3) & 0xFF);

    EPD_1IN54_SendCommand(0x45); /* SET_RAM_Y_ADDRESS_START_END_POSITION */
    EPD_1IN54_SendData(Ystart & 0xFF);
    EPD_1IN54_SendData((Ystart >> 8) & 0xFF);
    EPD_1IN54_SendData(Yend & 0xFF);
    EPD_1IN54_SendData((Yend >> 8) & 0xFF);
}

static void EPD_1IN54_SetCursor(UWORD Xstart, UWORD Ystart)
{
    EPD_1IN54_SendCommand(0x4E); /* SET_RAM_X_ADDRESS_COUNTER */
    EPD_1IN54_SendData((Xstart >> 3) & 0xFF);

    EPD_1IN54_SendCommand(0x4F); /* SET_RAM_Y_ADDRESS_COUNTER */
    EPD_1IN54_SendData(Ystart & 0xFF);
    EPD_1IN54_SendData((Ystart >> 8) & 0xFF);
}

static void EPD_1IN54_TurnOnDisplay(void)
{
    /* SSD1681 typical sequence */
    EPD_1IN54_SendCommand(0x22); /* DISPLAY_UPDATE_CONTROL_2 */
    EPD_1IN54_SendData(0xF7);
    EPD_1IN54_SendCommand(0x20); /* MASTER_ACTIVATION */
    EPD_1IN54_ReadBusy();
}

void EPD_1IN54_Init(UBYTE Mode)
{
    (void)Mode;

    EPD_1IN54_Reset();

    /* soft reset */
    EPD_1IN54_SendCommand(0x12);
    EPD_1IN54_ReadBusy();

    /* Driver output control */
    EPD_1IN54_SendCommand(0x01);
    EPD_1IN54_SendData(0xC7);   /* 199 */
    EPD_1IN54_SendData(0x00);
    EPD_1IN54_SendData(0x00);

    /* Data entry mode */
    EPD_1IN54_SendCommand(0x11);
    EPD_1IN54_SendData(0x03);

    /* RAM X start/end */
    EPD_1IN54_SendCommand(0x44);
    EPD_1IN54_SendData(0x00);
    EPD_1IN54_SendData(0x18);   /* 200/8 - 1 = 24 = 0x18 */

    /* RAM Y start/end */
    EPD_1IN54_SendCommand(0x45);
    EPD_1IN54_SendData(0x00);
    EPD_1IN54_SendData(0x00);
    EPD_1IN54_SendData(0xC7);   /* 199 */
    EPD_1IN54_SendData(0x00);

    /* Border waveform */
    EPD_1IN54_SendCommand(0x3C);
    EPD_1IN54_SendData(0x05);

    /* VCOM */
    EPD_1IN54_SendCommand(0x2C);
    EPD_1IN54_SendData(0x36);

    /* Dummy line */
    EPD_1IN54_SendCommand(0x3A);
    EPD_1IN54_SendData(0x1A);

    /* Gate time */
    EPD_1IN54_SendCommand(0x3B);
    EPD_1IN54_SendData(0x08);

    /* set RAM pointer */
    EPD_1IN54_SendCommand(0x4E);
    EPD_1IN54_SendData(0x00);

    EPD_1IN54_SendCommand(0x4F);
    EPD_1IN54_SendData(0x00);
    EPD_1IN54_SendData(0x00);

    EPD_1IN54_ReadBusy();
}

void EPD_1IN54_Display(UBYTE *Image)
{
    UWORD Width, Height;
    UDOUBLE Addr = 0;

    if (Image == NULL) {
        return;
    }

    Width  = (EPD_1IN54_WIDTH % 8 == 0) ? (EPD_1IN54_WIDTH / 8) : (EPD_1IN54_WIDTH / 8 + 1);
    Height = EPD_1IN54_HEIGHT;

    /* set RAM pointer to 0,0 */
    EPD_1IN54_SendCommand(0x4E);
    EPD_1IN54_SendData(0x00);

    EPD_1IN54_SendCommand(0x4F);
    EPD_1IN54_SendData(0x00);
    EPD_1IN54_SendData(0x00);

    /* write RAM */
    EPD_1IN54_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            Addr = i + j * Width;
            EPD_1IN54_SendData(Image[Addr]);
        }
    }

    EPD_1IN54_TurnOnDisplay();

//    EPD_1IN54_TurnOnDisplay();
//    UWORD Width, Height;
//    UDOUBLE Addr = 0;

//    if (Image == NULL) {
//        return;
//    }

//    Width  = (EPD_1IN54_WIDTH % 8 == 0) ? (EPD_1IN54_WIDTH / 8) : (EPD_1IN54_WIDTH / 8 + 1);
//    Height = EPD_1IN54_HEIGHT;

//    EPD_1IN54_SetWindow(0, 0, EPD_1IN54_WIDTH - 1, EPD_1IN54_HEIGHT - 1);
//    EPD_1IN54_SetCursor(0, 0);

//    /* write BW RAM */
//    EPD_1IN54_SendCommand(0x24);
//    for (UWORD j = 0; j < Height; j++) {
//        for (UWORD i = 0; i < Width; i++) {
//            Addr = i + j * Width;
//            EPD_1IN54_SendData(~Image[Addr]);
//        }
//    }

//    /* write second RAM buffer too, improves update reliability on some SSD1681 panels */
//    EPD_1IN54_SetCursor(0, 0);
//    EPD_1IN54_SendCommand(0x26);
//    for (UWORD j = 0; j < Height; j++) {
//        for (UWORD i = 0; i < Width; i++) {
//            Addr = i + j * Width;
//            EPD_1IN54_SendData(~Image[Addr]);
//        }
//    }

//    EPD_1IN54_TurnOnDisplay();
}

void EPD_1IN54_Clear(void)
{
//    UWORD Width, Height;

//    Width  = (EPD_1IN54_WIDTH % 8 == 0) ? (EPD_1IN54_WIDTH / 8) : (EPD_1IN54_WIDTH / 8 + 1);
//    Height = EPD_1IN54_HEIGHT;

//    EPD_1IN54_SetWindow(0, 0, EPD_1IN54_WIDTH - 1, EPD_1IN54_HEIGHT - 1);
//    EPD_1IN54_SetCursor(0, 0);

//    EPD_1IN54_SendCommand(0x24);
//    for (UWORD j = 0; j < Height; j++) {
//        for (UWORD i = 0; i < Width; i++) {
//            EPD_1IN54_SendData(0xFF); /* white */
//        }
//    }

//    EPD_1IN54_TurnOnDisplay();
    UWORD Width, Height;

    Width  = (EPD_1IN54_WIDTH % 8 == 0) ? (EPD_1IN54_WIDTH / 8) : (EPD_1IN54_WIDTH / 8 + 1);
    Height = EPD_1IN54_HEIGHT;

    EPD_1IN54_SendCommand(0x4E);
    EPD_1IN54_SendData(0x00);

    EPD_1IN54_SendCommand(0x4F);
    EPD_1IN54_SendData(0x00);
    EPD_1IN54_SendData(0x00);

    EPD_1IN54_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_1IN54_SendData(0xFF);
        }
    }

    EPD_1IN54_TurnOnDisplay();
}

void EPD_1IN54_FillBlack(void)
{
//    UWORD Width, Height;

//    Width  = (EPD_1IN54_WIDTH % 8 == 0) ? (EPD_1IN54_WIDTH / 8) : (EPD_1IN54_WIDTH / 8 + 1);
//    Height = EPD_1IN54_HEIGHT;

//    EPD_1IN54_SetWindow(0, 0, EPD_1IN54_WIDTH - 1, EPD_1IN54_HEIGHT - 1);
//    EPD_1IN54_SetCursor(0, 0);

//    EPD_1IN54_SendCommand(0x24);
//    for (UWORD j = 0; j < Height; j++) {
//        for (UWORD i = 0; i < Width; i++) {
//            EPD_1IN54_SendData(0x00); /* black */
//        }
//    }

//    EPD_1IN54_TurnOnDisplay();
	
    UWORD Width, Height;

    Width  = (EPD_1IN54_WIDTH % 8 == 0) ? (EPD_1IN54_WIDTH / 8) : (EPD_1IN54_WIDTH / 8 + 1);
    Height = EPD_1IN54_HEIGHT;

    EPD_1IN54_SendCommand(0x4E);
    EPD_1IN54_SendData(0x00);

    EPD_1IN54_SendCommand(0x4F);
    EPD_1IN54_SendData(0x00);
    EPD_1IN54_SendData(0x00);

    EPD_1IN54_SendCommand(0x24);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_1IN54_SendData(0x00);
        }
    }

    EPD_1IN54_TurnOnDisplay();
}

void EPD_1IN54_Sleep(void)
{
    EPD_1IN54_SendCommand(0x10); /* DEEP_SLEEP */
    EPD_1IN54_SendData(0x01);
    delay_ms(100);
}

