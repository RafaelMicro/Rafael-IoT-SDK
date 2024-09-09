#ifndef __APP_SYSLOG_H__
#define __APP_SYSLOG_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#if defined(CONFIG_BUILD_COMPONENT_SYSLOG)
#include "syslog.h"
#endif

//=============================================================================
//                Public ENUM
//=============================================================================
typedef enum APP_SYSLOG_TYPE {
    APP_SYSLOG_CPC = 0,
    APP_SYSLOG_UART = 1,
    APP_SYSLOG_UPGRADE = 2,
    APP_SYSLOG_ZIGBEE = 3,
    APP_SYSLOG_HCI = 4,
    APP_SYSLOG_MAX,
} app_syslog_type_t;

typedef enum APP_SYSLOG_SUBTYPE_UART {
    APP_SYSLOG_UART_INIT = 0,
    APP_SYSLOG_UART_SEND = 1,
    APP_SYSLOG_UART_RECV = 2,
    APP_SYSLOG_UART_MAX,
} app_syslog_module_t;

#if defined(CONFIG_BUILD_COMPONENT_SYSLOG)
#define APP_SYSLOG_INSERT(type, sub_type, msg1, msg2)                          \
    syslog_insert(SYSLOG_MODULE_APPLICATION, type, sub_type, msg1, msg2);
void app_syslog_init(void);
#else
#define APP_SYSLOG_INSERT(type, sub_type, msg1, msg2)
#endif

#ifdef __cplusplus
};
#endif
#endif // __APP_SYSLOG_H__