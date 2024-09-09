/******************************************************************************
 * @file     sadc_reg.h
 * @version  V1.00
 * @brief    SADC register definition header file
 *
 * @copyright
 *****************************************************************************/
/** @defgroup SADC_Register SADC
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  SADC_Register header information
*/
#ifndef SADC_REG_H
#define SADC_REG_H

#if defined (__CC_ARM)
#pragma anon_unions
#endif



typedef union sadc_ctrl0_s {
    struct sadc_ctrl0_b {
        uint32_t cfg_sadc_ena      : 1;
        uint32_t cfg_sadc_vga_ena  : 1;
        uint32_t cfg_sadc_ldo_ena  : 1;
        uint32_t reserved1         : 5;
        uint32_t cfg_sadc_ck_free  : 2;
        uint32_t reserved2         : 22;
    } bit;
    uint32_t reg;
} sadc_ctrl0_t;

typedef union sadc_ctrl1_s {
    struct sadc_ctrl1_b {
        uint32_t cfg_sadc_rst           : 1;
        uint32_t reserved1              : 7;
        uint32_t cfg_sadc_afifo_rst     : 1;
        uint32_t reserved2              : 23;
    } bit;
    uint32_t reg;
} sadc_ctrl1_t;

typedef union sadc_ctrl2_s {
    struct sadc_ctrl2_b {
        uint32_t cfg_sadc_start    : 1;
        uint32_t reserved          : 31;
    } bit;
    uint32_t reg;
} sadc_ctrl2_t;

typedef union sadc_set0_s {
    struct sadc_set0_b {
        uint32_t cfg_sadc_smp_mode      : 1;
        uint32_t cfg_sadc_tmr_cksel     : 1;
        uint32_t cfg_sadc_afifo_ckpsel  : 1;
        uint32_t cfg_sadc_dbg_sel       : 4;
        uint32_t reserved               : 9;
        uint32_t cfg_sadc_tmr_ckdiv     : 16;
    } bit;
    uint32_t reg;
} sadc_set0_t;

typedef union sadc_set1_s {
    struct sadc_set1_b {
        uint32_t cfg_sadc_bit           : 4;
        uint32_t cfg_sadc_chx_sel       : 4;
        uint32_t cfg_sadc_osr           : 4;
        uint32_t cfg_sadc_tst           : 4;
        uint32_t cfg_sadc_val_tst       : 12;
        uint32_t reserved               : 4;
    } bit;
    uint32_t reg;
} sadc_set1_t;

typedef union sadc_pnsel_ch_s {
    struct sadc_pnsel_ch_b {
        uint32_t cfg_sadc_psel_ch       : 4;
        uint32_t cfg_sadc_nsel_ch       : 4;
        uint32_t cfg_sadc_gain_ch       : 6;
        uint32_t cfg_sadc_ref_in_ch     : 2;
        uint32_t cfg_sadc_pull_ch       : 8;
        uint32_t cfg_sadc_tacq_ch       : 3;
        uint32_t reserved1              : 1;
        uint32_t cfg_sadc_edly_ch       : 3;
        uint32_t reserved2              : 1;
    } bit;
    uint32_t reg;
} sadc_pnsel_ch_t;

typedef union sadc_set_ch_s {
    struct sadc_set_ch_b {
        uint32_t reserved               : 31;
        uint32_t cfg_sadc_burst_ch      : 1;
    } bit;
    uint32_t reg;
} sadc_set_ch_t;

typedef union sadc_thd_ch_s {
    struct sadc_thd_ch_b {
        uint32_t cfg_sadc_lthd_ch       : 14;
        uint32_t reserved1              : 2;
        uint32_t cfg_sadc_hthd_ch       : 14;
        uint32_t reserved2              : 2;
    } bit;
    uint32_t reg;
} sadc_thd_ch_t;

typedef union sadc_ana_set0_s {
    struct sadc_ana_set0_b {
        uint32_t    aux_adc_debug       : 1;
        uint32_t    aux_adc_mode        : 1;
        uint32_t    aux_adc_outputstb   : 1;
        uint32_t    aux_adc_ospn        : 1;
        uint32_t    aux_adc_clk_sel     : 2;
        uint32_t    aux_adc_mcap        : 2;
        uint32_t    aux_adc_mdly        : 2;
        uint32_t    aux_adc_sel_duty    : 2;
        uint32_t    aux_adc_os          : 2;
        uint32_t    reserved1           : 2;
        uint32_t    aux_adc_br          : 4;
        uint32_t    aux_adc_pw          : 3;
        uint32_t    reserved2           : 1;
        uint32_t    aux_adc_stb_bit     : 3;
        uint32_t    reserved3           : 1;
        uint32_t    aux_pw              : 3;
        uint32_t    reserved4           : 1;

    } bit;
    uint32_t reg;
} sadc_ana_set0_t;

typedef union sadc_ana_set1_s {
    struct sadc_ana_set1_b {
        uint32_t    aux_vga_cmsel       : 4;
        uint32_t    aux_vga_comp        : 2;
        uint32_t    aux_vga_sin         : 2;

        uint32_t    aux_vga_lout        : 1;
        uint32_t    aux_vga_sw_vdd      : 1;
        uint32_t    aux_vga_vldo        : 2;
        uint32_t    aux_vga_acm         : 4;

        uint32_t    aux_vga_pw          : 6;
        uint32_t    aux_dc_adj          : 2;

        uint32_t    aux_test_mode       : 1;
        uint32_t    cfg_en_clkaux       : 1;
        uint32_t    aux_vga_test_aio_en : 1;
        uint32_t    reserved1           : 5;

    } bit;
    uint32_t reg;
} sadc_ana_set1_t;

typedef union sadc_wdma_ctl0_s {
    struct sadc_wdma_ctl0_b {
        uint32_t cfg_sadc_wdma_ctl0     : 1;
        uint32_t reserved               : 31;
    } bit;
    uint32_t reg;
} sadc_wdma_ctl0_t;

typedef union sadc_wdma_ctl1_s {
    struct sadc_wdma_ctl1_b {
        uint32_t cfg_sadc_wdma_ctl1     : 1;
        uint32_t reserved               : 31;
    } bit;
    uint32_t reg;
} sadc_wdma_ctl1_t;

typedef union sadc_wdma_set0_s {
    struct sadc_wdma_set0_b {
        uint32_t cfg_sadc_seg_size     : 16;
        uint32_t cfg_sadc_blk_size     : 16;
    } bit;
    uint32_t reg;
} sadc_wdma_set0_t;

typedef union sadc_wdma_set2_s {
    struct sadc_wdma_set2_b {
        uint32_t cfg_sadc_init_addr    : 1;
        uint32_t reserved1             : 3;
        uint32_t cfg_sadc_dma_data_fmt : 2;
        uint32_t reserved2             : 26;
    } bit;
    uint32_t reg;
} sadc_wdma_set2_t;

typedef union sadc_int_s {
    struct sadc_int_b {
        uint32_t wdma            : 1;
        uint32_t wdma_error      : 1;
        uint32_t done            : 1;
        uint32_t valid           : 1;
        uint32_t mode_done       : 1;
        uint32_t reserved1       : 3;
        uint32_t monitor_low     : 10;
        uint32_t monitor_high    : 10;
        uint32_t reserved2       : 4;
    } bit;
    uint32_t reg;
} sadc_int_t;

typedef union sadc_r0_s {
    struct sadc_r0_b {
        uint32_t sadc_o          : 14;
        uint32_t reserved1       : 2;
        uint32_t sadc_o_chx      : 4;
        uint32_t reserved2       : 12;
    } bit;
    uint32_t reg;
} sadc_r0_t;

typedef union sadc_r1_s {
    struct sadc_r1_b {
        uint32_t sadc_i_12b      : 12;
        uint32_t reserved        : 4;
        uint32_t sadc_num_res    : 16;
    } bit;
    uint32_t reg;
} sadc_r1_t;

typedef union sadc_r2_s {
    struct sadc_r2_b {
        uint32_t sadc_i_syn      : 12;
        uint32_t reserved1       : 4;
        uint32_t sadc_busy       : 1;
        uint32_t sadc_ana_ena    : 1;
        uint32_t reserved2       : 14;
    } bit;
    uint32_t reg;
} sadc_r2_t;

typedef struct {
    __IO sadc_ctrl0_t          sadc_ctl0;            /*0x00*/
    __IO sadc_ctrl1_t          sadc_ctl1;            /*0x04*/
    __IO sadc_ctrl2_t          sadc_ctl2;            /*0x08*/
    __IO sadc_set0_t           sadc_set0;            /*0x0C*/
    __IO sadc_set1_t           sadc_set1;            /*0x10*/
    __IO uint32_t              sadc_reserved1;       /*0x14*/
    __IO uint32_t              sadc_reserved2;       /*0x18*/
    __IO uint32_t              sadc_reserved3;       /*0x1C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch0;       /*0x20*/
    __IO sadc_set_ch_t         sadc_set_ch0;         /*0x24*/
    __IO sadc_thd_ch_t         sadc_thd_ch0;         /*0x28*/
    __IO uint32_t              sadc_reserved4;       /*0x2C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch1;       /*0x30*/
    __IO sadc_set_ch_t         sadc_set_ch1;         /*0x34*/
    __IO sadc_thd_ch_t         sadc_thd_ch1;         /*0x38*/
    __IO uint32_t              sadc_reserved5;       /*0x3C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch2;       /*0x40*/
    __IO sadc_set_ch_t         sadc_set_ch2;         /*0x44*/
    __IO sadc_thd_ch_t         sadc_thd_ch2;         /*0x48*/
    __IO uint32_t              sadc_reserved6;       /*0x4C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch3;       /*0x50*/
    __IO sadc_set_ch_t         sadc_set_ch3;         /*0x54*/
    __IO sadc_thd_ch_t         sadc_thd_ch3;         /*0x58*/
    __IO uint32_t              sadc_reserved7;       /*0x5C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch4;       /*0x60*/
    __IO sadc_set_ch_t         sadc_set_ch4;         /*0x64*/
    __IO sadc_thd_ch_t         sadc_thd_ch4;         /*0x68*/
    __IO uint32_t              sadc_reserved8;       /*0x6C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch5;       /*0x70*/
    __IO sadc_set_ch_t         sadc_set_ch5;         /*0x74*/
    __IO sadc_thd_ch_t         sadc_thd_ch5;         /*0x78*/
    __IO uint32_t              sadc_reserved9;       /*0x7C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch6;       /*0x80*/
    __IO sadc_set_ch_t         sadc_set_ch6;         /*0x84*/
    __IO sadc_thd_ch_t         sadc_thd_ch6;         /*0x88*/
    __IO uint32_t              sadc_reserved10;      /*0x8C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch7;       /*0x90*/
    __IO sadc_set_ch_t         sadc_set_ch7;         /*0x94*/
    __IO sadc_thd_ch_t         sadc_thd_ch7;         /*0x98*/
    __IO uint32_t              sadc_reserved11;      /*0x9C*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch8;       /*0xA0*/
    __IO sadc_set_ch_t         sadc_set_ch8;         /*0xA4*/
    __IO sadc_thd_ch_t         sadc_thd_ch8;         /*0xA8*/
    __IO uint32_t              sadc_reserved12;      /*0xAC*/
    __IO sadc_pnsel_ch_t       sadc_pnsel_ch9;       /*0xB0*/
    __IO sadc_set_ch_t         sadc_set_ch9;         /*0xB4*/
    __IO sadc_thd_ch_t         sadc_thd_ch9;         /*0xB8*/
    __IO sadc_ana_set0_t       sadc_ana_set0;        /*0xBC*/
    __IO sadc_ana_set1_t       sadc_ana_set1;        /*0xC0*/
    __IO uint32_t              sadc_reserved13;      /*0xC4*/
    __IO uint32_t              sadc_reserved14;      /*0xC8*/
    __IO uint32_t              sadc_reserved15;      /*0xCC*/
    __IO uint32_t              sadc_reserved16;      /*0xD0*/
    __IO uint32_t              sadc_reserved17;      /*0xD4*/
    __IO uint32_t              sadc_reserved18;      /*0xD8*/
    __IO uint32_t              sadc_reserved19;      /*0xDC*/
    __IO uint32_t              sadc_reserved20;      /*0xE0*/
    __IO uint32_t              sadc_reserved21;      /*0xE4*/
    __IO uint32_t              sadc_reserved22;      /*0xE8*/
    __IO uint32_t              sadc_reserved23;      /*0xEC*/
    __IO uint32_t              sadc_reserved24;      /*0xF0*/
    __IO uint32_t              sadc_reserved25;      /*0xF4*/
    __IO uint32_t              sadc_reserved26;      /*0xF8*/
    __IO uint32_t              sadc_reserved27;      /*0xFC*/
    __IO sadc_wdma_ctl0_t      sadc_wdma_ctl0;       /*0x100*/
    __IO sadc_wdma_ctl1_t      sadc_wdma_ctl1;       /*0x104*/
    __IO sadc_wdma_set0_t      sadc_wdma_set0;       /*0x108*/
    __IO uint32_t              sadc_wdma_set1;       /*0x10C*/
    __IO sadc_wdma_set2_t      sadc_wdma_set2;       /*0x110*/
    __IO uint32_t              sadc_wdma_r0;         /*0x114*/
    __IO uint32_t              sadc_wdma_r1;         /*0x118*/
    __IO uint32_t              sadc_reserved29;      /*0x11C*/
    __IO sadc_int_t            sadc_int_clear;       /*0x120*/
    __IO sadc_int_t            sadc_int_mask;        /*0x124*/
    __IO sadc_int_t            sadc_int_status;      /*0x128*/
    __IO sadc_r0_t             sadc_r0;              /*0x12C*/
    __IO sadc_r1_t             sadc_r1;              /*0x130*/
    __IO sadc_r2_t             sadc_r2;              /*0x134*/
} SADC_T;


/*@}*/ /* end of peripheral_group SADC_Register */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif      /* end of __RT584_SADC_REG_H__ */

