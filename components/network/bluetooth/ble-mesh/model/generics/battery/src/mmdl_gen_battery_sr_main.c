/**
 * @file mmdl_gen_battery_sr_main.c
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

#include "prod.h"
#include "mmdl_common.h"
#include "mmdl_gen_battery_sr.h"
#include "mmdl_gen_battery_common.h"
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
static void mmdl_generic_battery_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_battery_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);


void mmdl_generic_battery_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_GEN_BATTERY_GET_OPCODE:
        mmdl_generic_battery_get(p_evt_msg, p_element, p_model);
        break;
    }
}

static void mmdl_generic_battery_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_battery_send_status(p_evt_msg, p_element, p_model);
}

static void mmdl_generic_battery_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    gen_battery_state_t         *p_gen_battery_state   = (gen_battery_state_t *)p_model->p_state_var;
    gen_battery_status_msg_t    *p_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_battery_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_battery_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_DEFAULT_TRANS_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_battery_status_msg_t);
        p_msg = (gen_battery_status_msg_t *)pt_req->parameter;
        p_msg->level = p_gen_battery_state->battery_level;
        p_msg->time_to_discharge = p_gen_battery_state->battery_time_to_discharge;
        p_msg->time_to_charge = p_gen_battery_state->battery_time_to_charge;
        p_msg->flag = p_gen_battery_state->battery_flag;


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

void mmdl_generic_battery_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv = NULL;;
    gen_battery_state_t         *p_gen_battery_state = (gen_battery_state_t *)p_model->p_state_var;
    uint16_t                    element_address = p_element->element_address;
    uint16_t                    publish_address = p_model->p_publish_info->address;
    uint8_t                     ttl = p_model->p_publish_info->publish_ttl;
    uint32_t                    parameter_size = sizeof(gen_battery_status_msg_t);

    do
    {
        if (publish_address == MESH_UNASSIGNED_ADDR)
        {
            /* the publish address is not valid, ignore command */
            break;
        }

        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + parameter_size);
        if (pt_tlv == NULL)
        {
            break;
        }
        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + parameter_size;

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = publish_address;
        pt_req->src_addr = element_address;
        pt_req->ttl = ttl;
        pt_req->parameter_len = parameter_size;

        gen_battery_status_msg_t *p_msg = (gen_battery_status_msg_t *)pt_req->parameter;
        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_DEFAULT_TRANS_STATUS_OPCODE);
        p_msg->level = p_gen_battery_state->battery_level;
        p_msg->time_to_discharge = p_gen_battery_state->battery_time_to_discharge;
        p_msg->time_to_charge = p_gen_battery_state->battery_time_to_charge;
        p_msg->flag = p_gen_battery_state->battery_flag;

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




