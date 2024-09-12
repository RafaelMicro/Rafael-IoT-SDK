#ifndef _AT_CMD_H_
#define _AT_CMD_H_

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "atcmd_queue.h"
#include "atcmd_ble_param.h"

#define MAX_CMD_STR_LEN 20        //cmd

typedef struct atcmd_s
{
    bool is_no_connect_mode;
    atcmd_ble_param_t ble_param;
    atcmd_queue_t at_queue;
    atcmd_item_t running_at_item;
} atcmd_t;

void atcmd_init(atcmd_t *this);
ble_err_t atcmd_ble_param_init(atcmd_t *this);
ble_err_t atcmd_profile_init(atcmd_t *this, ble_svcs_evt_gaps_handler_t gap_callback, ble_svcs_evt_atcmd_handler_t atcmd_callback);
void atcmd_main_handle(atcmd_t *this);
void atcmd_uart_handle(atcmd_t *this, uint8_t *data, uint8_t length);
void atcmd_event_handle(atcmd_t *this, ble_module_evt_t event, void *p_param);
void atcmd_gap_service_handle(atcmd_t *this, ble_evt_att_param_t *p_param);
void atcmd_atcmd_service_handle(atcmd_t *this, ble_evt_att_param_t *p_param);


#endif //_AT_CMD_H_
