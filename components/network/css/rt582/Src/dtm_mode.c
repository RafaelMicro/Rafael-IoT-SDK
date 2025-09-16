/**************************************************************************************************
*    INCLUDES
*************************************************************************************************/
#include "stdio.h"
#include "string.h"

#include "mcu.h"
//#include "project_config.h"

#include "hosal_gpio.h"
#include "hosal_uart.h"
#include "hosal_sysctrl.h"
//#include "uart_drv.h"
//#include "retarget.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include "rf_common_init.h"
#include "rf_mcu.h"

#include "ruci.h"
#include "uart_bridge.h"

#include "dtm_mode.h"
/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
#ifndef DTM_CMD_SET_ALONE_MODE
#define DTM_CMD_SET_ALONE_MODE (false)
#endif

#ifndef GPIO_TEST
#define GPIO_TEST (2)
#endif

#define DTM_MODE_DBG_TRACE      (0)
#define DTM_MODE_TYPE_DBG_TRACE (0)
#define DTM_MODE_UASB_DBG       (0)

#ifndef DTM_MODE_ENTER_SLEEP_SUPPORT
#define DTM_MODE_ENTER_SLEEP_SUPPORT (0)
#endif

#if (DTM_MODE_DBG_TRACE == 1)
#define DTM_PRINTF(...) printf(__VA_ARGS__)
#else
#define DTM_PRINTF(...)
#endif
#if (DTM_MODE_TYPE_DBG_TRACE == 1)
#define DTM_CMD_PRINTF(...) printf(__VA_ARGS__)
#else
#define DTM_CMD_PRINTF(...)
#endif

#define PWR_LEVEL0_KEY (0)

TaskHandle_t xrf_dtm_taskHandle;
TaskHandle_t xrf_rfet_taskHandle;

volatile ruci_para_initiate_ble_t gdtm_init_cmd_s = {0};
volatile ruci_para_initiate_zigbee_t gdtm_init_z_cmd_s = {0};
volatile ruci_para_initiate_fsk_t gdtm_init_w_cmd_s = {0};

volatile ruci_para_set_ble_modem_t gdtm_set_modem_cmd_s = {0};
volatile ruci_para_set_ble_mac_t gdtm_set_mac_cmd_s = {0};

volatile ruci_para_set_fsk_modem_t gdtm_set_w_modem_cmd_s = {0};
volatile ruci_para_set_fsk_mac_t gdtm_set_w_mac_cmd_s = {0};
volatile ruci_para_set_fsk_preamble_t gdtm_set_w_preamble_cmd_s = {0};
volatile ruci_para_set_fsk_sfd_t gdtm_set_w_sfd_cmd_s = {0};

volatile ruci_para_set_rf_frequency_t gdtm_set_freq_cmd_s = {0};
volatile ruci_para_set_rfe_tx_enable_t gdtm_set_tx_en_cmd_s = {0};
volatile ruci_para_set_rfe_tx_disable_t gdtm_set_tx_disable_cmd_s = {0};
volatile ruci_para_set_rfe_rx_enable_t gdtm_rx_en_cmd_s = {0};
volatile ruci_para_set_rfe_rx_disable_t gdtm_set_rx_disable_cmd_s = {0};
volatile ruci_para_get_crc_count_t gdtm_get_crc_cmd_s = {0};
volatile ruci_para_get_crc_report_event_t gdtm_get_crc_event_s = {0};
volatile ruci_para_set_clock_mode_t gdtm_set_clock_cmd_s = {0};

volatile ruci_para_cnf_event_t gdtm_cnf_event_s = {0};
volatile ruci_para_dtm_burst_tx_done_event_t gdtm_tx_done_event = {0};

volatile ruci_para_set_tx_power_t gdtm_set_tx_pwr_cmd_s = {0};

bool gdtm_htol_mode_en = false;

bool volatile gdtm_uart_tx_start;

bool gdtm_uart_rx_flag;

uint8_t gdtm_uart_rx_byte;

uint8_t gdtm_uart_tx_buf[2];

uint8_t gdtm_uart_tx_size;

/* SFD, BLE Access Address */
uint32_t gdtm_sfd_content = 0x71764129;

/* Current machine state. */
static STATE gdtm_state = STATE_UNINITIALIZED;

/* radio channel */
static DATA_RATE gdtm_radio_phy = RADIO_PHY_2M;

CODED_SCHEME gdtm_coded_scheme;

/* current command status - initially "ok", may be set if error detected, or to packet count. */
static uint16_t gdtm_report_status;

/* Command has been processed - number of not yet reported event bytes. */
static bool gdtm_event_get_flag;

/* Payload length of transmitted PDU, bits 2:7 of 16-bit dtm command. */
static uint32_t gdtm_packet_length;

/* Bits 0..1 of 16-bit transmit command, or 0xFFFFFFFF. */
static PACKET_DATA_TYPE gdtm_packet_type;

/* Packet type field in PHR for DTM test, it should be configured according to gdtm_packet_type */
static PHR_DATA_TYPE gdtm_phr_type;

/* Number of valid packets received. */
volatile uint32_t gdtm_aggregated_crc_count;

/* Number of invalid packets received. */
volatile uint32_t gdtm_aggregated_crc_fail_count;

/* Flag toggled when event received */
volatile uint8_t gdtm_wait_notification_flag = 1;

volatile uint8_t gdtm_wait_consecutive_notification_flag = 1;

/* 0..39 physical channel number (base 2402 MHz, Interval 2 MHz), bits 8:13 of 16-bit dtm command. */
dtm_freq_t gdtm_phy_channel;

/* Time between start of Tx packets (in uS). */ //init value, it depends on packet length
uint16_t gdtm_tx_interval = 0;

/* Global variables to handle 2-wrie UART command from equipment */
volatile uint8_t gdtm_cmd_idx;

/* Combined 16 bit DTM command received from equipment */
volatile uint16_t gdtm_cmd;

/* Time of first DTM command byte received */
uint32_t gdtm_first_byte_time;

/* Timer counter to check DTM byte to byte time difference */
uint32_t gdtm_current_timer_counter;

/* DTM with UART Bridge Flag */
bool gdtm_uart_bridge_en = false;
bool g_test_switch_to_lab_test_tool = false;

char end_db_enter_te[] = "\r\nenter test_end";
char end_db_done_te[] = "\r\ndone test_end";
char end_db_done_cg[] = "\r\ndone crc_get";
char end_db_done_trx_dis[] = "\r\ndone trx_disable";
char end_db_wait_cgr[] = "\r\nwait crc_resp";
RF_FW_LOAD_SELECT g_fw_select;
bool g_gui_dtm_task;

#if (DTM_UART == UART_0)
#define DTM_UART_TX_IO 16
#define DTM_UART_RX_IO 17
#elif (DTM_UART == UART_1)
#define DTM_UART_TX_IO 28
#define DTM_UART_RX_IO 29
#endif
HOSAL_UART_DEV_DECL(uart_dev_dtm, DTM_UART, DTM_UART_TX_IO, DTM_UART_RX_IO,
                    UART_BAUDRATE_115200)

void rfet_init_default_callback(void);
RFET_INIT_CB g_rfet_init_cb = rfet_init_default_callback;

/*Global parameter */
comp_config_t p_comp_config;

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
void rfet_init_cb_registration(RFET_INIT_CB init_cb) {
    if (init_cb) {
        g_rfet_init_cb = init_cb;
    }
}

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
void rfet_init_default_callback(void) { return; }

void dtm_uasb_printf(char* str, uint16_t len) {
#if (DTM_MODE_UASB_DBG == 1)
    while (uasb_uart_log_tx((uint8_t*)str, len) != true)
        ;
#endif
}

#if 0
void dtm_uart_isr(uint32_t event, void *p_context)
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
        gdtm_uart_tx_start = false;
    }

    if (event & UART_EVENT_RX_DONE)
    {

        if (gdtm_cmd_idx < DTM_CMD_SIZE)
        {

            if (gdtm_cmd_idx == 0)
            {
                gdtm_cmd |= (gdtm_uart_rx_byte << 8);
            }
            else if (gdtm_cmd_idx == 1)
            {
                gdtm_cmd |= gdtm_uart_rx_byte;
            }

            gdtm_cmd_idx++;
        }

        uart_rx(DTM_UART, &gdtm_uart_rx_byte, 1);

        hosal_gpio_pin_toggle(22);

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

#if (UASB_SLEEP_ENABLE == ENABLE)
    uasb_task_resume();
#endif
#if (DTM_MODE_ENTER_SLEEP_SUPPORT == 1)
    vTaskResume(xrf_dtm_taskHandle);
#endif

}
#else
int dtm_uart_rx_callback(void* p_arg) {
    hosal_uart_receive(&uart_dev_dtm, &gdtm_uart_rx_byte, 1);

    if (gdtm_cmd_idx < DTM_CMD_SIZE) {

        if (gdtm_cmd_idx == 0) {
            gdtm_cmd |= (gdtm_uart_rx_byte << 8);
        } else if (gdtm_cmd_idx == 1) {
            gdtm_cmd |= gdtm_uart_rx_byte;
        }

        gdtm_cmd_idx++;
    }

    //uart_rx(DTM_UART, &gdtm_uart_rx_byte, 1);

    hosal_gpio_pin_toggle(22);
    return 0;
}

int dtm_uart_tx_callback(void* p_arg) {
    gdtm_uart_tx_start = false;
    return 0;
}
#endif

void dtm_uart_tx(uint16_t length) //UASB_Uart_Tx(uint16_t length)
{
    gdtm_uart_tx_start = true;
    //uart_tx(DTM_UART, gdtm_uart_tx_buf, length);
    hosal_uart_send(&uart_dev_dtm, gdtm_uart_tx_buf, length);
}

void dtm_uart_init(void) {
#if 0
    static uint32_t  handle;
    uart_config_t  uart_drv_config;
#endif

    /* uart buffer init */
    memset(gdtm_uart_tx_buf, 0, 2);

    gdtm_uart_rx_flag = false;
    gdtm_uart_tx_start = false;
    gdtm_uart_tx_size = 0;

#if 0
    /*init uart1, 115200, 8bits 1 stopbit, none parity, no flow control.*/
    uart_drv_config.baudrate = UART_BAUDRATE_115200;//UART_BAUDRATE_19200;//
    uart_drv_config.databits = UART_DATA_BITS_8;
    uart_drv_config.hwfc     = UART_HWFC_DISABLED;
    uart_drv_config.parity   = UART_PARITY_NONE;

    /* Important: p_contex will be the second parameter in uart callback.
     * In this example, we do NOT use p_context, (So we just use handle for sample)
     * but you can use it for whaterever you want. (It can be NULL, too)
     */
    handle = 0;
    uart_drv_config.p_context = (void *) &handle;
    uart_drv_config.stopbit  = UART_STOPBIT_ONE;
    uart_drv_config.interrupt_priority = IRQ_PRIORITY_NORMAL;

    /* init uart 1*/
    uart_init(DTM_UART, &uart_drv_config, dtm_uart_isr);

    /* set uart 1 DMA RX buf */
    uart_rx(DTM_UART, &gdtm_uart_rx_byte, 1);
#else
    hosal_uart_init(&uart_dev_dtm);

    hosal_uart_callback_set(&uart_dev_dtm, HOSAL_UART_RX_CALLBACK,
                            dtm_uart_rx_callback, &uart_dev_dtm);
    hosal_uart_callback_set(&uart_dev_dtm, HOSAL_UART_TX_CALLBACK,
                            dtm_uart_tx_callback, &uart_dev_dtm);

    hosal_uart_ioctl(&uart_dev_dtm, HOSAL_UART_MODE_SET,
                     (void*)HOSAL_UART_MODE_INT);
#endif
}

void dtm_uart_deinit(void) { hosal_uart_finalize(&uart_dev_dtm); }

/* Timer start API provided by platform */
void dtm_cmd_timer_start(void) {

#if 0
    /* start timer1 */
    TIMER_Start(TIMER1); //3846Hz <-> 260us per interrupt which is time interval of uart cmd
#endif
}

/* Timer counter get API based on platform */
uint32_t dtm_timer_count_update(void) {
#if 0
    if (Reprot_TMR1_Counter() == 1)
    {
        Clean_TMR1_Counter();
        return ++gdtm_current_timer_counter;              // only return at here (every 260us) /
    }
#endif
    return gdtm_current_timer_counter;
}

/* UART send API provided by platform */
void dtm_2wire_uart_send(uint16_t dtm_event) {
    gdtm_uart_tx_buf[0] = (uint8_t)((dtm_event >> 8) & 0xFF);
    gdtm_uart_tx_buf[1] = (uint8_t)(dtm_event & 0xFF);
    dtm_uart_tx(DTM_EVENT_SIZE);
}

/* RUCI command package API */
bool dtm_set_ruci_cmd(uint8_t* cmd, uint32_t cmd_length, uint8_t* result) {
    RF_MCU_TX_CMDQ_ERROR txCmdqError;
    RF_MCU_RX_CMDQ_ERROR rx_cmd_error;

    rx_cmd_error = RF_MCU_RX_CMDQ_GET_SUCCESS;
    txCmdqError = RF_MCU_TX_CMDQ_ERR_INIT;

    gdtm_wait_notification_flag = 1;

    RfMcu_HostCmdSet(RF_MCU_STATE_TX_FAIL);
    txCmdqError = RfMcu_CmdQueueSend(cmd, cmd_length);

    if (txCmdqError != RF_MCU_TX_CMDQ_SET_SUCCESS) {
        return false;
    }

    while (gdtm_wait_notification_flag == 1)
        ;

    RfMcu_EvtQueueRead(result, &rx_cmd_error);

    if (rx_cmd_error != RF_MCU_RX_CMDQ_GET_SUCCESS) {
        return false;
    }

    return true;
}

/* RUCI event validation check */
bool dtm_ruci_cmd_event_check(uint8_t cmd_hdr, uint8_t cmd_sub_hdr,
                              volatile ruci_para_cnf_event_t* pdtm_cnf_event) {
    /* check event type & sub header */
    if (pdtm_cnf_event->ruci_header.u8 != RUCI_PCI_EVENT_HEADER
        || pdtm_cnf_event->sub_header != RUCI_CODE_CNF_EVENT
        || pdtm_cnf_event->length != sRUCI_LENGTH(RUCI_LEN_CNF_EVENT)) {
        DTM_PRINTF("\r\nEvent type err");
        return false;
    }

    /* check if command header match */
    if (pdtm_cnf_event->pci_cmd_header != cmd_hdr
        || pdtm_cnf_event->pci_cmd_subheader != cmd_sub_hdr) {
        DTM_PRINTF("\r\nEvent subh err");
        return false;
    }

    /* check status */
    return (pdtm_cnf_event->status == 0) ? true : false;
}

void dtm_set_check_event_flag(void) { gdtm_wait_notification_flag = 1; }

bool dtm_check_crc_event(uint32_t* crc_count_get,
                         uint32_t* crc_fail_count_get) {

    RF_MCU_RX_CMDQ_ERROR rx_cmd_error = RF_MCU_RX_CMDQ_GET_SUCCESS;

    if (gdtm_wait_notification_flag == 1) {
        /* no event notification */
        return false;
    } else {
        /* get event notification */
        /* read event and check if valid */
        RfMcu_EvtQueueRead((uint8_t*)&gdtm_get_crc_event_s, &rx_cmd_error);

        if (rx_cmd_error != RF_MCU_RX_CMDQ_GET_SUCCESS) {
            return false;
        } else {
            if ((gdtm_get_crc_event_s.ruci_header.u8 == RUCI_PCI_EVENT_HEADER)
                && (gdtm_get_crc_event_s.sub_header
                    == RUCI_CODE_GET_CRC_REPORT_EVENT)
                && (gdtm_get_crc_event_s.length
                    == sRUCI_LENGTH(RUCI_LEN_GET_CRC_REPORT_EVENT))) {
                *crc_count_get = gdtm_get_crc_event_s.crc_success_count;
                *crc_fail_count_get = gdtm_get_crc_event_s.crc_fail_count;
            } else {
                return false;
            }
        }
    }

    return true;
}

/* The TX burst done event should be handled when TX count is limited */
bool dtm_check_tx_burst_done_event(void) {

    RF_MCU_RX_CMDQ_ERROR rx_cmd_error = RF_MCU_RX_CMDQ_GET_SUCCESS;

    if (gdtm_wait_notification_flag == 1) {
        /* no event notification */
        return false;
    } else {
        /* get event notification */
        /* read event and check if valid */
        RfMcu_EvtQueueRead((uint8_t*)&gdtm_tx_done_event, &rx_cmd_error);

        if (rx_cmd_error != RF_MCU_RX_CMDQ_GET_SUCCESS) {
            return false;
        } else {
            if ((gdtm_tx_done_event.ruci_header.u8 == RUCI_PCI_EVENT_HEADER)
                && (gdtm_tx_done_event.sub_header
                    == RUCI_CODE_DTM_BURST_TX_DONE_EVENT)
                && (gdtm_tx_done_event.length
                    == sRUCI_LENGTH(RUCI_LEN_DTM_BURST_TX_DONE_EVENT))) {
                return true;
            } else {
                return false;
            }
        }
    }
}

/* RUCI Command to get CRC number result from RFIC */
bool dtm_get_crc_count(volatile uint32_t* crc_count_get,
                       volatile uint32_t* crc_fail_count_get) {

    RF_MCU_RX_CMDQ_ERROR rx_cmd_error;
    rx_cmd_error = RF_MCU_RX_CMDQ_GET_SUCCESS;

    /* deliver get CRC command to RT569 */
    SET_RUCI_PARA_GET_CRC_COUNT(&gdtm_get_crc_cmd_s);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_get_crc_cmd_s, RUCI_GET_CRC_COUNT);

    /* set wait CRC response event */
    gdtm_wait_consecutive_notification_flag = 1;

    dtm_set_ruci_cmd((uint8_t*)&gdtm_get_crc_cmd_s, RUCI_LEN_GET_CRC_COUNT,
                     (uint8_t*)&gdtm_cnf_event_s);

    /* to wait CRC response */
    //gdtm_wait_notification_flag = 1;

    if (dtm_ruci_cmd_event_check(RUCI_PCI_COMMON_CMD_HEADER,
                                 RUCI_CODE_GET_CRC_COUNT, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nFails to send a CRC cmd");
        return false;
    } else {
        DTM_PRINTF("\r\nSend a CRC cmd");
    }

    //while(uasb_uart_log_tx((uint8_t*)end_db_wait_cgr, sizeof(end_db_wait_cgr)) != true);
    dtm_uasb_printf(end_db_wait_cgr, sizeof(end_db_wait_cgr));

    //while(gdtm_wait_notification_flag==1);
    while (gdtm_wait_consecutive_notification_flag == 1)
        ;

    RfMcu_EvtQueueRead((uint8_t*)&gdtm_get_crc_event_s, &rx_cmd_error);

    if (rx_cmd_error != RF_MCU_RX_CMDQ_GET_SUCCESS) {
        DTM_PRINTF("\r\nrx_confirm_error!=RF_MCU_RX_CMDQ_GET_SUCCESS");
        return false;
    }

    if ((gdtm_get_crc_event_s.ruci_header.u8 == RUCI_PCI_EVENT_HEADER)
        && (gdtm_get_crc_event_s.sub_header == RUCI_CODE_GET_CRC_REPORT_EVENT)
        && (gdtm_get_crc_event_s.length
            == sRUCI_LENGTH(RUCI_LEN_GET_CRC_REPORT_EVENT))) {
        *crc_count_get = gdtm_get_crc_event_s.crc_success_count;
        *crc_fail_count_get = gdtm_get_crc_event_s.crc_fail_count;
        DTM_PRINTF("\r\nCRC = 0x%x", *crc_count_get);
    } else {
        DTM_PRINTF("\r\nCRC event error");
        return false;
    }

    return true;
}

/* To calculate packet interval for DTM TX */
static uint32_t dtm_packet_interval_calculate(uint32_t test_payload_length,
                                              DATA_RATE mode) {
    uint32_t test_packet_length = 0; // [us] NOTE: bits are us at 1Mbit
    uint32_t packet_interval = 0;    // us
    uint32_t overhead_bits = 0;      // bits
    uint32_t i = 0;
    uint32_t timeout = 0;

    /* packet overhead
     * see BLE [Vol 6, Part F] page 213
     * 4.1 LE TEST PACKET FORMAT */

    if (mode == RADIO_PHY_2M) {
        // 16 preamble
        // 32 sync word
        //  8 PDU header, actually packetHeaderS0len * 8
        //  8 PDU length, actually packetHeaderLFlen
        // 24 CRC
        overhead_bits = 88; // 11 bytes
    } else if (mode == RADIO_PHY_1M) {
        //  8 preamble
        // 32 sync word
        //  8 PDU header, actually packetHeaderS0len * 8
        //  8 PDU length, actually packetHeaderLFlen
        // 24 CRC
        overhead_bits = 80; // 10 bytes
    }

    /* add PDU payload test_payload length */
    test_packet_length = (test_payload_length * 8); // in bits

    // add overhead calculated above
    test_packet_length += overhead_bits;
    // we remember this bits are us in 1Mbit

    if (mode == RADIO_PHY_2M) {
        test_packet_length /= 2; // double speed
    }

    /*
     * packet_interval = ceil((test_packet_length+249)/625)*625
     * NOTE: To avoid floating point an equivalent calculation is used.
     */
    //uint32_t i       = 0;
    //uint32_t timeout = 0;
    do {
        i++;
        timeout = i * 625;
    } while (test_packet_length + 249 > timeout);
    packet_interval = i * 625 - test_packet_length;

    return packet_interval;
}

/* RUCI command to set RFIC BLE SFD (Access Addres) */
bool dtm_set_sfd_cmd(uint32_t sfd) {
    SET_RUCI_PARA_SET_BLE_MAC(&gdtm_set_mac_cmd_s, sfd, 0x00, 0x00, 0x555555);
    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_mac_cmd_s, RUCI_SET_BLE_MAC);
    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_mac_cmd_s, RUCI_LEN_SET_BLE_MAC,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_BLE_CMD_HEADER, RUCI_CODE_SET_BLE_MAC,
                                 &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nFails to set BLE DTM SFD");
        return false;
    } else {
        DTM_PRINTF("\r\nTo set BLE DTM SFD");
    }

    return true;
}

/* RUCI command to set RFIC PHY mode */
bool dtm_set_phy_cmd(uint32_t phy) {
    DATA_RATE set_phy;

    switch (phy) {

        case LE_PHY_1M:
            gdtm_radio_phy = RADIO_PHY_1M;
            set_phy = RADIO_PHY_1M;
            break;
        case LE_PHY_2M:
            gdtm_radio_phy = RADIO_PHY_2M;
            set_phy = RADIO_PHY_2M;
            break;
        case LE_PHY_LE_CODED_S8:
            gdtm_radio_phy = RADIO_PHY_CODED;
            set_phy = RADIO_PHY_CODED;
            gdtm_coded_scheme = CODED_PHY_SCHEME_S8;
            break;
        case LE_PHY_LE_CODED_S2:
            gdtm_radio_phy = RADIO_PHY_CODED;
            set_phy = RADIO_PHY_CODED;
            gdtm_coded_scheme = CODED_PHY_SCHEME_S2;
            break;
        default: gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR; return false;
    }

    /* Package RUCI SET BLE MODEM command */
    SET_RUCI_PARA_SET_BLE_MODEM(&gdtm_set_modem_cmd_s, set_phy,
                                gdtm_coded_scheme);
    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_modem_cmd_s, RUCI_SET_BLE_MODEM);
    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_modem_cmd_s, RUCI_LEN_SET_BLE_MODEM,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_BLE_CMD_HEADER,
                                 RUCI_CODE_SET_BLE_MODEM, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nFails to set BLE DTM modem %d", gdtm_radio_phy);
        return false;
    } else {
        DTM_PRINTF("\r\nBLE DTM modem set phy:%d done, code scheme:%d",
                   gdtm_radio_phy, gdtm_coded_scheme);
    }

    return true;
}

/* RUCI command to set RFIC RF Frequency */
bool dtm_set_rf_freq_cmd(uint32_t freq) {
    /* Package RUCI SET RF Frequency command */
    //gdtm_set_freq_cmd_s.RfFrequency = 2402+2*freq;
    SET_RUCI_PARA_SET_RF_FREQUENCY(&gdtm_set_freq_cmd_s, freq);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_freq_cmd_s, RUCI_SET_RF_FREQUENCY);
    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_freq_cmd_s, RUCI_LEN_SET_RF_FREQUENCY,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_COMMON_CMD_HEADER,
                                 RUCI_CODE_SET_RF_FREQUENCY, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nFails to set DTM RF frequency");
        return false;
    } else {
        DTM_PRINTF("\r\nTo set up DTM RF frequency:%d", freq);
    }

    return true;
}

/* RUCI command to disable RFIC RX */
bool dtm_set_rx_disable_cmd(void) {
    /* Package RUCI RFE RX Disable command */
    SET_RUCI_PARA_SET_RFE_RX_DISABLE(&gdtm_set_rx_disable_cmd_s);

    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_rx_disable_cmd_s,
                     RUCI_NUM_SET_RFE_RX_DISABLE, (uint8_t*)&gdtm_cnf_event_s);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_rx_disable_cmd_s,
                        RUCI_SET_RFE_RX_DISABLE);
    if (dtm_ruci_cmd_event_check(RUCI_PCI_COMMON_CMD_HEADER,
                                 RUCI_CODE_SET_RFE_RX_DISABLE,
                                 &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nBLE DTM Rx disable fails");
        return false;
    }

    return true;
}

/* RUCI command to enable RFIC RX */
bool dtm_set_rx_enable_cmd(uint8_t report_cycle) {
    /* Package RUCI SET RFE RX Enable command */
    SET_RUCI_PARA_SET_RFE_RX_ENABLE(
        &gdtm_rx_en_cmd_s,
        report_cycle, // PeriodicReportCycle
        0xFF,         // PktType              Disable content check for DTM
        1,            // QueueType            Normal RX queue
        // 1,              // RxType               Continuous RX
        0,      // RxType               Packet RX
        0xFFFF, // RxTimeout            Don't care for continuous RX
        0,      // AckEnable            Disabling ACK for DTM
        0,      // AckInterval          Don't care for disabling ACK
        0,      // SecLevel             Disabling security
        0,      // SecADataLen          Don't care for disabling security
        0       // SecADataLen          Don't care for disabling security
    );

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_rx_en_cmd_s, RUCI_SET_RFE_RX_ENABLE);
    dtm_set_ruci_cmd((uint8_t*)&gdtm_rx_en_cmd_s, RUCI_LEN_SET_RFE_RX_ENABLE,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_COMMON_CMD_HEADER,
                                 RUCI_CODE_SET_RFE_RX_ENABLE, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nDTM Rx fails");
        return false;
    } else {
        DTM_PRINTF("\r\nDTM Rx starts");
    }
    return true;
}

/* RUCI command to disable RFIC TX */
bool dtm_set_tx_disable_cmd(void) {
    /* Package RUCI RFE TX Disable command */
    SET_RUCI_PARA_SET_RFE_TX_DISABLE(&gdtm_set_tx_disable_cmd_s);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_tx_disable_cmd_s,
                        RUCI_SET_RFE_TX_DISABLE);
    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_tx_disable_cmd_s,
                     RUCI_NUM_SET_RFE_TX_DISABLE, (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_COMMON_CMD_HEADER,
                                 RUCI_CODE_SET_RFE_TX_DISABLE,
                                 &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nBLE DTM Tx disable fails");
        return false;
    }

    return true;
}

/* RUCI command to enable RFIC TX */
bool dtm_set_tx_enable_cmd(uint8_t data_type, uint8_t phr_type,
                           uint16_t tx_interval, uint16_t packet_len,
                           uint16_t tx_count) {
    /* Package RUCI SET RFE TX command */
    SET_RUCI_PARA_SET_RFE_TX_ENABLE(
        &gdtm_set_tx_en_cmd_s,
        tx_interval, // PktInterval      TX packet interval
        packet_len,  // PktLength        TX packet length
        0,           // PktStepSize      Fixing step size
        tx_count,    // PktCount         TX packet count
        data_type,   // PktType          Packet type
        phr_type,    // pktPhrType       Packet type field in PHR
        0,           // AckEnable        Disabling ACK for DTM
        0,           // AckTimeout       Don't care for disabling ACK
        0,           // SecLevel         Disabling security
        0,           // SecADataLen      Don't care for disabling security
        0            // SecMDataLen      Don't care for disabling security
    );

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_tx_en_cmd_s,
                        RUCI_SET_RFE_TX_ENABLE);
    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_tx_en_cmd_s,
                     RUCI_LEN_SET_RFE_TX_ENABLE, (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_COMMON_CMD_HEADER,
                                 RUCI_CODE_SET_RFE_TX_ENABLE, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nDTM Tx fails");
        return false;
    } else {
        DTM_PRINTF("\r\nDTM Tx starts");
    }
    return true;
}

/* RUCI command to init RFIC BLE */
bool dtm_ble_init_cmd(void) {
    /* Package RUCI BLE Init command */
    SET_RUCI_PARA_INITIATE_BLE(&gdtm_init_cmd_s);
    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_init_cmd_s, RUCI_INITIATE_BLE);
    dtm_set_ruci_cmd((uint8_t*)&gdtm_init_cmd_s, RUCI_LEN_INITIATE_BLE,
                     (uint8_t*)&gdtm_cnf_event_s);
    if (dtm_ruci_cmd_event_check(RUCI_PCI_BLE_CMD_HEADER,
                                 RUCI_CODE_INITIATE_BLE, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nBLE DTM initilization fails");
        return false;
    }
    DTM_PRINTF("\r\nBLE DTM initilization succeeds");

    return true;
}

/* RUCI command to init RFIC ZIGBEE */
bool dtm_zigbee_init_cmd(void) {
    /* Package RUCI ZigBee Init command */
    SET_RUCI_PARA_INITIATE_ZIGBEE(&gdtm_init_z_cmd_s);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_init_z_cmd_s, RUCI_INITIATE_ZIGBEE);

    dtm_set_ruci_cmd((uint8_t*)&gdtm_init_z_cmd_s, RUCI_LEN_INITIATE_ZIGBEE,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI15P4_MAC_CMD_HEADER,
                                 RUCI_CODE_INITIATE_ZIGBEE, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nZigBee DTM initilization fails");
        return false;
    }
    DTM_PRINTF("\r\nZigBee DTM initilization succeeds");

    return true;
}

/* RUCI command to init RFIC WISUN */
bool dtm_wisun_init_cmd(void) {
    /* Package command */
    SET_RUCI_PARA_INITIATE_FSK(&gdtm_init_w_cmd_s, BAND_SUBG);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_init_w_cmd_s, RUCI_INITIATE_FSK);

    dtm_set_ruci_cmd((uint8_t*)&gdtm_init_w_cmd_s, RUCI_LEN_INITIATE_FSK,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_FSK_CMD_HEADER,
                                 RUCI_CODE_INITIATE_FSK, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nWiSun DTM initilization fails");
        return false;
    }
    DTM_PRINTF("\r\nWiSun DTM initilization succeeds");

    return true;
}

/* RUCI command to set RFIC WISUN modem*/
bool dtm_wisun_set_modem_cmd(uint8_t data_rate, uint8_t modulation_idx) {
    /* Package command */
    SET_RUCI_PARA_SET_FSK_MODEM(&gdtm_set_w_modem_cmd_s, data_rate,
                                modulation_idx);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_w_modem_cmd_s, RUCI_SET_FSK_MODEM);

    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_w_modem_cmd_s, RUCI_LEN_SET_FSK_MODEM,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_FSK_CMD_HEADER,
                                 RUCI_CODE_SET_FSK_MODEM, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nWiSun DTM set modem fails");
        return false;
    }
    DTM_PRINTF("\r\nWiSun DTM set modem succeeds");

    return true;
}

/* RUCI command to set RFIC WISUN MAC*/
bool dtm_wisun_set_mac_cmd(uint8_t crc_type, uint8_t whitening_enable) {
    /* Package command */
    SET_RUCI_PARA_SET_FSK_MAC(&gdtm_set_w_mac_cmd_s, crc_type,
                              whitening_enable);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_w_mac_cmd_s, RUCI_SET_FSK_MAC);

    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_w_mac_cmd_s, RUCI_LEN_SET_FSK_MAC,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_FSK_CMD_HEADER, RUCI_CODE_SET_FSK_MAC,
                                 &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nWiSun DTM set mac fails");
        return false;
    }
    DTM_PRINTF("\r\nWiSun DTM set mac succeeds");

    return true;
}

/* RUCI command to set RFIC WISUN preamble length*/
bool dtm_wisun_set_preamble_cmd(uint8_t preamble_length) {
    /* Package command */
    SET_RUCI_PARA_SET_FSK_PREAMBLE(&gdtm_set_w_preamble_cmd_s, preamble_length);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_w_preamble_cmd_s,
                        RUCI_SET_FSK_PREAMBLE);

    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_w_preamble_cmd_s,
                     RUCI_LEN_SET_FSK_PREAMBLE, (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_FSK_CMD_HEADER,
                                 RUCI_CODE_SET_FSK_PREAMBLE, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nWiSun DTM set preamble fails");
        return false;
    }
    DTM_PRINTF("\r\nWiSun DTM set preamble succeeds");

    return true;
}

/* RUCI command to set RFIC WISUN SFD content*/
bool dtm_wisun_set_sfd_cmd(uint32_t sfd_content) {
    /* Package command */
    SET_RUCI_PARA_SET_FSK_SFD(&gdtm_set_w_sfd_cmd_s, sfd_content);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_w_sfd_cmd_s, RUCI_SET_FSK_SFD);

    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_w_sfd_cmd_s, RUCI_LEN_SET_FSK_SFD,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_PCI_FSK_CMD_HEADER, RUCI_CODE_SET_FSK_SFD,
                                 &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nWiSun DTM set SFD fails");
        return false;
    }
    DTM_PRINTF("\r\nWiSun DTM set SFD succeeds");

    return true;
}

/* RUCI command to init RFIC BLE */
bool dtm_set_clock_cmd(bool enable_32m) {
    /* Package RUCI ZigBee Init command */
    SET_RUCI_PARA_SET_CLOCK_MODE(
        &gdtm_set_clock_cmd_s, 0x00; /* 0x00 = BLE */, 0x01;
        /* 0x01 = 2.4GHz */, ((enable_32m == true) ? 0x00 /* 0x00 = 32MHz */
                                                   : 0x01 /* 0x01 = 16MHz */));

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_clock_cmd_s, RUCI_SET_CLOCK_MODE);
    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_clock_cmd_s, RUCI_LEN_SET_CLOCK_MODE,
                     (uint8_t*)&gdtm_cnf_event_s);
    if (dtm_ruci_cmd_event_check(RUCI_PCI_COMMON_CMD_HEADER,
                                 RUCI_CODE_SET_CLOCK_MODE, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nSet clock mode fails");
        return false;
    }
    DTM_PRINTF("\r\nSet clock mode success");

    return true;
}

/* RUCI command to set TX power*/
bool dtm_set_tx_power_cmd(uint8_t rf_band, uint8_t tx_power) {
    /* Package command */
    SET_RUCI_PARA_SET_TX_POWER(&gdtm_set_tx_pwr_cmd_s, rf_band, tx_power);

    RUCI_ENDIAN_CONVERT((uint8_t*)&gdtm_set_tx_pwr_cmd_s, RUCI_SET_TX_POWER);

    dtm_set_ruci_cmd((uint8_t*)&gdtm_set_tx_pwr_cmd_s, RUCI_LEN_SET_TX_POWER,
                     (uint8_t*)&gdtm_cnf_event_s);

    if (dtm_ruci_cmd_event_check(RUCI_CMN_HAL_CMD_HEADER,
                                 RUCI_CODE_SET_TX_POWER, &gdtm_cnf_event_s)
        == false) {
        DTM_PRINTF("\r\nWiSun DTM set tx power fails");
        return false;
    }
    DTM_PRINTF("\r\nWiSun DTM set et tx power succeeds");

    return true;
}

/* To end DTM test */
bool dtm_test_done(void) {

    /* clear CRC count number */
    gdtm_aggregated_crc_count = 0;

    if (gdtm_state == STATE_RECEIVER_TEST) {
        if (dtm_set_rx_disable_cmd() == false) {
            return false;
        }
    }

    if (gdtm_state == STATE_TRANSMITTER_TEST) {
        if (dtm_set_tx_disable_cmd() == false) {
            return false;
        }
    }

    //while(uasb_uart_log_tx((uint8_t*)end_db_done_trx_dis, sizeof(end_db_done_trx_dis)) != true);
    dtm_uasb_printf(end_db_done_trx_dis, sizeof(end_db_done_trx_dis));

    DTM_PRINTF("\r\nBLE DTM Test Done (TX/RX Disabled)");

    memset((uint8_t*)&gdtm_cnf_event_s, 0, sizeof(ruci_para_cnf_event_t));

    gdtm_state = STATE_IDLE;

    return true;
}

DTM_ERROR_CODE dtm_cmd_le_setup(uint16_t cmd_all) {
    uint8_t control, parameter;

    control = (uint8_t)((cmd_all >> 8) & 0x3F);
    // Note that BLE conformance testers always use full length packets.
    parameter = (uint8_t)((cmd_all >> 2) & 0x3F);

    // If 1 Mbit or 2 Mbit radio mode is in use check for Vendor Specific payload.
    // Note that in a HCI adaption layer, as well as in the DTM PDU format,
    // the value 0x03 is a distinct bit pattern (PRBS15). Even though BLE does not
    // support PRBS15, this implementation re-maps 0x03 to DTM_PKT_VENDORSPECIFIC,
    // to avoid the risk of confusion, should the code be extended to greater coverage.

    // Clean out any non-retrieved event that might linger from an earlier test

    // Set default event; any error will set it to LE_TEST_STATUS_EVENT_ERROR

    //* freq = control 6 bits     */
    //* length = parameter 6 bits */

    /* to disable DTM TX and RX*/
    dtm_test_done();

    switch (control) {
        case LE_TEST_SETUP_RESET: /* control = 0x00 (Reset) */
            // if((parameter != 0x00) || (dtm_set_sfd_cmd(gdtm_sfd_content) == false)){
            if (parameter != 0x00) {
                gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
                return DTM_ERROR_ILLEGAL_CONFIGURATION;
            }
            /* reset counters */
            gdtm_aggregated_crc_count = 0;
            gdtm_packet_length = 0;
            break;
        case LE_TEST_SETUP_SET_UPPER: /* control = 0x01 */
            /* parameter = 0x00~0x03 (set upper 2 bits of data parameter) */
            if (parameter > 0x03) {
                gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
                return DTM_ERROR_ILLEGAL_CONFIGURATION;
            }
            gdtm_packet_length = parameter << 6;
            break;
        case LE_TEST_SETUP_SET_PHY: /* control = 0x02 */
            if (dtm_set_phy_cmd(parameter) == false) {
                gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
                return DTM_ERROR_ILLEGAL_CONFIGURATION;
            }
            break;
        case LE_TEST_SETUP_SELECT_MODULATION: /* control = 0x03 */
            /* parameter = 0x00~0x01 (assume TX has modulation index) */
            if (parameter > 0x01) {
                gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
                return DTM_ERROR_ILLEGAL_CONFIGURATION;
            }
            /* Only standard modulation is supported. */
            break;
        case LE_TEST_SETUP_READ_SUPPORTED: /* control = 0x04 */
            /* parameter = 0x00 (read test case support features) */
            if (parameter != 0x00) {
                gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
                return DTM_ERROR_ILLEGAL_CONFIGURATION;
            }
            /* 0XXXXXXXXXXX0110 indicate that 2Mbit and DLE is supported and stable modulation is not supported (No nRF5 device supports this).*/
            gdtm_report_status = 0x0006;
            break;
        case LE_TEST_SETUP_READ_MAX: /* control = 0x05 */
            // Read max supported value.
            switch (parameter) {
                case 0x00: // Read supportedMaxTxOctets
                case 0x02: // Read supportedMaxRxOctets
                    gdtm_report_status = 0x01FE;
                    break;
                case 0x01: // Read supportedMaxTxTime
                case 0x03: // Read supportedMaxRxTime
                    gdtm_report_status = 0x4290;
                    break;
                default:
                    gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
                    return DTM_ERROR_ILLEGAL_CONFIGURATION;
            }
            break;
        default:
            gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
            return DTM_ERROR_ILLEGAL_CONFIGURATION;
    }

    return DTM_SUCCESS;
}

DTM_ERROR_CODE dtm_cmd_le_tx_rx_test(uint16_t cmd_all) {
    DTM_CMD_OPCODE cmd_op;
    uint8_t frequency, length, pkt;
    cmd_op = (DTM_CMD_OPCODE)((cmd_all >> 14) & 0x03);
    frequency = (uint8_t)((cmd_all >> 8) & 0x3F);
    length = (uint8_t)((cmd_all >> 2) & 0x3F);
    pkt = (uint8_t)((cmd_all) & 0x03);
    if (gdtm_state != STATE_IDLE) {
        gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
        return DTM_ERROR_INVALID_STATE;
    }
    gdtm_packet_length = (gdtm_packet_length & 0xC0) | ((uint8_t)length & 0x3F);
    gdtm_packet_type = (PACKET_DATA_TYPE)pkt;
    gdtm_phy_channel = frequency;
    gdtm_phr_type = (PHR_DATA_TYPE)pkt;
    if ((gdtm_radio_phy == RADIO_PHY_1M || gdtm_radio_phy == RADIO_PHY_2M
         || gdtm_radio_phy == RADIO_PHY_CODED)
        && pkt == VENDOR_SPECIFIC) {
        gdtm_packet_type = VENDOR_SPECIFIC; //0x03 -> 0xffffffff
        gdtm_phr_type = PHR_VENDOR_SPECIFIC;
    }
    if (pkt != VENDOR_SPECIFIC && gdtm_phy_channel > PHYS_CH_MAX) //>39
    {
        gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
        return DTM_ERROR_ILLEGAL_CHANNEL;
    }
    if (cmd_op == DTM_CMD_LE_RECEIVER_TEST) //0x01
    {
        gdtm_state = STATE_RECEIVER_TEST;
        if (dtm_set_rf_freq_cmd(2402 + 2 * gdtm_phy_channel) == false) {
            return DTM_ERROR_ILLEGAL_CHANNEL;
        }
        if (dtm_set_rx_enable_cmd(0x00) == false) {
            return DTM_ERROR_ILLEGAL_CONFIGURATION;
        }
    } else if (cmd_op == DTM_CMD_LE_TRANSMITTER_TEST) //0x02
    {
        gdtm_state = STATE_TRANSMITTER_TEST;
        if (pkt != VENDOR_SPECIFIC
            && gdtm_packet_length > DTM_PAYLOAD_MAX_SIZE) //>255
        {
            gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
            return DTM_ERROR_ILLEGAL_LENGTH;
        }
        gdtm_tx_interval = dtm_packet_interval_calculate(
            gdtm_packet_length, gdtm_radio_phy); // new, I(L)=N*625us /
        if (dtm_set_rf_freq_cmd(2402 + (2 * gdtm_phy_channel)) == false) {
            return DTM_ERROR_ILLEGAL_CHANNEL;
        }
        if (dtm_set_tx_enable_cmd((uint8_t)(gdtm_packet_type & 0xFF),
                                  (uint8_t)(gdtm_phr_type & 0xFF),
                                  gdtm_tx_interval,
                                  (uint16_t)(gdtm_packet_length), 0xFFFF)
            == false) {
            return DTM_ERROR_ILLEGAL_CONFIGURATION;
        }
    } //end of if(cmd==LE_TRANSMITTER_TEST)
    return DTM_SUCCESS;
}

DTM_ERROR_CODE dtm_cmd_le_test_end(uint16_t cmd_all) {
    uint16_t cmd_switch = 0;
    if (gdtm_state == STATE_IDLE) {
        cmd_switch = (cmd_all & 0xFFF);
        if (cmd_switch == LE_TEST_SWITCH_CODE) {
            g_test_switch_to_lab_test_tool = true;
            gdtm_report_status = LE_TEST_STATUS_EVENT_SUCCESS;
            g_gui_dtm_task = false;
            return DTM_SUCCESS;
        }
        gdtm_report_status = LE_TEST_STATUS_EVENT_ERROR;
        return DTM_ERROR_INVALID_STATE;
    }

    //while(uasb_uart_log_tx((uint8_t*)end_db_endter_te, sizeof(end_db_endter_te))!= true);
    dtm_uasb_printf(end_db_enter_te, sizeof(end_db_enter_te));

    dtm_test_done();

    //while(uasb_uart_log_tx((uint8_t*)end_db_done_te, sizeof(end_db_done_te))!= true);

    dtm_uasb_printf(end_db_done_te, sizeof(end_db_done_te));

    dtm_get_crc_count(&gdtm_aggregated_crc_count,
                      &gdtm_aggregated_crc_fail_count);

    //while(uasb_uart_log_tx((uint8_t*)end_db_done_cg, sizeof(end_db_done_cg)) != true);
    dtm_uasb_printf(end_db_done_cg, sizeof(end_db_done_cg));

    gdtm_report_status =
        LE_PACKET_REPORTING_EVENT
        | gdtm_aggregated_crc_count; //* m_rx_pkt_count=0 in radio_reset() (/

    cmd_switch = (cmd_all << 4 & 0xFFF);
    if (cmd_switch == LE_TEST_SWITCH_CODE) {
        g_test_switch_to_lab_test_tool = true;
        gdtm_report_status = LE_TEST_STATUS_EVENT_SUCCESS;
        g_gui_dtm_task = false;
    }

    return DTM_SUCCESS;
}

DTM_ERROR_CODE dtm_cmd_handle(uint16_t cmd_all) {
    DTM_CMD_OPCODE cmd_op;
    cmd_op = (DTM_CMD_OPCODE)((cmd_all >> 14) & 0x03);
    gdtm_event_get_flag = true;
    gdtm_report_status = LE_TEST_STATUS_EVENT_SUCCESS;
    if (gdtm_state == STATE_UNINITIALIZED) {
        return DTM_ERROR_UNINITIALIZED;
    }
    DTM_PRINTF("\r\nDTM command all 0x%02X", cmd_all);

    switch (cmd_op) {
        case DTM_CMD_LE_TEST_SETUP: return dtm_cmd_le_setup(cmd_all);
        case DTM_CMD_LE_RECEIVER_TEST:
        case DTM_CMD_LE_TRANSMITTER_TEST: return dtm_cmd_le_tx_rx_test(cmd_all);
        case DTM_CMD_LE_TEST_END: return dtm_cmd_le_test_end(cmd_all);
        default: return DTM_ERROR_ILLEGAL_CONFIGURATION;
    }
}

/* Check if DTM command already handled by RFIC */
bool dtm_event_get(dtm_report_t* p_dtm_report) {
    bool was_new = gdtm_event_get_flag; //set to true in dtm_cmd_handle()

    // mark the current event as retrieved
    gdtm_event_get_flag = false;
    *p_dtm_report = gdtm_report_status;

    // return value indicates whether this value was already retrieved.
    return was_new;
}

/* Combine DTM command thorugh 2-wire UART */
bool dtm_2wire_cmd_combine(uint8_t rx_byte, uint32_t current_time) {
    if (gdtm_cmd_idx == 0) {
        gdtm_cmd_idx += 1;
        gdtm_cmd = (uint16_t)(rx_byte << 8);
        gdtm_first_byte_time = current_time;
        DTM_PRINTF("\r\nDTM CMD Combine time mismatch 1");
        return false;
    }

    if (current_time
        > (gdtm_first_byte_time + MAX_ITERATIONS_NEEDED_FOR_NEXT_BYTE)) {
        gdtm_cmd = (uint16_t)(rx_byte << 8);
        gdtm_first_byte_time = current_time;
        DTM_PRINTF("\r\nDTM CMD Combine time mismatch 2");
        return false;
    }

    gdtm_cmd_idx += 1;
    gdtm_cmd |= rx_byte;

    DTM_PRINTF("\r\nDTM CMD Combine:0x%04X, rx:0x%02X", gdtm_cmd, rx_byte);
    return true;
}

bool dtm_2wire_cmd_combine2(void) {
    if (gdtm_cmd_idx == 2) {
        return true;
    }

    return false;
}

/* DTM command handling, DTM command is received from equipment thorugh 2-wire UART */
void dtm_2wire_cmd_handler(uint32_t current_time) {
    DTM_ERROR_CODE err;
    //uint8_t rx_byte = 0xFF;
    uint16_t result;
    //uint32_t reg_val;

    if (dtm_2wire_cmd_combine2() == false) {
        return;
    }

    /* 16bit dtm command execute */
    //RfMcu_MemoryGet(0x0100, (uint8_t*)&reg_val, 4);
    //DTM_PRINTF("\r\nbefore cmd 569_reg_0x100:%08X",reg_val);

    err = dtm_cmd_handle(gdtm_cmd);

    DTM_PRINTF("\r\nDTM 2byte CMD:0x%X handled done, err: %d", gdtm_cmd, err);
    DTM_CMD_PRINTF("\r\ncmd_type:0x%04X", gdtm_cmd);

    //RfMcu_MemoryGet(0x0100, (uint8_t*)&reg_val, 4);
    //DTM_PRINTF("\r\nafter cmd 569_reg_0x100:%08X",reg_val);

    if (err == DTM_SUCCESS) {}

    enter_critical_section();
    gdtm_cmd_idx = 0;
    gdtm_cmd = 0;
    leave_critical_section();

    // Retrieve result of the operation. This implementation will busy-loop
    // for the duration of the byte transmissions on the UART.
    if (dtm_event_get(&result)) //"true" after running dtm_cmd_handle()
    {
        // Report command status on the UART. (Send EVENT out)
        dtm_2wire_uart_send(result);
        DTM_PRINTF("\r\nDTM send result:0x%04X", result);
    }
    /* RF evaluation tool initialization */
    if (g_test_switch_to_lab_test_tool == true) {
        //printf("......................");
        while (gdtm_uart_tx_start == true)
            ;
        //printf("......................");
        g_rfet_init_cb();
    }
}

/* ISR handler tirggered from RFIC */
void dtm_isr(uint8_t intStatus) {
    COMM_SUBSYSTEM_INTERRUPT interrupt_state_value;

    interrupt_state_value.u8 = intStatus;

    /* check event status */
    if ((interrupt_state_value.bf.EVENT_DONE) == 1) {

        if (gdtm_wait_notification_flag == 1) {
            gdtm_wait_notification_flag = 0;
        } else {
            if (gdtm_wait_consecutive_notification_flag == 1) {
                gdtm_wait_consecutive_notification_flag = 0;
            } else {
                DTM_PRINTF("\r\nWrong RUCI EVENT FLAG");
                BREAK();
            }
        }
    }

    //Clean Interrupt status
    RfMcu_InterruptClear(intStatus);
}

/* DTM process initialization */
DTM_ERROR_CODE dtm_init(void) {
    /* reset DTM variables */
    gdtm_event_get_flag = false;
    gdtm_state = STATE_IDLE;
    gdtm_packet_length = 0;
    gdtm_aggregated_crc_count = 0;
    gdtm_wait_notification_flag = 1;
    gdtm_current_timer_counter = 0;

    /* init BLE for DTM mode */
    if (dtm_ble_init_cmd() == false) {
        return DTM_ERROR_ILLEGAL_CONFIGURATION;
    }

    return DTM_SUCCESS;
}

/* This API is used for HTOL mode to directly hijack DTM CMD from UART */
bool dtm_htol_mode_command_set(uint16_t dtm_cmd) {

    if (gdtm_htol_mode_en == true) {

        if (gdtm_cmd != 0) {
            return false;
        }

        /* set command and command length */
        gdtm_cmd = dtm_cmd;
        gdtm_cmd_idx = 2;

        return true;
    } else {
        return false;
    }
}

void dtm_htol_mode_set_enable(bool en) { gdtm_htol_mode_en = en; }

/* DTM platform initialization */
void dtm_sys_init(RF_FW_LOAD_SELECT fw_select, bool reinit_rf) {
    if (reinit_rf == true) {
        if (!rf_common_init_by_fw(fw_select, dtm_isr)) {
            BREAK();
        }
    }
}

void dtm_sys_common_init(void) {
    /* init RF */
    //rf_common_init_by_fw(RF_FW_LOAD_SELECT_RUCI_CMD,dtm_isr);
#if ((RF_MCU_CONST_LOAD_SUPPORTED == 0)                                        \
     || (RF_MCU_CHIP_MODEL != RF_MCU_CHIP_569M0))
    dtm_sys_init(RF_FW_LOAD_SELECT_RUCI_CMD, true);
#else
    dtm_sys_init(RF_FW_LOAD_SELECT_INTERNAL_TEST, true);
#endif
    g_fw_select = RF_FW_LOAD_SELECT_RUCI_CMD;
    /* reset DTM variables */
    gdtm_event_get_flag = false;
    gdtm_state = STATE_IDLE;
    gdtm_packet_length = 0;
    gdtm_aggregated_crc_count = 0;
    gdtm_wait_notification_flag = 1;
    gdtm_current_timer_counter = 0;
}

bool dtm_sys_is_enable(void) { return gdtm_uart_bridge_en; }

void dtm_sys_set_enable(bool is_en) { gdtm_uart_bridge_en = is_en; }

void dtm_sys_gpiotest_isr(uint32_t pin, void* isr_param) {
#if (SLEEP_TRIGGER_VALUE == 1)
    lpm_set_low_power_level(LOW_POWER_LEVEL_NORMAL);
    /* test if real wake up by IO */
    lpm_low_power_mask(LOW_POWER_MASK_BIT_RESERVED31);

    if (g_test_switch_to_lab_test_tool == true) {
        vTaskResume(xrf_rfet_taskHandle);
    } else {
        vTaskResume(xrf_dtm_taskHandle);
    }
    return;
#endif
}

#if (SLEEP_TRIGGER_VALUE == 0)
void gpio_key_0_isr(uint32_t pin, void* isr_param) {
    /* Recover pull option*/
    pin_set_pullopt(GPIO_TEST, HOSAL_PULL_UP_100K);

    /* To avoid GPIO floating after wakeup*/
    delay_ms(1000);

    /* Power level set: Normal*/
    lpm_set_low_power_level(LOW_POWER_LEVEL_NORMAL);

    /* Never sleep*/
    lpm_low_power_mask(LOW_POWER_MASK_BIT_RESERVED31);

    /* Task resume*/
    if (g_test_switch_to_lab_test_tool == true) {
        vTaskResume(xrf_rfet_taskHandle);
    } else {
        vTaskResume(xrf_dtm_taskHandle);
    }
    return;
}
#endif

void dtm_sys_gpio_init(void) {
#if (DTM_MODE_ENTER_SLEEP_SUPPORT == 1)

    /*set GPIO20 for gpio input wakeup source*/
    pin_set_mode(GPIO_TEST, MODE_GPIO);

    /*In this example,  GPIO2 falling/rising edge will generate wakeup interrupt*/
    pin_set_pullopt(GPIO_TEST, HOSAL_PULL_UP_100K);

#if (SLEEP_TRIGGER_VALUE == 1)
    gpio_cfg_input(GPIO_TEST, GPIO_PIN_INT_EDGE_FALLING);
#else
    gpio_cfg_input(GPIO_TEST, GPIO_PIN_INT_EDGE_RISING);
#endif

    gpio_register_isr(GPIO_TEST, dtm_sys_gpiotest_isr, NULL);

    /*enable gpio2 pin for interrupt source*/
    gpio_int_enable(GPIO_TEST);

    /*enable debounce for GPIO2*/
    gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    gpio_debounce_enable(GPIO_TEST);

#if (SLEEP_TRIGGER_VALUE == 0)
    /* GPIO2 may be floating if pull down before sleep, which cause wakeup interrupt loss
       Hence, we use key 0 as wakeup interrupt*/
    lpm_enable_low_power_wakeup(LOW_POWER_WAKEUP_GPIO0);
    pin_set_mode(PWR_LEVEL0_KEY,
                 MODE_GPIO); /*Select PWR_LEVEL0_KEY as GPIO mode*/
    pin_set_pullopt(PWR_LEVEL0_KEY,
                    HOSAL_PULL_UP_100K); /*Configure PWR_LEVEL0_KEY 100K pull-up*/
    gpio_cfg_input(
        PWR_LEVEL0_KEY,
        GPIO_PIN_INT_EDGE_FALLING); /*Configure PWR_LEVEL0_KEY input*/
    gpio_register_isr(PWR_LEVEL0_KEY, gpio_key_0_isr, NULL);
    gpio_int_enable(
        PWR_LEVEL0_KEY); /*enable PWR_LEVEL0_KEY pin for interrupt source*/
    gpio_debounce_enable(PWR_LEVEL0_KEY);
#endif

#endif
}

#if (CHIP_VERSION == RT58X_SHUTTLE)
static void dtm_sys_sleep_config_init(void) {
#if ((CHIP_VERSION == RT58X_MPA) || (CHIP_VERSION == RT58X_SHUTTLE))
    SYSCTRL->SYS_LOWPOWER_CTRL |= 0x80;
    SYSCTRL->SYS_LOWPOWER_CTRL &=
        (~(1 << 7)); /* Disable SRAM auto deep-sleep control. */
    PMU->PMU_RCO32K.bit.PW_32K = 3;
    PMU->PMU_MEM_CTRL.bit.EN_BOD = DISABLE;
    PMU->PMU_MEM_CTRL.bit.EN_DECT = DISABLE;
#endif
}
#endif

uint32_t dtm_sys_comp_config(comp_config_t* p_config) {
#if (DTM_MODE_ENTER_SLEEP_SUPPORT == 1)

    if (p_config == NULL) {
        return STATUS_INVALID_PARAM;
    }

    p_config->comp_int_pol = COMP_CONFIG_INT_POL_RISING;
    p_config->comp_ch_sel = COMP_CONFIG_CHSEL_CHANNEL_0;
    p_config->comp_v_sel = COMP_CONFIG_VOLTAGE_5;
#endif
    return STATUS_SUCCESS;
}

void dtm_sys_sleep_config(void) {
#if (DTM_MODE_ENTER_SLEEP_SUPPORT == 1)
    dtm_sys_gpio_init();
    dtm_sys_comp_config(&p_comp_config);
#if (CHIP_VERSION == RT58X_SHUTTLE)
    Lpm_Power_Setting();
    dtm_sys_sleep_config_init();
#endif
#endif
}

void dtm_ub_enter_sleep_by_io(void) {
#if (DTM_MODE_ENTER_SLEEP_SUPPORT == 1)

    if (gpio_pin_get(GPIO_TEST) == SLEEP_TRIGGER_VALUE) {
#if (SLEEP_TRIGGER_VALUE == 0)
        pin_set_pullopt(GPIO_TEST, HOSAL_PULL_DOWN_100K);
#endif
        lpm_set_low_power_level(LOW_POWER_LEVEL_SLEEP0);
        lpm_low_power_unmask(LOW_POWER_MASK_BIT_RESERVED31);
        /* suspend until no more event */
        vTaskSuspend(xrf_dtm_taskHandle);
    }
#endif
}

/* DTM main task */
static void dtm_task(void* parameters_ptr) {
    uint32_t current_time;

    for (;;) {

        current_time = dtm_timer_count_update();

        /* start to check DTM 2-wire UART command from equipment */
        dtm_2wire_cmd_handler(current_time);

        /*[ITS 547]Request the EVK to enter sleep mode to enter sleep mode*/
#if (DTM_MODE_ENTER_SLEEP_SUPPORT == 1)
        if (gdtm_cmd == 0) {
            dtm_ub_enter_sleep_by_io();
        }
#endif
        portYIELD();
    }
}

void dtm_mode_init(void) {
    if (gdtm_htol_mode_en == false) {
        dtm_uart_init();
    }

    dtm_cmd_timer_start();

    dtm_init();

    /* DTM sleep function */
    dtm_sys_gpio_init();
    dtm_sys_sleep_config();

    xTaskCreate(dtm_task, "TASK_RF_DTM", 1024, NULL, E_TASK_PRIORITY_APP,
                &xrf_dtm_taskHandle);
}
