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
#include "mmdl_gen_onoff_cl.h"
#include "mmdl_gen_onoff_common.h"
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
static void mmdl_generic_onoff_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

void mmdl_generic_onoff_cl_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{

}

void mmdl_generic_onoff_cl_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t in_is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_GEN_ONOFF_STATUS_OPCODE:
        mmdl_generic_onoff_status(p_evt_msg, p_element, p_model);
        break;

    }

}

static void mmdl_generic_onoff_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_on_off_status_msg_t  *p_gen_on_off_status_msg = (gen_on_off_status_msg_t *)p_evt_msg->parameter;

    if ( p_model->is_extended)
    {
        general_parameter_t  call_back_param;
        if (p_model->p_upper_call_back != NULL)
        {
            ((ex_call_back)(p_model->p_upper_call_back))(p_element, p_model, call_back_param);
        }
        else
        {
            printf("upper call back not exist\n");
        }
    }
    else
    {
        if (p_model->p_user_call_back != NULL)
        {
            ((model_onoff_user_call_back)(p_model->p_user_call_back))(p_element->element_address, p_gen_on_off_status_msg->present_onoff);
        }
        else
        {
            printf("user call back not exist\n");
        }
    }
}


void mmdl_generic_onoff_send_set(mmdl_transmit_info_t tx_info, gen_on_off_set_msg_t *p_onoff_set)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    gen_on_off_set_msg_t     *p_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_on_off_set_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_on_off_set_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = tx_info.dst_addr;
        pt_req->src_addr = tx_info.src_addr;;
        pt_req->appkey_index = tx_info.appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_ONOFF_SET_OPCODE);
        pt_req->parameter_len = sizeof(gen_on_off_set_msg_t);
        p_msg = (gen_on_off_set_msg_t *)pt_req->parameter;
        p_msg->on_off = p_onoff_set->on_off;
        p_msg->tid = p_onoff_set->tid;
        /* TODO: the remaining time must process */
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

void mmdl_generic_onoff_send_unack_set(mmdl_transmit_info_t tx_info, gen_on_off_set_msg_t *p_onoff_set)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    gen_on_off_set_msg_t     *p_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_on_off_set_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_on_off_set_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = tx_info.dst_addr;
        pt_req->src_addr = tx_info.src_addr;;
        pt_req->appkey_index = tx_info.appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_ONOFF_SET_NO_ACK_OPCODE);
        pt_req->parameter_len = sizeof(gen_on_off_set_msg_t);
        p_msg = (gen_on_off_set_msg_t *)pt_req->parameter;
        p_msg->on_off = p_onoff_set->on_off;
        p_msg->tid = p_onoff_set->tid;
        /* TODO: the remaining time must process */
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






