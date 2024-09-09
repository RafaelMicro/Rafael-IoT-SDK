/**
 * \file            hosal_commmon.h
 * \brief           hosal_commmon include file
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

#ifndef HOSAL_COMMON_H
#define HOSAL_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#define HOSAL_SET_BIT(REG, BIT)   ((REG) |= (BIT))
#define HOSAL_CLEAR_BIT(REG, BIT) ((REG) &= ~(BIT))
#define HOSAL_READ_BIT(REG, BIT)  ((REG) & (BIT))
#define HOSAL_CLEAR_REG(REG)      ((REG) = (0x0))
#define HOSAL_WRITE_REG(REG, VAL) ((REG) = (VAL))
#define HOSAL_READ_REG(REG)       ((REG))
#define HOSAL_MODIFY_REG(REG, CLEARMASK, SETMASK)                              \
    HOSAL_WRITE_REG((REG),                                                     \
                    (((HOSAL_READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define HOSAL_POSITION_VAL(VAL) (__CLZ(__RBIT(VAL)))

#ifdef __cplusplus
}
#endif
#endif
