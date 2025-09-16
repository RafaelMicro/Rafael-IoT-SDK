/**
 * @file mmdl_gen_default_trans_time_sr_main.c
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
#include "mmdl_gen_default_trans_time_common.h"
#include "mmdl_gen_default_trans_time_sr.h"

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
static void mmdl_generic_default_trans_time_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_default_trans_time_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_default_trans_time_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);


void mmdl_generic_default_trans_time_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_GEN_DEFAULT_TRANS_GET_OPCODE:
        mmdl_generic_default_trans_time_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_DEFAULT_TRANS_SET_OPCODE:
        mmdl_generic_default_trans_time_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_DEFAULT_TRANS_SET_NO_ACK_OPCODE:
        mmdl_generic_default_trans_time_set(FALSE, p_evt_msg, p_element, p_model);
        break;
    }

}

void mmdl_generic_default_trans_time_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t           *pt_req;
    mesh_tlv_t                           *pt_tlv = NULL;
    gen_default_trans_time_state_t       *p_gen_default_trans_time_state    = (gen_default_trans_time_state_t *)p_model->p_state_var;
    uint16_t                             element_address = p_element->element_address;
    uint16_t                             publish_address = p_model->p_publish_info->address;
    uint8_t                              ttl = p_model->p_publish_info->publish_ttl;
    gen_default_trans_time_status_msg_t  *p_msg;
    do
    {
        if (publish_address == MESH_UNASSIGNED_ADDR)
        {
            /* the publish address is not valid, ignore command */
            break;
        }

        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_default_trans_time_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_default_trans_time_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = publish_address;
        pt_req->src_addr = element_address;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_DEFAULT_TRANS_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_default_trans_time_status_msg_t);;
        p_msg = (gen_default_trans_time_status_msg_t *)pt_req->parameter;

        p_msg->transition_time =
            ((p_gen_default_trans_time_state->default_trans_num_of_step) |
             (p_gen_default_trans_time_state->default_trans_step_resoultion << 6));

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

void mmdl_generic_default_trans_time_extend_set(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t transition_time)
{
    gen_default_trans_time_state_t  *p_gen_default_trans_time_state = (gen_default_trans_time_state_t *)p_model->p_state_var;

    do
    {
        /* if state not changed, break it */
        if ((p_gen_default_trans_time_state->default_trans_num_of_step == (transition_time & 0x3f)) &&
                (p_gen_default_trans_time_state->default_trans_step_resoultion == ((transition_time & 0xc0) >> 6)))
        {
            break;
        }

        p_gen_default_trans_time_state->default_trans_num_of_step = (transition_time & 0x3f);
        p_gen_default_trans_time_state->default_trans_step_resoultion = ((transition_time & 0xc0) >> 6);

        /* publish the state if publish is avaliable */
        mmdl_generic_default_trans_time_publish_state(p_element, p_model);


    } while (0);

}

static void mmdl_generic_default_trans_time_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_default_trans_time_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_generic_default_trans_time_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_default_trans_time_set_msg_t  *p_gen_default_trans_time_set_msg = (gen_default_trans_time_set_msg_t *)p_evt_msg->parameter;
    gen_default_trans_time_state_t    *p_gen_default_trans_time_state   = (gen_default_trans_time_state_t *)p_model->p_state_var;


    do
    {
        if (p_model->is_extended)
        {
            general_parameter_t  call_back_param;
            // this model is extended, call upper model to process
            call_back_param.u8 = p_gen_default_trans_time_set_msg->transition_time;
            ((ex_call_back)(p_model->p_upper_call_back))(p_element, p_model, call_back_param);
        }
        else
        {
            p_gen_default_trans_time_state->default_trans_num_of_step = (p_gen_default_trans_time_set_msg->transition_time & 0x3f);
            p_gen_default_trans_time_state->default_trans_step_resoultion = ((p_gen_default_trans_time_set_msg->transition_time & 0xc0) >> 6);
        }

    } while (0);

    if (ack_flag)
    {
        mmdl_generic_default_trans_time_send_status(p_evt_msg, p_element, p_model);
    }

}


static void mmdl_generic_default_trans_time_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t           *pt_req;
    mesh_tlv_t                           *pt_tlv;
    gen_default_trans_time_state_t       *p_gen_default_trans_time_state   = (gen_default_trans_time_state_t *)p_model->p_state_var;
    gen_default_trans_time_status_msg_t  *p_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_default_trans_time_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_default_trans_time_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_DEFAULT_TRANS_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_default_trans_time_status_msg_t);
        p_msg = (gen_default_trans_time_status_msg_t *)pt_req->parameter;
        p_msg->transition_time =
            ((p_gen_default_trans_time_state->default_trans_num_of_step) |
             (p_gen_default_trans_time_state->default_trans_step_resoultion << 6));


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




