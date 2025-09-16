/**
 * @file mmdl_gen_onoff_sr_main.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-06-09
 *
 * @copyright Copyright (c) 2022
 *
 */

//=============================================================================
//                Include
//=============================================================================
#include "mmdl_common.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================


//=============================================================================
//                Private Global Variables
//=============================================================================

/* store the useful data pointer for futher using */
static uint8_t                      ttl = 4;

//=============================================================================
//                Private Functions Declaration
//=============================================================================


void mmdl_client_send(uint16_t dst_addr, uint16_t src_addr, uint32_t opcode, uint16_t key_index, uint16_t param_len, uint8_t *p_param)
{
    mesh_app_mdl_evt_msg_req_t *pt_req;
    mesh_tlv_t   *pt_tlv;
    int status;
    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + param_len);
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + param_len;

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = dst_addr;
        pt_req->src_addr = src_addr;
        pt_req->ttl = ttl;
        pt_req->appkey_index = key_index;

        if (opcode != 0)
        {
            pt_req->opcode_invalid = 0;
            pt_req->opcode = opcode;
        }
        else
        {
            pt_req->opcode_invalid = 1;
        }
        pt_req->parameter_len = param_len;
        memcpy(pt_req->parameter, p_param, param_len);

        /* TODO: the remaining time must process */
        status = mesh_evt_msg_sendto(pt_tlv);
        if (status != 0)
        {
            printf("Send msg to mesh stack fail %d\n", status);
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}


void cfgmdl_client_send(uint32_t opcode, uint16_t dst, uint8_t *p_param, uint16_t pram_len)
{
    mesh_cfg_mdl_evt_msg_req_t  *p_cfg_msg_info;
    uint8_t         *p_temp;
    mesh_tlv_t      *pt_tlv = NULL;

    do
    {

        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) +  sizeof(mesh_cfg_mdl_evt_msg_req_t) + pram_len);
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_CFG_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_cfg_mdl_evt_msg_req_t) + pram_len;
        p_temp = pt_tlv->value;
        p_cfg_msg_info = (mesh_cfg_mdl_evt_msg_req_t *)p_temp;
        p_cfg_msg_info->dst_addr = dst;
        p_cfg_msg_info->src_addr = pib_primary_address_get();
        p_cfg_msg_info->opcode = opcode;
        p_cfg_msg_info->netkey_index = 0x0000; //only one network key for now
        p_cfg_msg_info->ttl = ttl;
        p_cfg_msg_info->mic_size = 4;
        if (pib_device_key_get(p_cfg_msg_info->config_key) == false)
        {
            printf("device key get fail\n");
            break;
        }

        p_cfg_msg_info->parameter_len = pram_len;

        if (pram_len > 0 && p_param != NULL)
        {
            memcpy(p_cfg_msg_info->parameter, p_param, pram_len);
        }

        if (mesh_evt_msg_sendto(pt_tlv) != 0)
        {
            printf("Send msg to mesh stack fail\n");
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}


