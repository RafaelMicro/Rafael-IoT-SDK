/***********************************************************************************************************************
 * @file     lpm.c
 * @version
 * @brief    Low Power Mode driver
 *
 * @copyright
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *    INCLUDES
 **********************************************************************************************************************/
#include "mcu.h"
#include "lpm.h"
#include "sysctrl.h"
//#include "rf_mcu_ahb.h"
#include "gpio.h"

/***********************************************************************************************************************
 *    GLOBAL VARIABLES
 **********************************************************************************************************************/
static volatile  uint32_t low_power_mask_status = LOW_POWER_NO_MASK;
static volatile  uint32_t comm_subsystem_wakeup_mask_status = COMM_SUBSYS_WAKEUP_NO_MASK;
static volatile  low_power_level_cfg_t low_power_level = LOW_POWER_LEVEL_NORMAL;
static volatile  low_power_wakeup_cfg_t low_power_wakeup = LOW_POWER_WAKEUP_NULL;
static volatile  uint32_t low_power_wakeup_update = 0;
static volatile  uint32_t low_power_wakeup_uart = 0;
static volatile  uint32_t low_power_wakeup_gpio = 0;
/***********************************************************************************************************************
 *    CONSTANTS AND DEFINES
 **********************************************************************************************************************/
#define LPM_SRAM0_RETAIN 0x1F
/***********************************************************************************************************************
 *    GLOBAL FUNCTIONS
 **********************************************************************************************************************/
void lpm_low_power_init(void)
{
        low_power_mask_status = LOW_POWER_NO_MASK;
        comm_subsystem_wakeup_mask_status = COMM_SUBSYS_WAKEUP_NO_MASK;
        low_power_level = LOW_POWER_LEVEL_NORMAL;
        low_power_wakeup = LOW_POWER_WAKEUP_NULL;
        low_power_wakeup_update = 0;
        low_power_wakeup_uart = 0;
        low_power_wakeup_gpio = 0;
}
/**
* @brief umask low power mode mask value
*/
void lpm_low_power_mask(uint32_t mask)
{
    low_power_mask_status |= mask;
}
/**
* @brief unmask low power mode mask value
*/
void lpm_low_power_unmask(uint32_t unmask)
{
    low_power_mask_status &= (~unmask);
}
/**
* @brief get low power mode mask status
*/
uint32_t lpm_get_low_power_mask_status(void)
{
    return low_power_mask_status;
}
/**
* @brief mask communication system wakeup value
*/
void lpm_comm_subsystem_wakeup_mask(uint32_t mask)
{
    comm_subsystem_wakeup_mask_status |= mask;
}
/**
* @brief unmask communication system wakeup value
*/
void lpm_comm_subsystem_wakeup_unmask(uint32_t unmask)
{
    comm_subsystem_wakeup_mask_status &= (~unmask);
}
/**
* @brief check communication system ready
*/
void lpm_comm_subsystem_check_system_ready(void)
{
    uint32_t status;
    do
    {
        status = COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_INFO;

    } while (((status & 0x01) != 1));

}
/**
* @brief disable wait communication subsystem 32k done
*/
void lpm_comm_subsystem_disable_wait_32k_done(void)
{
    uint8_t reg_buf[4];

    //RfMcu_MemoryGetAhb(SUBSYSTEM_CFG_WAIT_32K_DONE, reg_buf, 4);
    //reg_buf[3] &= ~SUBSYSTEM_CFG_WAIT_32K_DONE_DISABLE;                 //Clear Communication System Register 0x418 Bit26
    //RfMcu_MemorySetAhb(SUBSYSTEM_CFG_WAIT_32K_DONE, reg_buf, 4);
    //Reserved
}
/**
* @brief check communication subsystem slee mode
*/
void lpm_comm_subsystem_check_system_deepsleep(void)
{
    uint32_t status;

    do
    {
        status = ((COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_INFO & 0x6) >> 1);

        //printf(" %x \r\n", COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_INFO);

    } while ((status != 1UL));

}

/**
* @brief check communication subsystem slee mode
*/
void lpm_comm_subsystem_check_sleep_mode(uint32_t mode)
{
    uint32_t status;

    // do
    // {

    //     status = RfMcu_PowerStateGetAhb();

    //     //printf(" %x \r\n", status);

    // } while ((status != mode));
}
/**
* @brief get communication subsystem wakeup mask status
*/
uint32_t lpm_get_comm_subsystem_wakeup_mask_status(void)
{
    return comm_subsystem_wakeup_mask_status;
}
/**
* @brief set low power mode
*/
void lpm_set_low_power_level(low_power_level_cfg_t low_power_level_cfg)
{
    low_power_level = low_power_level_cfg;
    low_power_wakeup_update = 1;
}
/**
* @brief enable low power mode wake up source
*/
void lpm_enable_low_power_wakeup(low_power_wakeup_cfg_t low_power_wakeup_cfg)
{
    uint32_t wakeup_source = 0;
    uint32_t wakeup_source_index = 0;

    wakeup_source = (low_power_wakeup_cfg & 0xFFFF);

    if (wakeup_source == LOW_POWER_WAKEUP_GPIO)
    {
        wakeup_source_index = (low_power_wakeup_cfg >> 16);
        low_power_wakeup_gpio |= (1 << wakeup_source_index);
    }
    else if (wakeup_source == LOW_POWER_WAKEUP_UART_RX)
    {
        wakeup_source_index = (low_power_wakeup_cfg >> 16);
        low_power_wakeup_uart |= (1 << wakeup_source_index);
    }
    else
    {
        low_power_wakeup |= wakeup_source;
    }

    low_power_wakeup_update = 1;
}
/**
* @brief disable low power mode wake up source
*/
void lpm_disable_low_power_wakeup(low_power_wakeup_cfg_t low_power_wakeup_cfg)
{
    uint32_t wakeup_source = 0;
    uint32_t wakeup_source_index = 0;

    wakeup_source = (low_power_wakeup_cfg & 0xFFFF);

    if (wakeup_source == LOW_POWER_WAKEUP_GPIO)
    {
        wakeup_source_index = (low_power_wakeup_cfg >> 16);
        low_power_wakeup_gpio &= (~(1 << wakeup_source_index));
    }
    else if (wakeup_source == LOW_POWER_WAKEUP_UART_RX)
    {
        wakeup_source_index = (low_power_wakeup_cfg >> 16);
        low_power_wakeup_uart &= (~(1 << wakeup_source_index));
    }
    else
    {
        low_power_wakeup &= ~wakeup_source;
    }

    low_power_wakeup_update = 1;
}
/**
* @brief set low power mode wake up source
*/
void lpm_set_platform_low_power_wakeup(low_power_platform_enter_mode_cfg_t platform_low_power_mode)
{
    uint32_t i = 0;
    do
    {
        if (low_power_wakeup_update == 0)
        {
            break;
        }

        low_power_wakeup_update = 0;

        if (platform_low_power_mode == LOW_POWER_PLATFORM_ENTER_SLEEP)
        {

            /* UART0 sleep wake enable selection in Sleep */
            if (low_power_wakeup_uart & (1 << 0))
            {
//#if RT584_FPGA_MPA==1
//                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI3_OFF_SP = 1;  //Enable Power off in sleep
//                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI3_OFF_SP = 0;  //Disable Power off in sleep
//#endif
                UART0->wake_sleep_en = 1;
            }
            else
            {
                UART0->wake_sleep_en = 0;
            }

            /* UART1 sleep wake enable selection in Sleep */
            if (low_power_wakeup_uart & (1 << 1))
            {
#if RT584_FPGA_MPA==1
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI3_OFF_SP = 1;  //Enable Power off in sleep
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI3_OFF_SP = 0;  //Disable Power off in sleep
#endif
                UART1->wake_sleep_en = 1;
            }
            else
            {
                UART1->wake_sleep_en = 0;
            }

            /* UART2 sleep wake enable selection in Sleep */
            if (low_power_wakeup_uart & (1 << 2))
            {
#if RT584_SHUTTLE_IC==1
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI2_OFF_SP = 1;  //Enable Power off in sleep
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI2_OFF_SP = 0;  //Disable Power off in sleep
#elif RT584_FPGA_MPA==1
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI2_OFF_SP = 0;  //Disable Power off in sleep
				SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI3_OFF_SP = 0;  //Disable Power off in sleep
#endif
                UART2->wake_sleep_en = 1;
            }
            else
            {
                UART2->wake_sleep_en = 0;
            }
        }
        else if (platform_low_power_mode == LOW_POWER_PLATFORM_ENTER_DEEP_SLEEP)
        {

            SYSCTRL->sram_lowpower_3.bit.cfg_ds_rco32k_off = 1;  //Disable RCO32K in Deepsleep Mode

            if (low_power_wakeup & LOW_POWER_WAKEUP_RTC_TIMER)
            {
                SYSCTRL->sram_lowpower_3.bit.cfg_ds_rco32k_off = 0;
                SYSCTRL->sys_clk_ctrl2.bit.en_ck32_rtc = 1;
            }

            if (low_power_wakeup & LOW_POWER_WAKEUP_COMPARATOR)
            {

				
            }

            if (low_power_wakeup & LOW_POWER_WAKEUP_BOD_CMP)
            {

				
            }

            if (low_power_wakeup_uart & (1 << 0))
            {
#if RT584_FPGA_MPA==1
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI1_OFF_DS = 1;  //Enable Power off in  deep sleep
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI1_OFF_DS = 0;  //Disable Power off in deep sleep
#endif
                UART0->wake_sleep_en = 1;
            }
            else
            {
                UART0->wake_sleep_en = 0;
            }

            /* UART1 sleep wake enable selection in Sleep */
            if (low_power_wakeup_uart & (1 << 1))
            {
#if RT584_FPGA_MPA==1
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI3_OFF_DS = 1;  //Enable Power off in deep sleep
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI3_OFF_DS = 0;  //Disable Power off in deep sleep
#endif
                UART1->wake_sleep_en = 1;
            }
            else
            {
                UART1->wake_sleep_en = 0;
            }

            /* UART2 sleep wake enable selection in Sleep */
            if (low_power_wakeup_uart & (1 << 2))
            {
#if RT584_SHUTTLE_IC==1
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI2_OFF_DS = 1;  //Enable Power off in deep sleep
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI2_OFF_DS = 0;  //Disable Power off in deep sleep
#elif RT584_FPGA_MPA==1
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI2_OFF_DS = 1;  //Enable Power off in deep sleep
                SYSCTRL->SRAM_LOWPOWER_3.bit.CFG_PERI2_OFF_DS = 0;  //Disable Power off in deep sleep
#endif

                UART2->wake_sleep_en = 1;
            }
            else
            {
                UART2->wake_sleep_en = 0;
            }

        }
        else if (platform_low_power_mode == LOW_POWER_PLATFORM_ENTER_POWER_DOWN_MODE)
        {

            //
        }
    } while (0);
}
/**
* @brief disable sram in normal mode
*/
void lpm_set_sram_normal_shutdown(uint32_t value)
{
    set_sram_shutdown_normal(value);                                   /* SRAM shut-down control in Normal Mode, set the corresponding bit0~bit4 to shut-down SRAM0~SRAM4 in Normal Mode */
}
/**
* @brief disable sram in sleep and deep sleep mode
*/
void lpm_set_sram_sleep_deepsleep_shutdown(uint32_t value)
{
    if ((low_power_level == LOW_POWER_LEVEL_SLEEP0) || (low_power_level == LOW_POWER_LEVEL_SLEEP1))
    {
        set_sram_shutdown_sleep(value);
    }
    else if ((low_power_level == LOW_POWER_LEVEL_SLEEP2) || (low_power_level == LOW_POWER_LEVEL_SLEEP3))
    {
        set_sram_shutdown_deepsleep(value);
    }

}

/**
* @brief disable sram in sleep and deep sleep mode
*/
void lpm_set_gpio_deepsleep_wakeup_invert(uint32_t value)
{
    set_deepsleep_wakeup_invert(value);                                /* Set the corresponding bit0~bit31 to invert the GPIO0~GPIO31 for wakeup in DeepSleep Mode */
}

/**
* @brief disable sram in sleep and deep sleep mode
*/
void lpm_set_gpio_deepsleep_wakeup_invert_ex(uint8_t num, uint32_t value)    /* Set the corresponding bit0~bit31 to invert the GPIO0~GPIO31 for wakeup in DeepSleep Mode */
{
    gpio_setup_deep_sleep_io(num, value);
}
/**
* @brief disable sram in sleep and deep sleep mode
*/
void lpm_set_gpio_powerdown_wakeup_invert_ex(uint8_t num, uint32_t value)    /* Set the corresponding bit0~bit31 to invert the GPIO0~GPIO31 for wakeup in DeepSleep Mode */
{
    gpio_setup_deep_powerdown_io(num, value);
}

/**
* @brief check communication subsystem slee mode
*/
void lpm_comm_subsystem_power_status_check(commumication_subsystem_pwr_mode_cfg_t mode)
{
    uint32_t status;

    do
    {
        status = ((COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_INFO & 0x6) >> 1);

    } while ((status != mode));
}

void lpm_sub_system_low_power_mode(commumication_subsystem_pwr_mode_cfg_t mode)
{
    if (mode == COMMUMICATION_SUBSYSTEM_PWR_STATE_SLEEP)
    {
        COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_HOSTMODE;
        COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_SLEEP;
    }
    else if (mode == COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP)
    {
        COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_HOSTMODE;
        COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_DEEPSLEEP;
    }
    else
    {
        COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_HOSTMODE;
        COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_RESET;
        COMM_SUBSYSTEM_AHB->COMM_SUBSYSTEM_HOST |= COMMUMICATION_SUBSYSTEM_DIS_HOSTMODE;
        lpm_comm_subsystem_check_system_ready();
    }

    lpm_comm_subsystem_power_status_check(mode);
}
/**
* @brief set mcu enter low power mode
*/
void lpm_enter_low_power_mode(void)
{

    if (low_power_mask_status == LOW_POWER_NO_MASK)
    {

        if (low_power_level == ENTER_SLEEP)
        {
            lpm_set_platform_low_power_wakeup(LOW_POWER_PLATFORM_ENTER_SLEEP);                 /* set platform system wakeup source when entering sleep mode*/
            SYSCTRL->sys_power_state.bit.cfg_set_lowpower = LOW_POWER_PLATFORM_ENTER_SLEEP;                          /* platform system enter sleep mode */
            __WFI();
        }
        else if (low_power_level == ENTER_DEEP_SLEEP)
        {
            lpm_set_platform_low_power_wakeup(LOW_POWER_PLATFORM_ENTER_DEEP_SLEEP);             /* set platform system wakeup source when entering sleep mode*/
            SYSCTRL->sys_power_state.bit.cfg_set_lowpower = LOW_POWER_PLATFORM_ENTER_DEEP_SLEEP;                    /* platform system enter sleep mode */
            __WFI();
        }
        else if (low_power_level == ENTER_POWER_DOWN)
        {
            Lpm_Set_Platform_Low_Power_Wakeup(LOW_POWER_PLATFORM_ENTER_POWER_DOWN_MODE);             /* set platform system wakeup source when entering sleep mode*/
            SYSCTRL->sys_power_state.bit.cfg_set_lowpower = LOW_POWER_PLATFORM_ENTER_POWER_DOWN_MODE;                    /* platform system enter sleep mode */
            __WFI();
        }
    }
}
/**
* @brief set mcu to sleep mode
*/
void Rt584_Sleep_Mode(void)
{
    SYSCTRL->sys_power_state.bit.cfg_set_lowpower = LOW_POWER_PLATFORM_ENTER_SLEEP;
    __WFI();
}
/**
* @brief set mcu to deep sleep mode
*/
void Rt584_Deep_Sleep_Mode(void)
{
    SYSCTRL->sys_power_state.bit.cfg_set_lowpower = LOW_POWER_PLATFORM_ENTER_DEEP_SLEEP;
    __WFI();
}
/**
* @brief set mcu to power down mode
*/
void Rt584_Deep_Powerdown_Mode(void)
{
    SYSCTRL->sys_power_state.bit.cfg_set_lowpower = LOW_POWER_PLATFORM_ENTER_POWER_DOWN_MODE;
    __WFI();
}



