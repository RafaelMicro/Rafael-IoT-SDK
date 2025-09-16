/**
 * @file zigbee_gateway_cmd_handle.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-03-31
 *
 * @copyright Copyright (c) 2022
 *
 */

//=============================================================================
//                Include
//=============================================================================
#include "mcu.h"
#include <stdio.h>
#include <string.h>

/* Utility Library APIs */

#include "FreeRTOS.h"
#include "mmdl_client.h"
#include "cfgmdl_client.h"
#include "uart_handler.h"

#include "ble_mesh_gateway.h"
#include "ble_mesh_lib_api.h"

//=============================================================================
//                Private Definitions of const value
//=============================================================================

//=============================================================================
//                Private ENUM
//=============================================================================

typedef enum
{
    MESH_NWK_SUCCESS,
    MESH_NWK_INVALID_PARAMETER,
    MESH_NWK_INVALID_ROLE,
    MESH_NWK_INVALID_STATE,
    MESH_NWK_EMPTY,
} mesh_nwk_status_code;
//=============================================================================
//                Private Struct
//=============================================================================
typedef struct __attribute__((packed))
{
    uint8_t header[4];
    uint8_t len;
    uint8_t parameter[];
}
gateway_cmd_hdr;

typedef struct __attribute__((packed))
{
    uint16_t address;
    uint16_t appkey_index;
    uint8_t parameter[];  /*first 1 to 4 bytes is the opcode field*/
}
app_service_cmd_pd;

typedef struct __attribute__((packed))
{
    uint16_t address;
    uint8_t parameter[];
}
device_and_nwk_service_cmd_pd;

typedef struct __attribute__((packed))
{
    uint32_t command_id;
    uint8_t parameter[];
}
gateway_cmd_pd;

typedef struct __attribute__((packed))
{
    uint8_t cs;
}
gateway_cmd_end;


//=============================================================================
//                Private Function Declaration
//=============================================================================
//=============================================================================
//                Private Global Variables
//=============================================================================

uint8_t unprov_device_scan = false;
//=============================================================================
//                Functions
//=============================================================================
static uint8_t _gateway_checksum_calc(uint8_t *pBuf, uint8_t len)
{
    uint8_t cs = 0;

    for (int i = 0; i < len; i++)
    {
        cs += pBuf[i];
    }
    return (~cs);
}

void ble_mesh_gateway_cmd_send(uint32_t cmd_id, uint16_t addr, uint16_t key_index, uint32_t opcode, uint8_t *p_param, uint32_t len)
{
    uint8_t *p_gw_cmd_pkt, idx = 0, i;
    uint32_t pkt_len;
    uint32_t opcode_size = 0, sub_hdr_len = 0;

    do
    {
        if ((opcode & 0xc0) == 0xc0)
        {
            opcode_size = 3;
        }
        else if ((opcode & 0x80) == 0x80)
        {
            opcode_size = 2;
        }
        else
        {
            opcode_size = 1;
        }

        if (cmd_id == MESH_NWK_SVC_CMD)
        {
            sub_hdr_len = 0;
        }
        else if (cmd_id == DEVICE_CONFIGURATION_SVC_CMD)
        {
            sub_hdr_len = sizeof(device_and_nwk_service_cmd_pd);
        }
        else if (cmd_id == DEVICE_APPLICATION_SVC_CMD)
        {
            sub_hdr_len = sizeof(app_service_cmd_pd);
        }
        else
        {
            break;
        }

        pkt_len = sizeof(gateway_cmd_hdr) + sizeof(gateway_cmd_pd) + sub_hdr_len + len + opcode_size + sizeof(gateway_cmd_end);

        p_gw_cmd_pkt = pvPortMalloc(pkt_len);

        if (p_gw_cmd_pkt == NULL)
        {
            break;
        }

        ((gateway_cmd_hdr *)(p_gw_cmd_pkt))->header[0] = 0xFF;
        ((gateway_cmd_hdr *)(p_gw_cmd_pkt))->header[1] = 0xFC;
        ((gateway_cmd_hdr *)(p_gw_cmd_pkt))->header[2] = 0xFC;
        ((gateway_cmd_hdr *)(p_gw_cmd_pkt))->header[3] = 0xFF;
        ((gateway_cmd_hdr *)(p_gw_cmd_pkt))->len = sizeof(gateway_cmd_pd) + sub_hdr_len + len + opcode_size;


        idx += sizeof(gateway_cmd_hdr);

        ((gateway_cmd_pd *)(&p_gw_cmd_pkt[idx]))->command_id = cmd_id;

        if (cmd_id == MESH_NWK_SVC_CMD)
        {
            memcpy(((gateway_cmd_pd *)(&p_gw_cmd_pkt[idx]))->parameter, (uint8_t *)&opcode, opcode_size);
            memcpy(((gateway_cmd_pd *)(&p_gw_cmd_pkt[idx]))->parameter + opcode_size, p_param, len);

            idx += sizeof(gateway_cmd_pd);
            idx += (len + opcode_size);
        }
        else if (cmd_id == DEVICE_CONFIGURATION_SVC_CMD)
        {
            idx += sizeof(gateway_cmd_pd);

            ((device_and_nwk_service_cmd_pd *)(&p_gw_cmd_pkt[idx]))->address = addr;
            memcpy(((device_and_nwk_service_cmd_pd *)(&p_gw_cmd_pkt[idx]))->parameter, (uint8_t *)&opcode, opcode_size);
            memcpy(((device_and_nwk_service_cmd_pd *)(&p_gw_cmd_pkt[idx]))->parameter + opcode_size, p_param, len);

            idx += (sizeof(device_and_nwk_service_cmd_pd) + len + opcode_size);

        }
        else if (cmd_id == DEVICE_APPLICATION_SVC_CMD)
        {
            idx += sizeof(gateway_cmd_pd);
            ((app_service_cmd_pd *)(&p_gw_cmd_pkt[idx]))->address = addr;
            ((app_service_cmd_pd *)(&p_gw_cmd_pkt[idx]))->appkey_index = key_index;
            memcpy(((app_service_cmd_pd *)(&p_gw_cmd_pkt[idx]))->parameter, (uint8_t *)&opcode, opcode_size);
            memcpy(((app_service_cmd_pd *)(&p_gw_cmd_pkt[idx]))->parameter + opcode_size, p_param, len);

            idx += (sizeof(app_service_cmd_pd) + len + opcode_size);
        }

        ((gateway_cmd_end *)(&p_gw_cmd_pkt[idx]))->cs = _gateway_checksum_calc((uint8_t *) & (((gateway_cmd_hdr *)(p_gw_cmd_pkt))->len),
                ((gateway_cmd_hdr *)(p_gw_cmd_pkt))->len + 1);

        uart_handler_send(p_gw_cmd_pkt, pkt_len);

#if (!SUPPORT_DEBUG_CONSOLE_CLI)
        printf("------------------------      GW >>>> ------------------------\n");
        for (i = 0; i < pkt_len; i++)
        {
            printf("%02x ", p_gw_cmd_pkt[i]);
        }
        printf("\n");
#endif
    } while (0);

}

int8_t mesh_nwk_cmd_unprov_device_scan_get(uint8_t *p_parm, uint8_t parm_len)
{
    ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                              MESH_NWK_OPCODE_UNPROV_DEVICE_SCAN_STATUS, &unprov_device_scan, sizeof(unprov_device_scan));
    return 0;
}

int8_t mesh_nwk_cmd_unprov_device_scan_set(uint8_t *p_parm, uint8_t parm_len)
{
    if (p_parm[0] == 1)
    {
        ble_mesh_find_unprov_device_start();
        unprov_device_scan = 1;
    }
    else
    {
        ble_mesh_find_unprov_device_stop();
        unprov_device_scan = 0;
    }
    ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                              MESH_NWK_OPCODE_UNPROV_DEVICE_SCAN_STATUS, &unprov_device_scan, sizeof(unprov_device_scan));

    return MESH_NWK_SUCCESS;
}

int8_t mesh_nwk_cmd_prov_device(uint8_t *p_parm, uint8_t parm_len)
{
    uint16_t primary_addr;
    uint8_t i;

    if (parm_len < 16)
    {
        printf("invalid command parameter\n");
        return MESH_NWK_INVALID_PARAMETER;
    }

    primary_addr = (p_parm[0] | (p_parm[1] << 8));
    ble_mesh_select_unprovisioned_device(primary_addr, p_parm + 2);
    printf("Start provision device \n");
    for (i = 0; i < 16; i++)
    {
        printf("%02x ", p_parm[i]);
    }
    printf("\n");
    return MESH_NWK_SUCCESS;
}

int8_t mesh_nwk_cmd_nwk_key_get(uint8_t *p_parm, uint8_t parm_len)
{
    mesh_nwk_cmd_nwk_key_status staus_msg;

    if (pib_local_nwk_key_get(staus_msg.nwk_key) == true)
    {
        staus_msg.status = MESH_NWK_SUCCESS;
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_NWK_KEY_STAUS, (uint8_t *)&staus_msg, sizeof(staus_msg));
    }
    else
    {
        staus_msg.status = MESH_NWK_EMPTY;
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_NWK_KEY_STAUS, (uint8_t *)&staus_msg, sizeof(uint8_t));
    }
    return 0;
}


int8_t mesh_nwk_cmd_nwk_key_set(uint8_t *p_parm, uint8_t parm_len)
{
    mesh_nwk_cmd_nwk_key_status status_msg;

    if (parm_len != 16)
    {
        status_msg.status = MESH_NWK_INVALID_PARAMETER;
    }
    else
    {
        if (pib_local_nwk_key_set(p_parm) == false)
        {
            status_msg.status = MESH_NWK_INVALID_ROLE;
        }
        else
        {
            if (pib_local_nwk_key_get(status_msg.nwk_key) == true)
            {
                status_msg.status = MESH_NWK_SUCCESS;
            }
            else
            {
                status_msg.status = MESH_NWK_INVALID_STATE;
            }
        }
    }
    if (status_msg.status == MESH_NWK_SUCCESS)
    {
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_NWK_KEY_STAUS, (uint8_t *)&status_msg, sizeof(status_msg));
    }
    else
    {
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_NWK_KEY_STAUS, (uint8_t *)&status_msg, sizeof(uint8_t));

    }
    return 0;
}

int8_t mesh_nwk_cmd_app_key_get(uint8_t *p_parm, uint8_t parm_len)
{
    mesh_nwk_cmd_app_key_status status_msg;

    if (pib_local_app_key_get_by_idx(p_parm[0], status_msg.app_key) == true)
    {
        status_msg.status = MESH_NWK_SUCCESS;
        status_msg.app_key_idx = p_parm[0];
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_APP_KEY_STAUS, (uint8_t *)&status_msg, sizeof(status_msg));
    }
    else
    {
        status_msg.status = MESH_NWK_EMPTY;
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_APP_KEY_STAUS, (uint8_t *)&status_msg, sizeof(uint8_t));
    }
    return 0;
}


int8_t mesh_nwk_cmd_app_key_set(uint8_t *p_parm, uint8_t parm_len)
{
    mesh_nwk_cmd_app_key_status status_msg;

    if (parm_len != 17)
    {
        status_msg.status = MESH_NWK_INVALID_PARAMETER;
    }
    else
    {
        if (pib_local_app_key_set(p_parm[0], &p_parm[1]) == false)
        {
            status_msg.status = MESH_NWK_INVALID_PARAMETER;
        }
        else
        {
            if (pib_local_app_key_get_by_idx(p_parm[0], status_msg.app_key) == true)
            {
                status_msg.app_key_idx = p_parm[0];
                status_msg.status = MESH_NWK_SUCCESS;
                //ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                          //MESH_NWK_OPCODE_APP_KEY_STAUS, (uint8_t *)&status_msg, sizeof(status_msg));
            }
            else
            {
                status_msg.status = MESH_NWK_INVALID_STATE;
            }
        }
    }

    if (status_msg.status == MESH_NWK_SUCCESS)
    {
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_APP_KEY_STAUS, (uint8_t *)&status_msg, sizeof(status_msg));
    }
    else
    {
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_APP_KEY_STAUS, (uint8_t *)&status_msg, sizeof(uint8_t));

    }

    return 0;
}

int8_t mesh_nwk_cmd_dev_key_get(uint8_t *p_parm, uint8_t parm_len)
{
    mesh_nwk_cmd_dev_key_status staus_msg;

    if (pib_device_key_get(staus_msg.dev_key) == true)
    {
        staus_msg.status = MESH_NWK_SUCCESS;
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_DEV_KEY_STAUS, (uint8_t *)&staus_msg, sizeof(staus_msg));
    }
    else
    {
        staus_msg.status = MESH_NWK_EMPTY;
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_DEV_KEY_STAUS, (uint8_t *)&staus_msg, sizeof(uint8_t));
    }
    return 0;
}

int8_t mesh_nwk_cmd_dev_key_set(uint8_t *p_parm, uint8_t parm_len)
{
    mesh_nwk_cmd_dev_key_status status_msg;

    if (parm_len != 16)
    {
        status_msg.status = MESH_NWK_INVALID_PARAMETER;
    }
    else
    {
        if (pib_device_key_set(p_parm) == false)
        {
            status_msg.status = MESH_NWK_INVALID_ROLE;
        }
        else
        {
            if (pib_device_key_get(status_msg.dev_key) == true)
            {
                status_msg.status = MESH_NWK_SUCCESS;
            }
            else
            {
                status_msg.status = MESH_NWK_INVALID_STATE;
            }
        }
    }
    if (status_msg.status == MESH_NWK_SUCCESS)
    {
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_DEV_KEY_STAUS, (uint8_t *)&status_msg, sizeof(status_msg));
    }
    else
    {
        ble_mesh_gateway_cmd_send(MESH_NWK_SVC_CMD, 0, 0,
                                  MESH_NWK_OPCODE_DEV_KEY_STAUS, (uint8_t *)&status_msg, sizeof(uint8_t));

    }
    return 0;
}


static int8_t mesh_nwk_cmd_null(uint8_t *p_parm, uint8_t parm_len)
{
    return 0;
}


int8_t (* const prcss_mesh_nwk_cmd[])(uint8_t *p_data, uint8_t length) =
{
    mesh_nwk_cmd_unprov_device_scan_get, /* MESH_NWK_OPCODE_UNPROV_DEVICE_SCAN_STATUS_GET */
    mesh_nwk_cmd_unprov_device_scan_set, /* MESH_NWK_OPCODE_UNPROV_DEVICE_SCAN_STATUS_SET */
    mesh_nwk_cmd_null,                /* MESH_NWK_OPCODE_UNPROV_DEVICE_SCAN_STATUS */
    mesh_nwk_cmd_null,                /* MESH_NWK_OPCODE_UNPROV_DEVICE_LIST */
    mesh_nwk_cmd_prov_device,          /* MESH_NWK_OPCODE_DEVICE_PROV_SET */
    mesh_nwk_cmd_null,                /* MESH_NWK_OPCODE_DEVICE_PROV_STAUS */
    mesh_nwk_cmd_null,                /* 0x06 */
    mesh_nwk_cmd_null,                /* 0x07 */
    mesh_nwk_cmd_null,                /* 0x08 */
    mesh_nwk_cmd_null,                /* 0x09 */
    mesh_nwk_cmd_null,                /* 0x0a */
    mesh_nwk_cmd_null,                /* 0x0b */
    mesh_nwk_cmd_null,                /* 0x0c */
    mesh_nwk_cmd_null,                /* 0x0d */
    mesh_nwk_cmd_null,                /* 0x0e */
    mesh_nwk_cmd_null,                /* 0x0f */
    mesh_nwk_cmd_nwk_key_get,         /* 0x10 */
    mesh_nwk_cmd_nwk_key_set,         /* 0x11 */
    mesh_nwk_cmd_null,                /* 0x12 MESH_NWK_OPCODE_NWK_KEY_STAUS*/
    mesh_nwk_cmd_app_key_get,         /* 0x13 MESH_NWK_OPCODE_APP_KEY_GET*/
    mesh_nwk_cmd_app_key_set,         /* 0x14 MESH_NWK_OPCODE_APP_KEY_SET*/
    mesh_nwk_cmd_null,                /* 0x15 MESH_NWK_OPCODE_APP_KEY_STAUS*/
    mesh_nwk_cmd_dev_key_get,         /* 0x16 MESH_NWK_OPCODE_DEV_KEY_GET*/
    mesh_nwk_cmd_dev_key_set,         /* 0x17 MESH_NWK_OPCODE_DEV_KEY_SET*/
    mesh_nwk_cmd_null,                /* 0x18 MESH_NWK_OPCODE_DEV_KEY_STAUS*/
};
void ble_mesh_nwk_handle(uint8_t opcode, uint8_t *p_parm, uint8_t parm_len)
{
    prcss_mesh_nwk_cmd[opcode](p_parm, parm_len);
}


uint8_t ble_mesh_gateway_cmd_proc(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
    uint32_t param_len, i, len;
    uint8_t *p_buf;
    uint32_t opcode, opcode_size;

    p_buf = p_mesh_tlv->value;
    len = p_mesh_tlv->length;
    printf("------------------------ >>>> GW      ------------------------\n");
    for (i = 0; i < len; i++)
    {
        printf("%02x ", p_buf[i]);
    }
    printf("\n");

    //cmd_index = ((gateway_cmd_pd *)(&p_buf[5]))->command_id;
    gateway_cmd_pd *p_pd = (gateway_cmd_pd *)&p_buf[5];

    param_len = len - sizeof(gateway_cmd_hdr) - sizeof(gateway_cmd_pd) - sizeof(gateway_cmd_end);
    if (p_pd->command_id == DEVICE_CONFIGURATION_SVC_CMD)
    {
        device_and_nwk_service_cmd_pd *p_dev_nwk_cmd;
        p_dev_nwk_cmd = (device_and_nwk_service_cmd_pd *)p_pd->parameter;

        opcode = p_dev_nwk_cmd->parameter[0];

        if ((opcode & 0xc0) == 0xc0)
        {
            opcode_size = 3;
        }
        else if ((opcode & 0x80) == 0x80)
        {
            opcode_size = 2;
        }
        else
        {
            opcode_size = 1;
        }
        memcpy(&opcode, p_dev_nwk_cmd->parameter, opcode_size);

        param_len -= (sizeof(device_and_nwk_service_cmd_pd) + opcode_size);

        printf("BLE device and NWK command parameter(opcode 0x%04x):\n", opcode);
        for (i = 0; i < (param_len + opcode_size) ; i++)
        {
            printf("%02x ", p_dev_nwk_cmd->parameter[i]);
        }
        printf("\n");

        cfgmdl_client_send(opcode, p_dev_nwk_cmd->address, p_dev_nwk_cmd->parameter + opcode_size, param_len);
    }
    else if (p_pd->command_id == MESH_NWK_SVC_CMD)
    {
        opcode = p_pd->parameter[0];
        if ((opcode & 0xc0) == 0xc0)
        {
            opcode_size = 3;
        }
        else if ((opcode & 0x80) == 0x80)
        {
            opcode_size = 2;
        }
        else
        {
            opcode_size = 1;
        }
        memcpy(&opcode, p_pd->parameter, opcode_size);

        param_len -= opcode_size;

        ble_mesh_nwk_handle(opcode, p_pd->parameter + opcode_size, param_len);
    }
    else if (p_pd->command_id == DEVICE_APPLICATION_SVC_CMD)
    {
        app_service_cmd_pd *p_svc_cmd;
        p_svc_cmd = (app_service_cmd_pd *)p_pd->parameter;

        opcode = p_svc_cmd->parameter[0];

        if ((opcode & 0xc0) == 0xc0)
        {
            opcode_size = 3;
        }
        else if ((opcode & 0x80) == 0x80)
        {
            opcode_size = 2;
        }
        else
        {
            opcode_size = 1;
        }
        memcpy(&opcode, p_svc_cmd->parameter, opcode_size);


        param_len -= (sizeof(app_service_cmd_pd) + opcode_size);

        printf("Application service parameter(%d):\n", param_len);
        for (i = 0; i < (param_len + opcode_size) ; i++)
        {
            printf("%02x ", p_svc_cmd->parameter[i]);
        }
        printf("\n");

        mmdl_client_send(p_svc_cmd->address, pib_primary_address_get(),
                         opcode, p_svc_cmd->appkey_index, param_len, p_svc_cmd->parameter + opcode_size);
    }

    return true;

}

uart_handler_data_sts_t ble_mesh_gateway_cmd_parser(uint8_t *p_buf, uint16_t len, uint16_t *p_data_len, uint16_t *p_offset)
{
    //+-------------+-----------+---------------+------------+---------------+--------+
    //|  Header(4)  | Length(1) | Command ID(4) | Address(2) |  Parameter(N) | CS (1) |
    //+-------------+-----------+---------------+------------+---------------+--------+
    //| FF FC FC FF |           |               |            |               |        |
    //+-------------+-----------+---------------+------------+---------------+--------+
    uart_handler_data_sts_t t_return = UART_DATA_INVALID;

    uint16_t i = 0;
    uint16_t idx = 0;
    uint8_t cs = 0;

    uint8_t find = 0;

    gateway_cmd_hdr *hdr = NULL;
    gateway_cmd_end *end = NULL;
    do
    {
        /* find tag */
        if (len < 4)
        {
            break;
        }

        for (i = 0; i < len; i++)
        {
            if ((p_buf[i] == 0xFF) && (p_buf[i + 1] == 0xFC) &&
                    (p_buf[i + 2] == 0xFC) && (p_buf[i + 3] == 0xFF))
            {
                if (p_offset)
                {
                    idx = i;
                }
                find = 1;
                break;
            }
        }

        if (!find)
        {
            break;
        }

        if ((len - idx) < (sizeof(gateway_cmd_hdr) + sizeof(gateway_cmd_end)))
        {
            break;
        }

        hdr = (gateway_cmd_hdr *)(p_buf + idx);
        if (len < (idx + sizeof(gateway_cmd_hdr) + hdr->len +  sizeof(gateway_cmd_end)))
        {
            break;
        }

        end = (gateway_cmd_end *)&p_buf[idx + sizeof(gateway_cmd_hdr) + hdr->len];
        cs = _gateway_checksum_calc(&p_buf[idx + 4], (hdr->len + 1));
        if (cs != end->cs)
        {
            t_return = UART_DATA_CS_ERROR;
            printf(">> %s: checksum error:0x%x, correct:0x%x\n", __FUNCTION__, end->cs, cs);
            break;
        }
        else if (cs == end->cs)   /* show receved command data */
        {
            *p_data_len = sizeof(gateway_cmd_hdr) + hdr->len + sizeof(gateway_cmd_end);
            *p_offset = idx;
            t_return = UART_DATA_VALID_CRC_OK;
            break;
        }
        *p_data_len = sizeof(gateway_cmd_hdr) + hdr->len + sizeof(gateway_cmd_end);
        *p_offset = idx;
        t_return = UART_DATA_VALID;

    } while (0);

    return t_return;
}
