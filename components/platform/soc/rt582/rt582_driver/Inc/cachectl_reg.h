/**
 * \file            cachectl_reg.h
 * \brief           cachectl_reg.h include file
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
 * Author:     
 */
#ifndef CACHECTL_REG_H
#define CACHECTL_REG_H

#if defined (__CC_ARM)
#pragma anon_unions
#endif


typedef union cache_ccr_s
{
    struct cache_ccr_b
    {
        uint32_t CACHE_EN         : 1;
        uint32_t CACHE_WAY_1_EN   : 1;
        uint32_t RESERVED2        : 6;
        uint32_t CACHE_WAY_0_CLR  : 1;
        uint32_t CACHE_WAY_1_CLR  : 1;
        uint32_t RESERVED10       : 22;
    } bit;
    uint32_t reg;
} cache_ccr_t;


typedef struct
{
    __IO cache_ccr_t CCR;        /*0x00*/
} CACHECTL_T;


#if defined (__CC_ARM)
#pragma no_anon_unions
#endif

#endif
