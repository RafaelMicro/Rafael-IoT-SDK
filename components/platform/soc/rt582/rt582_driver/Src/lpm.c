/**
 * \file            lpm.c
 * \brief           low power mode driver file
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
 * Author: ives.lee
 */

#include "mcu.h"
#include "rf_mcu_ahb.h"

static volatile  uint32_t low_power_mask_status;
static volatile  uint32_t comm_subsystem_wakeup_mask_status;
static volatile  low_power_level_cfg_t low_power_level;
static volatile  low_power_wakeup_cfg_t low_power_wakeup;
static volatile  uint32_t low_power_wakeup_update;
static volatile  uint32_t low_power_wakeup_uart;
static volatile  uint32_t low_power_wakeup_gpio;

#define LPM_SRAM0_RETAIN 0x1E

void lpm_init(void) {
    low_power_mask_status = LOW_POWER_NO_MASK;
    comm_subsystem_wakeup_mask_status = COMM_SUBSYS_WAKEUP_NO_MASK;
    low_power_level = LOW_POWER_LEVEL_NORMAL;
    low_power_wakeup = LOW_POWER_WAKEUP_NULL;
    low_power_wakeup_update = 0;
    low_power_wakeup_uart = 0;
    low_power_wakeup_gpio = 0;
}

void lpm_low_power_mask(uint32_t mask) {
    low_power_mask_status |= mask;
}

void lpm_low_power_unmask(uint32_t unmask) {
    low_power_mask_status &= (~unmask);
}

uint32_t lpm_get_low_power_mask_status(void) {
    return low_power_mask_status;
}

void lpm_comm_subsystem_wakeup_mask(uint32_t mask) {
    comm_subsystem_wakeup_mask_status |= mask;
}

void lpm_comm_subsystem_wakeup_unmask(uint32_t unmask) {
    comm_subsystem_wakeup_mask_status &= (~unmask);
}

void lpm_comm_subsystem_check_system_ready(void) {
    uint32_t status;
    do {
        status = COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_INFO;
    } while (((status & 0x01) != 1));

}

void lpm_comm_subsystem_disable_wait_32k_done(void) {
    uint8_t reg_buf[4];

    RfMcu_MemoryGetAhb(SUBSYSTEM_CFG_WAIT_32K_DONE, reg_buf, 4);
    reg_buf[3] &= ~SUBSYSTEM_CFG_WAIT_32K_DONE_DISABLE;                 //Clear Communication System Register 0x418 Bit26
    RfMcu_MemorySetAhb(SUBSYSTEM_CFG_WAIT_32K_DONE, reg_buf, 4);
}

void lpm_comm_subsystem_check_system_deepsleep(void) {
    uint32_t status;
    do {
        status = ((COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_INFO & 0x6) >> 1);
    } while ((status != 1UL));
}


uint32_t lpm_get_comm_subsystem_wakeup_mask_status(void) {
    return comm_subsystem_wakeup_mask_status;
}


void lpm_set_low_power_level(low_power_level_cfg_t low_power_level_cfg) {
    low_power_level = low_power_level_cfg;
    low_power_wakeup_update = 1;
}


void lpm_enable_low_power_wakeup(low_power_wakeup_cfg_t low_power_wakeup_cfg) {
    uint32_t wakeup_source = 0;
    uint32_t wakeup_source_index = 0;

    wakeup_source = (low_power_wakeup_cfg & 0xFFFF);

    if (wakeup_source == LOW_POWER_WAKEUP_GPIO) {
        wakeup_source_index = (low_power_wakeup_cfg >> 16);
        low_power_wakeup_gpio |= (1 << wakeup_source_index);
    } else if (wakeup_source == LOW_POWER_WAKEUP_UART_RX) {
        wakeup_source_index = (low_power_wakeup_cfg >> 16);
        low_power_wakeup_uart |= (1 << wakeup_source_index);
    } else {
        low_power_wakeup |= wakeup_source;
    }

    low_power_wakeup_update = 1;
}

void lpm_disable_low_power_wakeup(low_power_wakeup_cfg_t low_power_wakeup_cfg) {
    uint32_t wakeup_source = 0;
    uint32_t wakeup_source_index = 0;

    wakeup_source = (low_power_wakeup_cfg & 0xFFFF);

    if (wakeup_source == LOW_POWER_WAKEUP_GPIO) {
        wakeup_source_index = (low_power_wakeup_cfg >> 16);
        low_power_wakeup_gpio &= (~(1 << wakeup_source_index));
    } else if (wakeup_source == LOW_POWER_WAKEUP_UART_RX) {
        wakeup_source_index = (low_power_wakeup_cfg >> 16);
        low_power_wakeup_uart &= (~(1 << wakeup_source_index));
    } else {
        low_power_wakeup &= ~wakeup_source;
    }

    low_power_wakeup_update = 1;
}

void lpm_set_platform_low_power_wakeup(low_power_platform_enter_mode_cfg_t platform_low_power_mode) {
    do {
        if (low_power_wakeup_update == 0) {
            break;
        }

        low_power_wakeup_update = 0;

        if (platform_low_power_mode == LOW_POWER_PLATFORM_ENTER_SLEEP) {
            /* HCLK sleep selection in Sleep */
            if ((low_power_wakeup & LOW_POWER_WAKEUP_RTC_TIMER) ||
                    (low_power_wakeup & LOW_POWER_WAKEUP_COMPARATOR) ||
                    (low_power_wakeup & LOW_POWER_WAKEUP_32K_TIMER) ||
                    (low_power_wakeup_gpio != 0)) {
                SYSCTRL->sys_clk_ctrl |= HCLK_SLEEP_RUN_IN_32K;
            } else {
                SYSCTRL->sys_clk_ctrl &= (~HCLK_SLEEP_RUN_IN_32K);
            }

            /* AUX comparator enable selection in Sleep */
            if (low_power_wakeup & LOW_POWER_WAKEUP_COMPARATOR) {
                PMU->PMU_COMP0.bit.AUX_COMP_EN_SP = 1;
            } else {
                PMU->PMU_COMP0.bit.AUX_COMP_EN_SP = 0;
            }

            /* UART0 sleep wake enable selection in Sleep */
            if (low_power_wakeup_uart & (1 << 0)) {
                SYSCTRL->sys_lowpower_ctrl |= UART0_SLEEP_WAKE_EN_MASK;
            } else {
                SYSCTRL->sys_lowpower_ctrl &= (~UART0_SLEEP_WAKE_EN_MASK);
            }

            /* UART1 sleep wake enable selection in Sleep */
            if (low_power_wakeup_uart & (1 << 1)) {
                SYSCTRL->sys_lowpower_ctrl |= UART1_SLEEP_WAKE_EN_MASK;
            } else {
                SYSCTRL->sys_lowpower_ctrl &= (~UART1_SLEEP_WAKE_EN_MASK);
            }

            /* UART2 sleep wake enable selection in Sleep */
            if (low_power_wakeup_uart & (1 << 2)) {
                SYSCTRL->sys_lowpower_ctrl |= UART2_SLEEP_WAKE_EN_MASK;
            } else {
                SYSCTRL->sys_lowpower_ctrl &= (~UART2_SLEEP_WAKE_EN_MASK);
            }
        } else if (platform_low_power_mode == LOW_POWER_PLATFORM_ENTER_DEEP_SLEEP) {
            if (low_power_wakeup & LOW_POWER_WAKEUP_RTC_TIMER) {
                SYSCTRL->sys_clk_ctrl |= HCLK_SLEEP_RUN_IN_32K;        /* HCLK sleep selection in Sleep, HCLK is forced to 32KHz in Sleep */
                SYSCTRL->sys_clk_ctrl |= RTC_PCLK_DS_MASK;             /* Enable RTC PCLK in Sleep, this allows RTC wakeup in DeepSleep */
                PMU->PMU_PWR_CTRL.bit.CFG_DS_32K_OFF = 0;              /* Enable 32KHz in DeepSleep mode, this allows RTC to work in DeepSleep */
            } else {
                SYSCTRL->sys_clk_ctrl &= (~HCLK_SLEEP_RUN_IN_32K);     /* HCLK sleep selection in Sleep, HCLK is disabled in Sleep */
                SYSCTRL->sys_clk_ctrl &= (~RTC_PCLK_DS_MASK);          /* Disable RTC PCLK in Sleep */
                PMU->PMU_PWR_CTRL.bit.CFG_DS_32K_OFF = 1;              /* Disable 32KHz in DeepSleep mode */
            }

            set_deepsleep_wakeup_pin(low_power_wakeup_gpio);           /* Set the corresponding bits to enable the wakeup of GPIOx in Deep Sleep */

            if (low_power_wakeup & LOW_POWER_WAKEUP_COMPARATOR) {
                PMU->PMU_COMP0.bit.AUX_COMP_EN_DS = 1;                 /* AUX comparator enable selection in Sleep, AUX comparator enable in DeepSleep */
                PMU->PMU_COMP0.bit.AUX_COMP_DS_WAKEUP_EN = 1;          /* AUX comparator wakeup enable selection in Sleep, AUX comparator wakeup enable in DeepSleep */
            } else {
                PMU->PMU_COMP0.bit.AUX_COMP_EN_DS = 0;                 /* AUX comparator enable selection in Sleep, AUX comparator disable in DeepSleep */
                PMU->PMU_COMP0.bit.AUX_COMP_DS_WAKEUP_EN = 0;          /* AUX comparator wakeup enable selection in Sleep, AUX comparator wakeup disable in DeepSleep */
            }
        }

    } while (0);
}

void lpm_set_gpio_deepsleep_wakeup_invert(uint32_t value) {
    set_deepsleep_wakeup_invert(value);                                /* Set the corresponding bit0~bit31 to invert the GPIO0~GPIO31 for wakeup in DeepSleep Mode */
}

void lpm_set_comparator_deepsleep_wakeup_invert(uint32_t value) {
    PMU->PMU_COMP0.bit.AUX_COMP_DS_INV = value;                        /* Invert the Aux Comparator Output for wakeup in DeepSleep Mode */
}

void lpm_set_sram_normal_shutdown(uint32_t value) {
    set_sram_shutdown_normal(value);                                   /* SRAM shut-down control in Normal Mode, set the corresponding bit0~bit4 to shut-down SRAM0~SRAM4 in Normal Mode */
}

void lpm_set_sram_sleep_deepsleep_shutdown(uint32_t value) {
    set_sram_shutdown_sleep(value);                                   /* SRAM shut-down control in Low Power Mode, set the corresponding bit0~bit4 to shut-down SRAM0~SRAM4 in Sleep/DeepSleep Mode */
}


void lpm_enter_low_power_mode(void) {
    pmu_dcdc_ctrl1_t pmu_cdcd_ctrl1_buf;

    if (low_power_mask_status == LOW_POWER_NO_MASK) {
        pmu_cdcd_ctrl1_buf.reg = PMU->PMU_DCDC_CTRL1.reg;

        PMU->PMU_DCDC_CTRL1.bit.DCDC_EN_COMP_LIGHT = 0;
        PMU->PMU_DCDC_CTRL1.bit.DCDC_MG_LIGHT = 0;

        if (low_power_level == LOW_POWER_LEVEL_SLEEP3) {
            COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_HOSTMODE;   /* enable host mode */
            COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_RESET;      /* communication system host control reset */
            lpm_comm_subsystem_check_system_ready();
            lpm_comm_subsystem_disable_wait_32k_done();
            COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_DEEPSLEEP;  /* communication system enter deep sleep mode */
            lpm_comm_subsystem_check_system_deepsleep();
            lpm_set_sram_sleep_deepsleep_shutdown(LPM_SRAM0_RETAIN);
            lpm_set_platform_low_power_wakeup(LOW_POWER_PLATFORM_ENTER_DEEP_SLEEP);        /* set platform system wakeup source when entering deep sleep mode*/
            SYSCTRL->power_state_ctrl = LOW_POWER_PLATFORM_ENTER_DEEP_SLEEP;               /* platform system enter deep sleep mode */
            __WFI();
        } else if (low_power_level == LOW_POWER_LEVEL_SLEEP2) {
            COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_HOSTMODE;   /* enable host mode */
            COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_RESET;      /* communication system host control reset */
            lpm_comm_subsystem_check_system_ready();
            lpm_comm_subsystem_disable_wait_32k_done();
            COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_DEEPSLEEP;  /* communication system enter deep sleep mode */
            lpm_comm_subsystem_check_system_deepsleep();

            lpm_set_platform_low_power_wakeup(LOW_POWER_PLATFORM_ENTER_SLEEP);             /* set platform system wakeup source when entering sleep mode*/
            SYSCTRL->power_state_ctrl = LOW_POWER_PLATFORM_ENTER_SLEEP;                    /* platform system enter sleep mode */
            __WFI();

            if (comm_subsystem_wakeup_mask_status != COMM_SUBSYS_WAKEUP_NO_MASK) {
                COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_HOSTMODE;   /* enable host mode */
                COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_WAKEUP;     /* communication system wakeup from deep sleep mode */

                /* Need to reload the communication subsystem code here */
            }
        } else if (low_power_level == LOW_POWER_LEVEL_SLEEP1) {
            COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_HOSTMODE;   /* enable host mode */
            COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_SLEEP;      /* communication system enter sleep mode */

            lpm_set_platform_low_power_wakeup(LOW_POWER_PLATFORM_ENTER_SLEEP);             /* set platform system wakeup source when entering sleep mode*/
            SYSCTRL->power_state_ctrl = LOW_POWER_PLATFORM_ENTER_SLEEP;                    /* platform system enter sleep mode */
            __WFI();

            if (comm_subsystem_wakeup_mask_status != COMM_SUBSYS_WAKEUP_NO_MASK) {
                COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_HOSTMODE;   /* enable host mode */
                COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_WAKEUP;     /* communication system wakeup from sleep mode */
            }
        } else if (low_power_level == LOW_POWER_LEVEL_SLEEP0) {
            lpm_set_platform_low_power_wakeup(LOW_POWER_PLATFORM_ENTER_SLEEP);                 /* set platform system wakeup source when entering sleep mode*/
            SYSCTRL->power_state_ctrl = LOW_POWER_PLATFORM_ENTER_SLEEP;                        /* platform system enter sleep mode */
            __WFI();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
        }

        PMU->PMU_DCDC_CTRL1.bit.DCDC_EN_COMP_LIGHT = pmu_cdcd_ctrl1_buf.bit.DCDC_EN_COMP_LIGHT;
        PMU->PMU_DCDC_CTRL1.bit.DCDC_MG_LIGHT = pmu_cdcd_ctrl1_buf.bit.DCDC_MG_LIGHT;
    }
}
