/***********************************************************************************************************************
 * @file     swi_reg.h
 * @version
 * @brief    software interrupt register defined
 *
 * @copyright
 **********************************************************************************************************************/
/** @defgroup SWI_Register SWI
*  @ingroup  peripheral_group
*  @{
*  @brief  SWI_Register header information
*/
#ifndef SWI_REG_H
#define SWI_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif
typedef struct
{
    __OM  uint32_t  enable_irq;     /*offset:0x00*/
    __OM  uint32_t  clear_irq;      /*offset:0x04*/
    __IM  uint32_t  irq_state;      /*offset:0x08*/
    __IO  uint32_t  data;           /*offset:0x0C*/
} SWI_T;

#define ENABLE_SOFT_IRQ            (1<<0)
#define CLEAR_SOFT_IRQ             (1<<0)

#define SWI_BIT0                    BIT0
#define SWI_BIT1                    BIT1
#define SWI_BIT2                    BIT2
#define SWI_BIT3                    BIT3
#define SWI_BIT4                    BIT4
#define SWI_BIT5                    BIT5
#define SWI_BIT6                    BIT6
#define SWI_BIT7                    BIT7
#define SWI_BIT8                    BIT8
#define SWI_BIT9                    BIT9
#define SWI_BIT10                   BIT10
#define SWI_BIT11                   BIT11
#define SWI_BIT12                   BIT12
#define SWI_BIT13                   BIT13
#define SWI_BIT14                   BIT14
#define SWI_BIT15                   BIT15
#define SWI_BIT16                   BIT16
#define SWI_BIT17                   BIT17
#define SWI_BIT18                   BIT18
#define SWI_BIT19                   BIT19
#define SWI_BIT20                   BIT20
#define SWI_BIT21                   BIT21
#define SWI_BIT22                   BIT22
#define SWI_BIT23                   BIT23
#define SWI_BIT24                   BIT24
#define SWI_BIT25                   BIT25
#define SWI_BIT26                   BIT26
#define SWI_BIT27                   BIT27
#define SWI_BIT28                   BIT28
#define SWI_BIT29                   BIT29
#define SWI_BIT30                   BIT30
#define SWI_BIT31                   BIT31
/*@}*/ /* end of peripheral_group SWI_Register */
#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* end of _RT584_SWI_REG_H */

