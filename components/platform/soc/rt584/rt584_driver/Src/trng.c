/***********************************************************************************************************************
 * @file     trng.c
 * @version
 * @brief    trng driver file
 *
 * @copyright
 **********************************************************************************************************************/
/***********************************************************************************************************************
*    INCLUDES
**********************************************************************************************************************/
#include "trng.h"
#include "pufs_rt_regs.h"

/***********************************************************************************************************************
 *    GLOBAL FUNCTIONS
 **********************************************************************************************************************/
/**
* @brief get random number, using PUF random number
* @return STATUS_SUCCESS
*/
uint32_t Get_Random_Number(uint32_t *p_buffer, uint32_t number)
{
    uint32_t  i;
    volatile uint32_t *ptr;

    Enable_Secure_Write_Protect();

    rt_write_rng_enable(true);

    ptr = (volatile uint32_t *) p_buffer;

    for (i = 0; i < number ; ++i)
    {
        *ptr++ = OTP_RNG_S->data;
    }

    rt_write_rng_enable(false);

    Disable_Secure_Write_Protect();

    return STATUS_SUCCESS;
}


