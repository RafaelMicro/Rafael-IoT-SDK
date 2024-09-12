/**
 * \file            sysfun.c
 * \brief           System function driver file
 *          
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
 *
 * Author:          ives.lee
 */
#include "assert_help.h"
#include "mcu.h"
#if defined(CONFIG_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#endif
static int critical_counter = 0;

void enter_critical_section(void) {

#if defined(CONFIG_FREERTOS)
    if (portNVIC_INT_CTRL_REG & 0xFF) {
        return;
    }

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        __disable_irq();
    } else {
        vPortEnterCritical();
    }
#else
    __disable_irq();
#endif
    critical_counter++;
}

void leave_critical_section(void) {

#if defined(CONFIG_FREERTOS)
    if (portNVIC_INT_CTRL_REG & 0xFF) {
        return;
    }
    critical_counter--;
    assert_param(critical_counter >= 0);

    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        vPortExitCritical();
    }

    if (critical_counter == 0) {

        if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
            __enable_irq();
        }
    }
#else
    critical_counter--;
    assert_param(critical_counter >= 0);
    if (critical_counter == 0) {
        __enable_irq();
    }
#endif
}

/*
 *      version_check is help function to check
 *   software project setting is the same as hardware IC version.
 *   If software project define "CHIP_VERSION" is
 *   not matched with hardware IC version, this functio will return 0, otherwise 1.
 *
 */
uint32_t version_check(void) {

    uint32_t version_info, ret = 1, chip_id, chip_rev;

    version_info = SYSCTRL->chip_info;

    chip_id = version_info & IC_CHIP_ID_MASK;
    chip_rev = version_info & IC_CHIP_REVISION_MASK;

#if (CHIP_VERSION == RT58X_MPA)

    if ((chip_id != IC_RT58X) || (chip_rev != IC_CHIP_REVISION_MPA)) {
        return 0; /*hardware is different from software setting*/
    }

#elif (CHIP_VERSION == RT58X_MPB)

    if ((chip_id != IC_RT58X) || (chip_rev != IC_CHIP_REVISION_MPB)) {
        return 0; /*hardware is different from software setting*/
    }

#elif (CHIP_VERSION == RT58X_SHUTTLE)

#error "SHUTTLE IC has been *** OBSOLETED!! ***"

#endif

    return ret;
}

chip_model_t GetOtpVersion() {
    uint32_t otp_rd_buf_addr[64];
    uint32_t i;
    uint8_t buf_Tmp[4];

    otp_version_t otp_version;
    chip_model_t chip_model;

    chip_model.type = CHIP_TYPE_UNKNOW;
    chip_model.version = CHIP_VERSION_UNKNOW;

    if (flash_read_otp_sec_page((uint32_t)otp_rd_buf_addr) != STATUS_SUCCESS) {
        return chip_model;
    }

    for (i = 0; i < 8; i++) {
        *(uint32_t*)buf_Tmp = otp_rd_buf_addr[(i / 4)];
        otp_version.buf[i] = buf_Tmp[(i % 4)];
    }

    if (otp_version.buf[0] == 0xFF) //otp version flag
    {
        return chip_model;
    }

    /*ASCII Value*/
    otp_version.buf[5] -= 0x30; /* ascii 0~9 0x30~0x39 */
    otp_version.buf[6] -= 0x40; /* ascii A~Z 0x41~0x5A */

    /*reference chip_define.h
     CHIP_ID(TYPE,VER)                   ((TYPE << 8) | VER)
     CHIP_MODEL                           CHIP_ID(CHIP_TYPE,CHIP_VERSION)
    */
    chip_model.type = (chip_type_t)otp_version.buf[5];

    chip_model.version = (chip_version_t)otp_version.buf[6];

    return chip_model;
}

/*
 *  system software reset
 *
 */
void Sys_Software_Reset(void) {

    while (flash_check_busy())
        ; //wait flash operation finish

    NVIC_SystemReset();
}

/*
 *  system PMU Mode
 *
 */
pmu_mode_cfg_t GetPmuMode(void) {
    pmu_mode_cfg_t Mode = PMU_MODE_DCDC;

    if ((PMU->PMU_EN_CTRL.bit.EN_LLDO_NM == 0)
        && (PMU->PMU_EN_CTRL.bit.EN_DCDC_NM == 1)) {
        Mode = PMU_MODE_DCDC;
    } else if ((PMU->PMU_EN_CTRL.bit.EN_LLDO_NM == 1)
               && (PMU->PMU_EN_CTRL.bit.EN_DCDC_NM == 0)) {
        Mode = PMU_MODE_LDO;
    }

    return Mode;
}
