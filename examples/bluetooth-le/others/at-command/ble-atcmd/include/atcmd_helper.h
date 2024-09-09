/**************************************************************************//**
* @file       helper.h
* @brief
*
*****************************************************************************/

#ifndef _HELPER_H_
#define _HELPER_H_

#include "stdbool.h"
#include "ble_app.h"
#include "ble_host_ref.h"

#define PRINTERR(status) printf("ERROR Happened! %s:%d status:%d\n",__FILE__,__LINE__,status);
#define PRINTERRR(reason) printf("ERROR Happened! %s:%d Reason:%s\n",__FILE__,__LINE__,reason);

#define CHECK(status)                                          \
    if(status != BLE_ERR_OK)                           \
    {                                                          \
        printf("ERROR Happened! %s:%d status:%d \n",__FILE__,__LINE__,status); \
        return status;                                         \
    }

#define CHECKNR(status)                                                        \
    if(status != BLE_ERR_OK)                                                   \
    {                                                                          \
        printf("ERROR Happened! %s:%d status:%d \n",__FILE__,__LINE__,status); \
    }

#define CHECK_BOOL(check_bool)                                                    \
    if(!check_bool)                                                               \
    {                                                                             \
        /*printf("ERROR Happened! %s:%d check:%d \n",__FILE__,__LINE__,check_bool);*/ \
    }

#define PRINT_IF_ELSE(cmp,print_true,print_false) \
    if (cmp)                                      \
    {                                             \
        printf(print_true);                       \
    }                                             \
    else                                          \
    {                                             \
        printf(print_false);                      \
    }

#define CHECK_HOST_ID(host_id)                  \
    if (host_id >= MAX_CONN_NO_APP)             \
    {                                           \
        return BLE_ERR_INVALID_HOST_ID;         \
    }

bool jump_to_main(void);
bool jump_to_main_isr(void);

#endif // _HELPER_H_
