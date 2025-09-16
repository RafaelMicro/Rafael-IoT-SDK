/**
 * @file mmdl_gen_location_sr_main.c
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
#include "mmdl_gen_location_common.h"
#include "mmdl_gen_location_sr.h"

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
static void mmdl_generic_location_global_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_location_global_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_location_global_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_location_global_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

static void mmdl_generic_location_local_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_location_local_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_location_local_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_generic_location_local_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);

void mmdl_generic_location_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_GEN_LOCATION_GLOBAL_GET_OPCODE:
        mmdl_generic_location_global_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LOCATION_GLOBAL_SET_OPCODE:
        mmdl_generic_location_global_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LOCATION_GLOBAL_SET_NO_ACK_OPCODE:
        mmdl_generic_location_global_set(FALSE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LOCATION_LOCAL_GET_OPCODE:
        mmdl_generic_location_local_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LOCATION_LOCAL_SET_OPCODE:
        mmdl_generic_location_local_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_GEN_LOCATION_LOCAL_SET_NO_ACK_OPCODE:
        mmdl_generic_location_local_set(FALSE, p_evt_msg, p_element, p_model);
        break;
    }

}

static void mmdl_generic_location_global_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_location_global_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_generic_location_global_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_location_global_set_msg_t  *p_gen_location_global_set_msg = (gen_location_global_set_msg_t *)p_evt_msg->parameter;
    gen_location_state_t           *p_gen_location_state   = (gen_location_state_t *)p_model->p_state_var;
    uint32_t                       state_changed = 0;


    do
    {
        //check the state change
        if ((p_gen_location_state->global_latitude == p_gen_location_global_set_msg->global_latitude) &&
                (p_gen_location_state->global_longitude == p_gen_location_global_set_msg->global_longitude) &&
                (p_gen_location_state->global_altitude == p_gen_location_global_set_msg->global_altitude))
        {
            break;
        }

        p_gen_location_state->global_latitude = p_gen_location_global_set_msg->global_latitude;
        p_gen_location_state->global_longitude = p_gen_location_global_set_msg->global_longitude;
        p_gen_location_state->global_altitude = p_gen_location_global_set_msg->global_altitude;


    } while (0);

    if (ack_flag)
    {
        mmdl_generic_location_global_send_status(p_evt_msg, p_element, p_model);
    }

    if (state_changed)
    {
        mmdl_generic_location_global_publish_state(p_element, p_model);
    }


}

static void mmdl_generic_location_global_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_location_state_t              *p_gen_location_state = (gen_location_state_t *)p_model->p_state_var;
    gen_location_global_status_msg_t  *p_gen_location_global_status_msg;
    mesh_app_mdl_evt_msg_req_t        *pt_req;
    mesh_tlv_t                        *pt_tlv;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_location_global_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_location_global_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_LOCATION_GLOBAL_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_location_global_status_msg_t);
        p_gen_location_global_status_msg = (gen_location_global_status_msg_t *)pt_req->parameter;
        p_gen_location_global_status_msg->global_latitude = p_gen_location_state->global_latitude;
        p_gen_location_global_status_msg->global_longitude = p_gen_location_state->global_longitude;
        p_gen_location_global_status_msg->global_altitude = p_gen_location_state->global_altitude;

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


void mmdl_generic_location_global_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv = NULL;;
    gen_location_state_t        *p_gen_location_state = (gen_location_state_t *)p_model->p_state_var;
    uint16_t                    element_address = p_element->element_address;
    uint16_t                    publish_address = p_model->p_publish_info->address;
    uint8_t                     ttl = p_model->p_publish_info->publish_ttl;
    uint32_t                    parameter_size = sizeof(gen_location_global_status_msg_t);

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
        pt_req->appkey_index = p_model->p_publish_info->app_key_index;
        pt_req->ttl = ttl;
        pt_req->parameter_len = parameter_size;

        gen_location_global_status_msg_t *p_msg = (gen_location_global_status_msg_t *)pt_req->parameter;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_LOCATION_GLOBAL_STATUS_OPCODE);
        p_msg->global_latitude = p_gen_location_state->global_latitude;
        p_msg->global_longitude = p_gen_location_state->global_longitude;
        p_msg->global_altitude = p_gen_location_state->global_altitude;

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

static void mmdl_generic_location_local_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_generic_location_global_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_generic_location_local_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    gen_location_local_set_msg_t  *p_gen_location_local_set_msg = (gen_location_local_set_msg_t *)p_evt_msg->parameter;
    gen_location_state_t          *p_gen_location_state   = (gen_location_state_t *)p_model->p_state_var;
    uint32_t                      state_changed = 0;


    do
    {
        //check the state change
        if ((p_gen_location_state->local_north == p_gen_location_local_set_msg->local_north) &&
                (p_gen_location_state->local_east == p_gen_location_local_set_msg->local_east) &&
                (p_gen_location_state->local_altitude == p_gen_location_local_set_msg->local_altitude) &&
                (p_gen_location_state->floor_number == p_gen_location_local_set_msg->floor_number) &&
                (p_gen_location_state->uncertainty == p_gen_location_local_set_msg->uncertainty))
        {
            break;
        }

        p_gen_location_state->local_north = p_gen_location_local_set_msg->local_north;
        p_gen_location_state->local_east = p_gen_location_local_set_msg->local_east;
        p_gen_location_state->local_altitude = p_gen_location_local_set_msg->local_altitude;
        p_gen_location_state->floor_number = p_gen_location_local_set_msg->floor_number;
        p_gen_location_state->uncertainty = p_gen_location_local_set_msg->uncertainty;


    } while (0);

    if (ack_flag)
    {
        mmdl_generic_location_local_send_status(p_evt_msg, p_element, p_model);
    }

    if (state_changed)
    {
        mmdl_generic_location_local_publish_state(p_element, p_model);
    }


}

static void mmdl_generic_location_local_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t       *pt_req;
    mesh_tlv_t                       *pt_tlv;
    gen_location_state_t             *p_gen_location_state = (gen_location_state_t *)p_model->p_state_var;
    gen_location_local_status_msg_t  *p_gen_location_local_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_location_local_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(gen_location_local_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;

        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_LOCATION_LOCAL_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(gen_location_local_status_msg_t);
        p_gen_location_local_status_msg = (gen_location_local_status_msg_t *)pt_req->parameter;
        p_gen_location_local_status_msg->local_north = p_gen_location_state->local_north;
        p_gen_location_local_status_msg->local_east = p_gen_location_state->local_east;
        p_gen_location_local_status_msg->local_altitude = p_gen_location_state->local_altitude;
        p_gen_location_local_status_msg->floor_number = p_gen_location_state->floor_number;
        p_gen_location_local_status_msg->uncertainty = p_gen_location_state->uncertainty;

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


void mmdl_generic_location_local_publish_state(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv = NULL;;
    gen_location_state_t        *p_gen_location_state = (gen_location_state_t *)p_model->p_state_var;
    uint16_t                    element_address = p_element->element_address;
    uint16_t                    publish_address = p_model->p_publish_info->address;
    uint8_t                     ttl = p_model->p_publish_info->publish_ttl;
    uint32_t                    parameter_size = sizeof(gen_location_local_status_msg_t);

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

        gen_location_local_status_msg_t *p_msg = (gen_location_local_status_msg_t *)pt_req->parameter;
        pt_req->opcode_invalid = 0;
        pt_req->opcode = BE2LE16(MMDL_GEN_LOCATION_LOCAL_STATUS_OPCODE);
        p_msg->local_north = p_gen_location_state->local_north;
        p_msg->local_east = p_gen_location_state->local_east;
        p_msg->local_altitude = p_gen_location_state->local_altitude;
        p_msg->floor_number = p_gen_location_state->floor_number;
        p_msg->uncertainty = p_gen_location_state->uncertainty;

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


