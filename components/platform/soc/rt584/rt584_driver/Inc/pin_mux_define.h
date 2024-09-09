/******************************************************************************
 * @file     pin_mux_define.h
 * @version
 * @brief    rt58x pin mux define header file
 *
 * @copyright
 ******************************************************************************/
/** @defgroup pin mux define
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  Pin Mux Define header information
*/

#ifndef PIN_MUX_DEFINE_H
#define PIN_MUX_DEFINE_H

#include "sysctrl.h"

#ifdef __cplusplus
extern "C" {
#endif


// #if (RT584_SHUTTLE_IC==1) || (RT584_FPGA_MPW==1)

// #else
// /* {Pin, Old_Pin_Mux, New_Pin_Mux_Out, New_Pin_Mux_In}*/
// uint32_t rt58x_pin_mux_define[32][8][4] =
// {
//     //GPIO0
//     {
//         {0, MODE_GPIO,          MODE_GPIO_EX,       MODE_PIN_MUX_NULL},
//         {0, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL},
//         {0, MODE_QSPI0_CSN1,    MODE_SPI0_CSN1_EX,  MODE_PIN_MUX_NULL},
//         {0, MODE_QSPI0_CSN2,    MODE_SPI0_CSN2_EX,  MODE_PIN_MUX_NULL},
//         {0, MODE_I2S,           MODE_I2S_BCK_EX,    MODE_PIN_MUX_NULL},
//         {0, MODE_EXT32K,        MODE_GPIO_EX,       MODE_PIN_MUX_NULL},
//         {0, MODE_QSPI0_CSN3,    MODE_SPI0_CSN3_EX,  MODE_PIN_MUX_NULL},
//         {0, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL},
//     },
//     //GPIO1
//     {
//         {1, MODE_GPIO,          MODE_GPIO_EX,       MODE_PIN_MUX_NULL},
//         {1, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL},
//         {1, MODE_QSPI0_CSN1,    MODE_SPI0_CSN1_EX,  MODE_PIN_MUX_NULL},
//         {1, MODE_QSPI0_CSN2,    MODE_SPI0_CSN2_EX,  MODE_PIN_MUX_NULL},
//         {1, MODE_I2S,           MODE_I2S_WCK_EX,    MODE_PIN_MUX_NULL},
//         {1, MODE_EXT32K,        MODE_GPIO_EX,       MODE_PIN_MUX_NULL},
//         {1, MODE_QSPI0_CSN3,    MODE_SPI0_CSN3_EX,  MODE_PIN_MUX_NULL},
//         {1, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL},
//     },
//     //GPIO2
//     {
//         {2, MODE_GPIO,          MODE_GPIO_EX,       MODE_PIN_MUX_NULL},
//         {2, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL},
//         {2, MODE_QSPI0_CSN1,    MODE_SPI0_CSN1_EX,  MODE_PIN_MUX_NULL},
//         {2, MODE_QSPI0_CSN2,    MODE_SPI0_CSN2_EX,  MODE_PIN_MUX_NULL},
//         {2, MODE_I2S,           MODE_I2S_SDO_EX,    MODE_PIN_MUX_NULL},
//         {2, MODE_EXT32K,        MODE_GPIO_EX,       MODE_PIN_MUX_NULL},
//         {2, MODE_QSPI0_CSN3,    MODE_SPI0_CSN3_EX,  MODE_PIN_MUX_NULL},
//         {2, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL},
//     },
//     //GPIO3
//     {
//         {3, MODE_GPIO,          MODE_GPIO_EX,       MODE_PIN_MUX_NULL},
//         {3, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL},
//         {3, MODE_QSPI0_CSN1,    MODE_SPI0_CSN1_EX,  MODE_PIN_MUX_NULL},
//         {3, MODE_QSPI0_CSN2,    MODE_SPI0_CSN2_EX,  MODE_PIN_MUX_NULL},
//         {3, MODE_I2S,           MODE_PIN_MUX_NULL,  MODE_I2S_SDI_EX},
//         {3, MODE_EXT32K,        MODE_GPIO_EX,       MODE_PIN_MUX_NULL},
//         {3, MODE_QSPI0_CSN3,    MODE_SPI0_CSN3_EX,  MODE_PIN_MUX_NULL},
//         {3, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL},
//     },
//     //GPIO4
//     {
//         {4, MODE_GPIO,          MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {4, MODE_QSPI0,         MODE_SPI0_SDATA_OUT2_EX,    MODE_QSPI0_SDATA2_EX},
//         {4, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {4, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {4, MODE_I2S,           MODE_I2S_MCLK_EX,           MODE_PIN_MUX_NULL},
//         {4, MODE_EXT32K,        MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {4, MODE_UART,          MODE_UART1_TX_EX,           MODE_PIN_MUX_NULL},
//         {4, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO5
//     {
//         {5, MODE_GPIO,          MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {5, MODE_QSPI0,         MODE_SPI0_SDATA_OUT3_EX,    MODE_QSPI0_SDATA3_EX},
//         {5, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {5, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {5, MODE_I2S,           MODE_I2S_MCLK_EX,           MODE_PIN_MUX_NULL},
//         {5, MODE_EXT32K,        MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {5, MODE_UART,          MODE_PIN_MUX_NULL,          MODE_UART1_RX_EX},
//         {5, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO6
//     {
//         {6, MODE_GPIO,          MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {6, MODE_QSPI0,         MODE_SPI0_SCLK_OUT_EX,      MODE_QSPI0_SCLK_EX},
//         {6, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {6, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {6, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {6, MODE_EXT32K,        MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {6, MODE_UART,          MODE_UART2_TX_EX,           MODE_PIN_MUX_NULL},
//         {6, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO7
//     {
//         {7, MODE_GPIO,          MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {7, MODE_QSPI0,         MODE_SPI0_CSN0_EX,          MODE_QSPI0_CSN_EX},
//         {7, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {7, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {7, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {7, MODE_EXT32K,        MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {7, MODE_UART,          MODE_PIN_MUX_NULL,          MODE_UART2_RX_EX},
//         {7, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO8
//     {
//         {8, MODE_GPIO,          MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {8, MODE_QSPI0,         MODE_SPI0_SDATA_OUT0_EX,    MODE_QSPI0_SDATA0_EX},
//         {8, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {8, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {8, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {8, MODE_EXT32K,        MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {8, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {8, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO9
//     {
//         {9, MODE_GPIO,          MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {9, MODE_QSPI0,         MODE_SPI0_SDATA_OUT1_EX,    MODE_QSPI0_SDATA1_EX},
//         {9, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {9, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {9, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {9, MODE_EXT32K,        MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {9, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {9, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO10
//     {
//         {10, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {10, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {10, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {10, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {10, MODE_I2S,          MODE_I2S_BCK_EX,            MODE_PIN_MUX_NULL},
//         {10, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {10, MODE_UART,         MODE_UART1_TX_EX,           MODE_PIN_MUX_NULL},
//         {10, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO11
//     {
//         {11, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {11, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {11, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {11, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {11, MODE_I2S,          MODE_I2S_WCK_EX,            MODE_PIN_MUX_NULL},
//         {11, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {11, MODE_UART,         MODE_PIN_MUX_NULL,          MODE_UART1_RX_EX},
//         {11, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO12
//     {
//         {12, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {12, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {12, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {12, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {12, MODE_I2S,          MODE_I2S_SDO_EX,            MODE_PIN_MUX_NULL},
//         {12, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {12, MODE_UART,         MODE_UART2_TX_EX,           MODE_PIN_MUX_NULL},
//         {12, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO13
//     {
//         {13, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {13, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {13, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {13, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {13, MODE_I2S,          MODE_PIN_MUX_NULL,          MODE_I2S_SDI_EX},
//         {13, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {13, MODE_UART,         MODE_PIN_MUX_NULL,          MODE_UART2_RX_EX},
//         {13, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO14
//     {
//         {14, MODE_GPIO,          MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {14, MODE_QSPI0,         MODE_SPI0_SDATA_OUT2_EX,    MODE_QSPI0_SDATA2_EX},
//         {14, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {14, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {14, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {14, MODE_QSPI1,         MODE_SPI1_SDATA_OUT2_EX,    MODE_QSPI1_SDATA2_EX},
//         {14, MODE_UART,          MODE_UART1_RTSN_EX,         MODE_PIN_MUX_NULL},
//         {14, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO15
//     {
//         {15, MODE_GPIO,          MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {15, MODE_QSPI0,         MODE_SPI0_SDATA_OUT3_EX,    MODE_QSPI0_SDATA3_EX},
//         {15, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {15, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {15, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {15, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {15, MODE_UART,          MODE_PIN_MUX_NULL,          MODE_UART1_CTSN_EX},
//         {15, MODE_PIN_MUX_NULL,  MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO16
//     {
//         {16, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {16, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {16, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {16, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {16, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {16, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {16, MODE_UART,         MODE_PIN_MUX_NULL,          MODE_UART0_RX_EX},
//         {16, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO17
//     {
//         {17, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {17, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {17, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {17, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {17, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {17, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {17, MODE_UART,         MODE_UART0_TX_EX,           MODE_PIN_MUX_NULL},
//         {17, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO18
//     {
//         {18, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {18, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {18, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {18, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {18, MODE_I2C,          MODE_I2CM0_SDA_OUT_EX,      MODE_I2CM0_SDA_EX},
//         {18, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {18, MODE_UART,         MODE_PIN_MUX_NULL,          MODE_UART1_CTSN_EX},
//         {18, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO19
//     {
//         {19, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {19, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {19, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {19, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {19, MODE_I2C,          MODE_I2CM0_SDA_OUT_EX,      MODE_I2CM0_SDA_EX},
//         {19, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {19, MODE_UART,         MODE_PIN_MUX_NULL,          MODE_UART1_CTSN_EX},
//         {19, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO20
//     {
//         {20, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {20, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {20, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {20, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {20, MODE_I2C,          MODE_I2CM0_SCL_OUT_EX,      MODE_I2CM0_SCL_EX},
//         {20, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {20, MODE_UART,         MODE_UART1_RTSN_EX,         MODE_PIN_MUX_NULL},
//         {20, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO21
//     {
//         {21, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {21, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {21, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {21, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {21, MODE_I2C,          MODE_I2CM0_SDA_OUT_EX,      MODE_I2CM0_SDA_EX},
//         {21, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {21, MODE_UART,         MODE_PIN_MUX_NULL,          MODE_UART1_CTSN_EX},
//         {21, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO22
//     {
//         {22, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {22, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {22, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {22, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {22, MODE_I2C,          MODE_I2CM0_SCL_OUT_EX,      MODE_I2CM0_SCL_EX},
//         {22, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {22, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {22, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO23
//     {
//         {23, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {23, MODE_PWM0,         MODE_PWM0_EX,               MODE_PIN_MUX_NULL},
//         {23, MODE_PWM1,         MODE_PWM1_EX,               MODE_PIN_MUX_NULL},
//         {23, MODE_PWM2,         MODE_PWM2_EX,               MODE_PIN_MUX_NULL},
//         {23, MODE_I2C,          MODE_I2CM0_SDA_OUT_EX,      MODE_I2CM0_SDA_EX},
//         {23, MODE_PWM3,         MODE_PWM3_EX,               MODE_PIN_MUX_NULL},
//         {23, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {23, MODE_PWM4,         MODE_PWM4_EX,               MODE_PIN_MUX_NULL},
//     },
//     //GPIO24
//     {
//         {24, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {24, MODE_QSPI0,        MODE_SPI0_SCLK_OUT_EX,      MODE_QSPI0_SCLK_EX},
//         {24, MODE_QSPI0_CSN1,   MODE_SPI0_CSN1_EX,          MODE_PIN_MUX_NULL},
//         {24, MODE_QSPI0_CSN2,   MODE_SPI0_CSN2_EX,          MODE_PIN_MUX_NULL},
//         {24, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {24, MODE_QSPI1,        MODE_SPI1_SCLK_OUT_EX,      MODE_QSPI1_SCLK_EX},
//         {24, MODE_QSPI0_CSN3,   MODE_SPI0_CSN3_EX,          MODE_PIN_MUX_NULL},
//         {24, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO25
//     {
//         {25, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {25, MODE_QSPI0,        MODE_SPI0_CSN0_EX,          MODE_QSPI0_CSN_EX},
//         {25, MODE_QSPI0_CSN1,   MODE_SPI0_CSN1_EX,          MODE_PIN_MUX_NULL},
//         {25, MODE_QSPI0_CSN2,   MODE_SPI0_CSN2_EX,          MODE_PIN_MUX_NULL},
//         {25, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {25, MODE_QSPI1,        MODE_SPI1_CSN0_EX,          MODE_QSPI1_CSN_EX},
//         {25, MODE_QSPI0_CSN3,   MODE_SPI0_CSN3_EX,          MODE_PIN_MUX_NULL},
//         {25, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO26
//     {
//         {26, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {26, MODE_QSPI0,        MODE_SPI0_SDATA_OUT0_EX,    MODE_QSPI0_SDATA0_EX},
//         {26, MODE_QSPI0_CSN1,   MODE_SPI0_CSN1_EX,          MODE_PIN_MUX_NULL},
//         {26, MODE_QSPI0_CSN2,   MODE_SPI0_CSN2_EX,          MODE_PIN_MUX_NULL},
//         {26, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {26, MODE_QSPI1,        MODE_SPI1_SDATA_OUT0_EX,    MODE_QSPI1_SDATA0_EX},
//         {26, MODE_QSPI0_CSN3,   MODE_SPI0_CSN3_EX,          MODE_PIN_MUX_NULL},
//         {26, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO27
//     {
//         {27, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {27, MODE_QSPI0,        MODE_SPI0_SDATA_OUT1_EX,    MODE_QSPI0_SDATA1_EX},
//         {27, MODE_QSPI0_CSN1,   MODE_SPI0_CSN1_EX,          MODE_PIN_MUX_NULL},
//         {27, MODE_QSPI0_CSN2,   MODE_SPI0_CSN2_EX,          MODE_PIN_MUX_NULL},
//         {27, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {27, MODE_QSPI1,        MODE_SPI1_SDATA_OUT1_EX,    MODE_QSPI1_SDATA1_EX},
//         {27, MODE_QSPI0_CSN3,   MODE_SPI0_CSN3_EX,          MODE_PIN_MUX_NULL},
//         {27, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO28
//     {
//         {28, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {28, MODE_QSPI0,        MODE_SPI0_SCLK_OUT_EX,      MODE_QSPI0_SCLK_EX},
//         {28, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {28, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {28, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {28, MODE_QSPI1,        MODE_SPI1_SCLK_OUT_EX,      MODE_QSPI1_SCLK_EX},
//         {28, MODE_UART,         MODE_UART1_TX_EX,           MODE_PIN_MUX_NULL},
//         {28, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO29
//     {
//         {29, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {29, MODE_QSPI0,        MODE_SPI0_CSN0_EX,          MODE_QSPI0_CSN_EX},
//         {29, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {29, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {29, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {29, MODE_QSPI1,        MODE_SPI1_CSN0_EX,          MODE_QSPI1_CSN_EX},
//         {29, MODE_UART,         MODE_PIN_MUX_NULL,          MODE_UART1_RX_EX},
//         {29, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO30
//     {
//         {30, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {30, MODE_QSPI0,        MODE_SPI0_SDATA_OUT0_EX,    MODE_QSPI0_SDATA0_EX},
//         {30, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {30, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {30, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {30, MODE_QSPI1,        MODE_SPI1_SDATA_OUT0_EX,    MODE_QSPI1_SDATA0_EX},
//         {30, MODE_UART,         MODE_UART2_TX_EX,           MODE_PIN_MUX_NULL},
//         {30, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
//     //GPIO31
//     {
//         {31, MODE_GPIO,         MODE_GPIO_EX,               MODE_PIN_MUX_NULL},
//         {31, MODE_QSPI0,        MODE_SPI0_SDATA_OUT1_EX,    MODE_QSPI0_SDATA1_EX},
//         {31, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {31, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {31, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//         {31, MODE_QSPI1,        MODE_SPI1_SDATA_OUT1_EX,    MODE_QSPI1_SDATA1_EX},
//         {31, MODE_UART,         MODE_PIN_MUX_NULL,          MODE_UART2_RX_EX},
//         {31, MODE_PIN_MUX_NULL, MODE_PIN_MUX_NULL,          MODE_PIN_MUX_NULL},
//     },
// };

// #endif

#ifdef __cplusplus
}
#endif

#endif      /* end of _PIN_MUX_DEFINE_H_ */
