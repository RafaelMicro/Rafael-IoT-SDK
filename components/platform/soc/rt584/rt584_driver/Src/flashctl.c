/***********************************************************************************************************************
 * @file     flashctl.c
 * @version
 * @brief
 *
 * @copyright
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *    INCLUDES
 **********************************************************************************************************************/
#include "flashctl.h"
#include "sysctrl.h"
/***********************************************************************************************************************
 *    GLOBAL FUNCTIONS
 **********************************************************************************************************************/

uint32_t flash_check_address(uint32_t flash_address, uint32_t length)
{
#if (!defined(CONFIG_FLASH_CHECK_ADDRESS_DISABLE))

    uint16_t  flash_size_id;

    //get flash size id
    flash_size_id = ( (flash_get_deviceinfo() >> FLASH_SIZE_ID_SHIFT) & 0xFF );

    if ( (flash_address < BOOT_LOADER_END_PROTECT_ADDR) || ( (flash_address + (length - 1) ) >= FLASH_END_ADDR(flash_size_id)) )
    {
        return STATUS_INVALID_PARAM;
    }

#endif
    return STATUS_SUCCESS;
}

/**
* @brief Flash_Get_Deviceinfo
*   Get the flash identifies for check flash size.
*/
uint32_t flash_get_deviceinfo(void)
{
    return (FLASH->flash_info);
}
/**
* @brief Flash_Get_flash size
*
*/
flash_size_t flash_size(void)
{
    uint32_t  flash_size_id;

    flash_size_id = ( (flash_get_deviceinfo() >> FLASH_SIZE_ID_SHIFT) & 0xFF );

    if (flash_size_id == FLASH_512K)
    {
        return FLASH_512K;
    }
    else if ( flash_size_id == FLASH_1024K)
    {
        return FLASH_1024K;
    }
    else if ( flash_size_id == FLASH_2048K)
    {
        return FLASH_2048K;
    }
    else
    {
        return FLASH_NOT_SUPPORT;
    }
}

/**
* @brief flash_read_page :
*   read one page. One page is 256 bytes, so buf_addr should have 256 bytes available.
*   this is non_block mode... so user should wait flash finish read outside this function.
*/

uint32_t flash_read_page(uint32_t buf_addr, uint32_t read_page_addr)
{

//#if LPWR_FLASH_PROTECT_ENABLE==1

//    if (flash_protect == TRUE)
//    {
//        return STATUS_INVALID_PARAM;
//    }

//#endif

//    if (flash_check_address(read_page_addr, LENGTH_PAGE) == STATUS_INVALID_PARAM)
//    {
//        return STATUS_INVALID_PARAM; //invalid addres range
//    }

    if (flash_check_busy())
    {
        return STATUS_EBUSY;
    }

    FLASH->command =  CMD_READPAGE;
    FLASH->flash_addr = read_page_addr;
    FLASH->mem_addr  = buf_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    return STATUS_SUCCESS;          /*remember to wait flash to finish read outside the caller*/
}
/**
* @brief
* flash_read_page_syncmode :
 *   read one page. One page is 256 bytes, so buf_addr should have 256 bytes available.
 *   This is block mode. when user call this function, system will wait all data in buf_addr
 *   the return.
*/
uint32_t flash_read_page_syncmode(uint32_t buf_addr, uint32_t read_page_addr)
{

//#if LPWR_FLASH_PROTECT_ENABLE==1

//    if (flash_protect == TRUE)
//    {
//        return STATUS_INVALID_PARAM;
//    }
//#endif

//    if (flash_check_address(read_page_addr, LENGTH_PAGE) == STATUS_INVALID_PARAM)
//    {
//        return STATUS_INVALID_PARAM; //invalid addres range
//    }

    if (flash_check_busy())
    {
        return STATUS_EBUSY;     /*flash busy.. please call this function again*/
    }


    FLASH->command =  CMD_READPAGE;
    FLASH->flash_addr = read_page_addr;
    FLASH->mem_addr  = buf_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    while (flash_check_busy()) {;}

    return STATUS_SUCCESS;      /*all data in register buffer now*/
}
/**
 * @brief Flash_Read_Byte
 *        The API function to get one byte data form address
*/
uint8_t flash_read_byte(uint32_t read_addr)
{

//#if LPWR_FLASH_PROTECT_ENABLE==1
//    if (flash_protect == TRUE)
//    {
//        return STATUS_INVALID_PARAM;
//    }
//#endif
    /*this is not a good idea to block function here....*/
    if (flash_check_busy())
    {
        return STATUS_EBUSY;     /*flash busy.. please call this function again*/
    }

    FLASH->command =  CMD_READBYTE;
    FLASH->flash_addr = read_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    while (flash_check_busy()) {;}

    return FLASH->flash_data >> 8;
}

uint32_t flash_read_byte_check_addr(uint32_t *buf_addr, uint32_t read_addr)
{
//#if LPWR_FLASH_PROTECT_ENABLE==1
//    if (flash_protect == TRUE)
//    {
//        return STATUS_INVALID_PARAM;
//    }
//#endif

//    if (flash_check_address(read_addr, LENGTH_BYTE) == STATUS_INVALID_PARAM)
//    {
//        return STATUS_INVALID_PARAM; //invalid addres range
//    }

    if (flash_check_busy())
    {
        return STATUS_EBUSY;     /*flash busy.. please call this function again*/
    }

    FLASH->command =  CMD_READBYTE;
    FLASH->flash_addr = read_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    while (flash_check_busy()) {;}

    *buf_addr = (FLASH->flash_data >> 8) & 0xFF;

    return STATUS_SUCCESS;
}
/**
* @brief Flash_Erase: erase flash address data
*/
uint32_t flash_erase(flash_erase_mode_t mode, uint32_t flash_addr)
{

//#if LPWR_FLASH_PROTECT_ENABLE==1
//    if (flash_protect == TRUE)
//    {
//        return STATUS_INVALID_PARAM;
//    }
//#endif

    if (mode > FLASH_ERASE_SECURE)
    {
        return  STATUS_INVALID_PARAM;
    }

    /* For Safety reason, we don't implement
     * erase chip command here. */
    switch (mode)
    {
    case FLASH_ERASE_PAGE:
    {
        if ( (flash_get_deviceinfo() & 0xFF) != PUYA_MANU_ID )
        {
            return STATUS_INVALID_PARAM; //invalid flash id
        }

        if (flash_check_address(flash_addr, LENGTH_PAGE) == STATUS_INVALID_PARAM)
        {
            return STATUS_INVALID_PARAM; //invalid addres range
        }

        FLASH->command =  CMD_ERASEPAGE;
        break;
    }
    case FLASH_ERASE_SECTOR:
    {
        if (flash_check_address(flash_addr, LENGTH_4KB) == STATUS_INVALID_PARAM)
        {
            return STATUS_INVALID_PARAM; //invalid addres range
        }

        FLASH->command =  CMD_ERASESECTOR;
        break;
    }
    case FLASH_ERASE_32K:
    {
        if (flash_check_address(flash_addr, LENGTH_32KB) == STATUS_INVALID_PARAM)
        {
            return STATUS_INVALID_PARAM; //invalid addres range
        }

        FLASH->command =  CMD_ERASE_BL32K;
        break;
    }
    case FLASH_ERASE_64K:
    {
        if (flash_check_address(flash_addr, LENGTH_64KB) == STATUS_INVALID_PARAM)
        {
            return STATUS_INVALID_PARAM; //invalid addres range
        }

        FLASH->command =  CMD_ERASE_BL64K;
        break;
    }
    case FLASH_ERASE_SECURE:
    {
        /*This is special command for erase secure register*/
        FLASH->command = CMD_ERASE_SEC_PAGE;
        break;
    }
    default:
        return STATUS_INVALID_PARAM;
    }


    /*2022/04/28 add, Device busy. try again.*/
    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }

    FLASH->flash_addr = flash_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    while (flash_check_busy()) {;}

    return STATUS_SUCCESS;
}
/**
* @brief flash write one page (256bytes) data
*/
uint32_t flash_write_page(uint32_t buf_addr, uint32_t write_page_addr)
{

//#if LPWR_FLASH_PROTECT_ENABLE==1
//    if (flash_protect == TRUE)
//    {
//        return STATUS_INVALID_PARAM;
//    }
//#endif

//    if (flash_check_address(write_page_addr, LENGTH_PAGE) == STATUS_INVALID_PARAM)
//    {
//        return STATUS_INVALID_PARAM; //invalid addres range
//    }

    /*2022/04/28 add, Device busy. try again.*/
    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }

    FLASH->command =  CMD_WRITEPAGE;
    FLASH->flash_addr = write_page_addr;
    FLASH->mem_addr  = buf_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    while (flash_check_busy()) {;}

    return STATUS_SUCCESS;
}

/**
* @brief write one byte data into flash.
*/
uint32_t flash_write_byte(uint32_t write_addr, uint8_t singlebyte)
{

//#if LPWR_FLASH_PROTECT_ENABLE==1
//    if (flash_protect == TRUE)
//    {
//        return STATUS_INVALID_PARAM;
//    }
//#endif

//    if (flash_check_address(write_addr, LENGTH_BYTE) == STATUS_INVALID_PARAM)
//    {
//        return STATUS_INVALID_PARAM; //invalid addres range
//    }

    /*2022/04/28 add, Device busy. try again.*/
    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }

    FLASH->command =  CMD_WRITEBYTE;
    FLASH->flash_addr = write_addr;
    FLASH->flash_data = singlebyte;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    return STATUS_SUCCESS;
}

/**
* @brief Flash_Verify_Page: CRC Check function
*/
uint32_t flash_verify_page(uint32_t read_page_addr)
{
//#if LPWR_FLASH_PROTECT_ENABLE==1
//    if (flash_protect == TRUE)
//    {
//        return STATUS_INVALID_PARAM;
//    }
//#endif

//    if (flash_check_address(read_page_addr, LENGTH_PAGE) == STATUS_INVALID_PARAM)
//    {
//        return STATUS_INVALID_PARAM; //invalid addres range
//    }

    /*2022/04/28 add, Device busy. try again.*/
    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }

    FLASH->command =  CMD_READVERIFY;
    FLASH->flash_addr = read_page_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    return STATUS_SUCCESS;
}
/**
* @brief get Flash status register
*/
uint32_t flash_get_status_reg(flash_status_t *status)
{

    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }

    if ((status->require_mode)&FLASH_STATUS_RW1)
    {

        FLASH->command =  CMD_READ_STATUS1;
        FLASH->pattern = FLASH_UNLOCK_PATTER;
        FLASH->start = STARTBIT;

        /*this check_busy is very short... it just send command then to receive data*/
        while (flash_check_busy()) {;}
        status->status1 = (uint8_t)((FLASH->flash_data) >> 8);
    }

    if (status->require_mode & FLASH_STATUS_RW2)
    {

        FLASH->command =  CMD_READ_STATUS2;
        FLASH->pattern = FLASH_UNLOCK_PATTER;
        FLASH->start = STARTBIT;

        while (flash_check_busy()) {;}
        status->status2 = (uint8_t)((FLASH->flash_data) >> 8);
    }

    /*2022/01/18: GD does NOT have status bytes3.*/

    return STATUS_SUCCESS;
}
/**
* @brief set Flash status register
*/

uint32_t flash_set_status_reg(const flash_status_t *status)
{


    /*2022/04/28 add, Device busy. try again.*/
    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }
    /*
     * 2022/01/18: GD only have status bytes1 and bytes2.
     * GD only support command 0x01. So if you want to write
     *
     */
    if (status->require_mode == FLASH_STATUS_RW1_2)
    {
        
        /*GD write status2 must two bytes */
        FLASH->command =  CMD_WRITE_STATUS;
        FLASH->status  = (uint32_t) (status->status1) | (uint32_t) ((status->status2) << 8);
        FLASH->pattern = FLASH_UNLOCK_PATTER;
        FLASH->start = STARTBIT;


        while (flash_check_busy()) {;}
    }
    else if (status->require_mode == FLASH_STATUS_RW1)
    {
        FLASH->command =  CMD_WRITE_STATUS1;
        FLASH->status  = (status->status1);
        FLASH->pattern = FLASH_UNLOCK_PATTER;
        FLASH->start = STARTBIT;

        while (flash_check_busy()) {;}
    }

    return STATUS_SUCCESS;

}
/**
* @brief  program secure page data
*             Note: write_page_addr must be alignment
*/

uint32_t flash_write_sec_register(uint32_t buf_addr, uint32_t write_reg_addr)
{
    uint32_t  addr;
    /*first we should check write_reg_addr*/
    addr = write_reg_addr >> 12;


    if ((addr > 3) || (write_reg_addr & 0xFF))
    {
        /*only support 3 secureity register.*/
        /*We need secure register write to be 256 bytes alignment*/
        return STATUS_INVALID_PARAM;
    }

    /*2022/04/28 add, Device busy. try again.*/
    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }

    FLASH->command =  CMD_WRITE_SEC_PAGE;
    FLASH->flash_addr = write_reg_addr;
    FLASH->mem_addr  = buf_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    return STATUS_SUCCESS;
}
/**
* @brief  read secure register data.
 * Note: read_page_addr must be alignment
 *
*/
uint32_t flash_read_sec_register(uint32_t buf_addr, uint32_t read_reg_addr)
{
    uint32_t  addr;
    /*first we should check read_reg_addr*/
    addr = read_reg_addr >> 12;

    /*2022/04/28 add, Device busy. try again.*/
    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }

    //if((addr>3)|| (read_reg_addr & 0xFF)) {
    if (addr > 3)
    {
        /*We need secure register read to be 256 bytes alignment*/
        return STATUS_INVALID_PARAM;
    }

    FLASH->command =  CMD_READ_SEC_PAGE;
    FLASH->flash_addr = read_reg_addr;
    FLASH->mem_addr  = buf_addr;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    while (flash_check_busy()) {;}

    return STATUS_SUCCESS;
}

/**
* @brief read flash unique ID
 *  flash ID is 128bits/16 bytes.
 *  if buf_length <16, it will return required length data only.
 *  if buf_length >16, it will return 16 bytes only.
 *  if buf_length = 0 , this function will return STATUS_INVALID_PARAM
*/
uint32_t flash_get_unique_id(uint32_t flash_id_buf_addr, uint32_t buf_length)
{
    uint32_t  i;
    uint8_t  temp[16], *ptr;

    if (flash_check_busy())
    {
        return  STATUS_EBUSY;
    }

    /*
     * Notice: we don't check flash_id_buf_addr value here..
     * it should be correct address in SRAM!
     */
    if (buf_length == 0)
    {
        return STATUS_INVALID_PARAM;
    }
    else if (buf_length > 16)
    {
        buf_length = 16;
    }

    FLASH->command =  CMD_READUID;
    FLASH->page_read_word = 0xF;
    FLASH->mem_addr  = (uint32_t) temp;
    FLASH->pattern = FLASH_UNLOCK_PATTER;
    FLASH->start = STARTBIT;

    ptr = (uint8_t *) flash_id_buf_addr;    /*set address*/

    while (flash_check_busy()) {;}

    FLASH->page_read_word = 0xFF;   /*restore read one page length by default*/

    /*move unique number from stack to assign buffer*/
    for (i = 0; i < buf_length; i++)
    {
        ptr[i] = temp[i];
    }

    return STATUS_SUCCESS;
}
/**
* @brief  according mcu clock to set flash timing
*
*/
void flash_timing_init(void)
{
    uint32_t   clk_mode, sys_clk;
    uint16_t   tdp, tres, tsus, trs, flash_type_id;

    flash_timing_mode_t  flash_timing;
    /*change AHB clock also need change flash timing.*/
    flash_type_id = flash_get_deviceinfo() & FLASH_TYPE_ID_MAKS;

    clk_mode = get_ahb_system_clk();

    sys_clk = 0;
    /*check flash type to adjust flash timing*/
    if (flash_type_id == GDWQ_ID)
    {
        tdp  = GDWQ_FLASH_TDP;
        tres = GDWQ_FLASH_TRES1;
        tsus = GDWQ_FLASH_TSUS;
        trs  = GDWQ_FLASH_TRS;
    }

    if (flash_type_id == GDLQ_ID)
    {
        tdp  = GDLQ_FLASH_TDP;
        tres = GDLQ_FLASH_TRES1;
        tsus = GDLQ_FLASH_TSUS;
        trs  = GDLQ_FLASH_TRS;
    }

    if (flash_type_id == PUYA_ID)
    {
        tdp  = PUYA_FLASH_TDP;
        tres = PUYA_FLASH_TRES1;
        tsus = PUYA_FLASH_TSUS;
        trs  = PUYA_FLASH_TRS;
    }


    if (clk_mode == SYS_32MHZ_CLK)
    {
        sys_clk = 32;
    }
    else if (clk_mode == SYS_16MHZ_CLK)
    {
        sys_clk = 16;
    }
    else if (clk_mode == SYS_RCO1MHZ_CLK)
    {
        sys_clk = 1;
    }
    else if (clk_mode == SYS_48MHZ_PLLCLK)
    {
        sys_clk = 48;
    }
    else if (clk_mode == SYS_64MHZ_PLLCLK)
    {
        sys_clk = 64;
    }
    else if (clk_mode == SYS_72MHZ_PLLCLK)
    {
        sys_clk = 72;
    }
    else if (clk_mode == SYS_36MHZ_PLLCLK)
    {
        sys_clk = 36;
    }
    else if (clk_mode == SYS_40MHZ_PLLCLK)
    {
        sys_clk = 40;
    }


    flash_timing.deep_pd_timing = tdp * sys_clk + 2;
    flash_timing.deep_rpd_timing = tres * sys_clk + 2;
    flash_timing.suspend_timing = tsus * sys_clk + 2;
    flash_timing.resume_timing = trs * sys_clk + 2;

    //for FPGA Verify
    flash_set_timing(&flash_timing);
}
/**
* @brief  settin flash deep power down, suspen, resume timing
*
*/
void flash_set_timing(flash_timing_mode_t *timing_cfg)
{
    FLASH->dpd = timing_cfg->deep_pd_timing;
    FLASH->rdpd = timing_cfg->deep_rpd_timing;
    FLASH->suspend = timing_cfg->suspend_timing;
    FLASH->resume  = timing_cfg->resume_timing;
    return;
}
/**
* @brief  read secure register data.
*                 Note: read_page_addr must be alignment
*/
static uint32_t flash_read_otp_sec_register(uint32_t buf_addr, uint32_t read_reg_addr)
{
    return STATUS_SUCCESS;
}
/**
* @brief read otp sec page data
*/
uint32_t flash_read_otp_sec_page(uint32_t buf_addr)
{
    return STATUS_SUCCESS;
}

/**
* @brief Enable flash Suspend fucntion
*/
void flash_enable_suspend(void)
{
    FLASH->control_set = FLASH->control_set | 0x200;
}

/**
*@brief Disable flash Suspend fucntion
*/
void flash_disable_suspend(void)
{
    FLASH->control_set = FLASH->control_set & ~(0x200);
}

/**
*@brief get flash control register value
*/
uint32_t flash_get_control_reg(void)
{
    return FLASH->control_set;
}

