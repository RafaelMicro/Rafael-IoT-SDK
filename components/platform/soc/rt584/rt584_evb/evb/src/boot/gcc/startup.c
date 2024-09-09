/****************************************************************************
 * @file     startup_cm33.c
 * @version
 * @brief   startup config
 *
 * @copyright
*****************************************************************************/

#include "mcu.h"
#include "system_mcu.h"
#include "cmsis_gcc.h"
/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    extern uint32_t __STACK_SEAL;
#endif

extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
void Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler               (void) __attribute__ ((weak));
void MemManage_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));


void gpio_handler                    (void) __attribute__ ((weak, alias("Default_Handler")));

void timer0_handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void timer1_handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void timer2_handler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void slowtimer0_handler               (void) __attribute__ ((weak, alias("Default_Handler")));
void slowtimer1_handler               (void) __attribute__ ((weak, alias("Default_Handler")));

void wdt_handler                     (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));

void SWI0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SWI1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));

void Dma0_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void Dma1_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));

void UART0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));

void IRM_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));

void I2C_Master0_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C_Master1_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C_Slave_Handler               (void) __attribute__ ((weak, alias("Default_Handler")));

void QSPI0_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void QSPI1_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));

void I2s0_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));

void Pwm0_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void Pwm1_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void Pwm2_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void Pwm3_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void Pwm4_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));

void FlashCtl_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));

void OTP_Handler                     (void) __attribute__ ((weak, alias("Default_Handler")));

void Crypto_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));

void Bod_Comp_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));

void CCM_AES_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void Sec_Ctrl_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));

void CommSubsys_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void Sadc_Handler                    (void) __attribute__ ((weak, alias("Default_Handler")));
void Aux_Comp_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[64];
const VECTOR_TABLE_Type __VECTOR_TABLE[64] __VECTOR_TABLE_ATTRIBUTE =
{
    (VECTOR_TABLE_Type)(&__INITIAL_SP),       /*     Initial Stack Pointer */
    Reset_Handler,                            /*     Reset Handler */
    NMI_Handler,                              /* -14 NMI Handler */
    HardFault_Handler,                        /* -13 Hard Fault Handler */
    MemManage_Handler,                        /* -12 MPU Fault Handler */
    BusFault_Handler,                         /* -11 Bus Fault Handler */
    UsageFault_Handler,                       /* -10 Usage Fault Handler */
    SecureFault_Handler,                      /*  -9 Secure Fault Handler */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    0,                                        /*     Reserved */
    SVC_Handler,                              /*  -5 SVC Handler */
    DebugMon_Handler,                         /*  -4 Debug Monitor Handler */
    0,                                        /*     Reserved */
    PendSV_Handler,                           /*  -2 PendSV Handler */
    SysTick_Handler,                          /*  -1 SysTick Handler */

    /* Core IoT Interrupts */
    gpio_handler,                             /*   0 GPIO Handler */
    timer0_handler,                           /*   1 Timer0 Handler */
    timer1_handler,                           /*   2 Timer1 Handler */
    timer2_handler,                           /*   3 Timer2 Handler */
    slowtimer0_handler,                        /*   4 Slow timer0 Handler */
    slowtimer1_handler,                        /*   5 Slow timer1 Handler */
    wdt_handler,                              /*   6 WatchDog Timer Handler */
    RTC_Handler,                              /*   7 RTC Timer Handler */
    0,                                        /*   8 Reserved */
    SWI0_Handler,                            /*   9 Software0 Handler */
    SWI1_Handler,                            /*   10 Software1 Handler */
    0,                                        /*   11 Reserved */
    Dma0_Handler,                             /*   12 DMA Channel 0 Handler */
    Dma1_Handler,                             /*   13 DMA Channel 1 Handler */
    0,                                        /*   14 Reserved */
    0,                                        /*   15 Reserved */
    UART0_Handler,                            /*   16 UART0 Handler */
    UART1_Handler,                            /*   17 UART1 Handler */
    UART2_Handler,                            /*   18 UART2 Handler */
    0,                                        /*   19 Reserved */
    IRM_Handler,                              /*   20 IRM Handler */
    I2C_Master0_Handler,                      /*   21 I2C Master0 Handler */
    I2C_Master1_Handler,                      /*   22 I2C Master1 Handler */
    I2C_Slave_Handler,                        /*   23 I2C Slave Handler*/
    0,                                        /*   24 Reserved */
    QSPI0_Handler,                            /*   25 QSPI0 Handler */
    QSPI1_Handler,                            /*   26 QSPI1 Handler */
    0,                                        /*   27 Reserved */
    0,                                        /*   28 Reserved */
    I2s0_Handler,                             /*   29 I2S Handler */
    0,                                        /*   30 Reserved */
    0,                                        /*   31 Reserved */
    Pwm0_Handler,                             /*   32 PWM0 Handler */
    Pwm1_Handler,                             /*   33 PWM1 Handler */
    Pwm2_Handler,                             /*   34 PWM2 Handler */
    Pwm3_Handler,                             /*   35 PWM3 Handler */
    Pwm4_Handler,                             /*   36 PWM4 Handler */
    0,                                        /*   37 Reserved */
    0,                                        /*   38 Reserved */
    FlashCtl_Handler,                         /*   39 Flash Controler Handler */
    OTP_Handler,                              /*   40 OTP Handler */
    Crypto_Handler,                           /*   41 Crypto Handler */
    Bod_Comp_Handler,                         /*   42 Bod Comparator Handler */
    CCM_AES_Handler,                          /*   43 AES CCM Handler*/
    Sec_Ctrl_Handler,                         /*   44 Secure Control Handler*/
    CommSubsys_Handler,                       /*   45 Comm SubSystem Handler*/
    Sadc_Handler,                             /*   46 SADC Handler*/
    Aux_Comp_Handler,                         /*   47 Aux Comparator Handler*/
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{

    __set_MSP((uint32_t)(&__INITIAL_SP));
    __set_PSP((uint32_t)(&__INITIAL_SP));

#if 1
    __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
    __set_PSPLIM((uint32_t)(&__STACK_LIMIT));
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

#ifdef BOOTLOADER
    SystemInit_Bootloader();
#else
    systeminit();                             /* CMSIS System Initialization */
#endif
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*----------------------------------------------------------------------------
  Hard Fault Handler
 *----------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
    while (1);
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
    while (1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic pop
#endif

