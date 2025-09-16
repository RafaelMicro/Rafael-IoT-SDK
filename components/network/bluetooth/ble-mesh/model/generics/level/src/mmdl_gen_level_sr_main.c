/**
 * @file mmdl_gen_onoff_sr_main.c
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-21
 *
 * @copyright Copyright (c) 2022
 *
 */

//=============================================================================
//                Include
//=============================================================================
#include "mmdl_common.h"
#include "mmdl_gen_level_common.h"
#include "mmdl_gen_level_sr.h"
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
static void mmdl_generic_level_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_level_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_level_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_level_delta_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_level_move_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *in_p_model);

void mmdl_generic_level_sr_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{

}

void mmdl_generic_level_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_GEN_LEVEL_GET_OPCODE:
        mmdl_generic_level_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LEVEL_SET_OPCODE:
        mmdl_generic_level_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LEVEL_SET_NO_ACK_OPCODE:
        mmdl_generic_level_set(FALSE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LEVEL_DELTA_SET_OPCODE:
        mmdl_generic_level_delta_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LEVEL_DELTA_SET_NO_ACK_OPCODE:
        mmdl_generic_level_delta_set(FALSE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LEVEL_MOVE_SET_OPCODE:
        mmdl_generic_level_move_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LEVEL_MOVE_SET_NO_ACK_OPCODE:
        mmdl_generic_level_move_set(FALSE, p_evt_msg, p_element, p_model);
        break;

    }

}

static void mmdl_generic_level_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_level_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_generic_level_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_level_set_msg_t  *p_gen_level_set_msg = (gen_level_set_msg_t *)p_evt_msg->parameter;
    gen_level_state_t    *p_gen_level_state = (gen_level_state_t *)p_model->p_state_var;
    uint32_t             state_changed = 0;
    do
    {

        if (tid == p_gen_level_set_msg->tid)
        {
            return;
        }
        else
        {
            tid = p_gen_level_set_msg->tid;
        }

        //check the state change
        if (p_gen_level_state->level_state == p_gen_level_set_msg->level)
        {
            break;
        }

        //if state changed, check is the extended or not
        p_gen_level_state->level_state = p_gen_level_set_msg->level;
        //because transition time unsupported now
        p_gen_level_state->current_level = p_gen_level_set_msg->level;
        p_gen_level_state->target_level = p_gen_level_set_msg->level;

        state_changed = 1;

    } while (0);

    if (ack_flag)
    {
        mmdl_generic_level_send_status(p_evt_msg, p_element, p_model);
    }

    /* publish the state if publish is avaliable */
    if (state_changed)
    {
        if (p_model->is_extended)
        {
            general_parameter_t  call_back_param;
            // this model is extended, call upper model to process
            call_back_param.u16 = p_gen_level_state->target_level;
            ((ex_call_back)(p_model->p_upper_call_back))(p_element, p_model, call_back_param);
        }
        else
        {
            ((model_level_user_call_back)(p_model->p_user_call_back))(p_element->element_address, p_gen_level_state->target_level);
        }

    }
    else if (state_changed)
    {
        mmdl_generic_level_publish_state(p_element, p_model);
    }
}

static void mmdl_generic_level_delta_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_level_delta_set_msg_t  *p_gen_level_delta_set_msg = (gen_level_delta_set_msg_t *)p_evt_msg->parameter;
    gen_level_state_t          *p_gen_level_state = (gen_level_state_t *)p_model->p_state_var;
    uint32_t                   state_changed = 0;

    do
    {

        if (tid == p_gen_level_delta_set_msg->tid)
        {
            return;
        }
        else
        {
            tid = p_gen_level_delta_set_msg->tid;
        }

        p_gen_level_state->delta_level = p_gen_level_delta_set_msg->delta_level;
        p_gen_level_state->transition_time = p_gen_level_delta_set_msg->transition_time;
        p_gen_level_state->delay = p_gen_level_delta_set_msg->delay;

        p_gen_level_state->level_state += p_gen_level_state->delta_level;
        p_gen_level_state->target_level = p_gen_level_state->level_state;
        p_gen_level_state->current_level = p_gen_level_state->level_state;
        state_changed = 1;

    } while (0);

    if (ack_flag)
    {
        mmdl_generic_level_send_status(p_evt_msg, p_element, p_model);
    }

    if (state_changed)
    {
        if (p_model->is_extended)
        {
            general_parameter_t  call_back_param;
            // this model is extended, call upper model to process
            call_back_param.u16 = p_gen_level_state->target_level;
            ((ex_call_back)(p_model->p_upper_call_back))(p_element, p_model, call_back_param);
        }
        else
        {
            ((model_level_user_call_back)(p_model->p_user_call_back))(p_element->element_address, p_gen_level_state->target_level);
        }

    }
    /* publish the state if publish is avaliable */
    else if (state_changed)
    {
        mmdl_generic_level_publish_state(p_element, p_model);
    }

}

static void mmdl_generic_level_move_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_level_move_set_msg_t  *p_gen_level_move_set_msg = (gen_level_move_set_msg_t *)p_evt_msg->parameter;
    gen_level_state_t         *p_gen_level_state = (gen_level_state_t *)p_model->p_state_var;
    uint32_t                  state_changed = 0;

    do
    {
        if (tid == p_gen_level_move_set_msg->tid)
        {
            return;
        }
        else
        {
            tid = p_gen_level_move_set_msg->tid;
        }

        p_gen_level_state->move_delta_level = p_gen_level_move_set_msg->delta_level;
        p_gen_level_state->transition_time = p_gen_level_move_set_msg->transition_time;
        p_gen_level_state->delay = p_gen_level_move_set_msg->delay;

        state_changed = 1;

    } while (0);

    if (ack_flag)
    {
        mmdl_generic_level_send_status(p_evt_msg, p_element, p_model);
    }

    if (state_changed && p_model->is_extended)
    {
        general_parameter_t  call_back_param;
        // this model is extended, call upper model to process
        call_back_param.u16 = p_gen_level_state->target_level;
        ((ex_call_back)(p_model->p_upper_call_back))(p_element, p_model, call_back_param);
    }
    /* publish the state if publish is avaliable */
    else if (state_changed)
    {
        mmdl_generic_level_publish_state(p_element, p_model);
    }

}


static void mmdl_generic_level_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    gen_level_state_t           *p_gen_level_state   = (gen_level_state_t *)p_model->p_state_var;
    gen_level_status_msg_t      *p_msg;
    int                         status;


    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_level_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_level_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_LEVEL_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_level_status_msg_t);
        p_msg = (gen_level_status_msg_t *)pt_req->parameter;
        p_msg->present_level = p_gen_level_state->current_level;
        p_msg->target_level = p_gen_level_state->current_level;
        /* TODO: remaining time must be processed */
        p_msg->remaining_time = 0;
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

void mmdl_generic_level_extend_set(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, int16_t level_state)
{
    gen_level_state_t  *p_gen_level_state = (gen_level_state_t *)p_model->p_state_var;

    do
    {
        /* if state not changed, break it */
        if (p_gen_level_state->level_state == level_state)
        {
            break;
        }

        p_gen_level_state->level_state = level_state;
        p_gen_level_state->current_level = level_state;
        p_gen_level_state->target_level = level_state;


    } while (0);

}

void mmdl_generic_level_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv = NULL;;
    gen_level_state_t           *p_gen_level_state   = (gen_level_state_t *)p_model->p_state_var;
    gen_level_status_msg_t      *p_gen_level_status_msg;
    uint16_t                    element_address = p_element->element_address;
    uint16_t                    publish_address = p_model->p_publish_info->address;
    uint8_t                     ttl = p_model->p_publish_info->publish_ttl;

    do
    {
        if (publish_address == MESH_UNASSIGNED_ADDR)
        {
            /* the publish address is not valid, ignore command */
            break;
        }

        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_level_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_level_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = publish_address;
        pt_req->src_addr = element_address;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_LEVEL_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_level_status_msg_t);
        p_gen_level_status_msg = (gen_level_status_msg_t *)pt_req->parameter;
        p_gen_level_status_msg->present_level = p_gen_level_state->current_level;
        p_gen_level_status_msg->target_level = p_gen_level_state->target_level;
        /* TODO: remaining time must be processed */
        p_gen_level_status_msg->remaining_time = 0;

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




