/***********************************************************************************************************************
 * @file     flashctl_reg.h
 * @version
 * @brief    flash controller register definition header file
 *
 * @copyright
 **********************************************************************************************************************/
/** @defgroup FLASHCTRL_Register FLASHCTRL
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  FLASHCTRL_Register header information
*/
#ifndef FLASHCTL_REG_H
#define FLASHCTL_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

typedef struct
{
    __IO uint32_t command;        /*offset : 0x00*/
    __IO uint32_t flash_addr;     /*offset : 0x04*/
    __IO uint32_t start;          /*offset : 0x08*/
    __IO uint32_t status;         /*offset : 0x0c*/
    __IO uint32_t flash_data;     /*offset : 0x10*/
    __IO uint32_t mem_addr;       /*offset : 0x14*/
    __IO uint32_t control_set;    /*offset : 0x18*/
    __IO uint32_t crc;            /*offset : 0x1c*/
    __IO uint32_t dpd;            /*offset : 0x20*/
    __IO uint32_t rdpd;           /*offset : 0x24*/
    __IO uint32_t suspend;        /*offset : 0x28*/
    __IO uint32_t resume;         /*offset : 0x2c*/
    __IO uint32_t flash_instr;    /*offset : 0x30*/
    __IO uint32_t page_read_word; /*offset : 0x34*/
    __IO uint32_t flash_info;     /*offset : 0x38*/
    __IO uint32_t resv;           /*offset : 0x3c*/
    __IO uint32_t flash_int;      /*offset : 0x40*/
    __IO uint32_t pattern;        /*offset : 0x44*/
} FLASHCTL_T;


#define  CMD_READBYTE       0x00
#define  CMD_READ_SDPF      0x02
#define  CMD_READVERIFY     0x04
#define  CMD_READPAGE       0x08
#define  CMD_READUID        0x09
#define  CMD_READSDPF       0x0A


#define  CMD_READ_STATUS1      0x0D
#define  CMD_READ_STATUS2      0x0E
#define  CMD_READ_STATUS3      0x0F

#define  CMD_ERASEPAGE         0x20
#define  CMD_ERASESECTOR       0x21
#define  CMD_ERASE_BL32K       0x22
#define  CMD_ERASE_BL64K       0x24

#define  CMD_WRITEBYTE         0x10
#define  CMD_WRITEPAGE         0x18

#define  CMD_WRITE_STATUS      0x1C
#define  CMD_WRITE_STATUS1     0x1D
#define  CMD_WRITE_STATUS2     0x1E
#define  CMD_WRITE_STATUS3     0x1F


#define  CMD_READ_SEC_PAGE     ((1<<6) | CMD_READPAGE)
#define  CMD_WRITE_SEC_PAGE    ((1<<6) | CMD_WRITEPAGE)
#define  CMD_ERASE_SEC_PAGE    ((1<<6) | CMD_ERASESECTOR)

#define  STARTBIT          (1)
#define  BUSYBIT           (1<<8)

#define  CMD_FLASH_RESET_ENABLE     0x66
#define  CMD_FLASH_RESET            0x99
#define  CMD_MANUAL_MODE            0x30


/*@}*/ /* end of peripheral_group FLASHCTRL_Register */
#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif /* end of _RT584_FLASHCTL_REG_H_ */
