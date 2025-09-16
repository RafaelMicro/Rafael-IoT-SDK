/**
 * Copyright (c) 2021 All Rights Reserved.
 */
/** @file ble_mesh_lib_api.c
 *
 * @author Rex
 * @version 0.1
 * @date 2021/10/19
 * @license
 * @description
 */

//=============================================================================
//                Include
//=============================================================================
#include <string.h>
/* OS API */
#include "FreeRTOS.h"

/* Mesh Library api */
#include "mesh_api.h"

/* Utility Library api */
#include "log.h"
#include "pib.h"

//=============================================================================
//                Public Global Variables Declaration
//=============================================================================


//=============================================================================
//                Private Definitions of const value
//=============================================================================
//=============================================================================
//                Private ENUM
//=============================================================================

//=============================================================================
//                Private Struct
//=============================================================================

//=============================================================================
//                Private Function Declaration
//=============================================================================

//=============================================================================
//                Private Global Variables
//=============================================================================

//=============================================================================
//                Functions
//=============================================================================
void ble_mesh_find_unprov_device_start(void)
{
    mesh_tlv_t   *pt_tlv;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_START_SCAN_UNPROV_DEVICE_REQ;
        pt_tlv->length = 0;

        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Start find unprovision device fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

void ble_mesh_find_unprov_device_stop(void)
{
    mesh_tlv_t   *pt_tlv;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_STOP_SCAN_UNPROV_DEVICE_REQ;
        pt_tlv->length = 0;

        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Stop find unprovision device fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

void ble_mesh_provision_start(uint8_t from_gatt)
{
    mesh_prov_start_req_t *p_start_req;
    mesh_tlv_t   *pt_tlv;
    uint8_t      uuid[16];
    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_prov_start_req_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_PROV_START_REQ;
        pt_tlv->length = sizeof(mesh_prov_start_req_t);
        p_start_req = (mesh_prov_start_req_t *)pt_tlv->value;
        pib_uuid_get(uuid);
        memcpy(p_start_req->uuid, uuid, 16);
        p_start_req->gatt_provision = from_gatt;

        p_start_req->beacon_interval = 1000;

        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Mesh provision fail\n");
            break;
        }
        else
        {
            printf("Mesh provision start\n");
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

void ble_mesh_provision_stop(void)
{
    mesh_tlv_t   *pt_tlv;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_PROV_STOP_REQ;
        pt_tlv->length = 0;

        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Stop mesh provision fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

void ble_mesh_proxy_enable(void)
{
    mesh_tlv_t   *pt_tlv;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_PROXY_ENABLE_REQ;
        pt_tlv->length = 0;
        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Enable proxy service fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

void ble_mesh_proxy_disable(void)
{
    mesh_tlv_t   *pt_tlv;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_PROXY_DISABLE_REQ;
        pt_tlv->length = 0;
        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Disable proxy service fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

void ble_mesh_select_unprovisioned_device(uint16_t primary_address, uint8_t *p_uuid)
{
    mesh_tlv_t   *pt_tlv;
    mesh_prov_device_req_t *p_unprov_device;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_prov_device_req_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_LINK_OPEN_UNPROV_DEVICE_REQ;
        pt_tlv->length = sizeof(mesh_prov_device_req_t);
        p_unprov_device = (mesh_prov_device_req_t *)pt_tlv->value;
        p_unprov_device->primary_address = primary_address;
        memcpy(p_unprov_device->uuid, p_uuid, sizeof(p_unprov_device->uuid));
        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Link open fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

bool ble_mesh_device_name_set(uint8_t len, uint8_t *p_device_name)
{
    mesh_tlv_t *pt_tlv = NULL;
    bool set_success = false;


    if (len <= 31)
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + len);

        if (pt_tlv != NULL)
        {
            pt_tlv->type = TYPE_MESH_DEVICE_NAME_SET_REQ;
            pt_tlv->length = len;
            memcpy(pt_tlv->value, p_device_name, len);
            if (mesh_evt_msg_sendto(pt_tlv) != 0)
            {
                printf("device name set fail\n");
            }
            else
            {
                set_success = true;
            }
        }
    }

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }

    return set_success;
}

void ble_trsps_connection_create(void)
{
    mesh_tlv_t   *pt_tlv;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_BLE_TRSPS_CONNECT_CREATE_REQ;
        pt_tlv->length = 0;
        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Enable ble trsps fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

void ble_trsps_connection_cancel(void)
{
    mesh_tlv_t   *pt_tlv;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_BLE_TRSPS_CONNECT_CANCEL_REQ;
        pt_tlv->length = 0;
        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Disable ble trsps fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

bool ble_trsps_data_set(uint16_t len, uint8_t *p_data)
{
    mesh_tlv_t   *pt_tlv;
    bool set_success = false;


    pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + len);

    if (pt_tlv != NULL)
    {
        pt_tlv->type = TYPE_MESH_BLE_TRSPS_DATA_SET_REQ;
        pt_tlv->length = len;
        memcpy(pt_tlv->value, p_data, len);
        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("BLE TRSPS data set fail\n");
        }
        else
        {
            set_success = true;
        }
    }

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }

    return set_success;
}

