/**
 * @file cpc_system_secondary.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdbool.h>
#include <string.h>

#include "cpc.h"
#include "cpc_api.h"
#include "cpc_config.h"
#include "cpc_drv.h"
#include "cpc_hdlc.h"
#include "cpc_system_common.h"
#include "mem_pool.h"

#include "mcu.h"

#include "FreeRTOS.h"
#include "timers.h"

#define CPC_PACKED_ENDPOINT_PRESENT 0 // Not yet implemented

#define CPC_DROP()                                                             \
    {                                                                          \
        configASSERT(false);                                                   \
        return;                                                                \
    }

#define CHECK_DROP(cond)                                                       \
    {                                                                          \
        if (cond)                                                              \
            CPC_DROP();                                                        \
    }

/*******************************************************************************
 ***************************  GLOBAL VARIABLES   ********************************
 ******************************************************************************/
extern void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode);
extern cpc_drv_capabilities_t cpc_driver_capabilities;

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static bool restart_flag = false;
static TimerHandle_t sResetTimer;
static uint32_t enter_irq_end_ms;
static cpc_timer_handle_t enter_irq_timer;

static bool process_uframes_flag = false;

static uint32_t fc_validation_value;

/***************************************************************************/ /**
                                                                               * System endpoint handle
                                                                               ******************************************************************************/
static cpc_endpoint_handle_t system_ep = {.ep = NULL};

/***************************************************************************/ /**
                                                                               * bootloader_reboot_mode.
                                                                               * 0 - At the next reboot application is executed.
                                                                               * 1 -  At the next reboot bootloader is executed. Depending on the
                                                                               *      configuration of the bootloader, it either updates the application or
                                                                               *      starts an NCP mode.
                                                                               ******************************************************************************/
static cpc_system_reboot_mode_t prop_bootloader_reboot_mode =
    REBOOT_APPLICATION;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

static void on_write_completed(cpc_user_endpoint_id_t endpoint_id, void* buffer,
                               void* arg, status_t status);

static status_t open_endpoint(void);
static void on_system_ep_error(cpc_user_endpoint_id_t endpoint_id);

static void send_reset_reason(void);

static void on_poll(uint8_t endpoint_id, void* arg, void* poll_data,
                    uint32_t poll_data_length, void** reply_data,
                    uint32_t* reply_data_lenght, void** on_write_complete_arg);

static void enter_irq_timer_callback(cpc_timer_handle_t* handle, void* data);

__weak void system_on_information_received(uint8_t endpoint_id, void* arg);
__weak const char* cpc_secondary_app_version(void);

__weak void cpc_set_secondary_rf_cert_band(uint32_t band) {}

/***************************************************************************/ /**
                                                                               * Called when secondary app version is requested.
                                                                               * The format is up to the user. The string should be null terminated.
                                                                               ******************************************************************************/
__weak const char* cpc_secondary_app_version(void) { return "UNDEFINED"; }

static status_t cpc_secondary_ep_option(cpc_endpoint_option_t option,
                                        void* value) {
    status_t status = cpc_set_endpoint_option(&system_ep, option, (void*)value);
    if (status != STATUS_OK)
        cpc_close_endpoint(&system_ep);
    return status;
}

/***************************************************************************/ /**
                                                                               * Initialize CPC System
                                                                               ******************************************************************************/
static status_t open_endpoint(void) {
    status_t status;

    status = cpc_open_service_endpoint(&system_ep, CPC_ENDPOINT_SYSTEM,
                                       CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE,
                                       10);
    if (status == CPC_STATUS_ALREADY_EXISTS)
        return CPC_STATUS_ALREADY_INITIALIZED;
    else if (status != STATUS_OK)
        return status;

    CPC_CHECK_STATUS_OK(
        cpc_secondary_ep_option(CPC_ENDPOINT_ON_POLL, (void*)on_poll));

    if (system_on_information_received) {
        CPC_CHECK_STATUS_OK(
            cpc_secondary_ep_option(CPC_ENDPOINT_ON_UFRAME_RECEIVE,
                                    (void*)system_on_information_received));
    }

    CPC_CHECK_STATUS_OK(cpc_secondary_ep_option(
        CPC_ENDPOINT_ON_UFRAME_WRITE_COMPLETED, (void*)on_write_completed));

    CPC_CHECK_STATUS_OK(cpc_secondary_ep_option(
        CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, (void*)on_write_completed));

    CPC_CHECK_STATUS_OK(cpc_secondary_ep_option(CPC_ENDPOINT_ON_ERROR,
                                                (void*)on_system_ep_error));
    return STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Process the system endpoint
                                                                               ******************************************************************************/
void cpc_system_process(void) {

    if (restart_flag) {
        cpc_endpoint_state_t state = cpc_get_endpoint_state(&system_ep);

        if (state == CPC_STATE_FREED) {
            if (open_endpoint() == STATUS_OK)
                restart_flag = false;
            else {
                configASSERT(false);
                restart_flag = true;
            }
        } else if (state != CPC_STATE_CLOSED)
            cpc_close_endpoint(&system_ep);
    }

    if (process_uframes_flag) {
        void* data;
        uint16_t data_length;
        status_t status = cpc_read(&system_ep, &data, &data_length, 0,
                                   CPC_FLAG_NO_BLOCK
                                       | CPC_FLAG_UNNUMBERED_INFORMATION);
        if (status == STATUS_OK) {
            if (data_length > 0)
                fc_validation_value += ((uint8_t*)data)[0];
            cpc_free_rx_buffer(data);
        }
    }
}

/***************************************************************************/ /**
                                                                               * Initialize CPC System
                                                                               ******************************************************************************/
status_t cpc_system_init(void) {
    status_t status = open_endpoint();
    send_reset_reason();
    return status;
}

/***************************************************************************/ /**
                                                                               * System endpoint on error callback
                                                                               ******************************************************************************/
static void on_system_ep_error(cpc_user_endpoint_id_t endpoint_id) {
    configASSERT(endpoint_id == CPC_ENDPOINT_SYSTEM);
    restart_flag = true;
}

/***************************************************************************/ /**
                                                                               * Reset system; Should not return
                                                                               ******************************************************************************/

//  __weak void sscpc_system_reset(cpc_system_reboot_mode_t reboot_mode)
// {
//   (void)reboot_mode;
// #if (defined(CATALOG_EMLIB_CORE_PRESENT))
// #if (defined(CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT))
//   // The reset command asked to perform a reset.
//   BootloaderResetCause_t* resetCause = (BootloaderResetCause_t*) (RAM_MEM_BASE);

//   // Set reset reason to bootloader entry
//   switch (reboot_mode) {
//     case REBOOT_APPLICATION:
//       resetCause->reason = BOOTLOADER_RESET_REASON_GO;
//       break;
//     case REBOOT_BOOTLOADER:
//       resetCause->reason = BOOTLOADER_RESET_REASON_BOOTLOAD;
//       break;
//     default:
//       configASSERT(false);
//       break;
//   }

//   resetCause->signature = BOOTLOADER_RESET_SIGNATURE_VALID;
// #endif  // CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT

// #if defined(RMU_PRESENT)
// // Clear resetcause
//   RMU->CMD = RMU_CMD_RCCLR;
// // Trigger a software system reset
//   RMU->CTRL = (RMU->CTRL & ~_RMU_CTRL_SYSRMODE_MASK) | RMU_CTRL_SYSRMODE_EXTENDED;
// #endif  // RMU_PRESENT

//   CHIP_Reset();
// #else

// #endif  // CATALOG_EMLIB_CORE_PRESENT
//   exit(1);
// }

/***************************************************************************/ /**
                                                                               * System endpoint on write completed callback
                                                                               ******************************************************************************/
static void on_write_completed(cpc_user_endpoint_id_t endpoint_id, void* buffer,
                               void* arg, status_t status) {
    (void)endpoint_id;
    (void)status;
    // Unnumbered ACK has no payload
    if (buffer != NULL)
        cpc_free_command_buffer(buffer);
    if ((uint32_t)arg == 0xDEADBEEF)
        cpc_system_reset(prop_bootloader_reboot_mode);
}

/***************************************************************************/ /**
                                                                               * Endpoint was closed, notify the host
                                                                               ******************************************************************************/
status_t cpc_send_disconnection_notification(uint8_t endpoint_id) {
    status_t status;
    cpc_system_cmd_t* tx_command;
    cpc_system_property_cmd_t* tx_prop_command;
    cpc_endpoint_state_t* ep_state;

    status = cpc_get_system_command_buffer(&tx_command);
    if (status != STATUS_OK)
        return status;

    tx_command->header.command_id = CMD_SYSTEM_PROP_VALUE_IS;
    tx_command->header.seq = 0;

    tx_prop_command = (cpc_system_property_cmd_t*)(tx_command->payload);
    tx_prop_command->property_id = EP_ID_TO_PROPERTY_STATE(endpoint_id);

    ep_state = (cpc_endpoint_state_t*)tx_prop_command->payload;
    tx_command->header.length = sizeof(cpc_system_property_cmd_t)
                                + sizeof(cpc_endpoint_state_t);

    *ep_state = CPC_STATE_CLOSING;

    status = cpc_write(
        &system_ep, (void*)tx_command,
        sizeof(cpc_system_cmd_header_t) + tx_command->header.length, 0, NULL);
    configASSERT(status == STATUS_OK); // Ignore Error

    return STATUS_OK;
}

/***************************************************************************/ /**
                                                                               * Get system reset reason.
                                                                               ******************************************************************************/
__weak cpc_system_status_t cpc_get_reset_reason(void) {
    cpc_system_status_t last_status = STATUS_RESET_POWER_ON;
#if defined(RMU_PRESENT)

    uint32_t reset_cause;

    reset_cause = RMU_ResetCauseGet();

#if defined(_RMU_RSTCAUSE_MASK)
    if (reset_cause & RMU_RSTCAUSE_PORST)
        last_status = STATUS_RESET_POWER_ON;
    else if (reset_cause & RMU_RSTCAUSE_AVDDBOD)
        last_status = STATUS_RESET_FAULT;
    else if (reset_cause & RMU_RSTCAUSE_DVDDBOD)
        last_status = STATUS_RESET_FAULT;
    else if (reset_cause & RMU_RSTCAUSE_DECBOD)
        last_status = STATUS_RESET_FAULT;
    else if (reset_cause & RMU_RSTCAUSE_EXTRST)
        last_status = STATUS_RESET_EXTERNAL;
    else if (reset_cause & RMU_RSTCAUSE_WDOGRST)
        last_status = STATUS_RESET_WATCHDOG;
    else if (reset_cause & RMU_RSTCAUSE_LOCKUPRST)
        last_status = STATUS_RESET_CRASH;
    else if (reset_cause & RMU_RSTCAUSE_SYSREQRST)
        last_status = STATUS_RESET_SOFTWARE;
    else if (reset_cause & RMU_RSTCAUSE_EM4RST)
        last_status = STATUS_RESET_OTHER;
#else
#if defined(_EMU_RSTCTRL_AVDDBODRMODE_MASK)
    if (reset_cause & rmuResetAVDD)
        last_status = STATUS_RESET_FAULT;
#endif
#if defined(_EMU_RSTCTRL_IOVDD0BODRMODE_MASK)
    if (reset_cause & rmuResetIOVDD0)
        last_status = STATUS_RESET_FAULT;
#endif
#if defined(_EMU_RSTCTRL_DECBODRMODE_MASK)
    if (reset_cause & rmuResetDecouple)
        last_status = STATUS_RESET_FAULT;
#endif
#if defined(_EMU_RSTCTRL_WDOG0RMODE_MASK)
    if (reset_cause & rmuResetWdog0)
        last_status = STATUS_RESET_WATCHDOG;
#endif
#if defined(_EMU_RSTCTRL_WDOG1RMODE_MASK)
    if (reset_cause & rmuResetWdog1)
        last_status = STATUS_RESET_WATCHDOG;
#endif
#if defined(_EMU_RSTCTRL_LOCKUPRMODE_MASK)
    if (reset_cause & rmuResetCoreLockup)
        last_status = STATUS_RESET_CRASH;
#endif
#if defined(_EMU_RSTCTRL_SELOCKUPRMODE_MASK)
    if (reset_cause & rmuResetSELockup)
        last_status = STATUS_RESET_CRASH;
#endif
#if defined(_EMU_RSTCTRL_SYSRMODE_MASK)
    if (reset_cause & rmuResetSys)
        last_status = STATUS_RESET_SOFTWARE;
#endif
#if defined(_EMU_RSTCTRL_SESYSRMODE_MASK)
    if (reset_cause & rmuResetSESys)
        last_status = STATUS_RESET_SOFTWARE;
#endif
#endif

    RMU_ResetCauseClear();
#endif

    return last_status;
}

static void reset_cb(TimerHandle_t xTimer) {
    //cpc_system_reset(0);
}

/***************************************************************************/ /**
                                                                               * Send reset reason
                                                                               ******************************************************************************/
static void send_reset_reason(void) {
    cpc_system_cmd_t* tx_command;
    cpc_system_property_cmd_t* tx_prop_command;
    cpc_system_status_t* tx_prop_last_status;

    configASSERT(cpc_get_system_command_buffer(&tx_command) == STATUS_OK);

    tx_command->header.command_id = CMD_SYSTEM_PROP_VALUE_IS;
    tx_command->header.seq = 0; /* Seq is irrelevant in u-frame information */

    tx_prop_command = (cpc_system_property_cmd_t*)(tx_command->payload);
    tx_prop_command->property_id = PROP_LAST_STATUS;

    tx_prop_last_status = (cpc_system_status_t*)tx_prop_command->payload;
    *tx_prop_last_status = cpc_get_reset_reason();

    tx_command->header.length = sizeof(cpc_system_property_cmd_t)
                                + sizeof(cpc_system_status_t);
    cpc_write(&system_ep, (void*)tx_command,
              sizeof(cpc_system_cmd_header_t) + tx_command->header.length,
              CPC_FLAG_UNNUMBERED_INFORMATION, NULL);

    // configASSERT(status == STATUS_OK); //Ignore error
    sResetTimer = xTimerCreate("Reset_T", 500, false, NULL, reset_cb);
    xTimerStart(sResetTimer, 0);
}

/*******************************************************************************
 ***************************  PROPERTY-GET HANDLERS  ***************************
 ******************************************************************************/

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_LAST_STATUS
                                                                               *   When replying to any CMD_PROPERTY_GET/SET which result in an unsuccessful
                                                                               *   operation, the RCP will reply with PROP_LAST_STATUS instead. Here, this
                                                                               *   property is specifically queried. There is nothing more meaningful than
                                                                               *   to reply with a STATUS_OK.
                                                                               ******************************************************************************/
static void on_property_get_last_status(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    prop_cmd_buff->property_id = PROP_LAST_STATUS;
    *((cpc_system_status_t*)(prop_cmd_buff->payload)) = STATUS_OK;
    tx_command->header.length = sizeof(cpc_property_id_t)
                                + sizeof(cpc_system_status_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_PROTOCOL_VERSION
                                                                               *   Ship the hardcoded major and minor version number back to the primary.
                                                                               ******************************************************************************/
static void on_property_get_protocol_version(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    prop_cmd_buff->property_id = PROP_PROTOCOL_VERSION;
    prop_cmd_buff->payload[0] = CPC_PROTOCOL_VERSION;
    tx_command->header.length = sizeof(cpc_property_id_t) + sizeof(uint8_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_SECONDARY_CPC_VERSION
                                                                               *   Ship the hardcoded major and minor version number back to the primary.
                                                                               ******************************************************************************/
static void
on_property_get_secondary_cpc_version(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    uint32_t* version = (uint32_t*)(prop_cmd_buff->payload);

    prop_cmd_buff->property_id = PROP_SECONDARY_CPC_VERSION;
    version[0] = 1;
    version[1] = 0;
    version[2] = 0;
    tx_command->header.length = sizeof(cpc_property_id_t)
                                + (3 * sizeof(uint32_t));
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_SECONDARY_APP_VERSION
                                                                               *   Send a string version of the secondary application to the primary
                                                                               ******************************************************************************/
static void
on_property_get_secondary_app_version(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    const char* app_version;
    uint32_t app_version_len;

    prop_cmd_buff->property_id = PROP_SECONDARY_APP_VERSION;
    app_version = cpc_secondary_app_version();
    app_version_len = strlen(app_version) + 1;
    memcpy(prop_cmd_buff->payload, app_version,
           (app_version_len > CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH)
               ? CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH
               : app_version_len);

    tx_command->header.length = sizeof(cpc_property_id_t) + app_version_len;
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_CAPABILITIES
                                                                               *   Send the capabilities of the secondary to the primary
                                                                               ******************************************************************************/
static void on_property_get_capabilities(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    uint32_t* capabilities = (uint32_t*)(prop_cmd_buff->payload);

    prop_cmd_buff->property_id = PROP_CAPABILITIES;
    *capabilities = CPC_PACKED_ENDPOINT_PRESENT
                        * CPC_CAPABILITIES_PACKED_ENDPOINT_MASK
                    + cpc_driver_capabilities.uart_flowcontrol
                          * CPC_CAPABILITIES_UART_FLOW_CONTROL_MASK;
    tx_command->header.length = sizeof(cpc_property_id_t) + sizeof(uint32_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_RX_CAPABILITY
                                                                               *   Send the rx buffer capability of the secondary to the primary
                                                                               ******************************************************************************/
static void on_property_get_rx_capabilities(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    uint16_t* rx_capability = (uint16_t*)(prop_cmd_buff->payload);

    xTimerStop(sResetTimer, 0);
    prop_cmd_buff->property_id = PROP_RX_CAPABILITY;
    *rx_capability = CONFIG_CPC_RX_PAYLOAD_MAX_LENGTH;
    tx_command->header.length = sizeof(cpc_property_id_t) + sizeof(uint16_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_FC_VALIDATION_VALUE
                                                                               *   Send the flow control validation value of the secondary to the primary
                                                                               ******************************************************************************/
static void on_property_get_fc_validation_value(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    uint32_t* flow_control_validation_value =
        (uint32_t*)(prop_cmd_buff->payload);

    prop_cmd_buff->property_id = PROP_FC_VALIDATION_VALUE;
    *flow_control_validation_value = fc_validation_value;
    tx_command->header.length = sizeof(cpc_property_id_t) + sizeof(uint32_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_BUS_SPEED_VALUE
                                                                               *   Send the bus speed value of the secondary to the primary.
                                                                               ******************************************************************************/
static void on_property_get_bus_speed_value(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    uint32_t* bus_speed_value = (uint32_t*)(prop_cmd_buff->payload);

    prop_cmd_buff->property_id = PROP_BUS_SPEED_VALUE;
    *bus_speed_value = cpc_drv_get_bus_speed();
    tx_command->header.length = sizeof(cpc_property_id_t) + sizeof(uint32_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_BOOTLOADER_INFO
                                                                               *   Reply to the PRIMARY the bootloader infos.
                                                                               ******************************************************************************/
static void on_property_get_bootloader_info(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* tx_property =
        (cpc_system_property_cmd_t*)tx_command->payload;
    uint32_t* infos = (uint32_t*)(tx_property->payload);

    tx_property->property_id = PROP_BOOTLOADER_INFO;
    infos[0] = (uint32_t)0;
    infos[1] = 0xFFFFFFFF;
    tx_command->header.length = sizeof(cpc_property_id_t)
                                + 3 * sizeof(uint32_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_BOOTLOADER_REBOOT_MODE
                                                                               *   Reply to the PRIMARY the bootloader reboot mode.
                                                                               ******************************************************************************/
static void
on_property_get_bootloader_reboot_mode(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* tx_property =
        (cpc_system_property_cmd_t*)tx_command->payload;
    cpc_system_reboot_mode_t* mode =
        (cpc_system_reboot_mode_t*)(tx_property->payload);

    tx_property->property_id = PROP_BOOTLOADER_REBOOT_MODE;
    *mode = prop_bootloader_reboot_mode;
    tx_command->header.length = sizeof(cpc_property_id_t)
                                + sizeof(cpc_system_reboot_mode_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_SECURITY_STATE
                                                                               *   Reply to the PRIMARY the security state.
                                                                               ******************************************************************************/
static void on_property_get_security_state(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* tx_property =
        (cpc_system_property_cmd_t*)tx_command->payload;
    uint32_t* security_state = (uint32_t*)(tx_property->payload);

#ifdef CATALOG_CPC_SECURITY_PRESENT
    tx_property->property_id = PROP_SECURITY_STATE;
    *security_state = cpc_security_get_state();
#else
    tx_property->property_id = PROP_LAST_STATUS;
    *security_state = STATUS_UNIMPLEMENTED;
#endif

    tx_command->header.length = sizeof(cpc_property_id_t) + sizeof(uint32_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_ENDPOINT_STATE_x
                                                                               *   The primary queried the status of a specific endpoint number.
                                                                               ******************************************************************************/
static void on_property_get_endpoint_state(cpc_system_cmd_t* tx_command,
                                           uint8_t ep_id) {
    cpc_system_property_cmd_t* reply_prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    cpc_endpoint_state_t* reply_ep_state =
        (cpc_endpoint_state_t*)reply_prop_cmd_buff->payload;
    cpc_endpoint_handle_t dummy_ep = {.ep = NULL, .id = ep_id};

    reply_prop_cmd_buff->property_id = EP_ID_TO_PROPERTY_STATE(ep_id);
    *reply_ep_state = cpc_get_endpoint_state(&dummy_ep);
    tx_command->header.length = sizeof(cpc_property_id_t)
                                + sizeof(cpc_endpoint_state_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_ENDPOINT_ENCRYPTION_x
                                                                               *   The primary queried the encryption state of a specific endpoint number.
                                                                               ******************************************************************************/
static void on_property_get_endpoint_encryption(cpc_system_cmd_t* tx_command,
                                                uint8_t ep_id) {
    cpc_system_property_cmd_t* reply_prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    bool* reply_ep_encryption = (bool*)reply_prop_cmd_buff->payload;
    cpc_endpoint_handle_t dummy_ep = {.ep = NULL, .id = ep_id};

    reply_prop_cmd_buff->property_id = EP_ID_TO_PROPERTY_ENCRYPTION(ep_id);
    *reply_ep_encryption = cpc_get_endpoint_encryption(&dummy_ep);
    tx_command->header.length = sizeof(cpc_property_id_t) + sizeof(bool);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_ENDPOINT_STATES
                                                                               *   The primary queried the status of all endpoints.
                                                                               ******************************************************************************/
static void on_property_get_endpoint_states(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* reply_prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;
    uint8_t* reply_ep_states = (uint8_t*)(reply_prop_cmd_buff->payload);

    reply_prop_cmd_buff->property_id = PROP_ENDPOINT_STATES;

    // process 2 endpoints per iteration
    for (size_t i = 0; i != CPC_ENDPOINT_MAX_COUNT / 2; i++) {
        cpc_endpoint_handle_t dummy_ep1 = {.ep = NULL, .id = 2 * i};
        cpc_endpoint_handle_t dummy_ep2 = {.ep = NULL, .id = 2 * i + 1};

        cpc_endpoint_state_t ep1_state = cpc_get_endpoint_state(&dummy_ep1);
        cpc_endpoint_state_t ep2_state = cpc_get_endpoint_state(&dummy_ep2);

        // Although an 'cpc_endpoint_state_t' is an 8 bit value, the number of
        // values in the enum makes it possible to encode it with a nibble (4 bits)
        // as only 3 bits are required to encode those 6 values. Put the first
        // endpoint in the low nibble and the second in the high nibble.
        // This aggregation will make it possible to send all 256 endpoint states
        // in one reply as it will fit within 255 bytes (limited by the length field
        // within a command frame)
        reply_ep_states[i] = (ep2_state << 4) | ep1_state;
    }

    tx_command->header.length = sizeof(cpc_property_id_t)
                                + (CPC_ENDPOINT_MAX_COUNT * sizeof(uint8_t)
                                   / 2);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_GET
                                                                               * Property ID: PROP_DEBUG_COUNTERS
                                                                               *   The primary queried the debug counters.
                                                                               ******************************************************************************/
#if (CPC_DEBUG_CORE_EVENT_COUNTERS == 1)
static void on_property_get_core_debug_counters(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* reply_prop_cmd_buff =
        (cpc_system_property_cmd_t*)tx_command->payload;

    memcpy(reply_prop_cmd_buff->payload, &cpc_core_debug.core_counters,
           sizeof(cpc_core_debug_counters_t));
    reply_prop_cmd_buff->property_id = PROP_CORE_DEBUG_COUNTERS;
    tx_command->header.length = sizeof(cpc_property_id_t)
                                + sizeof(cpc_core_debug_counters_t);
}
#endif

/***************************************************************************/ /**
                                                                               * Handler for when the primary asks about a property not found:
                                                                               *   As with any property-get/set which is unsuccessful, the rcp replies with
                                                                               *   a property id of PROP_LAST_STATUS. Since the property the primary asked about
                                                                               *   can't be handled by the rcp, the status returned is STATUS_PROP_NOT_FOUND.
                                                                               ******************************************************************************/
static void on_property_get_set_not_found(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* tx_property_cmd =
        (cpc_system_property_cmd_t*)tx_command->payload;

    tx_property_cmd->property_id = PROP_LAST_STATUS;
    *((cpc_system_status_t*)(tx_property_cmd->payload)) = STATUS_PROP_NOT_FOUND;
    tx_command->header.length = sizeof(cpc_property_id_t)
                                + sizeof(cpc_system_status_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_SET:
                                                                               * Property ID: PROP_ENDPOINT_STATE_x
                                                                               *   The primary notifies the secondary of an endpoint state change
                                                                               ******************************************************************************/
static void on_property_set_state(uint8_t endpoint_id,
                                  cpc_system_cmd_t* tx_command,
                                  cpc_system_cmd_t* rx_command) {
    (void)rx_command;

    cpc_system_property_cmd_t* tx_property_command =
        (cpc_system_property_cmd_t*)tx_command->payload;

    cpc_remote_disconnected(endpoint_id);

    tx_command->header.length = sizeof(cpc_property_id_t);
    tx_property_command->property_id = EP_ID_TO_PROPERTY_STATE(endpoint_id);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_SET:
                                                                               * Property ID: PROP_UFRAME_PROCESSING
                                                                               *   The primary enables/disables u-frame processing on the secondary.
                                                                               ******************************************************************************/
static void on_property_set_uframe_processing(cpc_system_cmd_t* tx_command,
                                              cpc_system_cmd_t* rx_command) {
    cpc_system_property_cmd_t* tx_prop_cmd =
        (cpc_system_property_cmd_t*)rx_command->payload;
    cpc_system_property_cmd_t* rx_prop_cmd =
        (cpc_system_property_cmd_t*)tx_command->payload;

    process_uframes_flag = *(bool*)rx_prop_cmd->payload;

    tx_prop_cmd->property_id = PROP_UFRAME_PROCESSING;
    tx_command->header.length = sizeof(cpc_property_id_t);
}

/***************************************************************************/ /**
                                                                               * Command ID:  CMD_PROPERTY_SET:
                                                                               * Property ID: PROP_BOOTLOADER_REBOOT_MODE
                                                                               *   The primary sets the reboot mode.
                                                                               ******************************************************************************/
static void
on_property_set_bootloader_reboot_mode(cpc_system_cmd_t* tx_command,
                                       cpc_system_cmd_t* rx_command) {
    cpc_system_property_cmd_t* tx_prop_cmd =
        (cpc_system_property_cmd_t*)tx_command->payload;
    cpc_system_property_cmd_t* rx_prop_cmd =
        (cpc_system_property_cmd_t*)rx_command->payload;
    cpc_system_reboot_mode_t* rx_mode =
        (cpc_system_reboot_mode_t*)rx_prop_cmd->payload;
    cpc_system_reboot_mode_t* tx_mode =
        (cpc_system_reboot_mode_t*)tx_prop_cmd->payload;

    tx_command->header.length = sizeof(cpc_property_id_t);

    switch (*rx_mode) {
        case REBOOT_APPLICATION: {
            /* We can always reset to the application */
            prop_bootloader_reboot_mode = REBOOT_APPLICATION;
            tx_prop_cmd->property_id = PROP_BOOTLOADER_REBOOT_MODE;
            *tx_mode = REBOOT_APPLICATION;
            tx_command->header.length += sizeof(cpc_system_reboot_mode_t);
            break;
        }

        case REBOOT_BOOTLOADER: {
            /* Check if the bootloader interface is present */

#if (defined(CPC_HAS_BOOTLOADER_SUPPORT))
            prop_bootloader_reboot_mode = REBOOT_BOOTLOADER;
            *tx_mode = REBOOT_BOOTLOADER;
#else
            /* We don't support bootloader reboot, stay in application reboot */
            prop_bootloader_reboot_mode = REBOOT_APPLICATION;
            /* Reply that we stay in reboot application mode */
            *tx_mode = REBOOT_APPLICATION;
#endif
            tx_prop_cmd->property_id = PROP_BOOTLOADER_REBOOT_MODE;
            tx_command->header.length += sizeof(cpc_system_reboot_mode_t);
            break;
        }

        default: {
            // The requested bootloader mode doesn't exist. Reply with a
            // PROP_LAST_STATUS::STATUS_INVALID_ARGUMENT instead
            tx_prop_cmd->property_id = PROP_LAST_STATUS;
            *((cpc_system_status_t*)(tx_prop_cmd->payload)) =
                STATUS_INVALID_ARGUMENT;
            tx_command->header.length += sizeof(cpc_system_status_t);
            break;
        }
    }
}

/***************************************************************************/ /**
                                                                               * Handler for when the primary sets a read-only property
                                                                               ******************************************************************************/
static void on_property_set_read_only(cpc_system_cmd_t* tx_command) {
    cpc_system_property_cmd_t* tx_property_cmd =
        (cpc_system_property_cmd_t*)tx_command->payload;

    tx_property_cmd->property_id = PROP_LAST_STATUS;
    *((cpc_system_status_t*)(tx_property_cmd->payload)) =
        STATUS_INVALID_COMMAND_FOR_PROP;
    tx_command->header.length = sizeof(cpc_property_id_t)
                                + sizeof(cpc_system_status_t);
}

static void on_property_set_rf_cert_band(cpc_system_cmd_t* tx_command,
                                         cpc_system_cmd_t* rx_command) {
    cpc_system_property_cmd_t* tx_prop_cmd =
        (cpc_system_property_cmd_t*)tx_command->payload;
    cpc_system_property_cmd_t* rx_prop_cmd =
        (cpc_system_property_cmd_t*)rx_command->payload;
    cpc_system_rf_band_t* band = (cpc_system_rf_band_t*)rx_prop_cmd->payload;

    tx_command->header.length = sizeof(cpc_property_id_t);
    tx_prop_cmd->property_id = PROP_RF_CERT_BAND;
    cpc_set_secondary_rf_cert_band(*band);
}

/*******************************************************************************
 ***************************  COMMAND HANDLERS  ********************************
 ******************************************************************************/

/***************************************************************************/ /**
                                                                               * Handle no-op from PRIMARY:
                                                                               *   This functions is called when a no-op command is received from the PRIMARY.
                                                                               *   The RCP simply sends a no-op back so that the primary can assert the success
                                                                               *   of the operation.
                                                                               ******************************************************************************/
static void on_noop(cpc_system_cmd_t* noop, uint32_t* reply_data_lenght) {
    noop->header.command_id = CMD_SYSTEM_NOOP;
    noop->header.length = 0;
    *reply_data_lenght = sizeof(cpc_system_cmd_header_t);
}

/***************************************************************************/ /**
                                                                               * Handle reset request
                                                                               ******************************************************************************/
__weak cpc_system_status_t
cpc_secondary_on_reset_request(cpc_system_reboot_mode_t reboot_mode) {
    switch (reboot_mode) {
        case REBOOT_APPLICATION: {
            return STATUS_OK;
        } // We can always reboot into application
#if (defined(CATALOG_GECKO_BOOTLOADER_INTERFACE_PRESENT))
        case REBOOT_BOOTLOADER: {
            BootloaderInformation_t btl_info;
            bootloader_getInfo(&btl_info);

            // In case of bootloader reboot, check that a bootloader is present
            return (btl_info.type == NO_BOOTLOADER) ? STATUS_FAILURE
                                                    : STATUS_OK;
        }
#endif
        default: {
            configASSERT(false);
            break;
        }
    }
    return STATUS_FAILURE;
}

/***************************************************************************/ /**
                                                                               * Handle reset from PRIMARY:
                                                                               *   This functions is called when a reset command is received from the PRIMARY.
                                                                               *   The secondary will emit back a reset command with a status indicating
                                                                               *   wheter or not it can reset in the desired mode dictated by 'prop_bootloader
                                                                               *   _reboot_mode'
                                                                               ******************************************************************************/
static void on_reset(cpc_system_cmd_t* reset, uint32_t* reply_data_lenght,
                     void** on_write_complete_arg) {
    cpc_system_status_t* reset_status;

    reset_status = (cpc_system_status_t*)(reset->payload);
    reset->header.command_id = CMD_SYSTEM_RESET;

    reset->header.length = sizeof(cpc_system_status_t);
    *reply_data_lenght = sizeof(cpc_system_cmd_header_t) + reset->header.length;

    *reset_status = cpc_secondary_on_reset_request(prop_bootloader_reboot_mode);

    // Set on_write_complete argument to 0xDEADBEEF. This will
    // indicate to the on_write_complete callback to reset the device.
    // We need to postpone the reset to after the
    // write completion.
    *on_write_complete_arg = (*reset_status == STATUS_OK) ? (void*)0xDEADBEEF
                                                          : 0;
}

/***************************************************************************/ /**
                                                                               * Handle property-get from PRIMARY:
                                                                               *   This functions is called when a property-get command is received from the
                                                                               *   PRIMARY. Causes the SECONDARY to emit a "CMD_PROP_VALUE_IS " command for the
                                                                               *   given property identifier.
                                                                               ******************************************************************************/
static void on_property_get(cpc_system_cmd_t* rx_command,
                            cpc_system_cmd_t* reply,
                            uint32_t* reply_data_lenght) {
    cpc_system_property_cmd_t* rx_property_cmd =
        (cpc_system_property_cmd_t*)rx_command->payload;

    // Reply to a PROPERTY-GET with a PROPERTY-IS
    reply->header.command_id = CMD_SYSTEM_PROP_VALUE_IS;

    // Populate the reply command buffer according to the property_id
    switch (rx_property_cmd->property_id) {
        case PROP_LAST_STATUS: {
            on_property_get_last_status(reply);
            break;
        }
        case PROP_PROTOCOL_VERSION: {
            on_property_get_protocol_version(reply);
            break;
        }
        case PROP_SECONDARY_CPC_VERSION: {
            on_property_get_secondary_cpc_version(reply);
            break;
        }
        case PROP_SECONDARY_APP_VERSION: {
            on_property_get_secondary_app_version(reply);
            break;
        }
        case PROP_CAPABILITIES: {
            on_property_get_capabilities(reply);
            break;
        }
        case PROP_RX_CAPABILITY: {
            on_property_get_rx_capabilities(reply);
            break;
        }
        case PROP_FC_VALIDATION_VALUE: {
            on_property_get_fc_validation_value(reply);
            break;
        }
        case PROP_BUS_SPEED_VALUE: {
            on_property_get_bus_speed_value(reply);
            break;
        }
        case PROP_BOOTLOADER_INFO: {
            on_property_get_bootloader_info(reply);
            break;
        }
        case PROP_BOOTLOADER_REBOOT_MODE: {
            on_property_get_bootloader_reboot_mode(reply);
            break;
        }
        case PROP_SECURITY_STATE: {
            on_property_get_security_state(reply);
            break;
        }
        case PROP_ENDPOINT_STATES: {
            on_property_get_endpoint_states(reply);
            break;
        }
#if (CPC_DEBUG_CORE_EVENT_COUNTERS == 1)
        case PROP_CORE_DEBUG_COUNTERS: {
            on_property_get_core_debug_counters(reply);
            break;
        }
#endif
        default: {
            // Deal with endpoint state range
            if (rx_property_cmd->property_id >= EP_ID_TO_PROPERTY_STATE(0)
                && rx_property_cmd->property_id
                       <= EP_ID_TO_PROPERTY_STATE(255)) {
                uint8_t ep_id = PROPERTY_ID_TO_EP_ID(
                    rx_property_cmd->property_id);
                on_property_get_endpoint_state(reply, ep_id);
                break;
            }

            if (rx_property_cmd->property_id >= EP_ID_TO_PROPERTY_ENCRYPTION(0)
                && rx_property_cmd->property_id
                       <= EP_ID_TO_PROPERTY_ENCRYPTION(255)) {
                uint8_t ep_id = PROPERTY_ID_TO_EP_ID(
                    rx_property_cmd->property_id);
                on_property_get_endpoint_encryption(reply, ep_id);
                break;
            }

            on_property_get_set_not_found(reply);
            break;
        }
    }

    *reply_data_lenght = sizeof(cpc_system_cmd_header_t) + reply->header.length;
}

/***************************************************************************/ /**
                                                                               * Handle property-set from PRIMARY:
                                                                               *   This functions is called when a property-set command is received from the
                                                                               *   PRIMARY. Causes the RCP to emit a "CMD_PROP_VALUE_IS " command for the given
                                                                               *   property identifier.
                                                                               ******************************************************************************/
static void on_property_set(cpc_system_cmd_t* rx_command,
                            cpc_system_cmd_t* reply,
                            uint32_t* reply_data_lenght) {
    cpc_system_property_cmd_t* rx_property_cmd =
        (cpc_system_property_cmd_t*)(rx_command->payload);

    // Reply to a PROPERTY-GET with a PROPERTY-IS
    reply->header.command_id = CMD_SYSTEM_PROP_VALUE_IS;

    // Populate the reply command buffer according to the property_id
    if (rx_property_cmd->property_id >= PROP_ENDPOINT_STATE_0
        && rx_property_cmd->property_id <= PROP_ENDPOINT_STATE_255) {
        on_property_set_state(
            PROPERTY_ID_TO_EP_ID(rx_property_cmd->property_id), reply,
            rx_command);
        *reply_data_lenght = sizeof(cpc_system_cmd_header_t)
                             + reply->header.length;
        return;
    }

    switch (rx_property_cmd->property_id) {
        case PROP_LAST_STATUS:
        case PROP_PROTOCOL_VERSION:
        case PROP_SECONDARY_CPC_VERSION:
        case PROP_SECONDARY_APP_VERSION:
        case PROP_CAPABILITIES:
        case PROP_BOOTLOADER_INFO:
        case PROP_SECURITY_STATE:
        case PROP_ENDPOINT_STATES: {
            on_property_set_read_only(reply);
            break;
        } // All those properties fall through read-only handling
        case PROP_BOOTLOADER_REBOOT_MODE: {
            on_property_set_bootloader_reboot_mode(reply, rx_command);
            break;
        }
        case PROP_UFRAME_PROCESSING: {
            on_property_set_uframe_processing(reply, rx_command);
            break;
        }
        case PROP_RF_CERT_BAND: {
            on_property_set_rf_cert_band(reply, rx_command);
            break;
        }
        default: {
            on_property_get_set_not_found(reply);
            break;
        }
    }
    *reply_data_lenght = sizeof(cpc_system_cmd_header_t) + reply->header.length;
    return;
}

static inline void cpc_on_poll_unnumbered_handle(cpc_system_cmd_t* rx_command,
                                                 cpc_system_cmd_t* reply_data,
                                                 uint32_t* reply_data_lenght,
                                                 void** on_write_complete_arg) {
    cpc_system_property_cmd_t* rx_property_cmd =
        (cpc_system_property_cmd_t*)rx_command->payload;
    switch (rx_command->header.command_id) {
        case CMD_SYSTEM_RESET: {
            on_reset(reply_data, reply_data_lenght, on_write_complete_arg);
            break;
        }

        case CMD_SYSTEM_PROP_VALUE_GET: {
            if (rx_property_cmd->property_id == PROP_RX_CAPABILITY
                || rx_property_cmd->property_id == PROP_CAPABILITIES
                || rx_property_cmd->property_id == PROP_BUS_SPEED_VALUE
                || rx_property_cmd->property_id == PROP_PROTOCOL_VERSION
                || rx_property_cmd->property_id == PROP_BOOTLOADER_INFO
                || rx_property_cmd->property_id == PROP_SECONDARY_CPC_VERSION
                || rx_property_cmd->property_id == PROP_SECONDARY_APP_VERSION)
                on_property_get(rx_command, reply_data, reply_data_lenght);
            break;
        }

        case CMD_SYSTEM_PROP_VALUE_SET: {
            if (rx_property_cmd->property_id == PROP_BOOTLOADER_REBOOT_MODE
                || rx_property_cmd->property_id == PROP_RF_CERT_BAND)
                on_property_set(rx_command, reply_data, reply_data_lenght);
            break;
        }

        default: CPC_DROP(); // Drop packet
    }
}

static inline void cpc_on_poll_data_handle(cpc_system_cmd_t* rx_command,
                                           cpc_system_cmd_t* reply_data,
                                           uint32_t* reply_data_lenght) {
    switch (rx_command->header.command_id) {
        case CMD_SYSTEM_NOOP: return on_noop(reply_data, reply_data_lenght);
        case CMD_SYSTEM_PROP_VALUE_GET:
            return on_property_get(rx_command, reply_data, reply_data_lenght);
        case CMD_SYSTEM_PROP_VALUE_SET:
            return on_property_set(rx_command, reply_data, reply_data_lenght);
        default: CPC_DROP(); // Drop packet, Command not supported
    }
}

/***************************************************************************/ /**
                                                                               * Called by CPC core when uframe/poll is received
                                                                               ******************************************************************************/
static void on_poll(uint8_t endpoint_id, void* arg, void* poll_data,
                    uint32_t poll_data_length, void** reply_data,
                    uint32_t* reply_data_lenght, void** on_write_complete_arg) {
    uint32_t frame_type = (uint32_t)arg;
    cpc_system_cmd_t* rx_command = (cpc_system_cmd_t*)poll_data;
    cpc_system_cmd_t* tx_command;
    *reply_data = NULL;
    *reply_data_lenght = 0;

    CHECK_DROP(frame_type != CPC_HDLC_FRAME_TYPE_UNNUMBERED
               && frame_type != CPC_HDLC_FRAME_TYPE_DATA);
    CHECK_DROP(endpoint_id != CPC_ENDPOINT_SYSTEM);
    // Make sure the length of the payload from the command matches the returned length.
    CHECK_DROP(rx_command->header.length
               != (poll_data_length - sizeof(cpc_system_cmd_header_t)));

    *on_write_complete_arg = NULL;

    // Allocate command buffer. Freed on acknowledgment. (On write completion callback)
    // If no more memory, leave the reply data to NULL and let the upper layer retry later
    if (cpc_get_system_command_buffer(&tx_command) != STATUS_OK)
        return;

    *reply_data = tx_command;

    // Assign the sequence number of the request to the reply so the primary can
    // route it back to the right request.
    tx_command->header.seq = rx_command->header.seq;

    // Only the reset can be a u-frame (non encrypted)
    switch (frame_type) {
        case CPC_HDLC_FRAME_TYPE_UNNUMBERED:
            return cpc_on_poll_unnumbered_handle(rx_command, *reply_data,
                                                 reply_data_lenght,
                                                 on_write_complete_arg);
        case CPC_HDLC_FRAME_TYPE_DATA:
            return cpc_on_poll_data_handle(rx_command, *reply_data,
                                           reply_data_lenght);
        default: CPC_DROP(); // Drop packet
    }
}

/***************************************************************************/ /**
                                                                               * Function called when enter_irq_timer expires.
                                                                               ******************************************************************************/
static void enter_irq_timer_callback(cpc_timer_handle_t* handle, void* data) {
    (void)handle;

    uint32_t now_ms = cpc_timer_tick_to_ms(cpc_timer_get_tick_count());
    uint32_t end_ms = *(uint32_t*)data;
    while ((cpc_timer_tick_to_ms(cpc_timer_get_tick_count()) - now_ms)
           <= end_ms) {}
}
