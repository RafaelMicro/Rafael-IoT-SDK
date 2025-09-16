/**
 * @file mmdl_trsp_sr_main.c
 * @author Jeffrey (jeffrey.lin@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-08-01
 *
 * @copyright Copyright (c) 2022
 *
 */

//=============================================================================
//                Include
//=============================================================================
#include "mmdl_common.h"
#include "mmdl_trsp_common.h"
#include "mmdl_trsp_sr.h"
//=============================================================================
//                Private Definitions of const value
//=============================================================================


//=============================================================================
//                Private Global Variables
//=============================================================================

/* store the useful data pointer for futher using */

static uint8_t              ttl = 4;

//=============================================================================
//                Private Functions Declaration
//=============================================================================
static void mmdl_rafael_trsp_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, uint16_t src_address);

static void mmdl_rafael_trsp_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);

void mmdl_rafael_trsp_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE32(p_evt_msg->opcode))
    {
    case MMDL_RAFAEL_TRSP_SET_OPCODE:
        mmdl_rafael_trsp_set(TRUE, p_evt_msg, p_element, p_model, is_broadcast);
        break;

    case MMDL_RAFAEL_TRSP_SET_NO_ACK_OPCODE:
        mmdl_rafael_trsp_set(FALSE, p_evt_msg, p_element, p_model, is_broadcast);
        break;


    default:
        break;
    }
}


static void mmdl_rafael_trsp_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t in_is_broadcast)
{
    raf_trsp_cb_params_t *p_raf_trsp_cb_params;
    do
    {
        if (p_model->p_user_call_back != NULL)
        {
            p_raf_trsp_cb_params = pvPortMalloc(sizeof(raf_trsp_cb_params_t) + p_evt_msg->parameter_len);

            if (p_raf_trsp_cb_params != NULL)
            {
                p_raf_trsp_cb_params->dst_addr = p_evt_msg->dst_addr;
                p_raf_trsp_cb_params->src_addr = p_evt_msg->src_addr;
                p_raf_trsp_cb_params->appkey_index = p_evt_msg->appkey_index;
                p_raf_trsp_cb_params->is_group = in_is_broadcast;
                p_raf_trsp_cb_params->data_len = p_evt_msg->parameter_len;
                memcpy(p_raf_trsp_cb_params->data, p_evt_msg->parameter, p_evt_msg->parameter_len);

                ((model_rafael_trsp_user_call_back)(p_model->p_user_call_back))(p_raf_trsp_cb_params);
            }
            vPortFree(p_raf_trsp_cb_params);
        }

        if (ack_flag && (!in_is_broadcast))
        {
            mmdl_rafael_trsp_send_status(p_evt_msg, p_element->element_address);
        }
    } while (0);
}

void mmdl_rafael_trsp_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, uint16_t src_address)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    int status;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + p_evt_msg->parameter_len);
        if (pt_tlv == NULL)
        {
            printf("rafael trsp status malloc fail\n");
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + p_evt_msg->parameter_len;

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = src_address;
        pt_req->appkey_index = 0;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE32(MMDL_RAFAEL_TRSP_STATUS_OPCODE);
        pt_req->parameter_len = p_evt_msg->parameter_len;
        memcpy(pt_req->parameter, p_evt_msg->parameter, p_evt_msg->parameter_len);

        status = mesh_evt_msg_sendto(pt_tlv);
        if (status != 0)
        {
            printf("rafael trsp send status to mesh stack fail %d\n", status);
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
}

