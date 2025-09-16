/**
 * @file mmdl_time_sr_main.c
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
#include "mmdl_time_common.h"
#include "mmdl_time_sr.h"

//=============================================================================
//                Private Definitions of const value
//=============================================================================


//=============================================================================
//                Private Global Variables
//=============================================================================

/* store the useful data pointer for futher using */

static uint8_t              ttl = 4;
const time_state_t          MMDL_TIME_STATE_DEFAULT = {0, 0, 0, 0, 0, 0, 0, TIME_DELTA_SECONDS_UTC_TAI_2016,
                                                       TIME_DELTA_SECONDS_UTC_TAI_2016, 0, 0
                                                      };
const uint16_t              MMDL_DAYS_ACCU_PER_MONTH_TBL[2][12] =
{
    {31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366},
    {31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
};
const uint8_t               MMDL_DAYS_PER_MONTH_TBL[2][12] =
{
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

//=============================================================================
//                Private Functions Declaration
//=============================================================================
static void mmdl_time_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_time_status(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_time_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
//static void mmdl_time_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_time_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timezone_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timezone_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timezone_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timedelta_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timedelta_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timedelta_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timerole_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timerole_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_timerole_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);


void mmdl_time_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{
    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_TIME_GET_OPCODE:
        mmdl_time_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_TIME_STATUS_OPCODE:
        mmdl_time_status(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_TIMEZONE_GET_OPCODE:
        mmdl_timezone_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_TIMEDELTA_GET_OPCODE:
        mmdl_timedelta_get(p_evt_msg, p_element, p_model);
        break;

    default:
        break;
    }
}


void mmdl_time_setup_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{

    /* store the data in private variables */
    ttl                     = 4;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_TIME_SET_OPCODE:
        mmdl_time_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_TIMEZONE_SET_OPCODE:
        mmdl_timezone_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_TIMEDELTA_SET_OPCODE:
        mmdl_timedelta_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_TIMEROLE_GET_OPCODE:
        mmdl_timerole_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_TIMEROLE_SET_OPCODE:
        mmdl_timerole_set(TRUE, p_evt_msg, p_element, p_model);
        break;

    default:
        break;
    }
}

static void mmdl_time_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_time_UTC_to_TAI_update(p_element, p_model);
    mmdl_time_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_time_status(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    time_state_t       *p_time_state = (time_state_t *)p_model->p_state_var;
    time_status_msg_t  *p_time_status_msg = (time_status_msg_t *)p_evt_msg->parameter - 1;   //1: sizeof(time_status_msg_sec0_t.dummy)
    bool               send_status = false;

    do
    {
        switch (p_time_state->time_role)
        {
        case MMDL_TIME_SRV_TIME_ROLE_MESH_TIME_RELAY:
            send_status = true;
        case MMDL_TIME_SRV_TIME_ROLE_MESH_TIME_CLIENT:
            p_time_state->tai_seconds = p_time_status_msg->msg_sec0.tai_seconds;
            p_time_state->subsecond = p_time_status_msg->msg_sec0.subsecond;
            p_time_state->uncertainty = p_time_status_msg->msg_sec0.uncertainty;
            p_time_state->time_authority = p_time_status_msg->msg_sec1.time_authority;
            p_time_state->tai_utc_delta = p_time_status_msg->msg_sec1.tai_utc_delta + TIME_OFFSET_UTC_TAI_DELTA;
            p_time_state->time_zone_offset = p_time_status_msg->msg_sec1.time_zone_offset + TIME_OFFSET_ZONE_15MIN;
            mmdl_time_TAI_to_UTC_update(p_element, p_model);
            break;

        case MMDL_TIME_SRV_TIME_ROLE_NONE:
        case MMDL_TIME_SRV_TIME_ROLE_MESH_TIME_AUTHORITY:
        default:
            break;
        }
        if (send_status == true)
        {
            //To Do, using TTL=0 if the value of the Time Role state is 0x02 (Time Relay) and the Publish Address for the Time Server model is not set to unassigned address.
            mmdl_time_send_status(p_evt_msg, p_element, p_model);
        }
    } while (0);
}


static void mmdl_time_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    time_state_t       *p_time_state = (time_state_t *)p_model->p_state_var;
    time_status_msg_t  *p_time_status_msg = (time_status_msg_t *)p_evt_msg->parameter - 1;   //1: sizeof(time_status_msg_sec0_t.dummy)

    do
    {
        p_time_state->tai_seconds = p_time_status_msg->msg_sec0.tai_seconds;
        p_time_state->subsecond = p_time_status_msg->msg_sec0.subsecond;
        p_time_state->uncertainty = p_time_status_msg->msg_sec0.uncertainty;
        p_time_state->time_authority = p_time_status_msg->msg_sec1.time_authority;
        p_time_state->tai_utc_delta = p_time_status_msg->msg_sec1.tai_utc_delta + TIME_OFFSET_UTC_TAI_DELTA;
        p_time_state->time_zone_offset = p_time_status_msg->msg_sec1.time_zone_offset + TIME_OFFSET_ZONE_15MIN;
        mmdl_time_TAI_to_UTC_update(p_element, p_model);
    } while (0);

    if (ack_flag)
    {
        mmdl_time_send_status(p_evt_msg, p_element, p_model);
    }
}

static void mmdl_time_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    time_state_t                *p_time_state = (time_state_t *)p_model->p_state_var;
    time_status_msg_t           *p_time_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(time_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(time_status_msg_t) - 2 * 8;    //2: 2*dummy

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;
        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE16(MMDL_TIME_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(time_status_msg_t) - 2 * 8;          //2: 2*dummy
        p_time_status_msg = (time_status_msg_t *)pt_req->parameter - 1;   //1: dummy
        p_time_status_msg->msg_sec0.tai_seconds = p_time_state->tai_seconds;
        p_time_status_msg->msg_sec0.subsecond = p_time_state->subsecond;
        p_time_status_msg->msg_sec0.uncertainty = p_time_state->uncertainty;
        p_time_status_msg->msg_sec1.time_authority = p_time_state->time_authority;
        p_time_status_msg->msg_sec1.tai_utc_delta = p_time_state->tai_utc_delta - TIME_OFFSET_UTC_TAI_DELTA;
        p_time_status_msg->msg_sec1.time_zone_offset = p_time_state->time_zone_offset - TIME_OFFSET_ZONE_15MIN;

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


static void mmdl_timezone_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_timezone_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_timezone_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    time_state_t        *p_time_state = (time_state_t *)p_model->p_state_var;
    timezone_set_msg_t  *p_timezone_set_msg = (timezone_set_msg_t *)p_evt_msg->parameter;

    do
    {
        mmdl_time_UTC_to_TAI_update(p_element, p_model);
        p_time_state->time_zone_offset_new = p_timezone_set_msg->time_zone_offset_new + TIME_OFFSET_ZONE_15MIN;
        p_time_state->tai_zone_change = p_timezone_set_msg->tai_zone_change;
        mmdl_time_TAI_to_UTC_update(p_element, p_model);
    } while (0);

    if (ack_flag)
    {
        mmdl_timezone_send_status(p_evt_msg, p_element, p_model);
    }
}

static void mmdl_timezone_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    time_state_t                *p_time_state = (time_state_t *)p_model->p_state_var;
    timezone_status_msg_t       *p_timezone_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(timezone_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(timezone_status_msg_t) - 1;      //1: dummy

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;
        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE16(MMDL_TIMEZONE_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(timezone_status_msg_t) - 1;      //1: dummy
        p_timezone_status_msg = (timezone_status_msg_t *)pt_req->parameter;
        p_timezone_status_msg->time_zone_offset = p_time_state->time_zone_offset - TIME_OFFSET_ZONE_15MIN;
        p_timezone_status_msg->time_zone_offset_new = p_time_state->time_zone_offset_new - TIME_OFFSET_ZONE_15MIN;
        p_timezone_status_msg->tai_zone_change = p_time_state->tai_zone_change;

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


static void mmdl_timedelta_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_timedelta_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_timedelta_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    time_state_t         *p_time_state = (time_state_t *)p_model->p_state_var;
    timedelta_set_msg_t  *p_timedelta_set_msg = (timedelta_set_msg_t *)p_evt_msg->parameter;

    do
    {
        mmdl_time_UTC_to_TAI_update(p_element, p_model);
        p_time_state->tai_utc_delta_new = p_timedelta_set_msg->upcoming.tai_utc_delta_new + TIME_OFFSET_UTC_TAI_DELTA;
        p_time_state->tai_delta_change = p_timedelta_set_msg->upcoming.tai_delta_change;
        mmdl_time_TAI_to_UTC_update(p_element, p_model);
    } while (0);

    if (ack_flag)
    {
        mmdl_timedelta_send_status(p_evt_msg, p_element, p_model);
    }
}


static void mmdl_timedelta_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    time_state_t                *p_time_state = (time_state_t *)p_model->p_state_var;
    timedelta_status_msg_t      *p_timedelta_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(timedelta_status_msg_t) - 1);    //1: dummy
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(timedelta_status_msg_t) - 1;   //1: dummy

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;
        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE16(MMDL_TIMEZONE_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(timedelta_status_msg_t) - 1;                                 //1: dummy
        p_timedelta_status_msg = (timedelta_status_msg_t *)pt_req->parameter;
        p_timedelta_status_msg->current.tai_utc_delta = p_time_state->tai_utc_delta - TIME_OFFSET_UTC_TAI_DELTA;
        p_timedelta_status_msg->upcoming.tai_utc_delta_new = p_time_state->tai_utc_delta_new - TIME_OFFSET_UTC_TAI_DELTA;
        p_timedelta_status_msg->upcoming.tai_delta_change = p_time_state->tai_delta_change;

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

static void mmdl_timerole_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_timerole_send_status(p_evt_msg, p_element, p_model);
}


static void mmdl_timerole_set(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    time_state_t        *p_time_state = (time_state_t *)p_model->p_state_var;
    timerole_set_msg_t  *p_timerole_set_msg = (timerole_set_msg_t *)p_evt_msg->parameter;

    do
    {
        p_time_state->time_role = p_timerole_set_msg->time_role;
    } while (0);

    if (ack_flag)
    {
        mmdl_timerole_send_status(p_evt_msg, p_element, p_model);
    }
}

static void mmdl_timerole_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    time_state_t                *p_time_state = (time_state_t *)p_model->p_state_var;
    timerole_status_msg_t       *p_timerole_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(timerole_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(timerole_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;
        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE16(MMDL_TIMEROLE_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(timerole_status_msg_t);
        p_timerole_status_msg = (timerole_status_msg_t *)pt_req->parameter;
        p_timerole_status_msg->time_role = p_time_state->time_role;

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


void time_UTC_to_TAI(rtc_time_t *utc_time, time_state_t *p_time_state)
{
    //time_state_t            *p_time_state = (time_state_t *)p_model->p_state_var;
    //rtc_time_t current_time;
    int32_t sec_delta;
    uint32_t days;
    uint32_t q32;       //quotient
    uint32_t r32;       //remainder;
    uint8_t i, j, leap_offset = 1;

    //rtc_get_time(&current_time);
    //if (current_time.tm_year < (p_time_state->UTC_year % 100))    //Year from xy99 to xz00
    {
        //p_time_state->UTC_year++;
    }
    q32 = utc_time->tm_year / 400;
    r32 = utc_time->tm_year % 400;
    days = q32 * TIME_CONV_DAYS_400Y;

    q32 = r32 / 100;
    r32 = r32 % 100;
    days += (q32 * TIME_CONV_DAYS_100Y);

    if ((q32 != 0) && (r32 < 4))    //100(but not 400) year integral multiple
    {
        leap_offset = 0;
    }

    q32 = r32 / 4;
    r32 = r32 % 4;
    days += (q32 * TIME_CONV_DAYS_4Y);
    days += (r32 * TIME_CONV_DAYS_Y);

    if (r32 == 0)
    {
        j = 0;      //leap year
    }
    else
    {
        j = 1;
        days += leap_offset;
    }

    q32 = (utc_time->tm_mon - 1);
    i = 0;
    while (i < q32)
    {
        days += MMDL_DAYS_PER_MONTH_TBL[j][i];
        i++;
    }
    days += (utc_time->tm_day - 1);
    //Why(-1): 2000/01/01 23:00:00 => 0y0m0d 23h0m0s

    p_time_state->tai_seconds += (days * TIME_CONV_SECONDS_D);
    p_time_state->tai_seconds += (utc_time->tm_hour * TIME_CONV_SECONDS_H);
    p_time_state->tai_seconds += (utc_time->tm_min * TIME_CONV_SECONDS_M);
    //p_time_state->tai_seconds += (current_time.tm_sec + TIME_DELTA_SECONDS_UTC_TAI_2016);
    p_time_state->tai_seconds += utc_time->tm_sec;

    sec_delta = p_time_state->tai_utc_delta - (p_time_state->time_zone_offset * TIME_CONV_SECONDS_ZONE);
    //p_time_state->tai_seconds += sec_delta;

    if ((p_time_state->tai_seconds + sec_delta) >= 1)
    {
        p_time_state->tai_seconds += sec_delta;
    }
    else
    {
        p_time_state->tai_seconds = 1;
    }
    //days =
}



/* rtc_arm_isr is RTC user interrupt, please do NOT call any block function
 * And you should design the code as short as possible.
 * Here we demo here is a very bad example.
 */

void rtc_arm_isr(uint32_t rtc_status)
{
    rtc_time_t  current_time, alarm_tm;
    uint32_t    alarm_mode;
    int        show_time_info = 0;
    static  int  first = 1;

    rtc_get_time(&current_time);

    /*check status to know why interrupt source*/
    if (rtc_status & RTC_INT_SECOND_BIT)
    {
        if (first)
        {
            /* In our test design, the default clk is 32000,
             * we should wait it to end. All code here is for test only.
             * It will not (and should not) used in real product.
             */
            first = 0;

            alarm_tm.tm_year = 20;
            alarm_tm.tm_mon = 03;
            alarm_tm.tm_day = 21;
            alarm_tm.tm_hour = 12;
            alarm_tm.tm_min = 30;
            alarm_tm.tm_sec = 48;

            /*we hope to get interrupt at
                      XX:XX:00  -- every new second.
                      XX:30:00  -- for every hour
                      12:00:00  -- for every day   --RTC_MODE_MATCH_HOUR_INTERRUPT
                      00:00:00 -- for every day    --RTC_MODE_EVERY_DAY_INTERRUPT

                   01 00:00:00 -- for every month
                01-01 00:00:00 -- for every year
                And last we hope to get
                      XX:30:48 ... for event interrupt.
             */

            alarm_mode = RTC_MODE_EVERY_SEC_INTERRUPT  | RTC_MODE_EN_SEC_INTERRUPT  |
                         RTC_MODE_MATCH_MIN_INTERRUPT  | RTC_MODE_EN_MIN_INTERRUPT  |
                         RTC_MODE_MATCH_HOUR_INTERRUPT | RTC_MODE_EN_HOUR_INTERRUPT |
                         RTC_MODE_EVERY_DAY_INTERRUPT  | RTC_MODE_EN_DAY_INTERRUPT  |
                         RTC_MODE_EVERY_MONTH_INTERRUPT | RTC_MODE_EN_MONTH_INTERRUPT |
                         RTC_MODE_EVERY_YEAR_INTERRUPT | RTC_MODE_EN_YEAR_INTERRUPT |
                         RTC_MODE_HOUR_EVENT_INTERRUPT | RTC_MODE_EVENT_INTERRUPT ;


            rtc_set_alarm(&alarm_tm, alarm_mode, rtc_arm_isr);

        }
        else
        {
            /*we don't show meesage.. because too many to show*/
            //gpio_pin_toggle(GPIO29);
        }
    }

    if (rtc_status & RTC_INT_MINUTE_BIT)
    {
        show_time_info = 1;
    }

    if (rtc_status & RTC_INT_HOUR_BIT)
    {
        show_time_info = 1;
    }

    if (rtc_status & RTC_INT_DAY_BIT)
    {
        show_time_info = 1;
    }


    if (rtc_status & RTC_INT_MONTH_BIT)
    {
        show_time_info = 1;
    }

    if (rtc_status & RTC_INT_YEAR_BIT)
    {
        show_time_info = 1;
    }

    if (rtc_status & RTC_INT_EVENT_BIT)
    {
        show_time_info = 1;
    }

    if (show_time_info)
    {
    }

    return;
}


void mmdl_time_sr_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    time_state_t *p_time_state = (time_state_t *)p_model->p_state_var;

    //p_time_state->tai_seconds = 0;        //At begining, the element does not know the time, a special value of 0x0000000000 is used.
    //p_time_state->tai_zone_change = 0;    //When an element does not know the TAI of Delta Change, a special value of 0x0000000000 is used.
    //p_time_state->tai_delta_change = 0;   //When an element does not know the TAI of Zone Change, a special value of 0x0000000000 is used.
    if (search_model(p_element, MMDL_TIME_SR_MDL_ID, &p_model))
    {
        *p_time_state = MMDL_TIME_STATE_DEFAULT;
        rtc_time_t  current_time, alarm_tm;
        uint32_t    alarm_mode;

        rtc_set_clk(39990);                     //Internal 40khz, related to the HW.
        rtc_get_time(&current_time);
        mmdl_time_UTC_set_user(current_time, 0, p_model);

        alarm_mode = 0;
        rtc_set_alarm(&alarm_tm, alarm_mode, rtc_arm_isr);
    }
}

void mmdl_time_UTC_set_user(rtc_time_t current_time, int8_t time_zone, ble_mesh_model_param_t *p_model)   //time_zone is from -12 to 12, by hours.
{
    time_state_t  *p_time_state = (time_state_t *)p_model->p_state_var;

    *p_time_state = MMDL_TIME_STATE_DEFAULT;
    p_time_state->time_zone_offset = (time_zone * (TIME_CONV_SECONDS_UTC_ZONE / TIME_CONV_SECONDS_ZONE));
    time_UTC_to_TAI(&current_time, p_time_state);
    p_time_state->UTC_year = current_time.tm_year;
    current_time.tm_year %= 100;                        //RTC year reg. is 0~99
    rtc_set_time(&current_time);
}

void mmdl_time_UTC_to_TAI_update(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    time_state_t  *p_time_state = (time_state_t *)p_model->p_state_var;
    rtc_time_t    current_time;

    rtc_get_time(&current_time);
    if (current_time.tm_year < (p_time_state->UTC_year % 100))    //Year from xy99 to xz00
    {
        p_time_state->UTC_year++;
    }
    current_time.tm_year = p_time_state->UTC_year;
    time_UTC_to_TAI(&current_time, p_time_state);
}

void mmdl_time_TAI_to_UTC_update(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    time_state_t  *p_time_state = (time_state_t *)p_model->p_state_var;
    rtc_time_t    current_time;
    int32_t       sec_delta;
    uint32_t      days;
    uint32_t      q32;       //quotient
    uint32_t      r32;       //remainder;
    uint8_t       i, j, leap_offset = 1;

    sec_delta = (p_time_state->time_zone_offset * TIME_CONV_SECONDS_ZONE) - p_time_state->tai_utc_delta;

    days = ((p_time_state->tai_seconds + sec_delta) / TIME_CONV_SECONDS_D);
    r32 = ((p_time_state->tai_seconds + sec_delta) % TIME_CONV_SECONDS_D);

    q32 = (r32 / TIME_CONV_SECONDS_H);
    r32 = (r32 % TIME_CONV_SECONDS_H);
    current_time.tm_hour = q32;

    q32 = (r32 / TIME_CONV_SECONDS_M);
    r32 = (r32 % TIME_CONV_SECONDS_M);
    current_time.tm_min = r32;
    current_time.tm_sec = q32;

    q32 = (days / TIME_CONV_DAYS_400Y);
    r32 = (days % TIME_CONV_DAYS_400Y);
    p_time_state->UTC_year = q32 * 400;

    q32 = (r32 / TIME_CONV_DAYS_100Y);
    r32 = (r32 % TIME_CONV_DAYS_100Y);
    p_time_state->UTC_year += (q32 * 100);

    if (q32 != 0)       //400x year integral multiple _(1/2)
    {
        leap_offset = 0;
    }

    q32 = (r32 / TIME_CONV_DAYS_4Y);
    r32 = (r32 % TIME_CONV_DAYS_4Y);
    p_time_state->UTC_year += (q32 * 4);
    current_time.tm_year = (q32 * 4);
    if (q32 != 0)       //100x (but not 400) year integral multiple _(2/2)
    {
        leap_offset = 1;
    }
    if ((r32 > TIME_CONV_DAYS_Y) && (leap_offset != 0))
    {
        r32 -= 1;
    }

    q32 = (r32 / TIME_CONV_DAYS_Y);
    r32 = (r32 % TIME_CONV_DAYS_Y);
    p_time_state->UTC_year += q32;
    current_time.tm_year += q32;

    if (q32 == 0)
    {
        j = 0;
    }
    else
    {
        j = 1;
    }
    for (i = 0; i < 12; i++)
    {
        if (r32 > MMDL_DAYS_PER_MONTH_TBL[j][i])
        {
            r32 -= MMDL_DAYS_PER_MONTH_TBL[j][i];
        }
        else
        {
            break;
        }
        i++;
    }
    if (i == 12)
    {
        //error!
    }
    else
    {
        //current_time.tm_mon = (i + 1);
        //current_time.tm_day = r32;
        if (r32 == 0)
        {
            current_time.tm_mon = (i + 2);
        }
        else
        {
            current_time.tm_mon = (i + 1);
        }
        current_time.tm_day = (r32 + 1);
    }
    rtc_set_time(&current_time);
}


