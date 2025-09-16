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
#include <math.h>
#include "ble_mesh_element.h"
#include "mmdl_common.h"
#include "mesh_mdl_handler.h"

uint8_t tid = 0;

//=============================================================================
//                Private Definitions of const value
//=============================================================================

extern ble_mesh_element_param_t g_element_info[];
//static ble_mesh_element_param_t *p_element_base = g_element_info;

void mmdl_evt_msg_cb(mesh_app_mdl_evt_msg_idc_t *pt_msg_idc)
{
    ble_mesh_element_param_t  *p_element;
    uint32_t                  i, element_idx;
    uint16_t                  primary_address;
    uint8_t                   is_broadcast = false;

    do
    {
        primary_address = pib_primary_address_get();

        /* the multicast addresses have to check 2 segments, one is group address, the other is virtual address */
        /* virtual address range : 0x8000 - 0xBFFF */
        /* group address range : 0xC000 - 0xFEFF  */
        is_broadcast = (pt_msg_idc->dst_addr >= 0x8000) ? true : false;

        if (is_broadcast)
        {
            if ((pt_msg_idc->dst_addr == MESH_FIXED_GROUP_ADDR_ALL_FRIENDS) ||
                    (pt_msg_idc->dst_addr == MESH_FIXED_GROUP_ADDR_ALL_RELAYS) ||
                    (pt_msg_idc->dst_addr == MESH_FIXED_GROUP_ADDR_ALL_NODES))
            {
                p_element = (g_element_info + 0);
                app_process_model_msg(pt_msg_idc, p_element, is_broadcast);
            }
            else
            {
                for (i = 0; i < pib_element_count_get(); i++)
                {
                    //the broadcast address, process all elements
                    p_element = (g_element_info + i);
                    app_process_model_msg(pt_msg_idc, p_element, is_broadcast);
                }
            }
        }
        else if (pt_msg_idc->dst_addr >= primary_address && pt_msg_idc->dst_addr < (primary_address + pib_element_count_get()))
        {
            // an unicast address match
            element_idx = (uint32_t)(pt_msg_idc->dst_addr - primary_address);
            p_element = (g_element_info + element_idx);
            app_process_model_msg(pt_msg_idc, p_element, is_broadcast);
        }

    } while (0);
}

uint32_t search_model(ble_mesh_element_param_t *p_element, uint32_t model_id, ble_mesh_model_param_t **p_model)
{
    uint32_t i, ret = 0;
    ble_mesh_model_param_t *p_model_param;

    for (i = 0; i < p_element->element_models_count; i++)
    {
        p_model_param = p_element->p_model[i];
        if (p_model_param->model_id == model_id)
        {
            *p_model = p_model_param;
            ret = 1;
            break;
        }
    }

    return ret;
}

uint32_t mmdl_model_binding_key_validate(uint16_t appkey_index, ble_mesh_model_param_t *p_model)
{
    uint32_t i, found = FALSE;

    for (i = 0; i < RAF_BLE_MESH_MODEL_BIND_LIST_SIZE; i++)
    {
        if (appkey_index == p_model->binding_list[i])
        {
            found =  TRUE;
            break;
        }
    }

    return found;
}

uint32_t mmdl_model_subscribe_address_validate(uint16_t subscribe_address, ble_mesh_model_param_t *p_model)
{
    uint32_t i, found = FALSE;

    if ((subscribe_address == MESH_FIXED_GROUP_ADDR_ALL_RELAYS) ||
            (subscribe_address == MESH_FIXED_GROUP_ADDR_ALL_NODES))
    {
        found = TRUE;
    }
    else
    {
        for (i = 0; i < RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE; i++)
        {
            if (subscribe_address == p_model->p_subscribe_list[i])
            {
                found = TRUE;
                break;
            }
        }
    }
    return found;
}

