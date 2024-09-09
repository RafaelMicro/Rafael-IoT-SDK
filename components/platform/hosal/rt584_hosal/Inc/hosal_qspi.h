/** @file hosal_spi.h
 * @license
 * @description
 */

#ifndef __HOSAL_SPI_H__
#define __HOSAL_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_NUMBER_OF_QSPI 2
#define QSPI0_BASE_CLK     20

// #define QSPI_NORMAL_SPI           0         /*!< SPI normal mode   */
// #define QSPI_DUAL_SPI             2         /*!< QSPI 2bits mode   */
// #define QSPI_QUAD_SPI             3         /*!< QSPI 4bits mode   */

// #define QSPI_CLK_32M              (0)                                             /*!< SPI running 32MHz   */
// #define QSPI_CLK_16M              (QSPI_MST_CLKDIV_EN | QSPI_MST_CLKDIV_16MHZ)      /*!< SPI running 16MHz   */
// #define QSPI_CLK_8M               (QSPI_MST_CLKDIV_EN | QSPI_MST_CLKDIV_8MHZ)       /*!< SPI running  8MHz   */
// #define QSPI_CLK_4M               (QSPI_MST_CLKDIV_EN | QSPI_MST_CLKDIV_4MHZ)       /*!< SPI running  4MHz   */
// #define QSPI_CLK_2M               (QSPI_MST_CLKDIV_EN | QSPI_MST_CLKDIV_2MHZ)       /*!< SPI running  2MHz   */
// #define QSPI_CLK_1M               (QSPI_MST_CLKDIV_EN | QSPI_MST_CLKDIV_1MHZ)       /*!< SPI running  1MHz   */

// #define QSPI_STATUS_TRANSFER_COMPLETE     (1 << 0)
// #define QSPI_STATUS_TRANSFER_SHORT        (1 << 1)

// #define QSPI_LSB_ORDER             (0)       /*!< SPI Least significant bit shifted out first.   */
// #define QSPI_MSB_ORDER             (1)       /*!< SPI Most significant bit shifted out first.    */

// #define QSPI_SELECT_SLAVE_0        (0)       /*!< Select Slave 0     */
// #define QSPI_SELECT_SLAVE_1        (1)       /*!< Select Slave 1     */
// #define QSPI_SELECT_SLAVE_2        (2)       /*!< Select Slave 2     */
// #define QSPI_SELECT_SLAVE_3        (3)       /*!< Select Slave 3     */

// #define QSPI_POLARITY_LOW          (0)
// #define QSPI_POLARITY_HIGH         (1)

// #define QSPI_CHIPSEL_ACTIVE_LOW    (0)       /*!< Active low for Chip Select, slave selected in CS in low    */
// #define QSPI_CHIPSEL_ACTIVE_HIGH   (1)       /*!< Active high for Chip Select,slave selected in CS in high   */

// #define QSPI_SLAVE_MODE            (0)       /*!< SPI controller will be set as slave mode.    */
// #define QSPI_MASTER_MODE           (1)       /*!< SPI controller will be set as master mode.   */

// #define QSPI_TX_FIFO_OFFSET        0
// #define QSPI_RX_FIFO_OFFSET        4

// #define QSPI_CNTL_NODMA_MASK       (0xFF)

// #define QSPI_BITSIZE_8             (1 << 4)
// #define QSPI_BITSIZE_32            (7 << 4)

// #define QSPI_DISABLE_OUT           (1 << 2)
// #define QSPI_DISABLE_IN            (1 << 3)

// #define QSPI_Xfer_Extend           (1 << 7)

// #define QSPI_CNTL_contXfer_SHIFT   (0)
// #define QSPI_CNTL_contXfer_En      (1 << QSPI_CNTL_contXfer_SHIFT)

// #define QSPI_CNTL_EDIAN_SHIFT      (1)
// #define QSPI_CNTL_LITTLE_ENDIAN    (1 << QSPI_CNTL_EDIAN_SHIFT)

// #define QSPI_CNTL_MSB_SHIFT        (2)
// #define QSPI_CNTL_CPHA_SHIFT       (3)
// #define QSPI_CNTL_CPOL_SHIFT       (4)

// #define QSPI_CNTL_MASTER_SHIFT     (5)
// #define QSPI_CNTL_MASTER           (1 << 5)
// #define QSPI_CNTL_SLAVE            (0 << 5)

// #define SPI_CNTL_SLAVE_SDATA_SHIFT  (6)
// #define SPI_CNTL_SLAVE_SDATA_OUT    (1 << 6)

// #define QSPI_CNTL_DMA_SHIFT         (10)

// #define QSPI_CNTL_mWaitEn_SHIFT     (11)

// #define QSPI_CNTL_rxWmark_SHIFT     (12)
// #define QSPI_CNTL_txWmark_SHIFT     (14)

// #define QSPI_CNTL_DMA_ENABLE        (1<<QSPI_CNTL_DMA_SHIFT)

// #define QSPI_CNTL_TX_1_8_WATERMARK     (00 < <QSPI_CNTL_txWmark_SHIFT)
// #define QSPI_CNTL_TX_1_4_WATERMARK     (01 << QSPI_CNTL_txWmark_SHIFT)
// #define QSPI_CNTL_TX_HALF_WATERMARK    (10 << QSPI_CNTL_txWmark_SHIFT)
// #define QSPI_CNTL_TX_3_4_WATERMARK     (11 << QSPI_CNTL_txWmark_SHIFT)

// #define QSPI_CNTL_RX_1_8_WATERMARK     (00 << QSPI_CNTL_rxWmark_SHIFT)
// #define QSPI_CNTL_RX_1_4_WATERMARK     (01 << QSPI_CNTL_rxWmark_SHIFT)
// #define QSPI_CNTL_RX_HALF_WATERMARK    (10 << QSPI_CNTL_rxWmark_SHIFT)
// #define QSPI_CNTL_RX_3_4_WATERMARK     (11 << QSPI_CNTL_rxWmark_SHIFT)

// #define QSPI_STATUS_xferIP           (1 << 0)
// #define QSPI_STATUS_AllCmdDone       (1 << 1)
// #define QSPI_STATUS_txEmpty          (1 << 2)
// #define QSPI_STATUS_txWmark          (1 << 3)
// #define QSPI_STATUS_txFull           (1 << 4)
// #define QSPI_STATUS_rxEmpty          (1 << 5)
// #define QSPI_STATUS_rxWmark          (1 << 6)
// #define QSPI_STATUS_rxFull           (1 << 7)

// #define QSPI_INT_txEmpty             (1 << 0)
// #define QSPI_INT_txWmark             (1 << 1)
// #define QSPI_INT_rxWmark             (1 << 2)
// #define QSPI_INT_rxFull              (1 << 3)
// #define QSPI_INT_xferDone            (1 << 4)
// #define QSPI_INT_rxNotEmpty          (1 << 5)

// #define QSPI_MST_CLKDIV_EN           (1 << 8)

// #define QSPI_MST_CLKDIV_16MHZ        (0)
// #define QSPI_MST_CLKDIV_8MHZ         (1)
// #define QSPI_MST_CLKDIV_4MHZ         (3)
// #define QSPI_MST_CLKDIV_2MHZ         (7)
// #define QSPI_MST_CLKDIV_1MHZ         (15)

// #define QSPI_DMA_ISR_TX              (1 << 1)
// #define QSPI_DMA_ISR_RX              (1 << 0)

// #define QSPI_DMA_ISR_CLEARALL        (QSPI_DMA_ISR_TX | QSPI_DMA_ISR_RX)

// #define QSPI_DMA_IER_TX              QSPI_DMA_ISR_TX
// #define QSPI_DMA_IER_RX              QSPI_DMA_ISR_RX

// #define QSPI_DMA_ENABLE              (1 << 0)
// #define QSPI_DMA_Dummy_ENABLE        (1 << 1)

// #define QSPI0_CLK_INDEX               (20)

// #define QSPI_STATE_UNINIT         0         /*!< SPI controller in uninitial state.   */
// #define QSPI_STATE_IDLE           1         /*!< SPI controller in idle state.        */
// #define QSPI_STATE_TRANSFER       2         /*!< SPI controller in CPU transfer state.  */
// #define QSPI_STATE_SETUP          4         /*!< SPI controller in setting controller state.   */
// #define QSPI_STATE_WRITE_DMA      8         /*!< SPI controller in DMA write (TX only) transfer state.   */
// #define QSPI_STATE_READ_DMA       16        /*!< SPI controller in DMA write/read transfer  state.   */

#define QSPI0_DATA2 14
#define QSPI0_DATA3 15
#define QSPI0_SCLK  6
#define QSPI0_CS0   7
#define QSPI0_DATA0 8
#define QSPI0_DATA1 9

#define QSPI1_DATA2 14
#define QSPI1_DATA3 15
#define QSPI1_SCLK  28
#define QSPI1_CS0   29
#define QSPI1_DATA0 30
#define QSPI1_DATA1 31

typedef void (*hosal_qspi_callback_t)(void* p_arg);

typedef enum {
    HOSAL_QSPI_NORMAL_SPI = QSPI_NORMAL_SPI,
    HOSAL_QSPI_DUAL_SPI = QSPI_DUAL_SPI,
    HOSAL_QSPI_QUAD_SPI = QSPI_QUAD_SPI,
} hosal_qspi_transfer_t;

typedef enum {
    HOSAL_QSPI_DATAWIDTH_SET,
    HOSAL_QSPI_DATAWIDTH_GET,
    HOSAL_QSPI_BITORDER_SET,
    HOSAL_QSPI_BITORDER_GET,
    HOSAL_QSPI_PHASE_SET,
    HOSAL_QSPI_PHASE_GET,
    HOSAL_QSPI_POLARITY_SET,
    HOSAL_QSPI_POLARITY_GET,
    HOSAL_QSPI_SLAVESELECT_SET,
    HOSAL_QSPI_SLAVESELECT_GET,
    HOSAL_QSPI_SLAVE_POLARTITY_SET,
    HOSAL_QSPI_SLAVE_POLARTITY_GET,
    HOSAL_QSPI_BAUD_SET,
    HOSAL_QSPI_BAUD_GET,
    HOSAL_QSPI_MODE_SET,
    HOSAL_QSPI_MODE_GET,
} hosal_qspi_dataset_t;

typedef enum {
    HOSAL_QSPI_BAUDRATE_32M = QSPI_CLK_32M,
    HOSAL_QSPI_BAUDRATE_16M = QSPI_CLK_16M,
    HOSAL_QSPI_BAUDRATE_8M = QSPI_CLK_8M,
    HOSAL_QSPI_BAUDRATE_4M = QSPI_CLK_4M,
    HOSAL_QSPI_BAUDRATE_2M = QSPI_CLK_2M,
    HOSAL_QSPI_BAUDRATE_1M = QSPI_CLK_1M,
} hosal_qspi_baudrate_t;

typedef enum {
    HOSAL_QSPI_DATASIZE_8 = QSPI_BITSIZE_8,
    HOSAL_QSPI_DATASIZE_32 = QSPI_BITSIZE_32,
} hosal_qspi_bitsize_t;

typedef enum {
    HOSAL_QSPI_LSB = SPI_LSB_ORDER,
    HOSAL_QSPI_MSB = SPI_MSB_ORDER,
} hosal_qspi_bitorder_t;

typedef enum {
    HOSAL_QSPI_POLARITY_LOW = 0,
    HOSAL_QSPI_POLARITY_HIGH = 1,
} hosal_qspi_polarity_t;

typedef enum {
    HOSAL_QSPI_PHASE_1EDGE = 0,
    HOSAL_QSPI_PHASE_2EDGE = 1,
} hosal_qspi_phase_t;

typedef enum {
    HOSAL_QSPI_CHIPSEL_ACTIVE_LOW = SPI_CHIPSEL_ACTIVE_LOW,
    HOSAL_QSPI_CHIPSEL_ACTIVE_HIGH = SPI_CHIPSEL_ACTIVE_HIGH,
} hosal_qspi_cs_polarity_t;

typedef enum {
    HOSAL_QSPI_SELECT_SLAVE_0 = SPI_SELECT_SLAVE_0,
    HOSAL_QSPI_SELECT_SLAVE_1 = SPI_SELECT_SLAVE_1,
    HOSAL_QSPI_SELECT_SLAVE_2 = SPI_SELECT_SLAVE_2,
    HOSAL_QSPI_SELECT_SLAVE_3 = SPI_SELECT_SLAVE_3,
} hosal_qspi_slave_select_t;

typedef enum {
    HOSAL_QSPI_SLAVE_MODE = SPI_SLAVE_MODE,
    HOSAL_QSPI_MASTER_MODE = SPI_MASTER_MODE,
} hosal_qspi_mode_t;

typedef enum {
    HOSAL_QSPI_STATE_UNINIT = QSPI_STATE_UNINIT,
    HOSAL_QSPI_STATE_IDLE = QSPI_STATE_IDLE,
    HOSAL_QSPI_STATE_TRANSFER = QSPI_STATE_TRANSFER,
    HOSAL_QSPI_STATE_SETUP = QSPI_STATE_SETUP,
    HOSAL_QSPI_STATE_WRITE_DMA = QSPI_STATE_WRITE_DMA,
    HOSAL_QSPI_STATE_READ_DMA = QSPI_STATE_READ_DMA,
} hosal_qspi_state_t;

typedef enum {
    HOSAL_QSPI_SUCCESS,
    HOSAL_QSPI_INVALID_PARAM,
    HOSAL_QSPI_INVALID_REQUEST,
    HOSAL_QSPI_BUSY,
    HOSAL_QSPI_NO_INIT,
    HOSAL_QSPI_ERROR,
    HOSAL_QSPI_TIMEOUT,
} hosal_qspi_status_t;

typedef enum {
    HOSAL_QSPI_TRANSFER_DMA,
} hosal_qspi_cb_type_t;

typedef struct {
    uint8_t qspi_id;
    uint8_t clk_pin;
    uint8_t cs_pin;
    uint8_t mosi_pin;
    uint8_t miso_pin;
    uint8_t data2;
    uint8_t data3;
    hosal_qspi_baudrate_t baud_rate;
    hosal_qspi_bitsize_t data_width;
    hosal_qspi_bitorder_t bit_order;
    hosal_qspi_polarity_t polarity;
    hosal_qspi_phase_t phase;
    hosal_qspi_cs_polarity_t cs_polarity;
    hosal_qspi_slave_select_t slave_select;
    hosal_qspi_mode_t mode;
} hosal_qspi_config_t;

typedef struct hosal_qspi_dev {
    QSPI_T* instance;
    IRQn_Type irq_num;
    hosal_qspi_config_t config;
    uint8_t* tx_buf;
    uint8_t* rx_buf;
    uint8_t* cmd;
    uint16_t tx_xfer_count;
    uint16_t rx_xfer_count;
    uint16_t cmd_length;
    uint16_t data_length;
    hosal_qspi_callback_t transfer_cb;
    void* p_transfer_arg;
    uint8_t qspi_state;
} hosal_qspi_dev_t;

typedef struct {
    uint8_t* cmd_buf;
    uint16_t cmd_length;
    uint8_t* write_buf;
    uint16_t write_length;
    uint8_t* read_buf;
    uint16_t read_length;
    uint8_t transfer_mode;
} hosal_qspi_flash_command_t;

// typedef struct hosal_qspi_dma_cfg {
//     uint8_t *dma_buf;
//     uint32_t dma_buf_size;
// } hosal_qspi_dma_cfg_t;

typedef hosal_qspi_status_t (*hosal_qspi_dev_dataset)(
    hosal_qspi_dev_t* qspi_dev, void* p_arg);

#define HOSAL_QSPI_DEV_DECL(dev, qspi, clk, cs, mosi, miso, d2, d3, baud, mod) \
    hosal_qspi_dev_t dev = {                                                   \
        .instance = qspi,                                                      \
        .config =                                                              \
            {                                                                  \
                .clk_pin = clk,                                                \
                .cs_pin = cs,                                                  \
                .mosi_pin = mosi,                                              \
                .miso_pin = miso,                                              \
                .data2 = d2,                                                   \
                .data3 = d3,                                                   \
                .baud_rate = baud,                                             \
                .data_width = HOSAL_QSPI_DATASIZE_8,                           \
                .bit_order = HOSAL_QSPI_MSB,                                   \
                .polarity = HOSAL_QSPI_POLARITY_LOW,                           \
                .phase = HOSAL_QSPI_PHASE_1EDGE,                               \
                .cs_polarity = HOSAL_QSPI_CHIPSEL_ACTIVE_LOW,                  \
                .mode = mod,                                                   \
            },                                                                 \
        .transfer_cb = NULL,                                                   \
    };

hosal_qspi_status_t hosal_qspi_init(hosal_qspi_dev_t* qspi_dev);
hosal_qspi_status_t hosal_qspi_transfer_dma(hosal_qspi_dev_t* qspi_dev,
                                            uint8_t* txbuf, uint8_t* rxbuf,
                                            uint16_t size);
hosal_qspi_status_t hosal_qspi_flash_command(hosal_qspi_dev_t* qspi_dev,
                                             hosal_qspi_flash_command_t* req);
hosal_qspi_status_t hosal_qspi_transfer_pio(hosal_qspi_dev_t* qspi_dev,
                                            uint8_t* txbuf, uint8_t* rxbuf,
                                            uint16_t size, uint32_t timeout);
hosal_qspi_status_t hosal_qspi_ioctl(hosal_qspi_dev_t* qspi_dev, int ctl,
                                     void* p_arg);
hosal_qspi_status_t hosal_qspi_deinit(hosal_qspi_dev_t* qspi_dev);
hosal_qspi_status_t
hosal_qspi_callback_register(hosal_qspi_dev_t* qspi_dev,
                             hosal_qspi_cb_type_t callback_type,
                             hosal_qspi_callback_t pfn_callback, void* arg);
hosal_qspi_status_t hosal_qspi_idle(hosal_qspi_dev_t* qspi_dev);
hosal_qspi_status_t hosal_qspi_abort(hosal_qspi_dev_t* qspi_dev);
uint8_t hosal_qspi_id_get(hosal_qspi_dev_t* qspi_dev);
hosal_qspi_dev_t* hosal_qspi_handle_get(uint8_t id);
hosal_qspi_status_t hosal_qspi_handle_set(hosal_qspi_dev_t* qspi_dev,
                                          uint8_t id);
hosal_qspi_status_t hosal_qspi_write_dma(hosal_qspi_dev_t* qspi_dev,
                                         hosal_qspi_flash_command_t* req);
hosal_qspi_status_t hosal_qspi_read_dma(hosal_qspi_dev_t* qspi_dev,
                                        hosal_qspi_flash_command_t* req);
hosal_qspi_status_t hosal_qspi_tranfer_command(hosal_qspi_dev_t* qspi_dev,
                                               hosal_qspi_flash_command_t* req);

#ifdef __cplusplus
}
#endif

#endif
