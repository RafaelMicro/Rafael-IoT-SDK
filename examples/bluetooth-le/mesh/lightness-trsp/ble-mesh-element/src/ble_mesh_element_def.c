/************************************************************************
 *
 * File Name  : ble_mesh_element_def.c
 * Description: This file contains the definitions of BLE element.
 *
 ************************************************************************/
#include "stdio.h"
#include "ble_mesh_element.h"
#include "mmdl_common.h"
#include "pib.h"
#include "mesh_app.h"
#include "mesh_mdl_handler.h"

/************************************************************************************************************
 * Declear the light lightness element example
 ************************************************************************************************************/
/* Element 0 */
/* Declare the state variable for models */
/* Extending model is shaeing the same subscription list with major model */
light_lightness_state_t     el0_light_lightness_state = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0001, 0xFFFF, 0xFFFF}; /*default state: 0xFFFF*/
publication_info_t          el0_light_lightness_model_publish_entry;
uint16_t                    el0_light_lightness_subscribe_list[RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE];

ble_mesh_model_param_t      el0_light_lightness_model =
{
    MMDL_LIGHT_LIGHTNESS_SR_MDL_ID,                     //model ID
    0,                                                  //rcv_tid
    FALSE,                                              //is extended
    &el0_light_lightness_state,                         //state
    &el0_light_lightness_model_publish_entry,           //publish ptr
    el0_light_lightness_subscribe_list,                 //subscript list ptr
    (void *)NULL,                                       //upper callback
    (void *)app_process_element_lightness_model_state       //call back
};

ble_mesh_model_param_t      el0_light_lightness_setup_model =
{
    MMDL_LIGHT_LIGHTNESS_SETUP_SR_MDL_ID,
    0,
    FALSE,
    &el0_light_lightness_state,
    (void *)NULL,                                        //publish ptr
    el0_light_lightness_subscribe_list,                  //subscript list ptr
    (void *)NULL,
    (void *)NULL
};

/* extended models and models variables */
gen_level_state_t           el0_gen_level_state = {0x7FFF, 0x7FFF, 0x7FFF, 0, 0, 0, 0, 0}; /*default state: 0x7FFF*/
publication_info_t          el0_gen_level_model_publish_entry;
uint16_t                    el0_gen_level_model_subscribe_list[RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE];

ble_mesh_model_param_t      el0_gen_level_model =
{
    MMDL_GEN_LEVEL_SR_MDL_ID,
    0,
    TRUE,
    &el0_gen_level_state,
    &el0_gen_level_model_publish_entry,                 //publish ptr
    el0_gen_level_model_subscribe_list,                 //subscript list ptr
    (void *)mmdl_light_lightness_ex_cb,
    (void *)NULL
};


gen_on_off_state_t          el0_gen_on_off_state = {0x01, 0x01, 0x01};   /*default state: on*/
publication_info_t          el0_gen_on_off_model_publish_entry;
uint16_t                    el0_gen_on_off_model_subscribe_list[RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE];

ble_mesh_model_param_t      el0_gen_on_off_model =
{
    MMDL_GEN_ONOFF_SR_MDL_ID,
    0,
    TRUE,
    &el0_gen_on_off_state,
    &el0_gen_on_off_model_publish_entry,                 //publish ptr
    el0_gen_on_off_model_subscribe_list,                 //subscript list ptr
    (void *)mmdl_light_lightness_ex_cb,
    (void *)NULL
};


scene_state_t              el0_scene_state;
uint16_t                   el0_scene_subscribe_list[RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE];
ble_mesh_model_param_t     el0_scene_model =
{
    MMDL_SCENE_SR_MDL_ID,
    0,
    FALSE,
    &el0_scene_state,
    (void *)NULL,                                       //publish ptr
    el0_scene_subscribe_list,                        //subscript list ptr
    (void *)NULL,
    (void *)app_process_element_scene_model_state
};

ble_mesh_model_param_t     el0_scene_setup_model =
{
    MMDL_SCENE_SETUP_SR_MDL_ID,
    0,
    FALSE,
    &el0_scene_state,
    (void *)NULL,                                       //publish ptr
    el0_scene_subscribe_list,                        //subscript list ptr
    (void *)NULL,
    (void *)app_process_element_scene_model_state
};

raf_trsp_state_t            el0_raf_trsp_state;
uint16_t                    el0_raf_trsp_subscribe_list[RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE];
ble_mesh_model_param_t      el0_raf_trsp_sr_model =
{
    MMDL_RAFAEL_TRSP_SR_MDL_ID,
    0,
    FALSE,
    &el0_raf_trsp_state,
    (void *)NULL,                                       //publish ptr
    el0_raf_trsp_subscribe_list,                        //subscript list ptr
    (void *)NULL,
    (void *)app_process_element_raf_trsp_sr_model_state
};

ble_mesh_model_param_t      el0_raf_trsp_cl_model =
{
    MMDL_RAFAEL_TRSP_CL_MDL_ID,
    0,
    FALSE,
    NULL,
    (void *)NULL,                                       //publish ptr
    NULL,                        //subscript list ptr
    (void *)NULL,
    (void *)app_process_element_raf_trsp_cl_model_state
};


/* Declare the model using in element */
ble_mesh_model_param_t *el0_light_lightness_element_model_list[] =
{
    &el0_light_lightness_model,
    &el0_light_lightness_setup_model,
    &el0_gen_level_model,
    &el0_gen_on_off_model,
    &el0_scene_model,
    &el0_scene_setup_model,
    &el0_raf_trsp_sr_model,
    &el0_raf_trsp_cl_model,
};

const uint8_t el0_models_count = (sizeof(el0_light_lightness_element_model_list) / sizeof(el0_light_lightness_element_model_list[0]));

/************************************************************************************************************/

ble_mesh_element_param_t g_element_info[] =
{
    // element 0, lightness element
    {
        el0_light_lightness_element_model_list,         /*model list*/
        el0_models_count,                                              /*model count*/
        0xFFFF,                                         /*element address*/
        0,                                              /*tx transcation id*/
    },


};

const uint8_t ble_mesh_element_count = (sizeof(g_element_info) / sizeof(g_element_info[0]));

void element_info_init(void)
{
    uint8_t i, j, k;
    for (i = 0; i < RAF_BLE_MESH_SUBSCRIPTION_LIST_SIZE; i++)
    {
        el0_light_lightness_subscribe_list[i] = 0x0000;
        el0_raf_trsp_subscribe_list[i] = 0x0000;
        el0_scene_subscribe_list[i] = 0x0000;
    }

    memset(&el0_scene_state, 0, sizeof(scene_state_t));
    scene_info_reset();

    for (i = 0 ; i < ble_mesh_element_count ; i++)
    {
        g_element_info[i].element_address = 0xFFFF;
        for (j = 0 ; j < g_element_info[i].element_models_count ; j++)
        {
            for (k = 0 ; k < RAF_BLE_MESH_MODEL_BIND_LIST_SIZE ; k++)
            {
                g_element_info[i].p_model[j]->binding_list[k] = 0xFFFF;
            }
        }
    }
}

