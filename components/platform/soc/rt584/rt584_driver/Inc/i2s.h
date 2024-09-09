/**************************************************************************//**
 * @file     i2s.h
 * @version
 * @brief    I2S driver header file
 *
 * @copyright
 ******************************************************************************/
/** @defgroup I2S_Driver I2S
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  I2S_Driver header information
*/
#ifndef __RT584_I2S_H__
#define __RT584_I2S_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "mcu.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

#define I2S_RDMA_ENABLE                 (0x01UL << I2S_RDMA_ENABLE_SHFT)
#define I2S_RDMA_RESET                  (0x01UL << I2S_RDMA_RESET_SHFT)
#define I2S_WDMA_ENABLE                 (0x01UL << I2S_WDMA_ENABLE_SHFT)
#define I2S_WDMA_RESET                  (0x01UL << I2S_WDMA_RESET_SHFT)
#define I2S_RDMA_IRQ_CLR                (0x01UL << I2S_RDMA_IRQ_CLR_SHFT)
#define I2S_RDMA_ERR_IRQ_CLR            (0x01UL << I2S_RDMA_ERR_IRQ_CLR_SHFT)
#define I2S_WDMA_IRQ_CLR                (0x01UL << I2S_WDMA_IRQ_CLR_SHFT)
#define I2S_WDMA_ERR_IRQ_CLR            (0x01UL << I2S_WDMA_ERR_IRQ_CLR_SHFT)
#define I2S_RDMA_IRQ_MASK_ENABLE        (0x01UL << I2S_RDMA_IRQ_MASK_SHFT)
#define I2S_RDMA_ERR_IRQ_MASK_ENABLE    (0x01UL << I2S_RDMA_ERR_IRQ_MASK_SHFT)
#define I2S_WDMA_IRQ_MASK_ENABLE        (0x01UL << I2S_WDMA_IRQ_MASK_SHFT)
#define I2S_WDMA_ERR_IRQ_MASK_ENABLE    (0x01UL << I2S_WDMA_ERR_IRQ_MASK_SHFT)

#define I2S_DISABLE_I2S()               (I2S_MASTER->I2S_MS_CTL0.bit.CFG_I2S_ENA = 0)
#define I2S_DISABLE_MCLK()              (I2S_MASTER->I2S_MS_CTL0.bit.CFG_MCK_ENA = 0)
#define I2S_RESET()                     (I2S_MASTER->I2S_MS_CTL1.bit.CFG_I2S_RST = 1)

#define I2S_GET_RDMA_ACCESS_POS()       (I2S_MASTER->I2S_RDMA_R0)
#define I2S_GET_WDMA_ACCESS_POS()       (I2S_MASTER->I2S_WDMA_R0)
#define I2S_GET_FW_ACCESS_POS(para_set) (para_set->i2s_fw_access_addr)

#define I2S_GET_RDMA_SEG_SIZE           (I2S_MASTER->I2S_RDMA_SET0)
#define I2S_GET_RDMA_BLK_SIZE           (I2S_MASTER->I2S_RDMA_SET0>>16)
#define I2S_GET_WDMA_SEG_SIZE           (I2S_MASTER->I2S_WDMA_SET0)
#define I2S_GET_WDMA_BLK_SIZE           (I2S_MASTER->I2S_WDMA_SET0>>16)

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/**
 *  @Brief I2S-XDMA type, RDMA or WDMA.
 */
typedef enum
{
    I2S_XDMA_RDMA = 0,
    I2S_XDMA_WDMA,
    I2S_XDMA_MAX,
}   i2s_xdma_t;

/**
 *  @Brief I2S SR mapping table
 */
typedef enum
{
    I2S_SR_48K,
    I2S_SR_32K,
    I2S_SR_16K,
    I2S_SR_8K,
    I2S_SR_MAX,
} i2s_sample_rate_t;

/**
 *  @Brief I2S Internal MCLK table
 */
typedef enum
{
    I2S_IMCLK_12P288M,
    I2S_IMCLK_8P192M,
    I2S_IMCLK_24P576M,
    I2S_IMCLK_16P384M,
    I2S_IMCLK_MAX,
} i2s_cfg_imck_rate_t;

/**
 *  @Brief I2S channel mode, Stereo / Left channel only / Right channel only
 */
typedef enum
{
    I2S_CH_STEREO,
    I2S_CH_MONO_L,
    I2S_CH_MONO_R,
    I2S_CH_MAX,
} i2s_ch_format_t;

/**
 *  @Brief I2S format table, standard I2S mode / Left Justified mode / Right Justified mode
 */
typedef enum
{
    I2S_FMT_LJ = 0,
    I2S_FMT_RJ,
    I2S_FMT_I2S,
    I2S_FMT_MAX,
} i2s_fmt_t;

/**
 *  @Brief MCLK selection index table
 */
typedef enum
{
    I2S_MCLK_ISEL_0 = 0,
    I2S_MCLK_ISEL_1,
    I2S_MCLK_ISEL_2,
    I2S_MCLK_ISEL_3,
    I2S_MCLK_ISEL_4,
    I2S_MCLK_ISEL_5,
    I2S_MCLK_ISEL_MAX,
} i2s_mclk_isel_t;

/**
 *  @Brief I2S MCLK division table
 */
typedef enum
{
    I2S_MCLK_DIV_1 = 0,
    I2S_MCLK_DIV_2,
    I2S_MCLK_DIV_4,
    I2S_MCLK_DIV_8,
    I2S_MCLK_DIV_16,
    I2S_MCLK_DIV_32,
    I2S_MCLK_DIV_MAX,
} i2s_mclk_div_t;

/**
 *  @Brief I2S BCLK OSR setting table
 */
typedef enum
{
    I2S_CFG_BCK_OSR_2 = 0,
    I2S_CFG_BCK_OSR_4,
    I2S_CFG_BCK_OSR_8,
    I2S_CFG_BCK_OSR_MAX,
} i2s_cfg_bck_osr_t;

/**
 *  @Brief I2S TRX mode setting table
 */
typedef enum
{
    I2S_TRX_MODE_TXRX = 0,
    I2S_TRX_MODE_TX,
    I2S_TRX_MODE_RX,
    I2S_TRX_MODE_MAX,
} i2s_cfg_i2s_mode_t, i2s_trx_mode_t;

/**
 *  @Brief I2S date depth
 */
typedef enum
{
    I2S_CFG_WID_16 = 0,
    I2S_CFG_WID_24,
    I2S_CFG_WID_32,
    I2S_CFG_WID_MAX,
} i2s_cfg_txd_wid_t, i2s_cfg_rxd_wid_t, i2s_bck_width_t;

/**
 *  @Brief I2S BLCK / WRCK ratio
 */
typedef enum
{
    I2S_BCK_RATIO_32 = 0,
    I2S_BCK_RATIO_48,
    I2S_BCK_RATIO_64,
    I2S_BCK_RATIO_MAX,
} i2s_cfg_bck_ratio_t;

/**
 * @brief I2S callback type
 */
typedef enum
{
    I2S_CB_WDMA,    /**< CB generated when the buffer is filled with samples. */
    I2S_CB_RDMA,   /**< CB generated when the requested channel is sampled. */
} i2s_cb_type_t;


/**
 *  @Brief Structure for I2S-xDMA configurarions
 */
typedef struct
{
    uint32_t    i2s_xdma_start_addr;
    uint32_t    i2s_fw_access_addr;
    uint16_t    i2s_xdma_seg_size;
    uint16_t    i2s_xdma_blk_size;
    uint8_t     i2s_xdma_seg_blk_ratio;
} i2s_xdma_ctrl_ptr_t, i2s_rdma_ctrl_ptr_t, i2s_wdma_ctrl_ptr_t;

/**
 * @brief Structure for I2S callback function
 */
typedef struct
{
    i2s_cb_type_t type;    /**< i2s type.   */
    uint16_t      blk_size;
    uint16_t      seg_size;
} i2s_cb_t;


/**
 *  @Brief Structure for I2S configurarions
 */
typedef struct
{
    i2s_rdma_ctrl_ptr_t *rdma_config;
    i2s_wdma_ctrl_ptr_t *wdma_config;
    i2s_sample_rate_t    sr;
    i2s_ch_format_t      ch;
    i2s_trx_mode_t       trx_mode;
    i2s_fmt_t            fmt;
    i2s_cfg_txd_wid_t    width;
    i2s_cfg_bck_ratio_t  bck_ratio;
    i2s_mclk_div_t       mck_div;
    i2s_cfg_bck_osr_t    bck_osr;
    i2s_cfg_imck_rate_t  imck_rate;
} i2s_para_set_t;


typedef void (*i2s_isr_handler_t)(i2s_cb_t *p_cb);

/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
void i2s_Register_Int_Callback(i2s_isr_handler_t i2s_int_callback);

/**
 * @brief Set I2S initialize
 *
 * @param[in]   i2s_para    Specifies the I2S parameter.
 *
 * @retval     none
 */
uint32_t I2s_Init(i2s_para_set_t *i2s_para);

/**
 * @brief Set I2S uninitialize
 *
 * @retval     none
 */
uint32_t I2s_Uninit(void);

/**
 * @brief Start I2S
 *
 * @param[in]   i2s_para    Specifies the I2S parameter.
 *
 * @retval     none
 */
uint32_t I2s_Start(i2s_para_set_t *i2s_para);

/**
 * @brief Stop I2S
 *
 * @retval     none
 */
uint32_t I2s_Stop(void);

/*@}*/ /* end of peripheral_group I2S_Driver */

#ifdef __cplusplus
}
#endif

#endif      /* end of __RT584_I2S_H__ */

