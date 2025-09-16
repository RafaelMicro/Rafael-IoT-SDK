/**
 * Copyright (c) 2021 All Rights Reserved.
 */
/** @file mesh_mdl_handler.c
 *
 * @author Rex
 * @version 0.1
 * @date 2021/10/19
 * @license
 * @description
 */

//=============================================================================
//                Include
//=============================================================================

/* user define element & model*/
#include "ble_mesh_element.h"
#include "mesh_mdl_handler.h"

//=============================================================================
//                Public Global Variables Declaration
//=============================================================================

//=============================================================================
//                Public Global Variables
//=============================================================================
extern ble_mesh_element_param_t g_element_info[];
extern light_lightness_state_t  el0_light_lightness_state;
extern gen_on_off_state_t       el1_gen_on_off_state;
//=============================================================================
//                Private Function
//=============================================================================

//=============================================================================
//                Public Function
//=============================================================================

void mmdl_init(void)
{
    ble_mesh_model_param_t  *p_model;
    uint32_t                i, j, k;
    uint16_t                primary_address;

    printf("BLE Mesh MMDL initial ...\n");

    primary_address = pib_primary_address_get();

    for (i = 0; i < pib_element_count_get(); i++)
    {
        g_element_info[i].element_address = primary_address + i;

        //set up the publish pointer and subscribe list pointer
        for (j = 0; j < g_element_info[i].element_models_count; j++)
        {
            p_model = g_element_info[i].p_model[j];
            for (k = 0; k < RAF_BLE_MESH_MODEL_BIND_LIST_SIZE; k++)
            {
                p_model->binding_list[k] = 0xffff;
            }
            switch (p_model->model_id)
            {
            case MMDL_GEN_ONOFF_SR_MDL_ID:
                mmdl_generic_onoff_sr_init((g_element_info + i), p_model);
                /* register the publish function */
                p_model->p_publish_info->p_mmdl_publish_func = mmdl_generic_onoff_publish_state;
                break;

            case MMDL_GEN_LEVEL_SR_MDL_ID:
                mmdl_generic_level_sr_init((g_element_info + i), p_model);
                /* register the publish function */
                p_model->p_publish_info->p_mmdl_publish_func = mmdl_generic_level_publish_state;
                break;

            case MMDL_SCENE_SR_MDL_ID:
                mmdl_scene_sr_init((g_element_info + i), p_model);
                break;

            case MMDL_LIGHT_LIGHTNESS_SR_MDL_ID:
            {
                mmdl_light_lightness_sr_init((g_element_info + i), p_model);
                /* register the publish function */
                p_model->p_publish_info->p_mmdl_publish_func = mmdl_light_lightness_publish_state;
            }
            break;
            }
        }
    }
}

void app_process_model_msg(mesh_app_mdl_evt_msg_idc_t *pt_msg_idc, ble_mesh_element_param_t *p_element, uint8_t is_broadcast)
{
    ble_mesh_model_param_t  *p_model;
    uint32_t                model_id, opcode;
    uint16_t                appkey_index, opcode_len;

    appkey_index = pt_msg_idc->appkey_index;

    opcode_len = (pt_msg_idc->opcode & 0xFFFF0000) ? 4 :
                 (pt_msg_idc->opcode & 0xFF00) ? 2 : 1;

    opcode = (opcode_len == 4) ? BE2LE32(pt_msg_idc->opcode) :
             (opcode_len == 2) ? BE2LE16(pt_msg_idc->opcode) : pt_msg_idc->opcode;

    switch (opcode)
    {
    //server model
    case MMDL_GEN_ONOFF_GET_OPCODE:
    case MMDL_GEN_ONOFF_SET_OPCODE:
    case MMDL_GEN_ONOFF_SET_NO_ACK_OPCODE:
        model_id = MMDL_GEN_ONOFF_SR_MDL_ID;
        if (search_model(p_element, model_id, &p_model))
        {
            if (mmdl_model_binding_key_validate(appkey_index, p_model))
            {
                if (!is_broadcast || (is_broadcast && mmdl_model_subscribe_address_validate(pt_msg_idc->dst_addr, p_model)))
                {
                    mmdl_generic_onoff_sr_handler(pt_msg_idc, p_element, p_model, is_broadcast);
                }
            }
        }
        break;

    case MMDL_GEN_LEVEL_GET_OPCODE:
    case MMDL_GEN_LEVEL_SET_OPCODE:
    case MMDL_GEN_LEVEL_SET_NO_ACK_OPCODE:
    case MMDL_GEN_LEVEL_DELTA_SET_OPCODE:
    case MMDL_GEN_LEVEL_DELTA_SET_NO_ACK_OPCODE:
    case MMDL_GEN_LEVEL_MOVE_SET_OPCODE:
    case MMDL_GEN_LEVEL_MOVE_SET_NO_ACK_OPCODE:
        model_id =  MMDL_GEN_LEVEL_SR_MDL_ID;
        if (search_model(p_element, model_id, &p_model))
        {
            if (mmdl_model_binding_key_validate(appkey_index, p_model))
            {
                if (!is_broadcast || (is_broadcast && mmdl_model_subscribe_address_validate(pt_msg_idc->dst_addr, p_model)))
                {
                    mmdl_generic_level_sr_handler(pt_msg_idc, p_element, p_model, is_broadcast);
                }
            }
        }
        break;

    case MMDL_SCENE_GET_OPCODE:
    case MMDL_SCENE_RECALL_OPCODE:
    case MMDL_SCENE_RECALL_NO_ACK_OPCODE:
    case MMDL_SCENE_REGISTER_GET_OPCODE:
        model_id =  MMDL_SCENE_SR_MDL_ID;
        if (search_model(p_element, model_id, &p_model))
        {
            if (mmdl_model_binding_key_validate(appkey_index, p_model))
            {
                if (!is_broadcast || (is_broadcast && mmdl_model_subscribe_address_validate(pt_msg_idc->dst_addr, p_model)))
                {
                    mmdl_scene_sr_handler(pt_msg_idc, p_element, p_model, is_broadcast);
                }
            }
        }
        break;

    case MMDL_SCENE_STORE_OPCODE:
    case MMDL_SCENE_STORE_NO_ACK_OPCODE:
    case MMDL_SCENE_DELETE_OPCODE:
    case MMDL_SCENE_DELETE_NO_ACK_OPCODE:
        model_id =  MMDL_SCENE_SETUP_SR_MDL_ID;
        if (search_model(p_element, model_id, &p_model))
        {
            if (mmdl_model_binding_key_validate(appkey_index, p_model))
            {
                if (!is_broadcast || (is_broadcast && mmdl_model_subscribe_address_validate(pt_msg_idc->dst_addr, p_model)))
                {
                    mmdl_scene_sr_handler(pt_msg_idc, p_element, p_model, is_broadcast);
                }
            }
        }
        break;

    case MMDL_LIGHT_LIGHTNESS_GET_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_SET_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_SET_NO_ACK_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_LINEAR_GET_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_LINEAR_SET_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_LINEAR_SET_NO_ACK_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_LAST_GET_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_DEFAULT_GET_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_RANGE_GET_OPCODE:
        model_id =  MMDL_LIGHT_LIGHTNESS_SR_MDL_ID;
        if (search_model(p_element, model_id, &p_model))
        {
            if (mmdl_model_binding_key_validate(appkey_index, p_model))
            {
                if (!is_broadcast || (is_broadcast && mmdl_model_subscribe_address_validate(pt_msg_idc->dst_addr, p_model)))
                {
                    mmdl_light_lightness_sr_handler(pt_msg_idc, p_element, p_model, is_broadcast);
                }
            }
        }
        break;

    case MMDL_LIGHT_LIGHTNESS_DEFAULT_SET_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_DEFAULT_SET_NO_ACK_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_RANGE_SET_OPCODE:
    case MMDL_LIGHT_LIGHTNESS_RANGE_SET_NO_ACK_OPCODE:
        model_id =  MMDL_LIGHT_LIGHTNESS_SETUP_SR_MDL_ID;
        if (search_model(p_element, model_id, &p_model))
        {
            if (mmdl_model_binding_key_validate(appkey_index, p_model))
            {
                if (!is_broadcast || (is_broadcast && mmdl_model_subscribe_address_validate(pt_msg_idc->dst_addr, p_model)))
                {
                    mmdl_light_lightness_sr_handler(pt_msg_idc, p_element, p_model, is_broadcast);
                }
            }
        }
        break;

    case MMDL_RAFAEL_TRSP_SET_OPCODE:
    case MMDL_RAFAEL_TRSP_SET_NO_ACK_OPCODE:
        model_id =  MMDL_RAFAEL_TRSP_SR_MDL_ID;
        if (search_model(p_element, model_id, &p_model))
        {
            if (mmdl_model_binding_key_validate(appkey_index, p_model))
            {
                if (!is_broadcast || (is_broadcast && mmdl_model_subscribe_address_validate(pt_msg_idc->dst_addr, p_model)))
                {
                    mmdl_rafael_trsp_sr_handler(pt_msg_idc, p_element, p_model, is_broadcast);
                }
            }
        }
        break;

    //client model
    case MMDL_RAFAEL_TRSP_STATUS_OPCODE:
        model_id =  MMDL_RAFAEL_TRSP_CL_MDL_ID;
        if (search_model(p_element, model_id, &p_model))
        {
            if (mmdl_model_binding_key_validate(appkey_index, p_model))
            {
                if (!is_broadcast || (is_broadcast && mmdl_model_subscribe_address_validate(pt_msg_idc->dst_addr, p_model)))
                {
                    mmdl_rafael_trsp_cl_handler(pt_msg_idc, p_element, p_model, is_broadcast);
                }
            }
        }
        break;
    default:
        printf("unsupport opcode 0x%08x \n", opcode);
        break;
    }
}

void app_process_element_lightness_model_state(uint16_t element_address, uint16_t state)
{
    uint8_t element_idx, duty;

    element_idx = element_address - pib_primary_address_get();

    printf("Set element[%d] act level %d\n", element_idx, state);
    if (element_idx == 0)
    {
        duty = 100-((state+1)/655);
        if ((state > 0) && (duty == 0))
        {
            duty = 1;
            duty = 1;
        }
        hosal_pwm_fmt0_duty(ELEMENT0_PWN_ID, duty);
    }
}

void app_process_element_scene_model_state(uint16_t element_address, uint8_t action, uint32_t *p_scene_state, void **p_extend_model_state_set)
{
    uint8_t element_idx;

    element_idx = element_address - pib_primary_address_get();

    if (action == SCENE_ACTION_STORE)
    {
        printf("Scene store, element addr 0x%04x\n", element_address);
        //update scene state by current state
        if (element_idx == 0)
        {
            *p_scene_state = el0_light_lightness_state.lightness_actual;
        }
        else if (element_idx == 1)
        {
            *p_scene_state = el1_gen_on_off_state.on_off_state;
        }
    }
    else if (action == SCENE_ACTION_RECALL)
    {
        //change current state by scene state
        printf("Scene recall, element addr 0x%04x state %d\n", element_address, *p_scene_state);

        if (element_idx == 0)
        {
            hosal_pwm_fmt0_duty(ELEMENT0_PWN_ID, 100-((*p_scene_state+1)/655));

            if (el0_light_lightness_state.lightness_actual != *p_scene_state)
            {
                el0_light_lightness_state.lightness_last = el0_light_lightness_state.lightness_actual;
                el0_light_lightness_state.lightness_actual = *p_scene_state;
            }
            *p_extend_model_state_set = mmdl_light_lightness_scene_set; /* the function to set state of extended model */
        }
        else if (element_idx == 1)
        {
            (*p_scene_state == 1)?hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 0):
                            hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 100);
            
            if (el1_gen_on_off_state.on_off_state != *p_scene_state)
            {
                el1_gen_on_off_state.on_off_state = *p_scene_state;
            }
        }
    }
    else if (action == SCENE_ACTION_DELETE)
    {
        printf("Scene delete, element addr 0x%04x state %d\n", element_address, *p_scene_state);
    }
    else
    {
        printf("Invalid scene action %d\n", action);
    }
}

void app_process_element_raf_trsp_sr_model_state(raf_trsp_cb_params_t *p_raf_trsp_cb_params)
{
    printf("Get Rafael TRSP data from address 0x%04x\n", p_raf_trsp_cb_params->src_addr);

    for (uint16_t i = 0; i < p_raf_trsp_cb_params->data_len; i++)
    {
        printf("%02x ", p_raf_trsp_cb_params->data[i]);
    }
    printf("\n");

}

void app_process_element_onoff_model_state(uint16_t element_address, uint16_t state)
{
    uint8_t element_idx;

    element_idx = element_address - pib_primary_address_get();

    if (element_idx == 1)
    {
        if(state == 1)
        {
            hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 0);
            printf("Set element[%d] on\n", element_idx);
        }
        else
        {
            hosal_pwm_fmt0_duty(ELEMENT1_PWN_ID, 100);
            printf("Set element[%d] off\n", element_idx);
        }
    }
}

