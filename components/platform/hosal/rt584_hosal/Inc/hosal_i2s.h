/**
 * \file            hosal_i2s.h
 * \brief           Hosal I2S driver header file
 *          
 */
/*
 * Copyright (c) year FirstName LASTNAME
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Author:          Kc.tseng
 */

#ifndef HOSAL_I2S_H
#define HOSAL_I2S_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief           Hosal I2S Simple Rate mapping table
 */
typedef enum {
    HOSAL_I2S_SR_48K,                           /*!< I2S_WCK 48K */
    HOSAL_I2S_SR_32K,                           /*!< I2S_WCK 32K */
    HOSAL_I2S_SR_16K,                           /*!< I2S_WCK 16K */
    HOSAL_I2S_SR_8K,                            /*!< I2S_WCK 8K */
    HOSAL_I2S_SR_MAX,                           /*!< I2S_WCK MAX */
} hosal_i2s_sample_rate_t;

/**
 * \brief           Hosal I2S internal mclk setting
 */
typedef enum {
    HOSAL_I2S_IMCLK_12P288M,                    /*!< PLL_CK = 32M, I2S_WCK = 48K */
    HOSAL_I2S_IMCLK_8P192M,                     /*!< PLL_CK = 32M, I2S_WCK = 8/16/32K */
    HOSAL_I2S_IMCLK_24P576M,                    /*!< PLL_CK = 64M, I2S_WCK = 48K */
    HOSAL_I2S_IMCLK_16P384M,                    /*!< PLL_CK = 64M, I2S_WCK = 64K */
    HOSAL_I2S_IMCLK_MAX,                        /*!< PLL_CK = 64M, I2S_WCK = 8/16/32K */
} hosal_i2s_cfg_imck_rate_t;

/**
 * \brief           Hosal I2S channel mode, Stereo / Left channel only / Right channel only
 */
typedef enum {
    HOSAL_I2S_CH_STEREO,                        /*!< I2S stereo */
    HOSAL_I2S_CH_MONO_L,                        /*!< I2S left only */
    HOSAL_I2S_CH_MONO_R,                        /*!< I2S right only */
    HOSAL_I2S_CH_MAX,                           /*!< I2S channel_max */
} hosal_i2s_ch_format_t;

/**
 * \brief            Hosal I2S format table
 */
typedef enum {
    HOSAL_I2S_FMT_LJ = 0,                       /*!< I2S left justified format */
    HOSAL_I2S_FMT_RJ,                           /*!< I2S right justified format */
    HOSAL_I2S_FMT_I2S,                          /*!< I2S standard format */
    HOSAL_I2S_FMT_MAX,                          /*!< I2S format_max */
} hosal_i2s_fmt_t;

/**
 * \brief           Hosal I2S MCLK selection index table
 */
typedef enum {
    HOSAL_I2S_MCLK_ISEL_0 = 0,                  /*!< mclk selection index 0 */
    HOSAL_I2S_MCLK_ISEL_1,                      /*!< mclk selection index 1 */
    HOSAL_I2S_MCLK_ISEL_2,                      /*!< mclk selection index 2 */
    HOSAL_I2S_MCLK_ISEL_3,                      /*!< mclk selection index 3 */
    HOSAL_I2S_MCLK_ISEL_4,                      /*!< mclk selection index 4 */
    HOSAL_I2S_MCLK_ISEL_5,                      /*!< mclk selection index 5 */
    HOSAL_I2S_MCLK_ISEL_MAX,                    /*!< mclk selection index max */
} hosal_i2s_mclk_isel_t;

/**
 * \brief           Hosal I2S MCLK division table
 */
typedef enum {
    HOSAL_I2S_MCLK_DIV_1 = 0,                   /*!< mclk division 1 */
    HOSAL_I2S_MCLK_DIV_2,                       /*!< mclk division 2 */
    HOSAL_I2S_MCLK_DIV_4,                       /*!< mclk division 4 */
    HOSAL_I2S_MCLK_DIV_8,                       /*!< mclk division 8 */
    HOSAL_I2S_MCLK_DIV_16,                      /*!< mclk division 16 */
    HOSAL_I2S_MCLK_DIV_32,                      /*!< mclk division 32 */
    HOSAL_I2S_MCLK_DIV_MAX,                     /*!< mclk division max */
} hosal_i2s_mclk_div_t;

/**
 * \brief           Hosal I2S BCLK OSR setting table
 */
typedef enum {
    HOSAL_I2S_CFG_BCK_OSR_2 = 0,                /*!< bclk osr 2 */
    HOSAL_I2S_CFG_BCK_OSR_4,                    /*!< bclk osr 4 */
    HOSAL_I2S_CFG_BCK_OSR_8,                    /*!< bclk osr 8 */
    HOSAL_I2S_CFG_BCK_OSR_MAX,                  /*!< bclk osr max */
} hosal_i2s_cfg_bck_osr_t;

/**
 * \brief           I2S TRX mode setting table
 */
typedef enum {
    HOSAL_I2S_TRX_MODE_TXRX = 0,                /*!< TRX */
    HOSAL_I2S_TRX_MODE_TX,                      /*!< TX */
    HOSAL_I2S_TRX_MODE_RX,                      /*!< RX */
    HOSAL_I2S_TRX_MODE_MAX,                     /*!< TRX max */
} hosal_i2s_trx_mode_t;

/**
 * \brief           Hosal I2S date depth
 */
typedef enum {
    HOSAL_I2S_CFG_WID_16 = 0,                   /*!< data width 16 */
    HOSAL_I2S_CFG_WID_24,                       /*!< data width 24 */
    HOSAL_I2S_CFG_WID_32,                       /*!< data width 32 */
    HOSAL_I2S_CFG_WID_MAX,                      /*!< data width max */
} hosal_i2s_cfg_txd_wid_t;

/**
 * \brief           Hosal I2S BLCK / WRCK ratio
 */
typedef enum {
    HOSAL_I2S_BCK_RATIO_32 = 0, /*!< blck / wrck ratio 32 */
    HOSAL_I2S_BCK_RATIO_48,     /*!< blck / wrck ratio 48 */
    HOSAL_I2S_BCK_RATIO_64,     /*!< blck / wrck ratio 64 */
    HOSAL_I2S_BCK_RATIO_MAX,    /*!< blck / wrck ratio max */
} hosal_i2s_cfg_bck_ratio_t;

/**
 * \brief           Hosal I2S xDMA configurarions structure
 */
typedef struct {
    uint32_t i2s_xdma_start_addr;               /*!< xDMA  start address */
    uint32_t i2s_fw_access_addr;                /*!< Firmware access address */
    uint16_t i2s_xdma_seg_size;                 /*!< xDMA Segment size */
    uint16_t i2s_xdma_blk_size;                 /*!< xDMA Block size */
    uint8_t  i2s_xdma_seg_blk_ratio;            /*!< xDMA Segment and Block ratio */
} hosal_i2s_xdma_ctrl_ptr_t, hosal_i2s_rdma_ctrl_ptr_t, hosal_i2s_wdma_ctrl_ptr_t;

/**
 * \brief           I2S configurarions structure
 */
typedef struct {
    hosal_i2s_rdma_ctrl_ptr_t*  rdma_config;    /*!< rdma config */
    hosal_i2s_wdma_ctrl_ptr_t*  wdma_config;    /*!< wdma config */
    hosal_i2s_sample_rate_t     sr;             /*!< sample rate */
    hosal_i2s_ch_format_t       ch;             /*!< channel format */
    hosal_i2s_trx_mode_t        trx_mode;       /*!< TRX mode */
    hosal_i2s_fmt_t             fmt;            /*!< data format */
    hosal_i2s_cfg_txd_wid_t     width;          /*!< data width */
    hosal_i2s_cfg_bck_ratio_t   bck_ratio;      /*!< bck / wrck ratio */
    hosal_i2s_mclk_div_t        mck_div;        /*!< mclk division */
    hosal_i2s_cfg_bck_osr_t     bck_osr;        /*!< bclk osr setting */
    hosal_i2s_cfg_imck_rate_t   imck_rate;      /*!< internal mclk setting */
} hosal_i2s_para_set_t;

/**
 * \brief           Hosal I2S callback data type
 */
typedef enum {
    HOSAL_I2S_CB_WDMA,                                /*!< CB generated when the buffer is filled with samples. */
    HOSAL_I2S_CB_RDMA,                                /*!< CB generated when the requested channel is sampled. */
} hosal_i2s_cb_type_t;

/**
 * \brief           Hosal I2S callback structure
 */
typedef struct {
    hosal_i2s_cb_type_t type;                   /*!< i2s type */
    uint16_t blk_size;                          /*!< xdma blockk size */
    uint16_t seg_size;                          /*!< xdma segment size */
} hosal_i2s_cb_t;

/**
 * \brief           i2S interrupt service routine callback for user application.
 * \param[in]       p_cb: the reason of rtc alarm routine trigger
 */
typedef void (*hosal_i2s_cb_fn)(hosal_i2s_cb_t* p_cb);

/**
 * \brief           Get i2s wdma access address
 * \return          I2S wdma access address
 */
uint32_t hosal_i2s_get_wdma_access_pos(void);

/**
 * \brief           Register i2s callback function
 * \param[in]       i2s_usr_callback: user callback function
 */
void hosal_i2s_callback_register(void* i2s_usr_callback);

/**
 * \brief           The initial config function according to the i2s_para to set i2s registers
 * \param[in]       i2s_para: i2s parameter
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_i2s_init(hosal_i2s_para_set_t* i2s_para);

/**
 * \brief           Uninitial i2s
 * \return          Function status, STATUS_SUCCESS
 */
uint32_t hosal_i2s_uninit(void);

/**
 * \brief           Start i2s, and enable xdma
 * \return          Function status, STATUS_SUCCESS, STATUS_INVALID_PARAM
 */
uint32_t hosal_i2s_start(hosal_i2s_para_set_t* i2s_para);

/**
 * \brief           Stop i2s
 * \return          Function status, STATUS_SUCCESS
 */
uint32_t hosal_i2s_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* End of HOSAL_I2S_H */
