
#include "mcu.h"
#include "pufs_rt_regs.h"


/*****************************************************************************
 * RT Internal API functions
 ****************************************************************************/
void rt_write_rng_enable(bool fun_en)
{
    uint32_t en =  OTP_RNG_S->enable;

    if (OTP_RNG_S->version == 0x0) // unsupport
    {
        return ;
    }

    if (fun_en)
    {
        SET_BIT(en, RT_RNG_FUN_ENABLE_BITS);
    }
    else
    {
        CLR_BIT(en, RT_RNG_FUN_ENABLE_BITS);
    }


    CLR_BIT(en, RT_RNG_OUT_ENABLE_BITS);

    OTP_RNG_S->enable = en;

    if (fun_en)
        while (!(GET_BIT(OTP_RNG_S->status, RT_RNG_STATUS_RN_READY_BITS))) ;
}


void set_otp2_lckwd_readonly(uint32_t number)
{
    uint32_t offset, mask;

    if (number > 255)
    {
        return;    /*otp2 only 1024 bytes. so cde255 is maximum*/
    }

    offset = (number >> 5);

    mask = (0xF << (offset << 2));

    OTP_PIF_S->cde_lock[0] |= mask;

}


void set_otp_lckwd_readonly(uint32_t number)
{
    uint32_t offset, mask;

    offset = (number >> 3);

    mask = (0x3 << ((number & 0x7) << 2));

    OTP_PIF_S->otp_lock[offset] |= mask;
}

void set_otp_lckwd_na(uint32_t number)
{
    uint32_t offset, mask;

    offset = (number >> 3);

    mask = (0xF << ((number & 0x7) << 2));

    OTP_PIF_S->otp_lock[offset] |= mask;
}




uint32_t get_otp_lckwd_state(uint32_t number)
{
    uint32_t offset, value;

    offset = (number >> 3);

    value = (OTP_PIF_S->otp_lock[offset] >> ((number & 0x7) << 2)) & 0xF;

    switch (value)
    {
    case 0:
    case 1:
    case 2:
    case 4:
    case 8:
        return OTP_LCK_RW;

    case 3:
    case 7:
    case 11:
        return OTP_LCK_RO;

    default:
        return OTP_LCK_NA;
    }

}

void set_otp_zeroized(uint32_t number)
{
    uint32_t offset, mask, value;

    if (number >= 256)
    {
        return;
    }

    offset = number >> 7;  /*otp_128 in one 4-bytes register */

    number &= 0x7F;        /*each register control OTP_128 */

    mask = 3 << ((number >> 3) * 2);

    OTP_PIF_S->zeroized_otp[offset] |= mask;

    /*first wait PTM busy state to 0.*/
    while (OTP_PTM_S->status & BIT0)
        ;

}

uint32_t get_otp_zeroized_state(uint32_t number)
{
    uint32_t offset, mask, value;
    uint32_t  *addr, test_value;

    if (number >= 256)
    {
        return OTP_NOT_ZEROIZED;    /*in fact, it is error*/
    }

    offset = number >> 7;  /*otp_128 in one 4-bytes register */

    number &= 0x7F;

    mask = 3 << ((number >> 3) * 2);

    value = (OTP_PIF_S->zeroized_otp[offset] >> ((number >> 3) * 2)) & 0x3;

    if (value == 3)
    {
        return OTP_ZEROIZED;
    }
    else
    {
        return OTP_NOT_ZEROIZED;
    }

}

void set_otp_postmasking(uint32_t lock_otp_number)
{
    uint32_t  bit_mask_shift, lock_otp_reg_index;

    bit_mask_shift = (lock_otp_number >> 3) << 1; /*this is otp_n index */

    if (bit_mask_shift < 32)
    {
        /*postmask in otp_psmsk_0 register offset 0x68*/
        /*set OTP postmsk*/
        OTP_CFG_S->otp_msk[0] = (0x3 << bit_mask_shift);
    }
    else
    {
        /*postmask in otp_psmsk_1 register offset 0x6C*/
        bit_mask_shift -= 32;
        OTP_CFG_S->otp_msk[1] = (0x3 << bit_mask_shift);
    }

    return;
}

/**
 * @brief Count 1's
 */
uint32_t count_ones(uint32_t num)
{
    uint32_t ret = 0;

    while (num != 0)
    {
        ++ret;
        num &= (num - 1);
    }

    return ret;
}
