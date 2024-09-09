// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __SYSLOG_H__
#define __SYSLOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

//=============================================================================
//                Public Definitions of const value
//=============================================================================

//=============================================================================
//                Public ENUM
//=============================================================================
typedef enum SYSLOG_TYPE {
    SYSLOG_MODULE_NONE = 0,
    SYSLOG_MODULE_HOSAL = 1,
    SYSLOG_MODULE_HCI = 2,
    SYSLOG_MODULE_15P4 = 3,
    SYSLOG_MODULE_ZIGBEE = 4,
    SYSLOG_MODULE_THREAD = 5,
    SYSLOG_MODULE_BLUETOOTH_LE = 6,
    SYSLOG_MODULE_APPLICATION,
    SYSLOG_MODULE_ALL,
} syslog_module_t;

//=============================================================================
//                Public Struct
//=============================================================================
typedef struct SYS_LOG_ENTRY {
    uint32_t tick;
    uint8_t module;
    uint8_t type;
    uint16_t sub_type;
    uint32_t msg1;
    uint32_t msg2;
} syslog_t;

typedef void (*print_fn)(syslog_t* log);

//=============================================================================
//                Public Function Declaration
//=============================================================================
void syslog_print_fn_register(syslog_module_t t_module, print_fn fn);
void syslog_insert(syslog_module_t t_module, uint8_t u8_type,
                   uint16_t u16_sub_type, uint32_t msg1, uint32_t msg2);
void syslog_insert_from_isr(syslog_module_t t_module, uint8_t u8_type,
                            uint16_t u16_sub_type, uint32_t msg1,
                            uint32_t msg2);
void syslog_print(syslog_module_t t_module, uint8_t u8_type);
void syslog_clear(void);
uint32_t syslog_init(void);

#ifdef __cplusplus
};
#endif
#endif /* __SYSLOG_H__ */
