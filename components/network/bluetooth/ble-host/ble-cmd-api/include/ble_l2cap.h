/** @file
 *
 * @brief BLE l2cap header file.
 *
 */

/**
 * @defgroup ble_l2cap L2CAP
 * @ingroup BLE_group
 * @{
 * @brief Define LE l2cap definitions, structures, and functions.
 * @}
 */

#ifndef __BLE_L2CAP_H__
#define __BLE_L2CAP_H__

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "ble_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/



/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
/**
 * @defgroup ble_l2cap_evt_type BLE L2CAP Event Type Definition
 * @{
 * @ingroup ble_l2cap
 * @details BLE module event type definition which is indicated the event from BLE stack.
 */
typedef uint8_t ble_l2cap_evt_t;
#define LE_L2CAP_EVT_CREDIT_BASED_CONNECT_REQ_RSP       0x01
#define LE_L2CAP_EVT_DISCONNECT                         0x02
#define LE_L2CAP_EVT_FLOW_CTRL_CREDIT_IND               0x03
#define LE_L2CAP_EVT_DATA_RECEIVED                      0x04


/** @brief BLE l2cap channel connect parameters.
 * @ingroup ble_l2cap
*/
typedef struct __attribute__((packed))
{
    uint8_t     host_id;
    uint16_t    spsm;
    uint16_t    mtu;
} ble_l2cap_chan_connect_t;


/** @brief BLE l2cap data send parameters.
 * @ingroup ble_l2cap
*/
typedef struct __attribute__((packed))
{
    uint8_t     host_id;
    uint16_t    dest_id;
    uint16_t    length;
    uint8_t     data[];
} ble_l2cap_data_send_t;


/** @brief BLE l2cap data event parameters.
 * @ingroup ble_l2cap
*/
typedef struct __attribute__((packed))
{
    uint8_t host_id;
    uint16_t psm;
    uint16_t length;
    uint8_t data[];
} ble_l2cap_data_param_t;


typedef struct __attribute__((packed))
{
    uint8_t  host_id;
    uint16_t dest_id;
    uint16_t mtu;
    uint16_t mps;
    uint16_t init_credits;
    uint16_t result;
} ble_l2cap_credit_based_conn_req_rsp_t;


typedef struct __attribute__((packed))
{
    uint8_t host_id;
    uint16_t cid;
    uint16_t credits;
} ble_l2cap_flow_ctrl_credit_ind_t;


/**
 * @brief BLE L2CAP Event Parameters.
 * @ingroup ble_l2cap
 */
typedef struct  __attribute__((packed))
{
    ble_l2cap_evt_t         event;    /**< BLE event @ref ble_l2cap_evt_type "ble_l2cap_evt_t". */
    uint16_t                length;
    union
    {
        ble_l2cap_data_param_t                  l2cap_data_param;
        ble_l2cap_credit_based_conn_req_rsp_t   l2cap_conn_req_rsp;
        ble_l2cap_flow_ctrl_credit_ind_t        l2cap_flow_ctrl_credit_ind;
    } evt_param;                                  /**< Event parameters. */
} ble_l2cap_evt_param_t;



/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/


/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/
ble_err_t ble_cmd_l2cap_chan_connect(ble_l2cap_chan_connect_t* p_param);
ble_err_t ble_cmd_l2cap_data_send(ble_l2cap_data_send_t* p_param);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BLE_L2CAP_H__*/
