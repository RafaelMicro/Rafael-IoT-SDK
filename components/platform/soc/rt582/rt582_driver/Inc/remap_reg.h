/**
 * \file            remap_reg.h
 * \brief           Remap register definition header file
 */
/*
 * Copyright (c) 2024 Rafal Micro
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
 * Author:
 */
#ifndef REMAP_REG_H
#define REMAP_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


typedef struct
{
    __IO  uint32_t  RESERVED1;                 //offset: 0x00
    __IO  uint32_t  RESERVED2;                 //offset: 0x04
    __IO  uint32_t  RESERVED3;                 //offset: 0x08
    __IO  uint32_t  RESERVED4;                 //offset: 0x0C
    __IO  uint32_t  SW_IRQ_SET;                //offset: 0x10
    __IO  uint32_t  SW_IRQ_CLR;                //offset: 0x14
    __IO  uint32_t  SW_IRQ_EN_SET;             //offset: 0x18
    __IO  uint32_t  SW_IRQ_EN_CLR;             //offset: 0x1C
} remap_t;


#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif

