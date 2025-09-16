/**
 * @file mmdl_schedular_sr_main.c
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
#include "mmdl_scheduler_common.h"
#include "mmdl_scheduler_sr.h"

//=============================================================================
//                Private Definitions of const value
//=============================================================================


//=============================================================================
//                Private Global Variables
//=============================================================================

/* store the useful data pointer for futher using */

static uint8_t                      ttl = 4;
const schedule_reg_state_t          MMDL_SCHEDULE_REG_DEFAULT = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MMDL_SCHEDULE_REG_ACTION_NO_ACTION, 0, 0, 0};

//=============================================================================
//                Private Functions Declaration
//=============================================================================
static void mmdl_scheduler_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scheduler_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scheduler_action_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scheduler_action_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scheduler_action_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

void mmdl_scheduler_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t in_is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_SCHEDULER_ACTION_GET_OPCODE:
        mmdl_scheduler_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCHEDULER_GET_OPCODE:
        mmdl_scheduler_action_get(p_evt_msg, p_element, p_model);
        break;

    default:
        break;
    }

}

void mmdl_scheduler_setup_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_SCHEDULER_ACTION_SET_OPCODE:
        mmdl_scheduler_action_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCHEDULER_ACTION_SET_NO_ACK_OPCODE:
        mmdl_scheduler_action_set(FALSE, p_evt_msg, p_element, p_model);
        break;

    default:
        break;
    }
}


static void mmdl_scheduler_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_scheduler_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_scheduler_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    scheduler_state_t           *p_scheduler_state = (scheduler_state_t *)p_model->p_state_var;
    scheduler_status_msg_t      *p_scheduler_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(scheduler_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(scheduler_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;
        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE16(MMDL_SCHEDULER_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(scheduler_status_msg_t);
        p_scheduler_status_msg = (scheduler_status_msg_t *)pt_req->parameter;
        p_scheduler_status_msg->schedules = p_scheduler_state->schedules;

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


static void mmdl_scheduler_action_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    scheduler_action_get_msg_t  *p_scheduler_action_get_msg = (scheduler_action_get_msg_t *)p_model->p_state_var;

    if (p_scheduler_action_get_msg->index < MMDL_SCHEDULE_REG_ENTRY_NUM)
    {
        mmdl_scheduler_action_send_status(p_evt_msg, p_element, p_model);
    }
}


static void mmdl_scheduler_action_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    scheduler_state_t           *p_scheduler_state = (scheduler_state_t *)p_model->p_state_var;
    scheduler_action_set_msg_t  *p_scheduler_action_set_msg = (scheduler_action_set_msg_t *)p_evt_msg->parameter;
    uint16_t index;

    do
    {
        index = p_scheduler_action_set_msg->time_reg.index;
        p_scheduler_state->schedule_reg_state[index].year = p_scheduler_action_set_msg->time_reg.year;
        p_scheduler_state->schedule_reg_state[index].months = p_scheduler_action_set_msg->time_reg.months;
        p_scheduler_state->schedule_reg_state[index].day = p_scheduler_action_set_msg->time_reg.day;
        p_scheduler_state->schedule_reg_state[index].hour = p_scheduler_action_set_msg->time_reg.hour;
        p_scheduler_state->schedule_reg_state[index].minute = p_scheduler_action_set_msg->time_reg.minute;
        p_scheduler_state->schedule_reg_state[index].second = p_scheduler_action_set_msg->time_reg.second;
        p_scheduler_state->schedule_reg_state[index].days_of_week = p_scheduler_action_set_msg->time_reg.days_of_week;
        p_scheduler_state->schedule_reg_state[index].action = p_scheduler_action_set_msg->time_reg.action;
        p_scheduler_state->schedule_reg_state[index].trans_time = p_scheduler_action_set_msg->time_reg.trans_time;
        p_scheduler_state->schedule_reg_state[index].scene_number = p_scheduler_action_set_msg->scene_number;

        index = (0x0001 << index);
        if (p_scheduler_state->schedule_reg_state[index].action != MMDL_SCHEDULE_REG_ACTION_NO_ACTION)
        {
            p_scheduler_state->schedules |= index;
        }
        else
        {
            p_scheduler_state->schedules &= (~index);
        }
    } while (0);

    if (ack_flag)
    {
        mmdl_scheduler_action_send_status(p_evt_msg, p_element, p_model);
    }
}

static void mmdl_scheduler_action_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t     *pt_req;
    mesh_tlv_t                     *pt_tlv;
    scheduler_state_t              *p_scheduler_state = (scheduler_state_t *)p_model->p_state_var;
    scheduler_action_status_msg_t  *p_scheduler_action_status_msg;
    uint8_t                        index;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(scheduler_action_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(scheduler_action_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;
        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE16(MMDL_SCHEDULER_ACTION_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(scheduler_action_status_msg_t);
        p_scheduler_action_status_msg = (scheduler_action_status_msg_t *)pt_req->parameter;
        index = p_scheduler_action_status_msg->time_reg.index;
        p_scheduler_action_status_msg->time_reg.year = p_scheduler_state->schedule_reg_state[index].year;
        p_scheduler_action_status_msg->time_reg.months = p_scheduler_state->schedule_reg_state[index].months;
        p_scheduler_action_status_msg->time_reg.day = p_scheduler_state->schedule_reg_state[index].day;
        p_scheduler_action_status_msg->time_reg.hour = p_scheduler_state->schedule_reg_state[index].hour;
        p_scheduler_action_status_msg->time_reg.minute = p_scheduler_state->schedule_reg_state[index].minute;
        p_scheduler_action_status_msg->time_reg.second = p_scheduler_state->schedule_reg_state[index].second;
        p_scheduler_action_status_msg->time_reg.days_of_week = p_scheduler_state->schedule_reg_state[index].days_of_week;
        p_scheduler_action_status_msg->time_reg.action = p_scheduler_state->schedule_reg_state[index].action;
        p_scheduler_action_status_msg->time_reg.trans_time = p_scheduler_state->schedule_reg_state[index].trans_time;
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

void mmdl_scheduler_sr_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    scheduler_state_t  *p_scheduler_state = (scheduler_state_t *)p_model->p_state_var;
    uint32_t           i32;

    if (search_model(p_element, MMDL_SCHEDULER_SR_MDL_ID, &p_model))
    {
        i32 = MMDL_SCHEDULE_REG_ENTRY_NUM;
        while (1)
        {
            i32--;
            p_scheduler_state->schedule_reg_state[i32] = MMDL_SCHEDULE_REG_DEFAULT;
            p_scheduler_state->schedule_reg_state[i32].Next = i32 + 1;
            if (i32 == 0)
            {
                break;
            }
        }
        p_scheduler_state->schedules = 0;
    }
}


