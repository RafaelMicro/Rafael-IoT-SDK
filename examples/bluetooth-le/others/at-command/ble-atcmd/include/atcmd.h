#ifndef _AT_CMD_H_
#define _AT_CMD_H_

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "ble_event.h"
#include "atcmd_queue.h"
#include "atcmd_ble_param.h"

#define MAX_CMD_STR_LEN 20        //cmd

typedef struct atcmd_s
{
    atcmd_ble_param_t ble_param;
    atcmd_queue_t at_queue;
    atcmd_item_t running_at_item;
} atcmd_t;

void atcmd_init(atcmd_t *this);
ble_err_t atcmd_ble_param_init(atcmd_t *this);
void atcmd_main_handle(atcmd_t *this);
void atcmd_uart_handle(atcmd_t *this, uint8_t *data, uint8_t length);




#endif //_AT_CMD_H_
