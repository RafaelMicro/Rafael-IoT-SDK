/******************************************************************************
*
* @File uart_bridge.h
* @Version
* $Revision:
* $Date:
* @Brief
* @Note
*
******************************************************************************/
#ifndef _UART_BRIDGE_H_
#define _UART_BRIDGE_H_

/******************************************************************************
* INCLUDES
******************************************************************************/
#include "mcu.h"

/*******************************************************************************
*   DEFINES
*******************************************************************************/
#define UASB_UART_HOST_STANDALONE               (0)
#ifndef UASB_UART_HW_FC_SUPPORTED
#define UASB_UART_HW_FC_SUPPORTED               (0)
#endif
#if (UASB_UART_HW_FC_SUPPORTED && !SUPPORT_UART1_FLOWCNTL)
#warning "Enable SUPPORT_UART1_FLOWCNTL to support UASB_UART_HW_FC_SUPPORTED"
#endif

#define UART_0                                  (0)
#define UART_1                                  (1)
#define CHOOSE_UART                             (UART_1)

#ifndef RF_UB_SAMPLE_CFG
#define RF_UB_SAMPLE_CFG                        (0)
#endif

#define GPIO_UART0_TX                           (17)
#define GPIO_UART0_RX                           (16)
#if (RF_UB_SAMPLE_CFG)
#define GPIO_UART1_TX                           (4)
#define GPIO_UART1_RX                           (5)
#define GPIO_UART1_RTS                          (14)
#define GPIO_UART1_CTS                          (15)
#define GPIO_UB_WAKE_SRC                        (2)
#define GPIO_UB_HOST_WAKE                       (0)
#else
#define GPIO_UART1_TX                           (28)
#define GPIO_UART1_RX                           (29)
#define GPIO_UART1_RTS                          (20)
#define GPIO_UART1_CTS                          (21)
#define GPIO_UB_WAKE_SRC                        (31)
#define GPIO_UB_HOST_WAKE                       (30)
#endif

#define UASB_MAX_UART_LEN                       (1024)//(300)//(255)//
#define UASB_RUCI_HDR_SIZE                      (1)

#define UASB_RUCI_HCI_CMD_DATA_LEN_OFST         (UASB_RUCI_HDR_SIZE+2)
#define UASB_RUCI_HCI_EVENT_LEN_OFST            (UASB_RUCI_HDR_SIZE+1)
#define UASB_HCI_LEN_SIZE                       (1+1+1)

#define UASB_RUCI_SF_LEN_OFST                   (UASB_RUCI_HDR_SIZE+1)
#define UASB_SF_LEN_SIZE                        (UASB_RUCI_SF_LEN_OFST + 2)

#define UASB_RUCI_PCI_SUB_HDR_OFFST             (UASB_RUCI_HDR_SIZE)
#define UASB_RUCI_PCI_LEN_OFFST                 (UASB_RUCI_PCI_SUB_HDR_OFFST+1)  // 2
#define UASB_RUCI_PCI_PARA_OFFST                (UASB_RUCI_PCI_LEN_OFFST+1)      // 3
#define UASB_PCI_LEN_OFFSET                     (UASB_RUCI_PCI_PARA_OFFST)       // 3

#define UASB_RUCI_CMN_LEN_OFFST                 (UASB_RUCI_HDR_SIZE+1)  // 2
#define UASB_RUCI_CMN_PARA_OFFST                (UASB_RUCI_CMN_LEN_OFFST+1)      // 3
#define UASB_CMN_LEN_OFFSET                     (UASB_RUCI_CMN_PARA_OFFST)       // 3

#define UASB_RUCI_MCU_SUB_HDR_OFFST             (UASB_RUCI_HDR_SIZE)
#define UASB_RUCI_MCU_LEN_OFFST                 (UASB_RUCI_MCU_SUB_HDR_OFFST+1)  // 2
#define UASB_RUCI_MCU_PARA_OFFST                (UASB_RUCI_MCU_LEN_OFFST+1)      // 3
#define UASB_MCU_LEN_OFFSET                     (UASB_RUCI_MCU_PARA_OFFST)       // 3

typedef uint8_t UASB_RUCI_CMD_TYPES;
#define UASB_RUCI_HCI_CMD                       ((UASB_RUCI_CMD_TYPES)0x01)
#define UASB_RUCI_ACL_DATA                      ((UASB_RUCI_CMD_TYPES)0x02)
#define UASB_RUCI_HCI_EVENT                     ((UASB_RUCI_CMD_TYPES)0x04)

typedef uint8_t UASB_SPI_CTRL_STATE;
#define UASB_SPI_CTRL_INIT                      ((UASB_SPI_CTRL_STATE)0x00)
#define UASB_SPI_READ_CMD_Q                     ((UASB_SPI_CTRL_STATE)0x01)
#define UASB_SPI_READ_DATA_Q                    ((UASB_SPI_CTRL_STATE)0x04)
#define UASB_SPI_READ_MCU_STATE                 ((UASB_SPI_CTRL_STATE)0x10)

typedef uint8_t UASB_INT_STATE;
#define UASB_INT_NO_STS                         ((UASB_INT_STATE)RF_MCU_INTR_NONE)
#define UASB_INT_CMD_DONE                       ((UASB_INT_STATE)RF_MCU_SW_0_INTR)
#define UASB_INT_TX_DONE                        ((UASB_INT_STATE)RF_MCU_SW_1_INTR)
#define UASB_INT_SYS_ASSERT                     ((UASB_INT_STATE)RF_MCU_SW_2_INTR)
#define UASB_INT_BANK_SWITCH                    ((UASB_INT_STATE)RF_MCU_SW_3_INTR)
#define UASB_INT_CMD_FAIL                       ((UASB_INT_STATE)RF_MCU_SW_4_INTR)
#define UASB_INT_RX_DONE                        ((UASB_INT_STATE)RF_MCU_BMU_RX_VALID_INTR)
#define UASB_INT_RTC_WAKE                       ((UASB_INT_STATE)RF_MCU_RTC_WAKE_INTR)

#define HCI_COMMAND_EVENT_MAX_LENGTH            (255)
#define HCI_ACL_DATA_MAX_LENGTH                 (255)
#define RUCI_DATA_MAX_LENGTH                    (UASB_MAX_UART_LEN)
#define HCI_READ_LOCAL_NAME_SIZE                (248)

#define BLE_ACL_UART_TRANS_OVERHEAD             (3)
#define BLE_ACL_LENGTH_OVERHEAD                 (2)
#define RUCI_COMMAND_EVENT_MAX_LENGTH           (255)

struct __attribute__((packed)) ble_hci_command_struct
{
    uint8_t transport_id;
    uint16_t ocf: 10;
    uint16_t ogf: 6;
    uint8_t length;
    uint8_t parameter[HCI_COMMAND_EVENT_MAX_LENGTH];
};

struct __attribute__((packed)) ble_hci_acl_data_struct
{
    uint8_t transport_id;
    uint16_t handle: 12;
    uint16_t pb_flag: 2;
    uint16_t bc_flag: 2;
    uint16_t length;
    uint8_t data[HCI_ACL_DATA_MAX_LENGTH];
};

struct __attribute__((packed)) ble_hci_acl_data_sn_struct
{
    uint8_t transport_id;
    uint16_t sequence;
    uint16_t handle: 12;
    uint16_t pb_flag: 2;
    uint16_t bc_flag: 2;
    uint16_t length;
    uint8_t data[HCI_ACL_DATA_MAX_LENGTH];
};

struct __attribute__((packed)) ble_hci_event_struct
{
    uint8_t transport_id;
    uint8_t event_code;
    uint8_t length;
    uint8_t parameter[HCI_COMMAND_EVENT_MAX_LENGTH];
};

struct __attribute__((packed)) ruci_command_event_struct
{
    uint8_t ruci_header;
    uint8_t sub_header;
    uint8_t length;
    uint8_t parameter[RUCI_COMMAND_EVENT_MAX_LENGTH];
};

struct __attribute__((packed)) ruci_data_struct
{
    uint8_t ruci_header;
    uint8_t sub_header;
    uint16_t length;
    uint8_t data[RUCI_COMMAND_EVENT_MAX_LENGTH];
};

typedef union ruci_ble_hci_message
{
    uint8_t general_data_array[RUCI_DATA_MAX_LENGTH + 10];
    uint8_t ble_hci_array[HCI_ACL_DATA_MAX_LENGTH + 10];
    struct ble_hci_command_struct hci_command;
    struct ble_hci_acl_data_struct hci_acl_data;
    struct ble_hci_event_struct hci_event;
    struct ruci_command_event_struct ruci_cmd;
    struct ruci_command_event_struct ruci_event;
    struct ruci_data_struct ruci_data;
} UNION_RUCI_HCI_MESSAGE;

struct ruci_hci_message_struct
{
    UNION_RUCI_HCI_MESSAGE ruci_hci_message;
};
/*******************************************************************************
*   GLOBAL FUNCTIONS
*******************************************************************************/
bool uasb_uart_log_tx(uint8_t *str, uint16_t length);
void uasb_init_cmd_interface(RF_FW_LOAD_SELECT fw_select, bool init_interfrace_en);
void uasb_init(bool baud_rate_high);

#endif /* _UART_BRIDGE_H_ */
