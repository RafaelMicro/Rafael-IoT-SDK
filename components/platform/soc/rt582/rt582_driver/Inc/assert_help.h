/**
 * \file            assert_help.h
 * \brief           assert_help include file
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

#ifndef ASSERT_H
#define ASSERT_H

#ifdef  DEBUG

#include <stdint.h>
#include "stdio.h"

#define dprintf(msg, ...)  do { printf(msg, __VA_ARGS__); }while(0)
#define dprintf0(msg)     do { printf(msg);}while(0)

#define assert_param(expr)    \
        do {                  \
            if(expr)          \
            {                 \
            }                 \
            else              \
            {                 \
                printf("ASSERT Failed in File %s line %d \n", __FILE__, __LINE__ ); \
                while(1);   \
            }                 \
        } while(0)

#else
#define assert_param(expr)     ((void)0)
#define dprintf(msg, ...)      ((void)0)
#define dprintf0(msg)          ((void)0)
#endif

#endif
