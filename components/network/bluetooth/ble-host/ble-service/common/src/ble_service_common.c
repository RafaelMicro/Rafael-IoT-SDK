/************************************************************************
 *
 * File Name  : ble_service_common.c
 * Description: This file contains the common definitions and functions of BLE profile
 *
 *
 ************************************************************************/
#include <string.h>
#include "FreeRTOS.h"
#include "ble_att_gatt.h"
#include "ble_service_common.h"
#include "log.h"


/**************************************************************************
 * BLE Profile Based UUID Definitions
 **************************************************************************/

/** Invalid UUID.
*/
const uint16_t attr_uuid_invalid[] =
{
    0,
};


/** Primary Service Descriptors UUID.
 * @note 16-bits UUID
 * @note UUID: 2800
*/
const uint16_t attr_uuid_type_primary_service[] =
{
    GATT_DECL_PRIMARY_SERVICE,
};


/** Secondary Service Descriptors UUID.
 * @note 16-bits UUID
 * @note UUID: 2801
*/
const uint16_t attr_uuid_type_secondary_service[] =
{
    GATT_DECL_SECONDARY_SERVICE,
};


/** Included Service Descriptors UUID.
 * @note 16-bits UUID
 * @note UUID: 2802
*/
const uint16_t attr_uuid_type_include[] =
{
    GATT_DECL_INCLUDE,
};


/** Characteristic Descriptors UUID.
 * @note 16-bits UUID
 * @note UUID: 2803
*/
const uint16_t attr_uuid_type_characteristic[] =
{
    GATT_DECL_CHARACTERISTIC,
};


/** Characteristic Extended Properties Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2900
*/
const uint16_t attr_uuid_type_charc_extended_properties[] =
{
    GATT_DESC_CHARC_EXTENDED_PROPERTIES,
};


/** Characteristic User Description Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2901
*/
const uint16_t attr_uuid_type_charc_user_description[] =
{
    GATT_DESC_CHARC_USER_DESCRIPTION,
};


/** Client Characteristic Configuration Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2902
*/
const uint16_t attr_uuid_type_client_charc_configuration[] =
{
    GATT_DESC_CLIENT_CHARC_CONFIGURATION,
};


/** Server Characteristic Configuration Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2903
*/
const uint16_t attr_uuid_type_server_charc_configuration[] =
{
    GATT_DESC_SERVER_CHARC_CONFIGURATION,
};


/** Characteristic Presentation Format Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2904
*/
const uint16_t attr_uuid_type_charc_presentation_format[] =
{
    GATT_DESC_CHARC_PRESENTATION_FORMAT,
};


/** Characteristic Aggregate Format Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2905
*/
const uint16_t attr_uuid_type_charc_aggregate_format[] =
{
    GATT_DESC_CHARC_AGGREGATE_FORMAT,
};

/** Valid Range Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2906
*/
const uint16_t attr_uuid_type_valid_range[] =
{
    GATT_DESC_VALID_RANGE,
};


/** Characteristic Aggregate Format Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2907
*/
const uint16_t attr_uuid_type_external_report_reference[] =
{
    GATT_DESC_EXTERNAL_REPORT_REFERENCE,
};


/** Report Reference Descriptor UUID.
 * @note 16-bits UUID
 * @note UUID: 2908
*/
const uint16_t attr_uuid_type_report_reference[] =
{
    GATT_DESC_REPORT_REFERENCE,
};


/**************************************************************************
 * BLE Profile NULL Access Definition
 **************************************************************************/
void attr_null_access(ble_evt_att_param_t *p_param)
{
}

/**************************************************************************
 * BLE Profile NULL Definition
 **************************************************************************/
#define ATTR_TEMP_DEF     \
    (void *)0,            \
    (void *)0,            \
    0,                    \
    0,                    \
    0,                    \
    attr_null_access,     \



const ble_att_param_t ATT_NULL_INVALID =
{
    ATTR_TEMP_DEF
};


/**************************************************************************
 * BLE Profile Public Functions
 **************************************************************************/

/** Query Index from Service Registered Index Mapping Array
*/
ble_err_t ble_svcs_common_info_index_query(uint8_t host_id, ble_gatt_role_t role, uint8_t maxCount, ble_svcs_common_info_t *p_info, uint8_t *p_index)
{
    uint8_t i;
    for (i = 0; i < maxCount; i++)
    {
        if ((p_info[i].host_id == host_id) && ((p_info[i].role | role) != 0))
        {
            *p_index = i;
            return BLE_ERR_OK;
        }
    }

    return BLE_ERR_INVALID_PARAMETER;
}



/** BLE Service Common Setting Initialization
*/
ble_err_t ble_svcs_common_init(uint8_t host_id, ble_gatt_role_t role, uint8_t maxCount, ble_svcs_common_info_t *p_info, uint8_t *p_index, uint8_t *p_count)
{
    // check host id
    if ( (host_id == BLE_HOSTID_RESERVED) ||
            (host_id >= BLE_SUPPORT_NUM_CONN_MAX))
    {
        return BLE_ERR_INVALID_HOST_ID;
    }

    if ((p_count != 0) && (ble_svcs_common_info_index_query(host_id, role, (*p_count), p_info, p_index) == BLE_ERR_OK))
    {
        // already exist
    }
    else
    {
        // check service supported maximum count
        if (*p_count >=  maxCount)
        {
            return BLE_ERR_CMD_NOT_SUPPORTED;
        }

        // new link
        *p_index = *p_count;

        // count++ for next link
        (*p_count)++;
    }

    p_info[*p_index].index   = *p_index;
    p_info[*p_index].role    = role;
    p_info[*p_index].host_id = host_id;

    return BLE_ERR_OK;
}


/** Get BLE Service handle mapping value
*/
ble_err_t ble_svcs_handles_mapping_get(ble_gatt_handle_table_param_t *p_param)
{
    ble_err_t status;
    ble_gatt_handle_table_param_t *p_ble_gatt_handle_table_param;

    status = BLE_ERR_OK;
    p_ble_gatt_handle_table_param = pvPortMalloc(sizeof(ble_gatt_handle_table_param_t));

    if (p_ble_gatt_handle_table_param != NULL)
    {
        p_ble_gatt_handle_table_param->host_id = p_param->host_id;
        p_ble_gatt_handle_table_param->gatt_role = p_param->gatt_role;
        p_ble_gatt_handle_table_param->p_element = p_param->p_element;
        p_ble_gatt_handle_table_param->p_handle_num_addr = p_param->p_handle_num_addr;

        status = ble_cmd_gatt_att_handle_mapping_get(p_ble_gatt_handle_table_param);
        if (status != BLE_ERR_OK) // send to BLE stack
        {
            log_info("<TYPE_BLE_GATT_ATT_HANDLE_MAPPING_GET> Send msg to BLE stack fail\n");
        }
        vPortFree(p_ble_gatt_handle_table_param);

    }
    else
    {
        log_info("<TYPE_BLE_GATT_ATT_HANDLE_MAPPING_GET> malloc fail\n");
        status = BLE_ERR_ALLOC_MEMORY_FAIL;
    }

    return status;
}


/** BLE Set Handle CCCD Write Request From the Client
 */
ble_err_t ble_svcs_handle_cccd_write_req(uint8_t *p_data, uint16_t length, uint16_t *p_cccd)
{
    uint16_t cccdValue;

    if (length != 2)
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    cccdValue = ((uint16_t)p_data[0] | ((uint16_t)p_data[1] << 8));

    if ( (cccdValue != BLEGATT_CCCD_NONE) &&
            (cccdValue != BLEGATT_CCCD_NOTIFICATION) &&
            (cccdValue != BLEGATT_CCCD_INDICATION) &&
            (cccdValue != BLEGATT_CCCD_NOTIFY_INDICATE))
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    *p_cccd = cccdValue;

    return BLE_ERR_OK;
}



/** BLE Set Automatically Handle CCCD Read Request or Read By Type Request From the Client
 */
ble_err_t ble_svcs_auto_handle_cccd_read_req(ble_evt_att_param_t *p_param, uint16_t cccd_value)
{
    ble_err_t status = BLE_ERR_INVALID_PARAMETER;
    uint8_t cccd[2];

    // check cccd_value is valid or not
    if ( (cccd_value != BLEGATT_CCCD_NONE) &&
            (cccd_value != BLEGATT_CCCD_NOTIFICATION) &&
            (cccd_value != BLEGATT_CCCD_INDICATION) &&
            (cccd_value != BLEGATT_CCCD_NOTIFY_INDICATE))
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    cccd[0] = (uint8_t)(cccd_value & 0xFF);
    cccd[1] = (uint8_t)((cccd_value >> 8) & 0xFF);

    // check opcode first
    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
    {
        ble_gatt_data_param_t *p_gatt_data_param;

        p_gatt_data_param = pvPortMalloc(sizeof(ble_gatt_data_param_t));
        if (p_gatt_data_param != NULL)
        {
            p_gatt_data_param->host_id = p_param->host_id;
            p_gatt_data_param->handle_num = p_param->handle_num;
            p_gatt_data_param->length = 2;
            p_gatt_data_param->p_data = (uint8_t *)cccd;

            status = ble_cmd_gatt_read_by_type_rsp(p_gatt_data_param);
            if (status != BLE_ERR_OK) // send to BLE stack
            {
                log_info("<TYPE_BLE_GATT_READ_BY_TYPE_RSP> Send msg to BLE stack fail\n");
            }
            vPortFree(p_gatt_data_param);
        }
        else
        {
            log_info("<TYPE_BLE_GATT_READ_BY_TYPE_RSP> malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }
    }
    break;

    case OPCODE_ATT_READ_REQUEST:
    {
        ble_gatt_data_param_t *p_gatt_data_param;

        p_gatt_data_param = pvPortMalloc(sizeof(ble_gatt_data_param_t));
        if (p_gatt_data_param != NULL)
        {
            p_gatt_data_param->host_id = p_param->host_id;
            p_gatt_data_param->handle_num = p_param->handle_num;
            p_gatt_data_param->length = 2;
            p_gatt_data_param->p_data = (uint8_t *)cccd;

            status = ble_cmd_gatt_read_rsp(p_gatt_data_param);
            if (status != BLE_ERR_OK) // send to BLE stack
            {
                log_info("<TYPE_BLE_GATT_READ_RSP> Send msg to BLE stack fail\n");
            }
            vPortFree(p_gatt_data_param);
        }
        else
        {
            log_info("<TYPE_BLE_GATT_READ_RSP> malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }
    }
    break;

    default:
        status = BLE_ERR_INVALID_PARAMETER;
        break;
    }

    return status;
}


/** BLE Set Automatically Handle Read Request or Read By Type Request From the Client
 */
ble_err_t ble_svcs_auto_handle_read_req(ble_evt_att_param_t *p_param, uint8_t *p_data, uint8_t length)
{
    ble_err_t status = BLE_ERR_INVALID_PARAMETER;

    switch (p_param->opcode)
    {
    case OPCODE_ATT_READ_BY_TYPE_REQUEST:
    {
        ble_gatt_data_param_t *p_gatt_data_param;

        p_gatt_data_param = pvPortMalloc(sizeof(ble_gatt_data_param_t));
        if (p_gatt_data_param != NULL)
        {
            p_gatt_data_param->host_id = p_param->host_id;
            p_gatt_data_param->handle_num = p_param->handle_num;
            p_gatt_data_param->p_data = p_data;
            p_gatt_data_param->length = length;

            status = ble_cmd_gatt_read_by_type_rsp(p_gatt_data_param);
            if (status != BLE_ERR_OK) // send to BLE stack
            {
                log_info("<TYPE_BLE_GATT_READ_BY_TYPE_RSP> Send msg to BLE stack fail\n");
            }
            vPortFree(p_gatt_data_param);
        }
        else
        {
            log_info("<TYPE_BLE_GATT_READ_BY_TYPE_RSP> malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }
    }
    break;

    case OPCODE_ATT_READ_REQUEST:
    {
        ble_gatt_data_param_t *p_gatt_data_param;

        p_gatt_data_param = pvPortMalloc(sizeof(ble_gatt_data_param_t));
        if (p_gatt_data_param != NULL)
        {
            p_gatt_data_param->host_id = p_param->host_id;
            p_gatt_data_param->handle_num = p_param->handle_num;
            p_gatt_data_param->p_data = p_data;
            p_gatt_data_param->length = length;

            status = ble_cmd_gatt_read_rsp(p_gatt_data_param);
            if (status != BLE_ERR_OK) // send to BLE stack
            {
                log_info("<TYPE_BLE_GATT_READ_RSP> Send msg to BLE stack fail\n");
            }
            vPortFree(p_gatt_data_param);
        }
        else
        {
            log_info("<TYPE_BLE_GATT_READ_RSP> malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }
    }
    break;

    default:
        status = BLE_ERR_INVALID_PARAMETER;
        break;
    }

    return (ble_err_t)status;
}



/** Handle BLE Service Read Blob Request
*/
ble_err_t ble_svcs_auto_handle_read_blob_req(ble_evt_att_param_t *p_param, uint8_t *p_data, uint8_t length)
{
    ble_att_error_code_t err_rsp = ERR_CODE_ATT_NO_ERROR;
    int status;
    uint16_t offset;

    // calculate offset
    offset = (((uint16_t)p_param->data[1] << 8) | p_param->data[0]);

    // check offset + length
    if (offset >= length)
    {
        err_rsp = ERR_CODE_ATT_INVALID_OFFSET;
    }

    if (err_rsp != ERR_CODE_ATT_NO_ERROR)
    {
        ble_gatt_err_rsp_param_t *p_gatt_data_param;

        p_gatt_data_param = pvPortMalloc(sizeof(ble_gatt_err_rsp_param_t));

        if (p_gatt_data_param != NULL)
        {
            p_gatt_data_param->host_id = p_param->host_id;
            p_gatt_data_param->handle_num = p_param->handle_num;
            p_gatt_data_param->opcode = OPCODE_ATT_READ_BLOB_REQUEST;
            p_gatt_data_param->err_rsp = err_rsp;

            status = ble_cmd_gatt_error_rsp(p_gatt_data_param);
            if (status != BLE_ERR_OK) // send to BLE stack
            {
                log_info("<TYPE_BLE_GATT_ERROR_RSP> Send msg to BLE stack fail\n");
            }
            vPortFree(p_gatt_data_param);
        }
        else
        {
            log_info("<TYPE_BLE_GATT_ERROR_RSP> malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }
    }
    else
    {
        ble_gatt_data_param_t *p_gatt_data_param;

        p_gatt_data_param = pvPortMalloc(sizeof(ble_gatt_data_param_t));
        if (p_gatt_data_param != NULL)
        {
            p_gatt_data_param->host_id = p_param->host_id;
            p_gatt_data_param->handle_num = p_param->handle_num;
            p_gatt_data_param->p_data = (p_data + offset);
            p_gatt_data_param->length = (length - offset);

            status = ble_cmd_gatt_read_blob_rsp(p_gatt_data_param);
            if (status != BLE_ERR_OK) // send to BLE stack
            {
                log_info("<TYPE_BLE_GATT_READ_BLOB_RSP> Send msg to BLE stack fail\n");
            }
            vPortFree(p_gatt_data_param);
        }
        else
        {
            log_info("<TYPE_BLE_GATT_READ_BLOB_RSP> malloc fail\n");
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
        }
    }

    return (ble_err_t)status;
}


/** BLE Service CCCD Set
*/
ble_err_t ble_svcs_cccd_set(uint8_t host_id, uint16_t handle_num, ble_gatt_cccd_val_t cccd_value)
{
    ble_err_t status;
    uint8_t cccd[2];
    ble_gatt_data_param_t p_param;

    // check cccd_value is valid or not
    if ( (cccd_value != BLEGATT_CCCD_NONE) &&
            (cccd_value != BLEGATT_CCCD_NOTIFICATION) &&
            (cccd_value != BLEGATT_CCCD_INDICATION) &&
            (cccd_value != BLEGATT_CCCD_NOTIFY_INDICATE))
    {
        return BLE_ERR_INVALID_PARAMETER;
    }

    cccd[0] = (uint8_t)(cccd_value & 0xFF);
    cccd[1] = (uint8_t)((cccd_value >> 8) & 0xFF);

    // send write request
    p_param.host_id = host_id;
    p_param.handle_num = handle_num;
    p_param.length = 2;
    p_param.p_data = (uint8_t *)cccd;

    status = ble_svcs_data_send(TYPE_BLE_GATT_WRITE_REQ, &p_param);

    return status;
}


ble_err_t ble_svcs_data_send(uint16_t type, ble_gatt_data_param_t *p_param)
{
    ble_err_t status;
    ble_gatt_data_param_t *p_data_param;

    status = BLE_ERR_OK;
    p_data_param = pvPortMalloc(sizeof(ble_gatt_data_param_t));
    if (p_data_param != NULL)
    {
        memcpy(p_data_param, p_param, sizeof(ble_gatt_data_param_t));
        switch (type)
        {
        case TYPE_BLE_GATT_READ_RSP:
            status = ble_cmd_gatt_read_rsp(p_data_param);
            break;

        case TYPE_BLE_GATT_READ_BY_TYPE_RSP:
            status = ble_cmd_gatt_read_by_type_rsp(p_data_param);
            break;

        case TYPE_BLE_GATT_READ_BLOB_RSP:
            status = ble_cmd_gatt_read_blob_rsp(p_data_param);
            break;

        case TYPE_BLE_GATT_NOTIFICATION:
            status = ble_cmd_gatt_notification(p_data_param);
            break;

        case TYPE_BLE_GATT_INDICATION:
            status = ble_cmd_gatt_indication(p_data_param);
            break;

        case TYPE_BLE_GATT_WRITE_REQ:
            status = ble_cmd_gatt_write_req(p_data_param);
            break;

        case TYPE_BLE_GATT_WRITE_CMD:
            status = ble_cmd_gatt_write_cmd(p_data_param);
            break;

        default:
            log_info("<ble_svcs_data_send> unknown type.\n");
            status = BLE_ERR_UNKNOW_TYPE;
            break;
        }
        if (status != BLE_ERR_OK)
        {
            log_info("<ble_svcs_data_send> Send msg to BLE stack fail %d, type=0x%02x\n", status, type);
        }
        vPortFree(p_data_param);
    }
    else
    {
        log_info("<ble_svcs_data_send> malloc fail, type=0x%02x\n", type);
        status = BLE_ERR_DATA_MALLOC_FAIL;
    }

    return status;
}
