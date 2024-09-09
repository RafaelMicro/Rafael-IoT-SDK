/***********************************************************************************************************************
 * @file     pufs_rt_regs.h
 * @version
 * @brief     PUFsecurity PUFrt register definition
 *
 * @copyright
 **********************************************************************************************************************/
/** @defgroup PUFS_register PUF
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  PUF_Register header information
*/
#ifndef PUFS_RT_REGS_H
#define PUFS_RT_REGS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


//----------------------REGISTER STRUCT-------------------------------------//
typedef struct pufs_pif_regs
{
    volatile  uint32_t  pif0;
    volatile  uint32_t  option1;
    volatile  uint32_t  option2;
    volatile  uint32_t  _pad0;
    volatile  uint32_t  cde_lock[4];           /*0x10~0x1C  cde_lckwd_xxx (cde_seg_xxx)*/
    volatile  uint32_t  secrp[2];              /*0x20~0x24 */
    volatile  uint32_t  _pad1[2];
    volatile  uint32_t  zeroized_puf;          /*0x30*/
    volatile  uint32_t  _pad2;
    volatile  uint32_t  zeroized_otp[2];       /*0x38 0x3C*/
    volatile  uint32_t  _pad3[12];             /*0x40 ~ 0x6C*/
    volatile  uint32_t  puf_lock[4];           /*0x70 ~ 0x7C*/
    volatile  uint32_t  otp_lock[32];          /*0x80 ~ 0xFC*/
} PUF_PIF_T;

typedef struct pufs_rng_regs
{
    volatile  uint32_t  version;                /*0x00*/
    volatile  uint32_t  _pad0;
    volatile  uint32_t  feature;                /*0x08*/
    volatile  uint32_t  _pad1;
    volatile  uint32_t  status;                 /*0x10*/
    volatile  uint32_t  _pad2[3];
    volatile  uint32_t  enable;                 /*0x20*/
    volatile  uint32_t  pathsel;                /*0x24*/
    volatile  uint32_t  _pad3[2];
    volatile  uint32_t  cfg0;                   /*0x30 */
    volatile  uint32_t  cfg1;                   /*0x34  ?*/
    volatile  uint32_t  cfg2;                   /*0x38 */
    volatile  uint32_t  cfg3;                   /*0x3C */
    volatile  uint32_t  wtrmark0;               /*0x40 */
    volatile  uint32_t  wtrmark1;               /*0x44 */
    volatile  uint32_t  _pad4[2];
    volatile  uint32_t  tfc0;                   /*0x50 RO */
    volatile  uint32_t  tfc1;
    volatile  uint32_t  tfc2;
    volatile  uint32_t  tfc3;
    volatile  uint32_t  fcnt0;                  /*0x60 RO */
    volatile  uint32_t  fcnt1;                  /*0x64 RO */
    volatile  uint32_t  _pad5;
    volatile  uint32_t  htclr;                  /*0x6C */
    volatile  uint32_t  data;                   /*0x70 */
    volatile  uint32_t  _pad6[3];
} PUF_RNG_T;

typedef struct pufs_cfg_regs
{
    volatile  uint32_t  version;
    volatile  uint32_t  features0;
    volatile  uint32_t  features1;
    volatile  uint32_t  features2;
    volatile  uint32_t  interrupt;              /*0x10 */
    volatile  uint32_t  _pad1[3];
    volatile  uint32_t  status;                 /*0x20 */
    volatile  uint32_t  pdstb;                  /*0x24 */
    volatile  uint32_t  cfg1;                   /*0x28 */
    volatile  uint32_t  cfg2;
    volatile  uint32_t  _pad2[12];              /*0x30~0x5C*/
    volatile  uint32_t  cde_msk[2];             /*0x60 */
    volatile  uint32_t  otp_msk[2];             /*0x68 */
    volatile  uint32_t  puf_msk;                /*0x70 */
    volatile  uint32_t  _pad3;
    volatile  uint32_t  sec_lock;               /*0x78 */
    volatile  uint32_t  reg_lock;               /*0x7C */
} PUF_CFG_T;

typedef struct pufs_ptm_regs
{
    volatile  uint32_t  status;                 /*0x00*/
    volatile  uint32_t  rd_mode;                /*0x04*/
    volatile  uint32_t  ptc_page;               /*0x08*/
    volatile  uint32_t  healthcfg;              /*0x0C*/
    volatile  uint32_t  set_pin;                /*0x10*/
    volatile  uint32_t  set_flag;               /*0x14*/
    volatile  uint32_t  _pad0[2];
    volatile  uint32_t  puf_chk;                /*0x20*/
    volatile  uint32_t  puf_enroll;             /*0x24*/
    volatile  uint32_t  puf_zeroize;            /*0x28*/
    volatile  uint32_t  _pad1;
    volatile  uint32_t  off_chk;                /*0x30*/
    volatile  uint32_t  auto_repair;            /*0x34*/
    volatile  uint32_t  otp_zeroize;            /*0x38*/
    volatile  uint32_t  _pad2;
    volatile  uint32_t  repair_pgm;
    volatile  uint32_t  repair_reg;
    volatile  uint32_t  _pad[14];
} PUF_PTM_T;


/*2023/03/03: Because we don't include RT584 header file, we define a  OTP_SECURE_BASE here*/
#define OTP_SECURE_BASE      (0x50000000UL+0x44000UL)

#define OTP2_BS              (OTP_SECURE_BASE)

#define OTP1_BS              (OTP_SECURE_BASE + 0x400)

#define PUF_BS               (OTP_SECURE_BASE + 0x800)

#define OTP_PIF_S            ((PUF_PIF_T *) (OTP_SECURE_BASE + 0x900))

#define OTP_RNG_S            ((PUF_RNG_T *) (OTP_SECURE_BASE + 0xA00))

#define OTP_CFG_S            ((PUF_CFG_T *) (OTP_SECURE_BASE + 0xA80))

#define OTP_PTM_S            ((PUF_PTM_T *) (OTP_SECURE_BASE + 0xB80))


//----------------------REGISTER BIT MASKS----------------------------------//
#define RT_PIF_00_PUFLCK_MASK              0x000f0000
#define RT_PIF_00_OTPLCK_MASK              0x00f00000

//----------------------REGISTER POSITION BITS------------------------------//
#define RT_RNG_FUN_ENABLE_BITS             0
#define RT_RNG_OUT_ENABLE_BITS             2
#define RT_RNG_STATUS_RN_READY_BITS        10

//----------------------REGISTER BIT MASKS----------------------------------//
#define RT_CFG_REGLCK_CDEMSK_MASK          0x000f0000
#define RT_CFG_REGLCK_KEYMSK_MASK          0x00f00000

//----------------------REGISTER BIT MASKS----------------------------------//
#define RT_PTM_STATUS_BUSY_MASK            0x00000001
#define RT_PTM_STATUS_ABNORMAL_MASK        0x0000001e


/** Setter bit manipulation macro */
#define SET_BIT(addr, shift) ((addr) |= (1u << (shift)))

/** Clearing bit manipulation macro */
#define CLR_BIT(addr, shift) ((addr) &= ~(1u << (shift)))

/** Getter bit manipulation macro */
#define GET_BIT(addr, shift) (bool)(((addr) & (1u << (shift))))

/** Clear-and-Set bit manipulation macro */
#define ASSIGN_BIT(addr, shift, value) \
    (addr = ((addr & ~(1u << (shift))) | (value << (shift))))

/** Set-or-Clear bit manipulation macro */
#define SET_OR_CLEAR_BIT(addr, shift, flag) \
    (flag)? (SET_BIT(addr, shift)): (CLR_BIT(addr, shift))



#define OTP_LCK_RW    0
#define OTP_LCK_RO    1
#define OTP_LCK_NA    2

#define OTP_NOT_ZEROIZED   0
#define OTP_ZEROIZED       1


/**
 * @brief Set RNG enable configuration
 *
 * @param regs      rng register
 * @param fun_en    whether to enable rng function
 * @param hw_out_en whether to enable RN output to RNG_HW_OUT
 */
void rt_write_rng_enable(bool fun_en);

/**
 * @brief Count 1's
 */
uint32_t count_ones(uint32_t num);
#if RT584_SHUTTLE_IC==1
uint32_t get_otp_zeroized_state(uint32_t number);

void set_otp2_lckwd_readonly(uint32_t number);

void set_otp_lckwd_readonly(uint32_t number);

void set_otp_lckwd_na(uint32_t number);

uint32_t get_otp_lckwd_state(uint32_t number);

void set_otp_postmasking(uint32_t lock_otp_number);

void set_otp_zeroized(uint32_t number);
#endif
/*@}*/ /* end of peripheral_group FLASHCTRL_Register */

#ifdef __cplusplus
} // closing brace for extern "C"
#endif

#endif /*_RT584_PUFS_RT_REGS_H__*/
