/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>

#include "mcu.h"
#include "string.h"
//#include "project_config.h"

#include "hosal_uart.h"
//#include "uart_drv.h"
//#include "retarget.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include "rf_common_init.h"
#include "rf_mcu.h"

#include "rf_tx_comp.h"
#include "rfb_sf.h"
#include "ruci.h"

#include "lpm.h"
#include "uart_bridge.h"

//include ble fw header
/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/
#define ms_sec(N) (N * 1000)

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define UASB_SLEEP_ENABLE            (DISABLE)
#define UASB_SLEEP_TASK_ENABLE       (DISABLE && UASB_SLEEP_ENABLE)
#define UASB_BREAK_ENABLE            (ENABLE)
#define UASB_RX_PKT_DUMP_ENABLE      (DISABLE)
#define BLE_SANITY_WITH_PY           (0)
#define CHECK_RX_PACKET_CORRECT      (0)
#define UASB_COMM_Q_MAX_SIZE         (16u)
#define UASB_PCI_TXDONE_EVENT_ENABLE (ENABLE)
#define RF_MCU_RTC_REG               (0x0414)

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
struct uasb_comm_cmd_state {
    uint8_t rp;
    uint8_t wp;
    uint8_t cmd_queue[UASB_COMM_Q_MAX_SIZE];
};

typedef enum {
    UASB_UART_RTS_SET = 0,
    UASB_UART_RTS_CLR
} uasb_uart_rts_status_t;

typedef struct __PACKED rf_ub_lpm_vsc_s {
    uint8_t hci_type;
    uint16_t opcode;
    uint8_t para_len;
    low_power_level_cfg_t lpm;
    uint8_t idle_thd_host;
    uint8_t idle_thd_hc;
    uint8_t bt_wake_act_mode;
    uint8_t host_wake_act_mode;
} rf_ub_lpm_vsc_t;

typedef struct __PACKED rf_ub_hci_vsc_tx_pwr_comp_cfg_s {
    uint8_t hci_type;
    uint16_t opcode;
    uint8_t para_len;
    uint8_t interval;
} rf_ub_hci_vsc_tx_pwr_comp_cfg_t;

typedef struct __PACKED rf_ub_hci_vsc_dummy_s {
    uint8_t hci_type;
    uint16_t opcode;
    uint8_t para_len;
    uint8_t dummy;
} rf_ub_hci_vsc_dummy_t, *rf_ub_hci_vsc_dummy_ptr_t;

typedef struct __PACKED rf_ub_hci_relay_cmd_s {
    uint8_t hci_type;
    uint16_t opcode;
    uint8_t para_len;
    uint8_t para[1];
} rf_ub_hci_relay_cmd_t, *rf_ub_hci_relay_cmd_ptr_t;

typedef struct __PACKED rf_ub_hci_cmd_comp_evt_format_s {
    uint8_t hci_type;
    uint8_t event_code;
    uint8_t para_len;
    uint8_t no_hci_cmd_pkt;
    uint16_t cmd_opcode;
    uint8_t ret_para[1];
} rf_ub_hci_cmd_comp_evt_format_t;

typedef struct rf_ub_sleep_ctrl_s {
    bool sleep_rdy;
    bool fw_reload_request;
    RF_FW_LOAD_SELECT fw_selected;
    low_power_level_cfg_t lpm;
    uint8_t bt_wake_act_mode;
    uint8_t host_wake_act_mode;
    uint32_t idle_thd_host;
    uint32_t idle_thd_hc;
    uint32_t last_active_rtc;
    uint32_t io_retent_table[MAX_NUMBER_OF_PINS];
} rf_ub_sleep_ctrl_t;

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
QueueHandle_t xrf_ub_uart_to_down_qhandle;
QueueHandle_t xrf_ub_down_to_uart_qhandle;
TaskHandle_t xrf_ub_uart_to_down_taskHandle;
TaskHandle_t xrf_ub_down_to_uart_taskHandle;
#if (UASB_SLEEP_TASK_ENABLE)
TaskHandle_t xrf_ub_sleep_taskHandle;
#endif

#if (CHOOSE_UART == UART_0)
#define UART_OPERATION_PORT 0
#elif (CHOOSE_UART == UART_1)
#define UART_OPERATION_PORT 1
#else
#error "undefined UART port"
#endif
HOSAL_UART_DEV_DECL(uart_dev_bridge, UART_OPERATION_PORT, 28, 29,
                    UART_BAUDRATE_115200)

static struct ruci_hci_message_struct gruci_hci_message_tx_block;
static struct ble_hci_acl_data_sn_struct ghci_message_tx_data;
static uint16_t gble_comm_tx_sn;

uint8_t guasb_comm_cmd_state;
struct uasb_comm_cmd_state guasb_comm_cmd_state_q;

uint8_t gUasbUartBuff[UASB_MAX_UART_LEN + 10];     //no wrap around
uint8_t gUasbUartTxBuff[UASB_MAX_UART_LEN + 10];   //no wrap around
uint8_t gUasbUart2SpiBuff[UASB_MAX_UART_LEN + 10]; //no wrap around

volatile uint8_t gUasbUartTxStart;
volatile uint8_t gUasbSpiMutexTx;

uint8_t guart_byte_data;
uint16_t gUasbUartIdx;

uint16_t gUasbTargetLen;
uint16_t gUasbUart2SpicLen;

volatile UASB_SPI_CTRL_STATE gUasbSpiState;
uint8_t gUasbMcuState;

volatile uint8_t gUasbRxReportCnt;

bool gUasbPySanity = false;
bool gUasbInterface = false;

#if (UASB_SLEEP_ENABLE)
rf_ub_sleep_ctrl_t rf_ub_slp_ctrl = {.sleep_rdy = FALSE,
                                     .fw_reload_request = FALSE,
                                     .lpm = LOW_POWER_LEVEL_NORMAL,
                                     .idle_thd_host = 0,
                                     .idle_thd_hc = 0};

static const rf_ub_lpm_vsc_t sLpmVsc = {
    .hci_type = UASB_RUCI_HCI_CMD, .opcode = 0xFC27, .para_len = 5};
#endif

static const rf_ub_hci_vsc_tx_pwr_comp_cfg_t sTxPwrCompCfgVsc = {
    .hci_type = UASB_RUCI_HCI_CMD, .opcode = 0xFC86, .para_len = 1};

static const rf_ub_hci_vsc_dummy_t sVscDummy[] = {
    {.hci_type = UASB_RUCI_HCI_CMD, .opcode = 0xFC3A, .para_len = 0x01},

    {.hci_type = UASB_RUCI_HCI_CMD, .opcode = 0xFCBB, .para_len = 0x2A},
    {.hci_type = UASB_RUCI_HCI_CMD, .opcode = 0xFD8B, .para_len = 0x3C}};

static const rf_ub_hci_relay_cmd_t sReadLocalNameHciCmd = {
    .hci_type = UASB_RUCI_HCI_CMD, .opcode = 0x0C14, .para_len = 0x00};

/**************************************************************************************************
 *    EXTERN
 *************************************************************************************************/
extern void RfMcu_InterruptDisableAll(void);
extern void RfMcu_InterruptEnableAll(void);

void uasb_data_rx_parse(void);
bool uasb_rx_report_handle(void); //UASB_RxReportUartTx(void)
#if (UASB_SLEEP_ENABLE)
void uasb_gpiotest_isr(uint32_t pin, void* isr_param);
void uasb_sleep_resume(void);
void uasb_dtu_sleep_ctrl(void);
void uasb_utd_sleep_ctrl(void);
void usab_peek_sleep_mode(rf_ub_lpm_vsc_t* hdr);
bool usab_rtc_is_expired(uint32_t compare_rtc);
void usab_last_active_rtc_update(void);
bool usab_idle_thd_host_sleep_available(void);
bool usab_idle_thd_hc_sleep_available(void);
void uasb_deinit(bool fw_reload);
#endif
bool usab_relocate_tx_pwr_comp_cfg(rf_ub_hci_vsc_tx_pwr_comp_cfg_t* hdr);
bool usab_vsc_relay_handler(ruci_head_t* hdr);
void usab_vsc_evt_gen(uint16_t opcode, uint8_t ret_len, uint8_t* ret_para);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
void uasb_comm_add_state(uint8_t state) {

    /* Igonre NONE state CMD add */
    if (state == 0) {
        return;
    }

    enter_critical_section();

    /* Check current space is available to add command */
    if (guasb_comm_cmd_state_q.cmd_queue[guasb_comm_cmd_state_q.wp] == 0) {

        guasb_comm_cmd_state_q.cmd_queue[guasb_comm_cmd_state_q.wp] = state;

        /* update write point value and check if already wrap around */
        if ((++guasb_comm_cmd_state_q.wp) == UASB_COMM_Q_MAX_SIZE) {
            guasb_comm_cmd_state_q.wp = 0;
        }
    } else {
        BREAK();
    }

    leave_critical_section();
}

uint8_t uasb_comm_get_state(void) {

    uint8_t ret, i = 0;

    //To Prevent This queue could be updated by ISR
    enter_critical_section();

    while (i < UASB_COMM_Q_MAX_SIZE) {
        i++;
        ret = guasb_comm_cmd_state_q.cmd_queue[guasb_comm_cmd_state_q.rp];
        if (ret != 0) {
            guasb_comm_cmd_state_q.cmd_queue[guasb_comm_cmd_state_q.rp] = 0;
            i = UASB_COMM_Q_MAX_SIZE;
        }

        if (++guasb_comm_cmd_state_q.rp == UASB_COMM_Q_MAX_SIZE) {
            guasb_comm_cmd_state_q.rp = 0;
        }
    }

    //Resume Interrupt
    leave_critical_section();

    return ret;
}

void uasb_comm_q_init(void) {

    uint8_t i;

    guasb_comm_cmd_state_q.rp = 0;
    guasb_comm_cmd_state_q.wp = 0;

    for (i = 0; i < UASB_COMM_Q_MAX_SIZE; i++) {
        guasb_comm_cmd_state_q.cmd_queue[i] = 0;
    }
}

bool uasb_comm_queue_exists(void) {
    bool queueExists = FALSE;

    enter_critical_section();

    if (guasb_comm_cmd_state_q.wp != guasb_comm_cmd_state_q.rp) {
        queueExists = TRUE;
    }

    leave_critical_section();

    return queueExists;
}

void wdt_init(void) {
    wdt_config_mode_t wdt_mode;
    wdt_config_tick_t wdt_cfg_ticks;

    /*
     *Remark: We should set each field of wdt_mode.
     *Otherwise, some field will become undefined.
     *It could be BUG.
     */
    wdt_mode.int_enable = 0;
    wdt_mode.reset_enable = 1;
    wdt_mode.lock_enable = 1;
    wdt_mode.prescale = WDT_PRESCALE_32; /* SYS_CLK/32. 1M Hz*/

    wdt_cfg_ticks.wdt_ticks = ms_sec(50);
    wdt_cfg_ticks.int_ticks = ms_sec(0);
    wdt_cfg_ticks.wdt_min_ticks = ms_sec(0);

    wdt_start(wdt_mode, wdt_cfg_ticks, NULL); /*0.5 seconds*/
}

void uasb_error_reboot(void) {
#if (UASB_BREAK_ENABLE == ENABLE)
    BREAK();
#else
    enter_critical_section();
    wdt_init();
    BREAK();
#endif
}

uint32_t uasb_read_rtc(void) {
    uint32_t rtc = xTaskGetTickCount();

    return rtc;
}

void uasb_task_resume(void) {
    portENTER_CRITICAL();
    vTaskResume(xrf_ub_uart_to_down_taskHandle);
    vTaskResume(xrf_ub_down_to_uart_taskHandle);
    portEXIT_CRITICAL();
}

#if 0
void uart1_isr(uint32_t event, void *p_context)
{

    /*Notice:
        UART_EVENT_TX_DONE  is for asynchronous mode send
        UART_EVENT_RX_DONE  is for synchronous  mode receive

        if system wants to use p_context as parameter, it can cast
        the type of p_context to original type.  like

        uint32_t  phandle;

        phandle = (uint32_t *) p_context;

     */

    if (event & UART_EVENT_TX_DONE)
    {
        /*if you use multi-tasking, signal the waiting task here.*/
        gUasbUartTxStart = 0;
    }

    if (event & UART_EVENT_RX_DONE)
    {

        /*if you use multi-tasking, signal the waiting task here.*/
        gUasbUartBuff[gUasbUartIdx++] = guart_byte_data;

        uart_rx(CHOOSE_UART, &guart_byte_data, 1);

#if (UASB_UART_HW_FC_SUPPORTED)
        if (gUasbUartIdx > UASB_MAX_UART_LEN / 2)
        {
            /* Set RTS to wait for Local UART RX Buffer Available*/
            uart_set_modem_status(CHOOSE_UART, UASB_UART_RTS_SET);
        }
#endif

        //uasb_data_rx_parse();

    }

    if (event & (UART_EVENT_RX_OVERFLOW | UART_EVENT_RX_BREAK |
                 UART_EVENT_RX_FRAMING_ERROR | UART_EVENT_RX_PARITY_ERROR ))
    {

        //it's almost impossible for those error case.
        //do something ...
        if (event & UART_EVENT_RX_FRAMING_ERROR)
        {

        }
        else
        {
            //wait WDT
            //while(1);
        }
    }

#if (UASB_SLEEP_TASK_ENABLE)
    //vTaskResume(xrf_ub_sleep_taskHandle);
#endif
}
#else
int rf_ub_rx_callback(void* p_arg) {
    /*if you use multi-tasking, signal the waiting task here.*/

    hosal_uart_receive(&uart_dev_bridge, &guart_byte_data, 1);

    gUasbUartBuff[gUasbUartIdx++] = guart_byte_data;

#if (UASB_UART_HW_FC_SUPPORTED)
    if (gUasbUartIdx > UASB_MAX_UART_LEN / 2) {
        /* Set RTS to wait for Local UART RX Buffer Available*/
        uart_set_modem_status(CHOOSE_UART, UASB_UART_RTS_SET);
    }
#endif

    return 0;
}

int rf_ub_tx_callback(void* p_arg) {
    /*if you use multi-tasking, signal the waiting task here.*/
    gUasbUartTxStart = 0;
    return 0;
}
#endif

void uasb_uart_tx(uint16_t length) //UASB_Uart_Tx(uint16_t length)
{
    gUasbUartTxStart = 1;
#if (UASB_SLEEP_ENABLE)
    gpio_pin_clear(GPIO_UB_HOST_WAKE);
#endif
    hosal_uart_send(&uart_dev_bridge, gUasbUartTxBuff, length);
}

bool uasb_uart_log_tx(uint8_t* str, uint16_t length) {

    if (gUasbUartTxStart == 1) {
        return false;
    }

    memcpy(gUasbUartTxBuff, str, length);
    uasb_uart_tx(length);

    return true;
}

void uasb_uart_init(bool baud_rate_high) {
#if 0
    static uint32_t  handle;

    uart_config_t  uart1_drv_config;
#endif

    /* uart buffer init */
    memset(gUasbUartBuff, 0, UASB_MAX_UART_LEN + 10);
    memset(gUasbUart2SpiBuff, 0, UASB_MAX_UART_LEN + 10);

    gUasbUartIdx = 0;
    gUasbTargetLen = 0;
    gUasbSpiMutexTx = 0;
    gUasbUart2SpicLen = 0;

#if 0
    /*init uart1, 115200, 8bits 1 stopbit, none parity, no flow control.*/
    if (baud_rate_high == true)
    {
        uart1_drv_config.baudrate = UART_BAUDRATE_1000000;//UART_BAUDRATE_1000000;//UART_BAUDRATE_115200;//UART_BAUDRATE_19200;//
    }
    else
    {
        uart1_drv_config.baudrate = UART_BAUDRATE_115200;//UART_BAUDRATE_19200;//
    }
    uart1_drv_config.databits = UART_DATA_BITS_8;
#if (UASB_UART_HW_FC_SUPPORTED)
    uart1_drv_config.hwfc     = UART_HWFC_ENABLED;
#else
    uart1_drv_config.hwfc     = UART_HWFC_DISABLED;
#endif
    uart1_drv_config.parity   = UART_PARITY_NONE;

    /* Important: p_contex will be the second parameter in uart callback.
     * In this example, we do NOT use p_context, (So we just use handle for sample)
     * but you can use it for whaterever you want. (It can be NULL, too)
     */
    handle = 0;
    uart1_drv_config.p_context = (void *) &handle;

    uart1_drv_config.stopbit  = UART_STOPBIT_ONE;
    uart1_drv_config.interrupt_priority = IRQ_PRIORITY_NORMAL;

    /* init uart 1*/
    uart_init(CHOOSE_UART, &uart1_drv_config, uart1_isr);
#if (UASB_UART_HW_FC_SUPPORTED)
    /* Initialized to clear RTS */
    uart_set_modem_status(CHOOSE_UART, UASB_UART_RTS_CLR);
#endif

#else
    if (baud_rate_high) {
        uart_dev_bridge.config.baud_rate = UART_BAUDRATE_1000000;
    }

    hosal_uart_init(&uart_dev_bridge);

    hosal_uart_callback_set(&uart_dev_bridge, HOSAL_UART_RX_CALLBACK,
                            rf_ub_rx_callback, &uart_dev_bridge);
    hosal_uart_callback_set(&uart_dev_bridge, HOSAL_UART_TX_CALLBACK,
                            rf_ub_tx_callback, &uart_dev_bridge);

    /* Configure UART to interrupt mode */
    hosal_uart_ioctl(&uart_dev_bridge, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT);
#if (CHOOSE_UART == UART_0)
    __NVIC_SetPriority(Uart0_IRQn, IRQ_PRIORITY_NORMAL);
#elif (CHOOSE_UART == UART_1)
    __NVIC_SetPriority(Uart1_IRQn, IRQ_PRIORITY_NORMAL);
#endif

#endif

    /* set uart 1 DMA RX buf */
    //hosal_uart_receive(&uart_dev_bridge, &guart_byte_data, 1);
}

#if (UASB_RX_PKT_DUMP_ENABLE)
#define BMU_PAGE_SIZE_BIT_SHIFT        (0x006)
#define BMU_PAGE_SIZE_BIT_MASK         (0x03f)
#define BMU_PAGE_SIZE                  (0x040)
#define MEM_BASE_ADDR_SHARED_BUFFER    (0x4000)
#define MEM_BASE_ADDR_BMU_LINK_TABLE   (0x5F80)
#define MEM_PKT_DUMP_BUF_SIZE          (0x0200)
#define REG_BMU_CFG                    (0x0010)
#define REG_NULL_CFG                   (0x0014)
#define REG_RX_PKT_HEAD                (0x004C)
#define REG_RX_PKT_LEN                 (0x0054)
#define REG_RX_PKT_HEAD_VALID_LEN_MASK (0x7F)
#define REG_COMMON_WORD_LEN            (0x04)

uint8_t MAC_RX_PKT_BUF[MEM_PKT_DUMP_BUF_SIZE];

bool uasb_asserted = FALSE;

void uasb_get_mac_normal_rx_queue(uint32_t bufferLength, uint8_t* pOutput) {
    uint32_t regTemp;
    uint16_t tempLength;
    uint16_t bufferAddress;
    uint16_t linkTableAddress;
    uint16_t pageNum;
    uint16_t loopIdx;
    uint16_t pInput;

    if (bufferLength) {
        tempLength = bufferLength;

        pageNum = ((tempLength + BMU_PAGE_SIZE_BIT_MASK)
                   >> BMU_PAGE_SIZE_BIT_SHIFT);

        RfMcu_MemoryGet(REG_RX_PKT_HEAD, (uint8_t*)&regTemp,
                        REG_COMMON_WORD_LEN);

        bufferAddress = (uint16_t)(regTemp & REG_RX_PKT_HEAD_VALID_LEN_MASK);

        for (loopIdx = 0; loopIdx < pageNum; loopIdx++) {

            pInput = (uint16_t)(MEM_BASE_ADDR_SHARED_BUFFER
                                + (bufferAddress << BMU_PAGE_SIZE_BIT_SHIFT));

            if (bufferLength > BMU_PAGE_SIZE) {
                RfMcu_MemoryGet(pInput, pOutput, BMU_PAGE_SIZE);
            } else {
                RfMcu_MemoryGet(pInput, pOutput, (bufferLength + 3) & 0x04);
            }

            pOutput += (BMU_PAGE_SIZE);
            tempLength = (tempLength >= BMU_PAGE_SIZE)
                             ? tempLength - BMU_PAGE_SIZE
                             : 0;

            linkTableAddress = MEM_BASE_ADDR_BMU_LINK_TABLE + bufferAddress;

            RfMcu_MemoryGet(linkTableAddress, (uint8_t*)&regTemp,
                            REG_COMMON_WORD_LEN);

            bufferAddress = (uint16_t)(regTemp
                                       & REG_RX_PKT_HEAD_VALID_LEN_MASK);
        }

        printf("[UASB]: Mac Normal Rx Packet\r\n");

        pOutput = MAC_RX_PKT_BUF;

        for (loopIdx = 0; loopIdx < bufferLength / 4; loopIdx++) {
            printf("[UASB]: 0x%x 0x%x 0x%x 0x%x\r\n", pOutput[4 * loopIdx + 0],
                   pOutput[4 * loopIdx + 1], pOutput[4 * loopIdx + 2],
                   pOutput[4 * loopIdx + 3]);
        }
    }
}

void uasb_get_mac_local_rx_queue(uint32_t bufferLength, uint8_t* pOutput) {
    uint32_t regTemp;
    uint16_t bufferAddress;
    uint16_t loopIdx;
    uint16_t pInput;

    if (bufferLength) {

        RfMcu_MemoryGet(REG_NULL_CFG, (uint8_t*)&regTemp, REG_COMMON_WORD_LEN);

        bufferAddress = (uint16_t)((regTemp >> 8)
                                   & REG_RX_PKT_HEAD_VALID_LEN_MASK);

        pInput = (uint16_t)(MEM_BASE_ADDR_SHARED_BUFFER
                            + (bufferAddress << BMU_PAGE_SIZE_BIT_SHIFT));

        RfMcu_MemoryGet(pInput, pOutput, BMU_PAGE_SIZE);

        printf("[UASB]: Mac Local Rx Packet\r\n");

        for (loopIdx = 0; loopIdx < bufferLength / 4; loopIdx++) {
            printf("[UASB]: 0x%x 0x%x 0x%x 0x%x\r\n", pOutput[4 * loopIdx + 0],
                   pOutput[4 * loopIdx + 1], pOutput[4 * loopIdx + 2],
                   pOutput[4 * loopIdx + 3]);
        }
    }
}

void uasb_get_mac_rx_packet(void) {
    uint32_t regTemp;
    uint16_t bufferLength;
    uint8_t* pOutput = MAC_RX_PKT_BUF;
    bool isNullRxQ = FALSE;

    memset(MAC_RX_PKT_BUF, 0, sizeof(MAC_RX_PKT_BUF));

    RfMcu_MemoryGet(REG_BMU_CFG, (uint8_t*)&regTemp, REG_COMMON_WORD_LEN);

    isNullRxQ = (bool)(regTemp & BIT1);

    RfMcu_MemoryGet(REG_RX_PKT_LEN, (uint8_t*)&regTemp, REG_COMMON_WORD_LEN);

    if (bufferLength) {
        if (isNullRxQ) {
            bufferLength = (uint16_t)((regTemp >> 16) & 0x0FFF);
            printf("[UASB]: Mac Local Rx Packet Length:%d\r\n", bufferLength);
            uasb_get_mac_local_rx_queue(bufferLength, pOutput);
        } else {
            bufferLength = (uint16_t)(regTemp & 0x0FFF);
            printf("[UASB]: Mac Normal Rx Packet Length:%d\r\n", bufferLength);
            uasb_get_mac_normal_rx_queue(bufferLength, pOutput);
        }
    }
}

void uasb_assert_handler(void) {
    uint32_t value;

    printf("[UASB]: Assert -\r\n");

    uasb_get_mac_rx_packet();

    RfMcu_MemoryGet(0x4008, (uint8_t*)&value, 4);

    uasb_asserted = TRUE;
}
#endif

#if (RF_FW_SUPPORT_BANKING)
extern void RfMcu_ImageLoadBank(const uint8_t* bank_image, uint32_t bank_size);
extern uint8_t* ble_fw_conn_bank;
extern uint32_t ble_fw_conn_bank_size;
extern uint8_t* ble_fw_non_conn_bank;
extern uint32_t ble_fw_non_conn_bank_size;

typedef uint8_t CODE_BANK_TYPE;
#define CODE_BANK_UNKNOWN_IMAGE ((CODE_BANK_TYPE)0)
#define CODE_BANK_NON_CONN      ((CODE_BANK_TYPE)1)
#define CODE_BANK_LL_CONN       ((CODE_BANK_TYPE)2)

#define RF_CODE_BANK_CTRL_PTR (0x400C)

typedef struct code_banking_ctrl_s {
    bool volatile switchReq;
    CODE_BANK_TYPE currBank;
    uint8_t rsvd[2];
} CODE_BANKING_CTRL_STRUCT_t, *CODE_BANKING_CTRL_STRUCT_PTR_t;
#endif

/* Uart_Spi_Bridge ISR status handle function for RT569 SW Interrupt */
void uasb_isr(uint8_t intStatus) {
    //uint8_t readSfr;

#if 0
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03)
    {
        /* FOR LEVEL TRIGGER ONLY, the MCU will keep entering INT if status not cleared */
        return;
    }
#endif

    // CMD --------------------------------------------------------------------
    if (intStatus & UASB_INT_CMD_DONE) {
        /* HCI Event */
        uasb_comm_add_state(UASB_INT_CMD_DONE);
    }
    // Status -----------------------------------------------------------------
    if (intStatus & UASB_INT_TX_DONE) {
        uasb_comm_add_state(UASB_INT_TX_DONE);
    }

#if (RF_FW_SUPPORT_BANKING)
    // Testing write
    if (intStatus & UASB_INT_BANK_SWITCH) {
        CODE_BANKING_CTRL_STRUCT_t cbVal;

        RfMcu_MemoryGet(RF_CODE_BANK_CTRL_PTR, (uint8_t*)&cbVal, 4);

        if (cbVal.currBank == CODE_BANK_NON_CONN) {
            RfMcu_ImageLoadBank((const uint8_t*)ble_fw_non_conn_bank,
                                ble_fw_non_conn_bank_size);
            cbVal.switchReq = FALSE;
        } else if (cbVal.currBank == CODE_BANK_LL_CONN) {
            RfMcu_ImageLoadBank((const uint8_t*)ble_fw_conn_bank,
                                ble_fw_conn_bank_size);
            cbVal.switchReq = FALSE;
        }

        RfMcu_MemorySet(RF_CODE_BANK_CTRL_PTR, (const uint8_t*)&cbVal, 4);
    }
#endif

    // Status -----------------------------------------------------------------
    if (intStatus & UASB_INT_SYS_ASSERT) {
#if (UASB_RX_PKT_DUMP_ENABLE)
        uasb_assert_handler();
#else
        uint32_t value;
        RfMcu_MemoryGet(0x4008, (uint8_t*)&value, 4);
        while (1)
            ;
#endif

        //uasb_comm_add_state(UASB_INT_SYS_ASSERT);
        //BREAK();
    }

    // RX packet --------------------------------------------------------------
    if (intStatus & UASB_INT_RX_DONE) {
#if (UASB_RX_PKT_DUMP_ENABLE)
        if (!uasb_asserted)
#endif
        {
            uasb_comm_add_state(UASB_INT_RX_DONE);
        }
    }

#if 0 /* only under test 32K RCO mode */
    // RTC Wakeup --------------------------------------------------------------
    if (intStatus & UASB_INT_RTC_WAKE)
    {
        uasb_comm_add_state(UASB_INT_RTC_WAKE);
    }
#endif

    RfMcu_InterruptClear(intStatus); //Clean Interrupt

#if (UASB_SLEEP_TASK_ENABLE)
    {
        BaseType_t yieldRequired = pdFALSE;
        if (rf_ub_slp_ctrl.sleep_rdy) {
            uasb_sleep_resume();
        }
        yieldRequired = xTaskResumeFromISR(xrf_ub_sleep_taskHandle);
        portYIELD_FROM_ISR(yieldRequired);
    }
#endif
}

void uasb_data_rx_parse(void) {
    bool is_uart_fail = false;
    uint8_t u8RuciType;
    uint16_t u16DataLen = 0xFFFF;

    if (gUasbUartIdx == 0) {
        return;
    }

    u8RuciType = gUasbUartBuff[0];

    /*ready to check length */
    switch ((u8RuciType & 0xF0) >> 4) {
        case RUCI_TYPE_PCI: /* PCI */
            if (gUasbUartIdx >= UASB_PCI_LEN_OFFSET) {
                /* Get Length */
                if (u8RuciType != RUCI_PCI_DATA_HEADER) {
                    u16DataLen = gUasbUartBuff[UASB_RUCI_PCI_LEN_OFFST];
                    gUasbTargetLen = u16DataLen + UASB_RUCI_PCI_PARA_OFFST;
                } else if (gUasbUartIdx >= UASB_PCI_LEN_OFFSET + 1) {
                    u16DataLen = gUasbUartBuff[UASB_RUCI_PCI_LEN_OFFST]
                                 | (gUasbUartBuff[UASB_RUCI_PCI_LEN_OFFST + 1]
                                    << 8);
                    gUasbTargetLen = u16DataLen + UASB_RUCI_PCI_PARA_OFFST
                                     + 1; /* 2 byte len of DATA */
                }
            }
            break;
        case RUCI_TYPE_BLE: /* HCI */

            if (u8RuciType != UASB_RUCI_HCI_CMD
                && u8RuciType != UASB_RUCI_ACL_DATA) {
                is_uart_fail = true;
                break;
            }

            if (gUasbUartIdx >= UASB_HCI_LEN_SIZE) {
                /* Get Length */
                if (u8RuciType == UASB_RUCI_HCI_EVENT) /* Event */
                {
                    u16DataLen = gUasbUartBuff[UASB_RUCI_HCI_EVENT_LEN_OFST];
                    gUasbTargetLen = u16DataLen + UASB_RUCI_HCI_EVENT_LEN_OFST
                                     + 1;
                } else {
                    if (u8RuciType == UASB_RUCI_HCI_CMD
                        && gUasbUartIdx >= UASB_HCI_LEN_SIZE + 1) {
                        u16DataLen =
                            gUasbUartBuff[UASB_RUCI_HCI_CMD_DATA_LEN_OFST];
                        gUasbTargetLen = u16DataLen
                                         + UASB_RUCI_HCI_CMD_DATA_LEN_OFST + 1;
                    } else if (u8RuciType == UASB_RUCI_ACL_DATA
                               && gUasbUartIdx >= UASB_HCI_LEN_SIZE + 2) {
                        u16DataLen =
                            gUasbUartBuff[UASB_RUCI_HCI_CMD_DATA_LEN_OFST]
                            | (gUasbUartBuff[UASB_RUCI_HCI_CMD_DATA_LEN_OFST
                                             + 1]
                               << 8);
                        gUasbTargetLen = u16DataLen
                                         + UASB_RUCI_HCI_CMD_DATA_LEN_OFST + 2;
                    }
                }
            }
            break;
        case RUCI_TYPE_CMN: /* CMN */
            if (gUasbUartIdx >= UASB_CMN_LEN_OFFSET) {
                u16DataLen = gUasbUartBuff[UASB_RUCI_CMN_LEN_OFFST];
                gUasbTargetLen = u16DataLen + UASB_RUCI_CMN_PARA_OFFST;
            }
            break;
        case RUCI_TYPE_SF: /* SF */
            if (gUasbUartIdx >= UASB_SF_LEN_SIZE) {
                u16DataLen = gUasbUartBuff[UASB_RUCI_SF_LEN_OFST]
                             + (gUasbUartBuff[UASB_RUCI_SF_LEN_OFST + 1] << 8);
                gUasbTargetLen = u16DataLen + UASB_SF_LEN_SIZE;

                if (u8RuciType != RUCI_CODE_IO_WRITE
                    && u8RuciType != RUCI_CODE_MEM_WRITE) {
                    if (gUasbTargetLen > 2120) {
                        is_uart_fail = true;
                    }
                }
            }
            break;
        default:
            //uasb_error_reboot();//BREAK();//while(1);   /* no matched command */
            //break;
            /* UART data error handling */
            is_uart_fail = true;
            break;
    }

    if (is_uart_fail == true || gUasbTargetLen > UASB_MAX_UART_LEN) {
        enter_critical_section();
        gUasbUartIdx--;
        memcpy(gUasbUartBuff, gUasbUartBuff + 1, gUasbUartIdx);
        leave_critical_section();
        return;
    }

    enter_critical_section();

    if (gUasbUartIdx >= gUasbTargetLen && gUasbTargetLen > 0) {
        /* all necessary data already received */
        /* notify function in UASB_Loop */

        if (gUasbSpiMutexTx == 0) {
            memcpy(gUasbUart2SpiBuff, gUasbUartBuff, gUasbTargetLen);
            gUasbUart2SpicLen = gUasbTargetLen;
        } else {
            BREAK();
        }

        /* clear UART Buffer */
        memset(gUasbUartBuff, 0, gUasbTargetLen);

        /* copy remaining data in buffer */
        if (gUasbUartIdx - gUasbTargetLen) {
            memcpy(gUasbUartBuff, gUasbUartBuff + gUasbTargetLen,
                   (gUasbUartIdx - gUasbTargetLen));
        }

        gUasbUartIdx -= gUasbTargetLen;

        gUasbTargetLen = 0;
    }

#if (UASB_UART_HW_FC_SUPPORTED)
    /* SW handled UART RX buffer, Clear RTS */
    uart_set_modem_status(CHOOSE_UART, UASB_UART_RTS_CLR);
#endif

    leave_critical_section();

    /* error handling for no enough length parameter */

    /* LED flashing */
}

#if (BLE_SANITY_WITH_PY)
#define TX_UART_DATA_WORKAROUND (TRUE)
uint8_t gAclDataBuf[UASB_MAX_UART_LEN + 1];
uint8_t gAclSeqNo;

bool uasb_data_q_write569(uint8_t qId) // UASB_DataQWrite569(uint8_t qId)
{

    RF_MCU_TXQ_ERROR tx_q_error = RF_MCU_TXQ_ERR_INIT;

#if (TX_UART_DATA_WORKAROUND)
    uint32_t tx_data_idx;
    uint8_t tx_data_duplicate, tx_data_length;
#endif

    /* to check state before issue TX send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03) {
        return false;
    }

    gAclSeqNo++;

    gAclDataBuf[0] = 0x02;
    gAclDataBuf[1] = gAclSeqNo;

    memcpy(&gAclDataBuf[2], &gUasbUart2SpiBuff[1], gUasbUart2SpicLen - 1);

#if (TX_UART_DATA_WORKAROUND)
    tx_data_length = (gAclDataBuf[4]);
    tx_data_duplicate = tx_data_length;

    for (tx_data_idx = 6; tx_data_idx < 6 + tx_data_length; tx_data_idx++) {
        if (gAclDataBuf[tx_data_idx] != tx_data_duplicate) {
            gAclDataBuf[tx_data_idx] = tx_data_duplicate;
        }
    }
#endif

    tx_q_error = RfMcu_TxQueueSendById(qId, gAclDataBuf, gUasbUart2SpicLen + 1);

    if (tx_q_error != RF_MCU_TXQ_SET_SUCCESS) {
        BREAK();
    }

    return true;
}
#else
#define TX_UART_DATA_WORKAROUND (FALSE)
#endif

bool uasb_ble_write_command(uint8_t* tx_data_ptr, uint8_t tx_data_len) {
#if 0
    /* to check state before issue TX send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03)
    {
        return false;
    }
#else
    RfMcu_HostWakeUpMcu();
#endif

    if (RfMcu_CmdQueueSend(tx_data_ptr, (uint32_t)tx_data_len)
        != RF_MCU_TX_CMDQ_SET_SUCCESS) {

        /* command fail */
        uasb_error_reboot();
        //return false;
    }

    return true;
}

bool uasb_ble_write_tx_data(uint8_t* tx_data_ptr, uint16_t tx_data_len) {
    uint32_t reg_val;
    uint8_t txq_id;
    RF_MCU_TXQ_ERROR tx_q_error;

#if 0
    /* to check state before issue TX send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03)
    {
        return false;
    }
#else
    RfMcu_HostWakeUpMcu();
#endif

    /* Check empty TXQ */
    RfMcu_MemoryGet(0x0048, (uint8_t*)&reg_val, 4);

    if (!reg_val) {
        uasb_error_reboot(); //BREAK();
    }

    /* TX Q is Full */
    if ((reg_val & 0x7F) == 0x00) {
        return false;
    }

    for (txq_id = 0; txq_id < 7; txq_id++) {
        if (reg_val & (1 << txq_id)) {
            break;
        }
    }

    /* Send data to TXQ */
    tx_q_error = RF_MCU_TXQ_ERR_INIT;
    tx_q_error = RfMcu_TxQueueSendById(txq_id, tx_data_ptr, tx_data_len);

    if (tx_q_error != RF_MCU_TXQ_SET_SUCCESS) {
        //BREAK();
        uasb_error_reboot();
    }

    return true;
}

bool uasb_data_q_write(uint8_t qId, uint8_t* tx_data_ptr,
                       uint16_t tx_data_len) // UASB_DataQWrite(uint8_t qId)
{
    // Send TX packet

    RF_MCU_TXQ_ERROR tx_q_error = RF_MCU_TXQ_ERR_INIT;

    /* to check state before issue TX send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03) {
        return false;
    }

    tx_q_error = RfMcu_TxQueueSendById(qId, tx_data_ptr, (uint32_t)tx_data_len);

    if (tx_q_error != RF_MCU_TXQ_SET_SUCCESS) {
        //BREAK();
        uasb_error_reboot();
    }

    return true;
}

bool uasb_cmd_q_write(uint8_t* tx_data_ptr,
                      uint8_t tx_data_len) //UASB_CmdQWrite(void)
{

    RF_MCU_TX_CMDQ_ERROR txCmdqError = RF_MCU_TX_CMDQ_ERR_INIT;

    /* to check state before issue cmd send */
    RfMcu_HostWakeUpMcu();
    if (RfMcu_PowerStateCheck() != 0x03) {
        return false;
    }

    txCmdqError = RfMcu_CmdQueueSend(tx_data_ptr, (uint32_t)tx_data_len);
    if (txCmdqError != RF_MCU_TX_CMDQ_SET_SUCCESS) {
        //BREAK();
        uasb_error_reboot();
    }

    return true;
}

/* MCU state of Host event */
void uasb_mcu_state_uart_tx(void) //UASB_McuStateUartTx(void)
{
    gUasbUartTxBuff[0] = RUCI_SF_HOST_EVENT_HEADER; /* RUCI Header */
    gUasbUartTxBuff[1] = RUCI_CODE_MCU_STATE;       /* RUCI subheader */
    gUasbUartTxBuff[2] = RUCI_PARA_LEN_MCU_STATE;   /* Length */
    gUasbUartTxBuff[3] = gUasbMcuState;             /* MCU State */
    uasb_uart_tx(RUCI_LEN_MCU_STATE);
    gUasbMcuState = 0;
}

void uasb_ble_hdlr(void) //UASB_BleHdlr()
{

    p_ruci_head_t pRuciHead = (p_ruci_head_t)(gUasbUart2SpiBuff);

    uint16_t length;

    //allocate task q and deliver to donw to up task.
    struct ruci_hci_message_struct* ruci_hci_message_ptr;

    /* request memory */
    if ((ruci_hci_message_ptr = pvPortMalloc(
             sizeof(struct ruci_hci_message_struct)))
        == NULL) {
        BREAK();
    }

    switch (pRuciHead->u8) {
        case UASB_RUCI_HCI_CMD:
            if (usab_vsc_relay_handler(pRuciHead)) {
                vPortFree(ruci_hci_message_ptr);
                return;
            }
#if (UASB_SLEEP_ENABLE)
            usab_last_active_rtc_update();
#endif
            length = gUasbUart2SpicLen;
            break;
        case UASB_RUCI_ACL_DATA:
#if (UASB_SLEEP_ENABLE)
            usab_last_active_rtc_update();
#endif
            length = gUasbUart2SpicLen;
            break;
        default:
            //BREAK();
            uasb_error_reboot();
    }

    memcpy(ruci_hci_message_ptr->ruci_hci_message.general_data_array,
           gUasbUart2SpiBuff, length);

    // deliver to DTU task
    while (xQueueSend(xrf_ub_down_to_uart_qhandle, (void*)&ruci_hci_message_ptr,
                      (TickType_t)0)
           != pdPASS)
        ;
}

void uasb_pci_hdlr(void) //UASB_PciHdlr()
{
    uint16_t length;
    //allocate task q and deliver to donw to up task.
    struct ruci_hci_message_struct* ruci_hci_message_ptr;

    /* request memory */
    if ((ruci_hci_message_ptr = pvPortMalloc(
             sizeof(struct ruci_hci_message_struct)))
        == NULL) {
        BREAK();
    }

#if 0

    pRUCI_HEAD pRuciHead;
    pRuciHead = (pRUCI_HEAD)(gUasbUart2SpiBuff);

    switch (pRuciHead->u8)
    {
    case (RUCI_PCI_DATA_HEADER):
        while (uasb_data_q_write(0) != true); //while(UASB_DataQWrite(0)!=true);
        break;

    default:
        while (uasb_cmd_q_write() != true); //while(UASB_CmdQWrite()!=true);
        break;
    }
#endif

    length = gUasbUart2SpicLen;
    memcpy(ruci_hci_message_ptr->ruci_hci_message.general_data_array,
           gUasbUart2SpiBuff, length);

    // deliver to DTU task
    while (xQueueSend(xrf_ub_down_to_uart_qhandle, (void*)&ruci_hci_message_ptr,
                      (TickType_t)0)
           != pdPASS)
        ;
}

void uasb_cmn_hdlr(void) {
    uint16_t length;
    //allocate task q and deliver to donw to up task.
    struct ruci_hci_message_struct* ruci_hci_message_ptr;

    /* request memory */
    if ((ruci_hci_message_ptr = pvPortMalloc(
             sizeof(struct ruci_hci_message_struct)))
        == NULL) {
        BREAK();
    }

    length = gUasbUart2SpicLen;
    memcpy(ruci_hci_message_ptr->ruci_hci_message.general_data_array,
           gUasbUart2SpiBuff, length);

    // deliver to DTU task
    while (xQueueSend(xrf_ub_down_to_uart_qhandle, (void*)&ruci_hci_message_ptr,
                      (TickType_t)0)
           != pdPASS)
        ;
}

void uasb_sf_hdlr(void) {
    uint16_t cmdOutLen = 0;

    /* Command parsing */
    cmdOutLen = rfb_sf_hdlr(gUasbUart2SpiBuff, gUasbUartTxBuff,
                            UASB_MAX_UART_LEN); //TBD: rfb_sf.h

    /* Command response */
    if (cmdOutLen) {
        uasb_uart_tx(cmdOutLen);
    }
}

/* this check queue is used to confirm if any data from RF ready to transmit to UART */
void uasb_uart_to_down_check_queue(void) {
    uint16_t tx_length;
    uint8_t transport_id;
    struct ruci_hci_message_struct* ruci_hci_message_ptr;
    //BaseType_t xQueRecCheck;

    if (gUasbUartTxStart == 1) {
        return;
    }

    //xQueRecCheck = xQueueReceive(xrf_ub_uart_to_down_qhandle, (void *) &ruci_hci_message_ptr, (TickType_t) 0);

    if (xQueueReceive(xrf_ub_uart_to_down_qhandle, (void*)&ruci_hci_message_ptr,
                      (TickType_t)0)
        == pdPASS) {

        transport_id =
            ruci_hci_message_ptr->ruci_hci_message.general_data_array[0];

        switch (transport_id) {
            case UASB_RUCI_ACL_DATA:
                /* count total length */
                tx_length =
                    ruci_hci_message_ptr->ruci_hci_message.general_data_array[3]
                    | (ruci_hci_message_ptr->ruci_hci_message
                           .general_data_array[4]
                       << 8);
#if (CHECK_RX_PACKET_CORRECT)
                /* check RX data */
                for (uint16_t i = 0; i < tx_length; i++) {
                    if (ruci_hci_message_ptr->ruci_hci_message.hci_acl_data
                            .data[i]
                        != (uint8_t)tx_length) {
                        uasb_error_reboot(); //while(1);
                    }
                }
#endif
                tx_length = tx_length
                            + 1 /*transport*/ + 2 /*handle*/ + 2 /*length*/;
                break;
            case UASB_RUCI_HCI_EVENT:
                /* count total length */
                tx_length =
                    ruci_hci_message_ptr->ruci_hci_message.general_data_array[2]
                    + 1 /*transport*/ + 1 /*event code*/ + 1 /*length*/;
                break;
            case RUCI_PCI_EVENT_HEADER:
            case RUCI_CMN_EVENT_HEADER:
                tx_length =
                    ruci_hci_message_ptr->ruci_hci_message.ruci_cmd.length
                    + 3; /*RUCI Header + SubHeader + Length*/
                break;
            case RUCI_PCI_DATA_HEADER:
                tx_length =
                    ruci_hci_message_ptr->ruci_hci_message.ruci_data.length
                    + 4; /*RUCI Header + SubHeader + 2B Length*/
                if (tx_length > UASB_MAX_UART_LEN) {
                    vPortFree(ruci_hci_message_ptr);
                    return;
                }
                break;
            default: uasb_error_reboot(); //BREAK();
        }

        //enter_critical_section();

        //check transport type, check tx length, copy to uart tx buf
        memcpy(gUasbUartTxBuff,
               ruci_hci_message_ptr->ruci_hci_message.general_data_array,
               tx_length);

        vPortFree(ruci_hci_message_ptr);

        //trigger uart TX
        uasb_uart_tx(tx_length);

#if (UASB_SLEEP_ENABLE)
        usab_last_active_rtc_update();
#endif

        //leave_critical_section();
    }
}

/* to handle data from UART to RF via Bus */
static void uasb_uart_to_down_task(void* parameters_ptr) {
    p_ruci_head_t pRuciHead;

    for (;;) {
        uasb_data_rx_parse();

        /* check if data from UART */
        if (gUasbUart2SpicLen) {
            gUasbSpiMutexTx = 1;
            pRuciHead = (p_ruci_head_t)(gUasbUart2SpiBuff);

            /* trigger SPI to RT569 */
            switch (pRuciHead->bf.type) {
                case (RUCI_TYPE_BLE):
                    uasb_ble_hdlr(); //UASB_BleHdlr();
                    break;
                case (RUCI_TYPE_PCI):
                    uasb_pci_hdlr(); //UASB_PciHdlr();
                    break;
                case (RUCI_TYPE_CMN): uasb_cmn_hdlr(); break;
                case (RUCI_TYPE_SF):
                    uasb_sf_hdlr(); //UASB_SfHdlr();
                    break;
                default: uasb_error_reboot(); //BREAK();
            }

            /* wait SPI done */
            memset(gUasbUart2SpiBuff, 0, gUasbUart2SpicLen);
            gUasbUart2SpicLen = 0;
            gUasbSpiMutexTx = 0;
        }

        uasb_uart_to_down_check_queue();
#if (UASB_SLEEP_ENABLE)
        /* suspend */
        uasb_utd_sleep_ctrl();
#endif
        portYIELD();
    }
}

/* CMDR from BUS to UART */
void uasb_cmd_event_handle(void) //UASB_CmdEventUartTx(void)
{
    uint16_t volatile rxLen;
    RF_MCU_RX_CMDQ_ERROR rxCmdError;
    struct ruci_hci_message_struct* ruci_hci_message_ptr;

    /* request memory */
    if ((ruci_hci_message_ptr = pvPortMalloc(
             sizeof(struct ruci_hci_message_struct)))
        == NULL) {
        BREAK();
    }

    /* read cmd queue */
    rxCmdError = RF_MCU_RX_CMDQ_ERR_INIT;
    rxLen = RfMcu_EvtQueueRead(
        ruci_hci_message_ptr->ruci_hci_message.general_data_array, &rxCmdError);

    if (!rxLen && (rxCmdError != RF_MCU_RX_CMDQ_GET_SUCCESS)) {
        uasb_error_reboot(); //BREAK();
    }

    while (xQueueSend(xrf_ub_uart_to_down_qhandle, (void*)&ruci_hci_message_ptr,
                      (TickType_t)0)
           != pdPASS)
        ;
}

/* TXDONE from BUS to UART */
void uasb_tx_done_event_handle(void) {
#if (UASB_PCI_TXDONE_EVENT_ENABLE == ENABLE)
    struct ruci_hci_message_struct* ruci_hci_message_ptr;

    /* request memory */
    if ((ruci_hci_message_ptr = pvPortMalloc(
             sizeof(struct ruci_hci_message_struct)))
        == NULL) {
        BREAK();
    }

    /* generate TX done command by transport layer */
    ruci_hci_message_ptr->ruci_hci_message.general_data_array[0] = 0x16;
    ruci_hci_message_ptr->ruci_hci_message.general_data_array[1] = 0x01;
    ruci_hci_message_ptr->ruci_hci_message.general_data_array[2] = 0x03;
    ruci_hci_message_ptr->ruci_hci_message.general_data_array[3] = 0x17;
    ruci_hci_message_ptr->ruci_hci_message.general_data_array[4] = 0x01;
    ruci_hci_message_ptr->ruci_hci_message.general_data_array[5] =
        gUasbMcuState;

    while (xQueueSend(xrf_ub_uart_to_down_qhandle, (void*)&ruci_hci_message_ptr,
                      (TickType_t)0)
           != pdPASS)
        ;
#endif
}

/* RX queue from SPI to UART */
bool uasb_rx_report_handle(void) //UASB_RxReportUartTx(void)
{
    uint16_t volatile rxLen;
    RF_MCU_RXQ_ERROR rx_queue_error;
    struct ruci_hci_message_struct* ruci_hci_message_ptr;
    //BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* request memory */
    if ((ruci_hci_message_ptr = pvPortMalloc(
             sizeof(struct ruci_hci_message_struct)))
        == NULL) {
        return false;
    }

    /* get data from RX Q*/
    rx_queue_error = RF_MCU_RXQ_ERR_INIT;

    rxLen = RfMcu_RxQueueRead(
        ruci_hci_message_ptr->ruci_hci_message.general_data_array,
        &rx_queue_error);

    if (!rxLen && (rx_queue_error != RF_MCU_RXQ_GET_SUCCESS)) {
        //uasb_error_reboot();
        vPortFree(ruci_hci_message_ptr);
        return false;
    }

    //while(xQueueSendFromISR(xrf_ub_uart_to_down_qhandle, (void *) &ruci_hci_message_ptr, &xHigherPriorityTaskWoken) != pdTRUE);
    while (xQueueSend(xrf_ub_uart_to_down_qhandle, (void*)&ruci_hci_message_ptr,
                      (TickType_t)0)
           != pdPASS)
        ;

    return true;
}

void uasb_rtc_wakeup_handle(void) {
#if 0
#if 0
    uint32_t reg_val;

    if (((update_cnt++) % 10) == 0)
    {
        printf("\r\n58xreg:0x40200034:0x%08X", inp32(0x40200034));
        /* confirm PMU setting of 569 */
        RfMcu_MemoryGet(0x0418, (uint8_t *)&reg_val, 4);
        printf("\r\n569reg:0x418:0x%08X", reg_val);
    }
    else
    {
        printf("#");
    }
#else
    printf("#");
#endif
#endif
}

void uasb_down_to_uart_check_queue(void) {
    uint8_t transport_id, ruci_hci_cmd_length;
    uint16_t ruci_hci_acl_data_length;
    struct ruci_hci_message_struct* ruci_hci_message_ptr;

    //actually, it is safer to check if the data to RF is done or not.
    //if(check_rf_write_status()!= true)return;

    /* Peek if it's a HCI command, and return if command queue is full */
    if (xQueuePeek(xrf_ub_down_to_uart_qhandle, (void*)&ruci_hci_message_ptr,
                   (TickType_t)0)
        == pdPASS) {
        transport_id =
            ruci_hci_message_ptr->ruci_hci_message.general_data_array[0];

        switch (transport_id) {
            case UASB_RUCI_HCI_CMD:
                if (RfMcu_CmdQueueFullCheck()) {
                    return;
                }
                break;

            default: break;
        }
    } else {
        return;
    }

    if (xQueueReceive(xrf_ub_down_to_uart_qhandle, (void*)&ruci_hci_message_ptr,
                      (TickType_t)0)
        == pdPASS) {

        transport_id =
            ruci_hci_message_ptr->ruci_hci_message.general_data_array[0];

        memcpy((uint8_t*)&gruci_hci_message_tx_block,
               (uint8_t*)ruci_hci_message_ptr,
               sizeof(struct ruci_hci_message_struct));
        switch (transport_id) {
            case UASB_RUCI_HCI_CMD:
                /* count total length */
                ruci_hci_cmd_length =
                    ruci_hci_message_ptr->ruci_hci_message.hci_command.length
                    + 1 /*transport*/ + 2 /*opcode*/ + 1 /*length*/;

                while (uasb_ble_write_command(
                           (uint8_t*)&gruci_hci_message_tx_block,
                           ruci_hci_cmd_length)
                       != true)
                    ;
                break;
            case UASB_RUCI_ACL_DATA:

                /* add sequence number*/
                ghci_message_tx_data.transport_id = UASB_RUCI_ACL_DATA;
                ghci_message_tx_data.sequence = gble_comm_tx_sn;
                ghci_message_tx_data.handle =
                    ruci_hci_message_ptr->ruci_hci_message.hci_acl_data.handle;
                ghci_message_tx_data.pb_flag =
                    ruci_hci_message_ptr->ruci_hci_message.hci_acl_data.pb_flag;
                ghci_message_tx_data.bc_flag =
                    ruci_hci_message_ptr->ruci_hci_message.hci_acl_data.bc_flag;
                ghci_message_tx_data.length =
                    ruci_hci_message_ptr->ruci_hci_message.hci_acl_data.length;

                if (gUasbPySanity == true) {
                    memset(ghci_message_tx_data.data,
                           (uint8_t)(ghci_message_tx_data.length & 0xFF),
                           ghci_message_tx_data.length);
                } else {
                    memcpy(ghci_message_tx_data.data,
                           ruci_hci_message_ptr->ruci_hci_message.hci_acl_data
                               .data,
                           ghci_message_tx_data.length);
                }

                /* count total length */
                ruci_hci_acl_data_length =
                    ruci_hci_message_ptr->ruci_hci_message.hci_acl_data.length
                    + 1 /*transport*/ + 2 /* sequence */ + 2 /*handle*/
                    + 2 /*length*/;

                while (uasb_ble_write_tx_data((uint8_t*)&ghci_message_tx_data,
                                              ruci_hci_acl_data_length)
                       != true)
                    ;

                gble_comm_tx_sn++;

                break;
            case RUCI_PCI_COMMON_CMD_HEADER:
            case RUCI_PCI_FSK_CMD_HEADER:
            case RUCI_PCI_BLE_CMD_HEADER:
            case RUCI_PCI15P4_MAC_CMD_HEADER:
            case RUCI_PCI_OQPSK_CMD_HEADER:
            case RUCI_CMN_SYS_CMD_HEADER:
            case RUCI_CMN_HAL_CMD_HEADER:
            case RUCI_PCI_ZWAVE_CMD_HEADER:
            case RUCI_PCI_OOK_CMD_HEADER:

                /* count total length */
                ruci_hci_cmd_length =
                    ruci_hci_message_ptr->ruci_hci_message.ruci_cmd.length
                    + 3; /*RUCI Header + SubHeader + 1B Length */
                while (uasb_cmd_q_write((uint8_t*)&gruci_hci_message_tx_block,
                                        ruci_hci_cmd_length)
                       != true)
                    ;

                break;
            case RUCI_PCI_DATA_HEADER:
                ruci_hci_acl_data_length =
                    ruci_hci_message_ptr->ruci_hci_message.ruci_data.length
                    + 4; /*RUCI Header + SubHeader + 2B Length */
                while (uasb_data_q_write(0,
                                         (uint8_t*)&gruci_hci_message_tx_block,
                                         ruci_hci_acl_data_length)
                       != true)
                    ;

                break;
            default: BREAK();
        }

        vPortFree(ruci_hci_message_ptr);
    }
}

void usab_check_state(void) {
    uint8_t hci_state = uasb_comm_get_state();

    switch (hci_state) {
        case UASB_INT_NO_STS: break;
        case UASB_INT_CMD_DONE:
            gUasbMcuState = (uint8_t)RfMcu_McuStateRead();
            RfMcu_HostCmdSet(gUasbMcuState);
            uasb_cmd_event_handle();
            break;
        case UASB_INT_TX_DONE:
            gUasbMcuState = (uint8_t)RfMcu_McuStateRead();
            RfMcu_HostCmdSet(gUasbMcuState);
            uasb_tx_done_event_handle();
            break;
        case UASB_INT_SYS_ASSERT: break;
        case UASB_INT_CMD_FAIL: break;
        case UASB_INT_RX_DONE: uasb_rx_report_handle(); break;
        case UASB_INT_RTC_WAKE: uasb_rtc_wakeup_handle(); break;
        default: BREAK();
    }
}

bool uasb_allow_task_suspend(void) {
    bool allowSuspend = FALSE;

    //portENTER_CRITICAL();
    if ((uxQueueMessagesWaiting(xrf_ub_down_to_uart_qhandle) == 0)
        && (uxQueueMessagesWaiting(xrf_ub_uart_to_down_qhandle) == 0)
        && (gUasbUart2SpicLen == 0) && (gUasbUartIdx == 0)
        && (!uasb_comm_queue_exists())) {
        allowSuspend = TRUE;
    }
    //portEXIT_CRITICAL();

    return allowSuspend;
}

/* to handle data from RF to UART */
static void uasb_down_to_uart_task(void* parameters_ptr) {
    for (;;) {
#if (UASB_SLEEP_ENABLE)
        if (rf_ub_slp_ctrl.fw_reload_request) {
            uasb_deinit(TRUE);
            rf_ub_slp_ctrl.fw_reload_request = FALSE;
        }
#endif
        usab_check_state();
        uasb_down_to_uart_check_queue();

#if (UASB_SLEEP_ENABLE)
        /* suspend */
        uasb_dtu_sleep_ctrl();
#endif
        portYIELD();
    }
}

#if (UASB_SLEEP_ENABLE)
void uasb_gpio_push_all(void) {
    uint32_t volatile gpio_id = 0;
    for (gpio_id = 0; gpio_id < MAX_NUMBER_OF_PINS; gpio_id++) {
        rf_ub_slp_ctrl.io_retent_table[gpio_id] = pin_get_mode(gpio_id);
    }
}

void uasb_gpio_pop_all(void) {
    uint32_t volatile gpio_id = 0;
    for (gpio_id = 0; gpio_id < MAX_NUMBER_OF_PINS; gpio_id++) {
        pin_set_mode(gpio_id, rf_ub_slp_ctrl.io_retent_table[gpio_id]);
    }
}

void uasb_gpio_sleep_all(void) {
    uint32_t volatile gpio_id = 0;
    uint32_t volatile retry_times = 2;

    while (retry_times) {
        retry_times--;
        for (gpio_id = 0; gpio_id < MAX_NUMBER_OF_PINS; gpio_id++) {
            if ((gpio_id != GPIO_UB_WAKE_SRC) && (gpio_id != GPIO_UB_HOST_WAKE)
                && (gpio_id != GPIO_UART0_RX) && (gpio_id != GPIO_UART0_TX)
                && (gpio_id != GPIO_UART1_CTS)) {
                pin_set_mode(gpio_id, MODE_GPIO);
                gpio_cfg_input(gpio_id, GPIO_PIN_NOINT);
                pin_set_pullopt(gpio_id, PULL_UP_100K);
            }
        }
    }

    gpio_pin_set(GPIO_UB_HOST_WAKE);

#if (UASB_UART_HW_FC_SUPPORTED)
#if 0 /* No need ? Still Pull high by default ? */
    gpio_cfg_output(GPIO_UART1_RTS);
    gpio_pin_set(GPIO_UART1_RTS);
#endif
#endif
}

void usab_host_wake_gpio_init(void) {
    gpio_pin_clear(GPIO_UB_HOST_WAKE);
    gpio_cfg_output(GPIO_UB_HOST_WAKE);
    pin_set_pullopt(GPIO_UB_HOST_WAKE, PULL_NONE);
}

#if (UASB_SLEEP_TASK_ENABLE)
void uasb_sleep_resume(void) {
    enter_critical_section();

    /* test if real wake up by IO */
    lpm_set_low_power_level(LOW_POWER_LEVEL_NORMAL);
    lpm_low_power_mask(LOW_POWER_MASK_BIT_RESERVED31);

    if (rf_ub_slp_ctrl.sleep_rdy) {
        uasb_gpio_pop_all();
        usab_host_wake_gpio_init();
    }

    rf_ub_slp_ctrl.sleep_rdy = FALSE;

    leave_critical_section();
}

void uasb_gpiotest_isr(uint32_t pin, void* isr_param) {
    BaseType_t yieldRequired = pdFALSE;

    if (rf_ub_slp_ctrl.lpm == LOW_POWER_LEVEL_SLEEP2) {
        rf_ub_slp_ctrl.fw_reload_request = TRUE;
    }

    uasb_sleep_resume();

    rf_ub_slp_ctrl.last_active_rtc = xTaskGetTickCountFromISR();

    xTaskResumeFromISR(xrf_ub_sleep_taskHandle);
    portYIELD_FROM_ISR(yieldRequired);
}

void uasb_common_sleep_ctrl(void) {
    uint32_t ret_mem_reg;

    enter_critical_section();

    if (!rf_ub_slp_ctrl.sleep_rdy) {
        if ((rf_ub_slp_ctrl.lpm == LOW_POWER_LEVEL_SLEEP2)
            || (rf_ub_slp_ctrl.lpm == LOW_POWER_LEVEL_SLEEP3)) {
#if (RF_TX_POWER_COMP)
            Tx_Power_Compensation_Deinit();
            Sadc_Compensation_Deinit();
#endif

            if (rf_ub_slp_ctrl.lpm == LOW_POWER_LEVEL_SLEEP3) {
                enter_critical_section();
                sys_get_retention_reg(7, &ret_mem_reg);
                sys_set_retention_reg(7, ret_mem_reg | BIT2);
                leave_critical_section();
            }
        }
        rf_ub_slp_ctrl.sleep_rdy = TRUE;
        uasb_gpio_push_all();
        uasb_gpio_sleep_all();

        lpm_set_low_power_level(rf_ub_slp_ctrl.lpm);
        lpm_low_power_unmask(LOW_POWER_MASK_BIT_RESERVED31);
    }

    leave_critical_section();
}
#else
void uasb_gpiotest_isr(uint32_t pin, void* isr_param) {
    BaseType_t yieldRequired = pdFALSE;

    if (rf_ub_slp_ctrl.lpm == LOW_POWER_LEVEL_SLEEP2) {
        rf_ub_slp_ctrl.fw_reload_request = TRUE;
    }
    /* test if real wake up by IO */
    Lpm_Set_Low_Power_Level(LOW_POWER_LEVEL_NORMAL);
    Lpm_Low_Power_Mask(LOW_POWER_MASK_BIT_RESERVED31);

    yieldRequired |= xTaskResumeFromISR(xrf_ub_down_to_uart_taskHandle);
    yieldRequired |= xTaskResumeFromISR(xrf_ub_uart_to_down_taskHandle);
    portYIELD_FROM_ISR(yieldRequired);

    uasb_gpio_pop_all();
    usab_host_wake_gpio_init();
}

void uasb_common_sleep_ctrl(void) {
    uint32_t ret_mem_reg;

    if (Lpm_Get_Low_Power_Mask_Status() == LOW_POWER_NO_MASK) {
        if ((rf_ub_slp_ctrl.lpm == LOW_POWER_LEVEL_SLEEP2)
            || (rf_ub_slp_ctrl.lpm == LOW_POWER_LEVEL_SLEEP3)) {
            Tx_Power_Compensation_Deinit();
            Sadc_Compensation_Deinit();

            if (rf_ub_slp_ctrl.lpm == LOW_POWER_LEVEL_SLEEP3) {
                enter_critical_section();
                sys_get_retention_reg(7, &ret_mem_reg);
                sys_set_retention_reg(7, ret_mem_reg | BIT2);
                leave_critical_section();
            }
        }
        uasb_gpio_push_all();
        uasb_gpio_sleep_all();
    }
    Lpm_Set_Low_Power_Level(rf_ub_slp_ctrl.lpm);
    Lpm_Low_Power_Unmask(LOW_POWER_MASK_BIT_RESERVED31);
}
#endif

bool uasb_sleep_ctrl_check(void) {
    if (((!rf_ub_slp_ctrl.idle_thd_hc && (gpio_pin_get(GPIO_UB_WAKE_SRC) == 1))
         || (rf_ub_slp_ctrl.idle_thd_hc && usab_idle_thd_hc_sleep_available()))
        && uasb_allow_task_suspend()) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void uasb_dtu_sleep_ctrl(void) {
    if (uasb_allow_task_suspend()) {
#if (!UASB_SLEEP_TASK_ENABLE)
        uasb_common_sleep_ctrl();
#endif
        /* suspend until no more event */
        vTaskSuspend(xrf_ub_down_to_uart_taskHandle);
    }
}

void uasb_utd_sleep_ctrl(void) {
    if (uasb_allow_task_suspend()) {
#if (!UASB_SLEEP_TASK_ENABLE)
        uasb_common_sleep_ctrl();
#endif
        /* suspend until no more event */
        vTaskSuspend(xrf_ub_uart_to_down_taskHandle);
    }
}

void usab_sleep_gpio_init(void) {
    uint32_t ret_mem_reg;

    /*set GPIO2 for gpio input wakeup source*/
    pin_set_mode(GPIO_UB_WAKE_SRC, MODE_GPIO);

    /*In this example,  GPIO2 falling edge will generate wakeup interrupt*/
    pin_set_pullopt(GPIO_UB_WAKE_SRC, PULL_UP_100K);

    gpio_cfg_input(GPIO_UB_WAKE_SRC, GPIO_PIN_INT_EDGE_FALLING);

    gpio_register_isr(GPIO_UB_WAKE_SRC, uasb_gpiotest_isr, NULL);

    /*enable gpio2 pin for interrupt source*/
    gpio_int_enable(GPIO_UB_WAKE_SRC);

    /*enable debounce for GPIO2*/
    gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    gpio_debounce_enable(GPIO_UB_WAKE_SRC);

    usab_host_wake_gpio_init();

    sys_get_retention_reg(7, &ret_mem_reg);

    if (ret_mem_reg & BIT2) {
        rf_ub_slp_ctrl.lpm = LOW_POWER_LEVEL_SLEEP3;
    }

    if (GPIO_UB_WAKE_SRC == 2) {
        lpm_enable_low_power_wakeup(LOW_POWER_WAKEUP_GPIO2);
    } else {
        /* implement other wakeup source here */
        BREAK();
    }
}

void usab_peek_sleep_mode(rf_ub_lpm_vsc_t* hdr) {
    if ((hdr->hci_type == sLpmVsc.hci_type) && (hdr->opcode == sLpmVsc.opcode)
        && (hdr->para_len == sLpmVsc.para_len)) {
        switch (hdr->lpm) {
            case 0x01: rf_ub_slp_ctrl.lpm = LOW_POWER_LEVEL_SLEEP0; break;

            case 0x02: rf_ub_slp_ctrl.lpm = LOW_POWER_LEVEL_SLEEP2; break;

            case 0x03: rf_ub_slp_ctrl.lpm = LOW_POWER_LEVEL_SLEEP3; break;

            default: rf_ub_slp_ctrl.lpm = LOW_POWER_LEVEL_NORMAL; break;
        }
        rf_ub_slp_ctrl.idle_thd_host = (uint32_t)hdr->idle_thd_host * 25 / 2;
        rf_ub_slp_ctrl.idle_thd_hc = (uint32_t)hdr->idle_thd_hc * 25 / 2;
        rf_ub_slp_ctrl.bt_wake_act_mode = hdr->bt_wake_act_mode;
        rf_ub_slp_ctrl.host_wake_act_mode = hdr->host_wake_act_mode;
    }
}

bool usab_rtc_is_expired(uint32_t compare_rtc) {
    uint32_t current_rtc = uasb_read_rtc();

    if ((int32_t)(current_rtc - compare_rtc) > (int32_t)0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void usab_last_active_rtc_update(void) {
    uint32_t current_rtc = uasb_read_rtc();

    current_rtc = (current_rtc == 0) ? 1 : current_rtc;

    rf_ub_slp_ctrl.last_active_rtc = current_rtc;
}

bool usab_idle_thd_host_sleep_available(void) {
    if (rf_ub_slp_ctrl.idle_thd_host) {
        if (usab_rtc_is_expired(rf_ub_slp_ctrl.last_active_rtc
                                + rf_ub_slp_ctrl.idle_thd_host)) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return TRUE;
    }
}

bool usab_idle_thd_hc_sleep_available(void) {
    if (rf_ub_slp_ctrl.idle_thd_hc) {
        if (usab_rtc_is_expired(rf_ub_slp_ctrl.last_active_rtc
                                + rf_ub_slp_ctrl.idle_thd_hc)) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return TRUE;
    }
}

void uasb_deinit(bool fw_reload) {

#if (0)
    usab_sleep_gpio_init();
    uasb_uart_init(true);
#endif

    if (fw_reload) {
        rf_common_init_by_fw(rf_ub_slp_ctrl.fw_selected, uasb_isr);
    }
}
#endif

bool usab_relocate_tx_pwr_comp_cfg(rf_ub_hci_vsc_tx_pwr_comp_cfg_t* hdr) {
    ruci_para_tx_pwr_comp_cfg_t relay_head;
    uint8_t ret_para = 0;

    if ((hdr->hci_type == sTxPwrCompCfgVsc.hci_type)
        && (hdr->opcode == sTxPwrCompCfgVsc.opcode)
        && (hdr->para_len == sTxPwrCompCfgVsc.para_len)) {
        relay_head.ruci_header.u8 = RUCI_SF_HOST_SYS_CMD_HEADER;
        relay_head.sub_header = RUCI_CODE_TX_PWR_COMP_CFG;
        relay_head.length = hdr->para_len;
        relay_head.interval = hdr->interval;

        rfb_sf_hdlr((uint8_t*)&relay_head, gUasbUartTxBuff,
                    UASB_MAX_UART_LEN); //TBD: rfb_sf.h
        usab_vsc_evt_gen(hdr->opcode, 1, &ret_para);
        return TRUE;
    }

    return FALSE;
}

bool usab_relocate_dummy_vsc(rf_ub_hci_vsc_dummy_ptr_t hdr) {
    uint32_t vsc_idx;
    uint8_t ret_para = 0;

    for (vsc_idx = 0; vsc_idx < sizeof(sVscDummy) / sizeof(sVscDummy[0]);
         vsc_idx++) {
        if ((hdr->hci_type == sVscDummy[vsc_idx].hci_type)
            && (hdr->opcode == sVscDummy[vsc_idx].opcode)
            && (hdr->para_len == sVscDummy[vsc_idx].para_len)) {
            usab_vsc_evt_gen(hdr->opcode, 1, &ret_para);
            return TRUE;
        }
    }

    return FALSE;
}

bool usab_relocate_hci_read_local_name(rf_ub_hci_relay_cmd_ptr_t hdr) {
    uint8_t ret_para[1 + HCI_READ_LOCAL_NAME_SIZE];
    uint8_t device_name[] = {'R', 'T', '5', '6', '9', 's',
                             'e', 'r', 'i', 'e', 's'};

    if ((hdr->hci_type == sReadLocalNameHciCmd.hci_type)
        && (hdr->opcode == sReadLocalNameHciCmd.opcode)
        && (hdr->para_len == sReadLocalNameHciCmd.para_len)) {
        memset(&ret_para[0], 0, sizeof(ret_para));
        memcpy(&ret_para[1], device_name, sizeof(device_name));
        usab_vsc_evt_gen(hdr->opcode, 1 + HCI_READ_LOCAL_NAME_SIZE, ret_para);
        return TRUE;
    }

    return FALSE;
}

bool usab_vsc_relay_handler(ruci_head_t* hdr) {
    bool isHostOnlyCmd = FALSE;

    if (usab_relocate_tx_pwr_comp_cfg((rf_ub_hci_vsc_tx_pwr_comp_cfg_t*)hdr)) {
        isHostOnlyCmd = TRUE;
    } else if (usab_relocate_dummy_vsc((rf_ub_hci_vsc_dummy_ptr_t)hdr)) {
        isHostOnlyCmd = TRUE;
    } else if (usab_relocate_hci_read_local_name(
                   (rf_ub_hci_relay_cmd_ptr_t)hdr)) {
        isHostOnlyCmd = TRUE;
    }
#if (UASB_SLEEP_ENABLE)
    else {
        usab_peek_sleep_mode((rf_ub_lpm_vsc_t*)hdr);
    }
#endif

    return isHostOnlyCmd;
}

void usab_vsc_evt_gen(uint16_t opcode, uint8_t ret_len, uint8_t* ret_para) {
    uint16_t cmdOutLen;
    rf_ub_hci_cmd_comp_evt_format_t* evt =
        (rf_ub_hci_cmd_comp_evt_format_t*)gUasbUartTxBuff;

    evt->hci_type = UASB_RUCI_HCI_EVENT;
    evt->event_code = 0x0E;
    evt->para_len = 0x03 + ret_len;
    evt->no_hci_cmd_pkt = 0x01;
    evt->cmd_opcode = opcode;
    memcpy(evt->ret_para, ret_para, ret_len);

    cmdOutLen = ((uint16_t)offsetof(rf_ub_hci_cmd_comp_evt_format_t, para_len)
                 + 1 + evt->para_len);

    uasb_uart_tx(cmdOutLen);
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void uasb_ble_sanity_set(bool py_sanity_en) { gUasbPySanity = py_sanity_en; }

void uasb_init_cmd_interface(RF_FW_LOAD_SELECT fw_select,
                             bool init_interfrace_en) {
#if (UASB_SLEEP_ENABLE)
    rf_ub_slp_ctrl.fw_selected = fw_select;
#endif

    if (init_interfrace_en == true) {
        if (!rf_common_init_by_fw(fw_select, uasb_isr)) {
            BREAK();
        }
        gUasbInterface = true;
    } else {
        gUasbInterface = false;
    }
}

#if (UASB_SLEEP_TASK_ENABLE)
void uasb_sleep_task(void* parameters_ptr) {
    for (;;) {
        if (usab_idle_thd_host_sleep_available() && (gUasbUartTxStart == 0)) {
            gpio_pin_set(GPIO_UB_HOST_WAKE);
        }

        if (!uasb_allow_task_suspend()) {
            uasb_task_resume();
        } else if ((eSuspended == eTaskGetState(xrf_ub_uart_to_down_taskHandle))
                   && (eSuspended
                       != eTaskGetState(xrf_ub_down_to_uart_taskHandle))) {
            vTaskResume(xrf_ub_uart_to_down_taskHandle);
        } else if ((eSuspended == eTaskGetState(xrf_ub_down_to_uart_taskHandle))
                   && (eSuspended
                       != eTaskGetState(xrf_ub_uart_to_down_taskHandle))) {
            vTaskResume(xrf_ub_down_to_uart_taskHandle);
        } else if ((eSuspended == eTaskGetState(xrf_ub_down_to_uart_taskHandle))
                   && (eSuspended
                       == eTaskGetState(xrf_ub_uart_to_down_taskHandle))) {
            if ((rf_ub_slp_ctrl.lpm != LOW_POWER_LEVEL_SLEEP0)
                && (rf_ub_slp_ctrl.lpm != LOW_POWER_LEVEL_SLEEP2)
                && (rf_ub_slp_ctrl.lpm != LOW_POWER_LEVEL_SLEEP3)) {
                __WFI();
            } else {
                if (uasb_sleep_ctrl_check()) {
                    uasb_common_sleep_ctrl();
                    vTaskSuspend(xrf_ub_sleep_taskHandle);
                } else {
                    __WFI();
                    uasb_task_resume();
                }
            }
        }

        portYIELD();
    }
}
#endif

void uasb_init(bool baud_rate_high) {

    uasb_comm_q_init();

    RfMcu_DmaInit();

#if (UASB_SLEEP_ENABLE)
    usab_sleep_gpio_init();
#endif

    /* init RF with BLE Controller FW */
    // rf_common_init_by_fw(RF_FW_LOAD_SELECT_BLE_CONTROLLER,uasb_isr);

    /* init UART */
    uasb_uart_init(baud_rate_high);

    /* init control state */
    gUasbSpiState = UASB_SPI_CTRL_INIT;

    gble_comm_tx_sn = 0;

    gUasbRxReportCnt = 0;

    gUasbPySanity = false;

    xrf_ub_uart_to_down_qhandle = xQueueCreate(
        30, sizeof(struct ruci_hci_message_struct*));
    xTaskCreate(uasb_uart_to_down_task, "TASK_RF_UB_UP_TO_DOWN", 1024, NULL,
                E_TASK_PRIORITY_APP, &xrf_ub_uart_to_down_taskHandle);

    xrf_ub_down_to_uart_qhandle = xQueueCreate(
        30, sizeof(struct ruci_hci_message_struct*));
    xTaskCreate(uasb_down_to_uart_task, "TASK_RF_UB_DOWN_TO_UP", 1024, NULL,
                E_TASK_PRIORITY_APP, &xrf_ub_down_to_uart_taskHandle);

#if (UASB_SLEEP_TASK_ENABLE)
    xTaskCreate(uasb_sleep_task, "TASK_RF_UB_SLEEP", 128, NULL,
                E_TASK_PRIORITY_APP, &xrf_ub_sleep_taskHandle);
#endif
}
