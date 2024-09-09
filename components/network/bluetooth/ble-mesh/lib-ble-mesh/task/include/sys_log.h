// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#ifndef __MESH_SYS_LOG_H__
#define __MESH_SYS_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
//=============================================================================
//                Include (Better to prevent)
//=============================================================================

//=============================================================================
//                Public Definitions of const value
//=============================================================================

//=============================================================================
//                Public ENUM
//=============================================================================
typedef enum SYSLOG_TYPE
{
    SYSLOG_LAYER_ALL = 0,
    SYSLOG_LAYER_BAERER = 1,
    SYSLOG_LAYER_MAX,
} syslog_layer_t;

//=============================================================================
//                Public Struct
//=============================================================================
typedef struct SYS_LOG_ENTRY
{
    uint32_t tick;
    uint8_t  layer;
    uint8_t  type;
    uint16_t sub_type;
    uint32_t msg1;
    uint32_t msg2;
} syslog_t;

typedef void (*print_fn)(syslog_t *log);

//=============================================================================
//                Public Function Declaration
//=============================================================================
void syslog_print_fn_register(syslog_layer_t t_layer, print_fn fn);
void syslog_insert(syslog_layer_t t_layer, uint8_t u8_type, uint16_t u16_sub_type, uint32_t msg1, uint32_t msg2);
void syslog_insert_from_isr(syslog_layer_t t_layer, uint8_t u8_type, uint16_t u16_sub_type, uint32_t msg1, uint32_t msg2);
void syslog_print(syslog_layer_t t_layer, uint8_t u8_type);
void syslog_clear(void);
uint32_t syslog_init(void);

#ifdef __cplusplus
};
#endif
#endif /* __MESH_SYS_LOG_H__ */
