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
#include "mmdl_gen_power_level_common.h"
#include "mmdl_gen_power_level_sr.h"
#include "mmdl_gen_power_onoff_common.h"
#include "mmdl_gen_power_onoff_sr.h"
#include "mmdl_gen_onoff_common.h"
#include "mmdl_gen_onoff_sr.h"
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
static void mmdl_generic_power_level_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_power_level_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_power_level_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

static void mmdl_generic_power_last_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_power_last_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

static void mmdl_generic_power_default_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_power_default_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_power_default_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

static void mmdl_generic_power_range_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_power_range_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_power_range_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t status_code);

void mmdl_generic_power_level_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_GEN_POWER_LEVEL_GET_OPCODE:
        mmdl_generic_power_level_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWER_LEVEL_SET_OPCODE:
        mmdl_generic_power_level_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWER_LEVEL_SET_NO_ACK_OPCODE:
        mmdl_generic_power_level_set(FALSE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWERLAST_GET_OPCODE:
        mmdl_generic_power_last_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWERDEFAULT_GET_OPCODE:
        mmdl_generic_power_default_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWERDEFAULT_SET_OPCODE:
        mmdl_generic_power_default_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWERDEFAULT_SET_NO_ACK_OPCODE:
        mmdl_generic_power_default_set(FALSE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWERRANGE_GET_OPCODE:
        mmdl_generic_power_range_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWERRANGE_SET_OPCODE:
        mmdl_generic_power_range_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_POWERRANGE_SET_NO_ACK_OPCODE:
        mmdl_generic_power_range_set(FALSE, p_evt_msg, p_element, p_model);
        break;
    }

}

void mmdl_generic_power_level_scene_set(ble_mesh_element_param_t *p_current_element)
{
    gen_power_level_state_t  *p_power_level_state;
    ble_mesh_model_param_t   *p_child_model;
    ble_mesh_model_param_t   *p_parent_model;

    if (search_model(p_current_element, MMDL_GEN_POWER_LEVEL_SR_MDL_ID, &p_parent_model))
    {
        p_power_level_state = (gen_power_level_state_t *)p_parent_model->p_state_var;

        /* search for the knowing extended model from spec in ths element */
        if (search_model(p_current_element, MMDL_GEN_ONOFF_SR_MDL_ID, &p_child_model))
        {
            uint8_t onoff_state;

            onoff_state = (p_power_level_state->power_actual_state > 0) ? 1 : 0;

            mmdl_generic_onoff_extend_set(p_current_element, p_child_model, onoff_state);
        }

        if (search_model(p_current_element, MMDL_GEN_LEVEL_SR_MDL_ID, &p_child_model))
        {
            int16_t level_state;

            level_state = p_power_level_state->power_actual_state - 32768;
            mmdl_generic_level_extend_set(p_current_element, p_child_model, level_state);
        }
    }
}

static void mmdl_generic_power_level_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_power_level_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_generic_power_level_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_power_level_set_msg_t  *p_gen_power_level_set_msg = (gen_power_level_set_msg_t *)p_evt_msg->parameter;
    gen_power_level_state_t    *p_gen_power_level_state   = (gen_power_level_state_t *)p_model->p_state_var;


    do
    {
        //check the state change
        if (p_gen_power_level_state->power_actual_state == p_gen_power_level_set_msg->power)
        {
            break;
        }

        p_gen_power_level_state->power_actual_state = p_gen_power_level_set_msg->power;


    } while (0);

    if (ack_flag)
    {
        mmdl_generic_power_level_send_status(p_evt_msg, p_element, p_model);
    }

}

static void mmdl_generic_power_level_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t    *pt_req;
    mesh_tlv_t                    *pt_tlv;
    gen_power_level_state_t       *p_gen_power_level_state   = (gen_power_level_state_t *)p_model->p_state_var;
    gen_power_level_status_msg_t  *p_gen_power_level_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_power_level_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_power_level_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_POWER_LEVEL_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_power_level_status_msg_t);
        p_gen_power_level_status_msg = (gen_power_level_status_msg_t *)pt_req->parameter;
        p_gen_power_level_status_msg->present_power = p_gen_power_level_state->power_actual_state;
        p_gen_power_level_status_msg->target_power = p_gen_power_level_state->power_actual_state;
        p_gen_power_level_status_msg->remaining_time = 0;

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


void mmdl_generic_power_level_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv = NULL;;
    gen_power_level_state_t     *p_generic_power_level_state = (gen_power_level_state_t *)p_model->p_state_var;
    uint16_t                    element_address = p_element->element_address;
    uint16_t                    publish_address = p_model->p_publish_info->address;
    uint8_t                     ttl = p_model->p_publish_info->publish_ttl;
    uint32_t                    parameter_size = sizeof(gen_power_level_status_msg_t);

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

        gen_power_level_status_msg_t *p_msg = (gen_power_level_status_msg_t *)pt_req->parameter;
        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_ONPOWERUP_STATUS_OPCODE);
        p_msg->present_power = p_generic_power_level_state->power_actual_state;
        p_msg->target_power = p_generic_power_level_state->power_actual_state;
        p_msg->remaining_time = 0;

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

static void mmdl_generic_power_level_update_extend(ble_mesh_element_param_t *p_current_element, ble_mesh_model_param_t *p_parent_model, ble_mesh_model_param_t *current_p_child_model)
{
    gen_power_level_state_t  *p_power_level_state = (gen_power_level_state_t *)p_parent_model->p_state_var;
    ble_mesh_model_param_t   *p_child_model;
    uint32_t                 model_id = current_p_child_model->model_id;

    /* search for the knowing extended model from spec in ths element */
    if (search_model(p_current_element, MMDL_GEN_ONOFF_SR_MDL_ID, &p_child_model) && model_id != MMDL_GEN_ONOFF_SR_MDL_ID)
    {
        uint8_t onoff_state;

        onoff_state = (p_power_level_state->power_actual_state > 0) ? 1 : 0;

        mmdl_generic_onoff_extend_set(p_current_element, p_child_model, onoff_state);
    }

    if (search_model(p_current_element, MMDL_GEN_LEVEL_SR_MDL_ID, &p_child_model) && model_id != MMDL_GEN_LEVEL_SR_MDL_ID)
    {
        int16_t level_state;

        level_state = p_power_level_state->power_actual_state - 32768;
        mmdl_generic_level_extend_set(p_current_element, p_child_model, level_state);
    }

}

void mmdl_generic_power_level_ex_cb(ble_mesh_element_param_t *p_current_element, ble_mesh_model_param_t *p_child_model, general_parameter_t p)
{
    ble_mesh_model_param_t   *p_parent_model;
    gen_power_level_state_t  *p_power_level_state;
    uint32_t                 state_changed = 0;

    do
    {
        /* search my parent model by model id */
        if (!search_model(p_current_element, MMDL_GEN_POWER_ONOFF_SR_MDL_ID, &p_parent_model))
        {
            break;
        }

        p_power_level_state = (gen_power_level_state_t *)p_parent_model->p_state_var;

        //process the message from different extend model by model_id
        switch (p_child_model->model_id)
        {
        case MMDL_GEN_ONOFF_SR_MDL_ID:
        {
            if (p.u8 == 0) //set device off, level -> 0
            {
                p_power_level_state->power_last_state = p_power_level_state->power_actual_state;
                p_power_level_state->power_actual_state = 0;
            }
            else //det device on, level -> last
            {
                p_power_level_state->power_last_state = p_power_level_state->power_actual_state;
                if (p_power_level_state->power_default_state > 0)
                {
                    p_power_level_state->power_actual_state = p_power_level_state->power_default_state;
                }
                else
                {
                    p_power_level_state->power_actual_state = p_power_level_state->power_last_state;
                }

                if ((p_power_level_state->power_range_max_state != 0) &&
                        (p_power_level_state->power_actual_state > p_power_level_state->power_range_max_state))
                {
                    p_power_level_state->power_actual_state = p_power_level_state->power_range_max_state;
                }

                if ((p_power_level_state->power_range_min_state != 0) &&
                        (p_power_level_state->power_actual_state < p_power_level_state->power_range_min_state))
                {
                    p_power_level_state->power_actual_state = p_power_level_state->power_range_min_state;
                }

            }
            state_changed = 1;
        }
        break;

        case MMDL_GEN_LEVEL_SR_MDL_ID:
        {
            p_power_level_state->power_last_state = p_power_level_state->power_actual_state;
            p_power_level_state->power_actual_state = p.u16 + 32768;
            state_changed = 1;
        }
        break;

        case MMDL_GEN_POWER_ONOFF_SR_MDL_ID:
        {
            if (p.u8 == 0) //Off. After being powered up
            {
                p_power_level_state->power_last_state = p_power_level_state->power_actual_state;
                p_power_level_state->power_actual_state = 0;

            }
            else if (p.u8 == 1) //Default. After being powered up
            {
                p_power_level_state->power_last_state = p_power_level_state->power_actual_state;

                if (p_power_level_state->power_default_state > 0)
                {
                    p_power_level_state->power_actual_state = p_power_level_state->power_default_state;
                }
                else
                {
                    p_power_level_state->power_actual_state = p_power_level_state->power_last_state;
                }

                if ((p_power_level_state->power_range_max_state != 0) &&
                        (p_power_level_state->power_actual_state > p_power_level_state->power_range_max_state))
                {
                    p_power_level_state->power_actual_state = p_power_level_state->power_range_max_state;
                }

                if ((p_power_level_state->power_range_min_state != 0) &&
                        (p_power_level_state->power_actual_state < p_power_level_state->power_range_min_state))
                {
                    p_power_level_state->power_actual_state = p_power_level_state->power_range_min_state;
                }


            }
            else if (p.u8 == 2) //Restore.
            {
                p_power_level_state->power_actual_state = p_power_level_state->power_last_state; //need to check 3.1.5.1.3
            }
            else
            {
                break;
            }
            state_changed = 1;
        }
        break;

        }

        if (state_changed)
        {
            /* process all children models */
            mmdl_generic_power_level_update_extend(p_current_element, p_parent_model, p_child_model);
        }
    } while (0);
}


void mmdl_generic_power_level_extend_set(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t onopowerup_state)
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

static void mmdl_generic_power_last_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_power_last_send_status(p_evt_msg, p_element, p_model);
}

static void mmdl_generic_power_last_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t   *pt_req;
    mesh_tlv_t                   *pt_tlv;
    gen_power_level_state_t      *p_gen_power_level_state = (gen_power_level_state_t *)p_model->p_state_var;
    gen_power_last_status_msg_t  *p_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_power_last_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_power_last_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_POWERLAST_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_power_last_status_msg_t);
        p_msg = (gen_power_last_status_msg_t *)pt_req->parameter;
        p_msg->present_power = p_gen_power_level_state->power_last_state;

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

static void mmdl_generic_power_default_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_power_default_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_generic_power_default_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_power_default_set_msg_t  *p_gen_power_default_set_msg = (gen_power_default_set_msg_t *)p_evt_msg->parameter;
    gen_power_level_state_t      *p_gen_power_level_state = (gen_power_level_state_t *)p_model->p_state_var;


    do
    {
        //check the state change
        if (p_gen_power_level_state->power_default_state == p_gen_power_default_set_msg->power)
        {
            break;
        }

        /*Value 0x0000 has a special meaning defined: use the value of the Generic Power Last state as the default value.*/
        if (p_gen_power_default_set_msg->power == 0)
        {
            p_gen_power_level_state->power_default_state = p_gen_power_level_state->power_last_state;
        }
        else
        {
            p_gen_power_level_state->power_default_state = p_gen_power_default_set_msg->power;
        }


    } while (0);

    if (ack_flag)
    {
        mmdl_generic_power_default_send_status(p_evt_msg, p_element, p_model);
    }

}

static void mmdl_generic_power_default_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_tlv_t   *pt_tlv;
    mesh_app_mdl_evt_msg_req_t      *pt_req;
    gen_power_level_state_t         *p_gen_power_level_state   = (gen_power_level_state_t *)p_model->p_state_var;
    gen_power_default_status_msg_t  *p_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_power_default_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_power_default_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_POWERDEFAULT_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_power_default_status_msg_t);
        p_msg = (gen_power_default_status_msg_t *)pt_req->parameter;
        p_msg->present_power = p_gen_power_level_state->power_default_state;

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

static void mmdl_generic_power_range_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_power_range_send_status(p_evt_msg, p_element, p_model, 0x00);
}


static void mmdl_generic_power_range_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_power_range_set_msg_t  *p_gen_power_range_set_msg = (gen_power_range_set_msg_t *)p_evt_msg->parameter;
    gen_power_level_state_t    *p_gen_power_level_state   = (gen_power_level_state_t *)p_model->p_state_var;
    uint8_t                    status_code = MMDL_RANGE_SUCCESS;

    do
    {
        //check the message valid
        if (p_gen_power_level_state->power_range_min_state > p_gen_power_range_set_msg->range_max)
        {
            break;
        }

        //check the state change
        if ((p_gen_power_level_state->power_range_min_state == p_gen_power_range_set_msg->range_min) &&
                (p_gen_power_level_state->power_range_max_state == p_gen_power_range_set_msg->range_max))
        {
            break;
        }

        p_gen_power_level_state->power_range_min_state = p_gen_power_range_set_msg->range_min;
        p_gen_power_level_state->power_range_max_state = p_gen_power_range_set_msg->range_max;

        //TODO: check the provided value for Range Min & Range Max can be set or not
    } while (0);

    if (ack_flag)
    {
        mmdl_generic_power_range_send_status(p_evt_msg, p_element, p_model, status_code);
    }

}

static void mmdl_generic_power_range_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t status_code)
{
    mesh_tlv_t                    *pt_tlv;
    mesh_app_mdl_evt_msg_req_t    *pt_req;
    gen_power_level_state_t       *p_gen_power_level_state   = (gen_power_level_state_t *)p_model->p_state_var;
    gen_power_range_status_msg_t  *p_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_power_range_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_power_range_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_POWERRANGE_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_power_range_status_msg_t);
        p_msg = (gen_power_range_status_msg_t *)pt_req->parameter;
        p_msg->status_code = status_code;
        p_msg->range_min = p_gen_power_level_state->power_range_min_state;
        p_msg->range_max = p_gen_power_level_state->power_range_max_state;

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



