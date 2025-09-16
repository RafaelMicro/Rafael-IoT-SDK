/**
 * @file mmdl_trsp_cl_main.c
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
#include "mmdl_trsp_cl.h"
#include "util.h"
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
static void mmdl_rafael_trsp_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast);

void mmdl_rafael_trsp_cl_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE32(p_evt_msg->opcode))
    {

    case MMDL_RAFAEL_TRSP_STATUS_OPCODE:
        mmdl_rafael_trsp_status(p_evt_msg, p_element, p_model, is_broadcast);
        break;

    default:
        break;
    }
}

static void mmdl_rafael_trsp_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t in_is_broadcast)
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
    } while (0);
}

int mmdl_rafael_trsp_send_set(mmdl_transmit_info_t tx_info, raf_trsp_set_msg_t *p_raf_trsp_set_msg)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    int                         status;

    do
    {
        status = MESH_ERR_NULL;
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + p_raf_trsp_set_msg->data_len);
        if (pt_tlv == NULL)
        {
            printf("rafael trsp set malloc fail\n");
            status = MESH_ERR_NO_MEM;
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + p_raf_trsp_set_msg->data_len;

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = tx_info.dst_addr;
        pt_req->src_addr = tx_info.src_addr;
        pt_req->appkey_index = tx_info.appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE32(MMDL_RAFAEL_TRSP_SET_OPCODE);
        pt_req->parameter_len = p_raf_trsp_set_msg->data_len;
        memcpy(pt_req->parameter, p_raf_trsp_set_msg->data, p_raf_trsp_set_msg->data_len);

        status = mesh_evt_msg_sendto(pt_tlv);
        if (status != MESH_SUCCESS)
        {
            printf("rafael trsp send set to mesh stack fail %d\n", status);
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
    return status;
}

int mmdl_rafael_trsp_send_unack_set(mmdl_transmit_info_t tx_info, raf_trsp_set_msg_t *p_raf_trsp_set_msg)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    int                         status;

    do
    {
        status = MESH_ERR_NULL;
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + p_raf_trsp_set_msg->data_len);
        if (pt_tlv == NULL)
        {
            status = MESH_ERR_NO_MEM;
            printf("rafael trsp unack set malloc fail\n");
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + p_raf_trsp_set_msg->data_len;

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = tx_info.dst_addr;
        pt_req->src_addr = tx_info.src_addr;
        pt_req->appkey_index = tx_info.appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE32(MMDL_RAFAEL_TRSP_SET_NO_ACK_OPCODE);
        pt_req->parameter_len = p_raf_trsp_set_msg->data_len;
        memcpy(pt_req->parameter, p_raf_trsp_set_msg->data, p_raf_trsp_set_msg->data_len);

        status = mesh_evt_msg_sendto(pt_tlv);
        if (status != MESH_SUCCESS)
        {
            printf("rafael trsp send unack set to mesh stack fail %d\n", status);
            break;
        }
    } while (0);

    if (pt_tlv)
    {
        vPortFree(pt_tlv);
    }
    return status;
}


