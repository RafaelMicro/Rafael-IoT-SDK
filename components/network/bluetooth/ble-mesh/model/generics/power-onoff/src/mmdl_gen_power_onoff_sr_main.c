/**
 * @file mmdl_gen_power_onoff_sr_main.c
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
#include "mmdl_gen_power_onoff_common.h"
#include "mmdl_gen_power_onoff_sr.h"

//#include "mmdl_light_lightness_common.h"
#include "mmdl_light_lightness_sr.h"
#include "mmdl_gen_onoff_sr.h"

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
static void mmdl_generic_onpowerup_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_onpowerup_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_onpowerup_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);


void mmdl_generic_onpowerup_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_GEN_ONPOWERUP_GET_OPCODE:
        mmdl_generic_onpowerup_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_ONPOWERUP_SET_OPCODE:
        mmdl_generic_onpowerup_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_ONPOWERUP_SET_NO_ACK_OPCODE:
        mmdl_generic_onpowerup_set(FALSE, p_evt_msg, p_element, p_model);
        break;
    }

}

static void mmdl_generic_onpowerup_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_onpowerup_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_generic_onpowerup_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_onpowerup_set_msg_t  *p_gen_onpowerup_set_msg = (gen_onpowerup_set_msg_t *)p_evt_msg->parameter;
    gen_onpowerup_state_t    *p_gen_onpowerup_state   = (gen_onpowerup_state_t *)p_model->p_state_var;
    uint32_t                 state_changed = 0;


    do
    {
        if (p_gen_onpowerup_set_msg->on_power_up >= MMDL_GEN_ONPOWERUP_STATE_PROHIBITED)
        {
            break;
        }
        //check the state change
        if (p_gen_onpowerup_state->on_power_up_state == p_gen_onpowerup_set_msg->on_power_up)
        {
            break;
        }

        p_gen_onpowerup_state->on_power_up_state = p_gen_onpowerup_set_msg->on_power_up;


    } while (0);

    if (ack_flag)
    {
        mmdl_generic_onpowerup_send_status(p_evt_msg, p_element, p_model);
    }

    if (state_changed && p_model->is_extended)
    {
        general_parameter_t  call_back_param;
        // this model is extended, call upper model to process
        call_back_param.u8 = p_gen_onpowerup_state->on_power_up_state;
        ((ex_call_back)(p_model->p_upper_call_back))(p_element, p_model, call_back_param);
    }
    else if (state_changed)
    {
        mmdl_light_lightness_publish_state(p_element, p_model);
    }


}

static void mmdl_generic_onpowerup_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    gen_onpowerup_state_t       *p_gen_onpowerup_state   = (gen_onpowerup_state_t *)p_model->p_state_var;
    gen_onpowerup_status_msg_t  *p_gen_onpowerup_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_onpowerup_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_onpowerup_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_ONPOWERUP_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_onpowerup_status_msg_t);
        p_gen_onpowerup_status_msg = (gen_onpowerup_status_msg_t *)pt_req->parameter;
        p_gen_onpowerup_status_msg->on_power_up = p_gen_onpowerup_state->on_power_up_state;

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


void mmdl_generic_power_onoff_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv = NULL;;
    gen_onpowerup_state_t       *p_generic_onpowerup_state = (gen_onpowerup_state_t *)p_model->p_state_var;
    uint16_t                    element_address = p_element->element_address;
    uint16_t                    publish_address = p_model->p_publish_info->address;
    uint8_t                     ttl = p_model->p_publish_info->publish_ttl;
    uint32_t                    parameter_size = sizeof(gen_onpowerup_status_msg_t);

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

        gen_onpowerup_status_msg_t *p_msg = (gen_onpowerup_status_msg_t *)pt_req->parameter;
        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_ONPOWERUP_STATUS_OPCODE);
        p_msg->on_power_up = p_generic_onpowerup_state->on_power_up_state;

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

void mmdl_generic_power_onoff_update_extend(ble_mesh_element_param_t *p_current_element, ble_mesh_model_param_t *p_parent_model, ble_mesh_model_param_t *p_current_child_model)
{
    gen_onpowerup_state_t   *p_onpowerup_state = (gen_onpowerup_state_t *)p_parent_model->p_state_var;
    ble_mesh_model_param_t  *p_child_model;
    uint32_t                model_id = p_current_child_model->model_id;

    /* search for the knowing extended model from spec in ths element */
    if (search_model(p_current_element, MMDL_GEN_ONOFF_SR_MDL_ID, &p_child_model) && model_id != MMDL_GEN_ONOFF_SR_MDL_ID)
    {

        mmdl_generic_onoff_extend_set(p_current_element, p_child_model, p_onpowerup_state->on_off_state);
    }
}

void mmdl_generic_power_onoff_ex_cb(ble_mesh_element_param_t *p_current_element, ble_mesh_model_param_t *p_child_model, general_parameter_t p)
{
    ble_mesh_model_param_t  *p_parent_model;
    gen_onpowerup_state_t   *p_onpowerup_state;
    uint32_t                state_changed = 0;

    do
    {
        /* search my parent model by model id */
        if (!search_model(p_current_element, MMDL_GEN_POWER_ONOFF_SR_MDL_ID, &p_parent_model))
        {
            break;
        }

        p_onpowerup_state = (gen_onpowerup_state_t *)p_parent_model->p_state_var;

        //process the message from different extend model by model_id
        switch (p_child_model->model_id)
        {
        case MMDL_GEN_ONOFF_SR_MDL_ID:
        {
            if (p.u8 == 0) //set device off
            {
                p_onpowerup_state->on_off_state = 0;
            }
            else //det device on, level -> last
            {
                p_onpowerup_state->on_off_state = 1;
            }
            state_changed = 1;
        }
        break;

        }

        if (state_changed)
        {
            /* process all children models */
            mmdl_generic_power_onoff_update_extend(p_current_element, p_parent_model, p_child_model);
        }


        if (state_changed && p_parent_model->is_extended)
        {
            general_parameter_t  call_back_param;
            // this model is extended, call upper model to process
            call_back_param.u8 = p_onpowerup_state->on_off_state;
            ((ex_call_back)(p_parent_model->p_upper_call_back))(p_current_element, p_parent_model, call_back_param);
        }
        else if (state_changed)
        {
            /* publish the state due to state changed */
            mmdl_generic_power_onoff_publish_state(p_current_element, p_parent_model);
        }
    } while (0);
}


void mmdl_generic_power_onoff_extend_set(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t onopowerup_state)
{
    gen_onpowerup_state_t  *p_gen_power_on_off_state = (gen_onpowerup_state_t *)p_model->p_state_var;

    do
    {
        /* if state not changed, break it */
        if (p_gen_power_on_off_state->on_power_up_state == onopowerup_state)
        {
            break;
        }

        p_gen_power_on_off_state->on_power_up_state = onopowerup_state;

        /* publish the state if publish is avaliable */
        mmdl_generic_power_onoff_publish_state(p_element, p_model);


    } while (0);

}

void mmdl_generic_power_onoff_scene_set(ble_mesh_element_param_t *p_current_element)
{
    gen_onpowerup_state_t   *p_onpowerup_state;
    ble_mesh_model_param_t  *p_child_model;
    ble_mesh_model_param_t  *p_parent_model;

    /* search for the knowing extended model from spec in ths element */

    if (search_model(p_current_element, MMDL_GEN_POWER_ONOFF_SR_MDL_ID, &p_parent_model))
    {
        p_onpowerup_state = (gen_onpowerup_state_t *)p_parent_model->p_state_var;
        if (search_model(p_current_element, MMDL_GEN_ONOFF_SR_MDL_ID, &p_child_model))
        {
            mmdl_generic_onoff_extend_set(p_current_element, p_child_model, p_onpowerup_state->on_off_state);
        }
    }
}


