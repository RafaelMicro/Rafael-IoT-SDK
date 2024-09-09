/**
 * @file mmdl_scene_sr.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-27
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef __MMDL_SCENE_COMMON_H__
#define __MMDL_SCENE_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define RAF_BLE_MESH_SCENE_ENTRY_COUNT 16

typedef uint8_t scene_action_t;
#define SCENE_ACTION_STORE          0x00
#define SCENE_ACTION_RECALL         0x01
#define SCENE_ACTION_DELETE         0x02
#define SCENE_ACTION_GET            0x03
#define SCENE_ACTION_REGISTER_GET   0x04

typedef enum
{
    MMDL_SCENE_ELEMENT1_UPDATE = 0x01,
    MMDL_SCENE_ELEMENT2_UPDATE = 0x02,
    MMDL_SCENE_ELEMENT3_UPDATE = 0x03,
    MMDL_SCENE_ELEMENT4_UPDATE = 0x04,
} scene_update_type_t;

typedef struct register_scene_value_s
{
    uint16_t                    scene_number;
    uint32_t                    scene_state;
} register_scene_value_t;

typedef struct scene_state_s
{
    register_scene_value_t  scene_register[RAF_BLE_MESH_SCENE_ENTRY_COUNT];
    uint16_t                current_scene;
    uint16_t                target_scene;
} scene_state_t;

typedef struct __attribute__((packed)) scene_store_msg_s
{
    uint16_t    scene_num;
} scene_store_msg_t;

typedef struct __attribute__((packed)) scene_recall_msg_s
{
    uint16_t    scene_num;
    uint8_t     tid;
    uint8_t     transition_time;
    uint8_t     delay;
} scene_recall_msg_t;

typedef struct __attribute__((packed)) scene_status_msg_s
{
    uint8_t     status_code;
    uint16_t    current_scene;
} scene_status_msg_t;

typedef struct __attribute__((packed)) scene_delete_msg_s
{
    uint16_t    scene_num;
} scene_delete_msg_t;

typedef struct __attribute__((packed)) scene_register_status_msg_s
{
    uint8_t     status_code;
    uint16_t    current_scene;
    uint16_t    scene_list[];
} scene_register_status_msg_t;


typedef void (*model_scene_call_back)(uint16_t element_address, uint8_t action, uint32_t *p_scene_state, void *p_extend_set_func);
typedef void (*model_scene_set_extend)(ble_mesh_element_param_t *p_current_element);

#ifdef __cplusplus
};
#endif

#endif /* __MMDL_SCENE_COMMON_H__*/

