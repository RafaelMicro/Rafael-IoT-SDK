#ifndef __MESH_APP_H__
#define __MESH_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>

/* Mesh model*/
#include "mmdl_opcodes.h"
#include "mmdl_common.h"
#include "mmdl_defs.h"

/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
typedef enum
{
    APP_BLE_MESH_EVT = 0,
    APP_BUTTON_EVT,
    APP_QUEUE_UART_MSG_EVT,
} app_queue_evt_t;

typedef struct
{
    mesh_tlv_t *pt_tlv;
    uint32_t event;
    uint32_t data;
} app_queue_t;

/**************************************************************************************************
 *    PUBLIC FUNCTIONS
 *************************************************************************************************/

/** @brief BLE initialization.
 * @details Initial BLE stack and application task.
 *
 * @return none.
 */
void app_init(void);


extern xQueueHandle app_msg_q;

#ifdef __cplusplus
};
#endif

#endif /* __MESH_APP_H__*/
