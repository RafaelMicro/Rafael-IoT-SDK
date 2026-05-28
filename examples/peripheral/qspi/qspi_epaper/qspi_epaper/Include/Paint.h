/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/******************************************************************************
* | File        :   GUI_Paint.h
* | Author      :   Waveshare electronics
* | Function    :   Achieve drawing: draw points, lines, boxes, circles and
*                   their size, solid dotted line, solid rectangle hollow
*                   rectangle, solid circle hollow circle.
* | Info        :
*   Achieve display characters: Display a single character, string, number
*   Achieve time display: adaptive size display time minutes and seconds
*----------------
* | This version:   V3.0
* | Date        :   2019-04-18
* | Info        :
* -----------------------------------------------------------------------------
* V3.0(2019-04-18):
* 1.Change:
*    Paint_DrawPoint(..., DOT_STYLE DOT_STYLE)
* => Paint_DrawPoint(..., DOT_STYLE Dot_Style)
*    Paint_DrawLine(..., LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
* => Paint_DrawLine(..., DOT_PIXEL Line_width, LINE_STYLE Line_Style)
*    Paint_DrawRectangle(..., DRAW_FILL Filled, DOT_PIXEL Dot_Pixel)
* => Paint_DrawRectangle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
*    Paint_DrawCircle(..., DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel)
* => Paint_DrawCircle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Filll)
*
* -----------------------------------------------------------------------------
* V2.0(2018-11-15):
* 1.add: Paint_NewImage()
*    Create an image's properties
* 2.add: Paint_SelectImage()
*    Select the picture to be drawn
* 3.add: Paint_SetRotate()
*    Set the direction of the cache
* 4.add: Paint_RotateImage()
*    Can flip the picture, Support 0-360 degrees,
*    but only 90.180.270 rotation is better
* 4.add: Paint_SetMirroring()
*    Can Mirroring the picture, horizontal, vertical, origin
* 5.add: Paint_DrawString_CN()
*    Can display Chinese(GB1312)
*
* -----------------------------------------------------------------------------
* V1.0(2018-07-17):
*   Create library
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documnetation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to  whom the Software is
* furished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
******************************************************************************/
#ifndef __PAINT_H
#define __PAINT_H

#include "DEV_Config.h"
#include "fonts.h"

///**
// * Image attributes
//**/
//typedef struct {
//    UBYTE *Image;
//    UWORD Width;
//    UWORD Height;
//    UWORD WidthMemory;
//    UWORD HeightMemory;
//    UWORD Color;
//    UWORD Rotate;
//    UWORD Mirror;
//    UWORD WidthByte;
//    UWORD HeightByte;
//    UWORD Scale;
//} PAINT;
//extern PAINT Paint;

///**
// * Display rotate
//**/
//#define ROTATE_0            0
//#define ROTATE_90           90
//#define ROTATE_180          180
//#define ROTATE_270          270

///**
// * Display Flip
//**/
//typedef enum {
//    MIRROR_NONE  = 0x00,
//    MIRROR_HORIZONTAL = 0x01,
//    MIRROR_VERTICAL = 0x02,
//    MIRROR_ORIGIN = 0x03,
//} MIRROR_IMAGE;
//#define MIRROR_IMAGE_DFT MIRROR_NONE

///**
// * image color
//**/
//#define WHITE          0xFF
//#define BLACK          0x00
//#define RED            BLACK

//#define IMAGE_BACKGROUND    WHITE
//#define FONT_FOREGROUND     BLACK
//#define FONT_BACKGROUND     WHITE

////#define TRUE 1
////#define FALSE 0

////4 Gray level
//#define  GRAY1 0x03 //Blackest
//#define  GRAY2 0x02
//#define  GRAY3 0x01 //gray
//#define  GRAY4 0x00 //white

///**
// * The size of the point
//**/
//typedef enum {
//    DOT_PIXEL_1X1  = 1,       // 1 x 1
//    DOT_PIXEL_2X2  ,      // 2 X 2
//    DOT_PIXEL_3X3  ,      // 3 X 3
//    DOT_PIXEL_4X4  ,      // 4 X 4
//    DOT_PIXEL_5X5  ,      // 5 X 5
//    DOT_PIXEL_6X6  ,      // 6 X 6
//    DOT_PIXEL_7X7  ,      // 7 X 7
//    DOT_PIXEL_8X8  ,      // 8 X 8
//} DOT_PIXEL;
//#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

///**
// * Point size fill style
//**/
//typedef enum {
//    DOT_FILL_AROUND  = 1,     // dot pixel 1 x 1
//    DOT_FILL_RIGHTUP  ,       // dot pixel 2 X 2
//} DOT_STYLE;
//#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

///**
// * Line style, solid or dashed
//**/
//typedef enum {
//    LINE_STYLE_SOLID = 0,
//    LINE_STYLE_DOTTED,
//} LINE_STYLE;

///**
// * Whether the graphic is filled
//**/
//typedef enum {
//    DRAW_FILL_EMPTY = 0,
//    DRAW_FILL_FULL,
//} DRAW_FILL;

///**
// * Custom structure of a time attribute
//**/
//typedef struct {
//    UWORD Year;  //0000
//    UBYTE  Month; //1 - 12
//    UBYTE  Day;   //1 - 30
//    UBYTE  Hour;  //0 - 23
//    UBYTE  Min;   //0 - 59
//    UBYTE  Sec;   //0 - 59
//} PAINT_TIME;
//extern PAINT_TIME sPaint_time;
// Display resolution
//#define COLORED                           1
//#define UNCOLORED                         0

//#define EPD_WIDTH       200
//#define EPD_HEIGHT      200

//// EPD 1.54 Inch commands
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_LINE_WIDTH                         0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define NOP                                         0xFF


//#define ROTATE_0            0
//#define ROTATE_90           1
//#define ROTATE_180          2
//#define ROTATE_270          3

//typedef struct paint_t {
//    unsigned char* image;
//    int width;
//    int height;
//    int rotate;
//}PAINT_T;
////init and Clear
//extern void DrawAbsolutePixel(int x, int y, int colored);
//extern void DrawPixel(int x, int y, int colored);
//extern void DrawHorizontalLine(int x, int y, int line_width, int colored);
//extern void DrawVerticalLine(int x, int y, int line_height, int colored);
//extern void Paint_Clear(int colored);
//extern unsigned char* Paint_GetImage(void);
//extern int Paint_GetWidth(void);
//extern int Paint_GetHeight(void);
//extern void Paint_SetWidth(int width);
//extern void Paint_SetHeight(int height);
//extern void Paint_SetRotate(int rotate);
//extern void DrawCharAt(int x, int y, char ascii_char, sFONT* font, int colored);
//extern void Paint_DrawRectangle(int x0, int y0, int x1, int y1, int colored);
//extern void Paint_DrawLine(int x0, int y0, int x1, int y1, int colored);
//extern void Paint_DrawStringAt(int x, int y, const char* text, sFONT* font, int colored);
//extern void Paint_DrawFilledRectangle(int x0, int y0, int x1, int y1, int colored);
//extern void Paint_DrawCircle(int x, int y, int radius, int colored);
//extern void Paint_DrawFilledCircle(int x, int y, int radius, int colored);
//extern void Paint_Init(unsigned char* image, int width, int height);
#endif





