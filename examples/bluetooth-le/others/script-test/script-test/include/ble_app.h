/**************************************************************************//**
* @file       user.h
* @brief
*
*****************************************************************************/

#ifndef _USER_H_
#define _USER_H_

#include "ble_api.h"
#include "stdbool.h"

typedef enum
{
    QUEUE_TYPE_APP_REQ,
    QUEUE_TYPE_OTHERS,
} app_queue_param_type;

typedef struct
{
    uint8_t   event;
    uint8_t   from_isr;
    app_queue_param_type  param_type;
    union
    {
        uint32_t  app_req;
        ble_tlv_t *pt_tlv;
    } param;
} app_queue_t;

// this function provide for cli_cmd_atplus.c
void uart_rx_data_handle(uint8_t *data, uint8_t length);

//This function provide for atcmd_helper.c
//Send request to make app_main_task receive
bool app_request_set(app_queue_param_type type, bool from_isr);


#endif // _USER_H_
