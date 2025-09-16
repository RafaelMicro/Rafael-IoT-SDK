/**
 * @file mmdl_scene_sr_main.c
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2022-07-25
 *
 * @copyright Copyright (c) 2022
 *
 */

//=============================================================================
//                Include
//=============================================================================
#include "mmdl_common.h"
#include "mmdl_scene_sr.h"
#include "mmdl_scene_common.h"
#include "flashctl.h"

//=============================================================================
//                Private Definitions of const value
//=============================================================================

//flash information
#define SCENE_DATA_START_ADDRESS       0xF0000
#define SCENE_DATA_END_ADDRESS         0xF2000
#define SCENE_DATA_FLASH_SECTOR_SIZE   0x1000
#define SCENE_DATA_FLASH_PAGE_SIZE     0x100
//scene data header
#define SCENE_DATA_HEADER              0x1357

#define GET_SCENE_FALSH_INDEX(addr) ((addr + 1) - pib_primary_address_get())
//=============================================================================
//                Private Structure
//=============================================================================
typedef struct __attribute__((packed)) scene_flash_data_s
{
    uint16_t       header;       //Specific string for the header of scene data
    uint16_t       element_idx;  //Used to distinguish which element the following scene belongs to
    uint32_t       crc32;        //CRC32 calculated by scene data
    scene_state_t  data;       //Scene data
} scene_flash_data_t;


//=============================================================================
//                Private Global Variables
//=============================================================================

/* store the useful data pointer for futher using */
static uint8_t          ttl;

//=============================================================================
//                Private Functions Declaration
//=============================================================================
static void mmdl_scene_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scene_recall(uint32_t is_ack, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scene_register_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scene_store(uint32_t is_ack, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scene_delete(uint32_t is_ack, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model);
static void mmdl_scene_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t status_code);
static void mmdl_scene_send_register_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t status_code);

static uint32_t scene_data_crc32( uint8_t *p_data, uint32_t data_len)
{
    uint16_t k;
    uint32_t i, chk_sum = ~0, len = data_len;

    for (i = 0; i < len; i ++ )
    {
        chk_sum ^= *p_data++;
        for (k = 0; k < 8; k ++)
        {
            chk_sum = chk_sum & 1 ? (chk_sum >> 1) ^ 0xedb88320 : chk_sum >> 1;
        }
    }
    return ~chk_sum;
}

static uint32_t scene_data_remove_all(void)
{
    uint32_t i;
    for (i = SCENE_DATA_START_ADDRESS; i < SCENE_DATA_END_ADDRESS; i+= SCENE_DATA_FLASH_SECTOR_SIZE)
    {
        while(flash_check_busy());
        flash_erase(FLASH_ERASE_SECTOR, i);
        while(flash_check_busy());
    }
}

static uint32_t scene_data_read(uint16_t element_address, uint8_t *p_data, uint8_t *p_data_length)
{
    uint8_t *p_buff, read_idx;
    uint32_t read_addr, i;
    scene_flash_data_t *p_scene_data;

    read_idx = GET_SCENE_FALSH_INDEX(element_address);
    *p_data_length = 0;

    p_buff = pvPortMalloc(SCENE_DATA_FLASH_PAGE_SIZE << 1);
    if (p_buff != NULL)
    {
        for (read_addr = SCENE_DATA_END_ADDRESS; read_addr >= SCENE_DATA_START_ADDRESS; read_addr-= SCENE_DATA_FLASH_PAGE_SIZE)
        {
            //read two pages at a time because we may find the "header" at the end of first page
            while(flash_check_busy());
            flash_read_page((uint32_t)p_buff, read_addr);
            while(flash_check_busy());
            flash_read_page((uint32_t)(p_buff+SCENE_DATA_FLASH_PAGE_SIZE), read_addr+SCENE_DATA_FLASH_PAGE_SIZE);
            while(flash_check_busy());
            //check one page only
            for(i = 0; i < SCENE_DATA_FLASH_PAGE_SIZE; i++)
            {
                p_scene_data = (scene_flash_data_t*)(p_buff + SCENE_DATA_FLASH_PAGE_SIZE - 1 - i);
                if ((p_scene_data->header == SCENE_DATA_HEADER) && (p_scene_data->element_idx == read_idx))
                {
                    if (p_scene_data->crc32 == scene_data_crc32((uint8_t*)&p_scene_data->data, sizeof(scene_state_t)))         
                    {
                        //valid data
                        *p_data_length = sizeof(scene_state_t);
                        memcpy(p_data, (uint8_t*)&p_scene_data->data, sizeof(scene_state_t));
                        break;
                    } 
                }
            }
            if (*p_data_length != 0)
            {
                break;                
            }
        }
        vPortFree(p_buff);
    }
}

static uint32_t scene_data_update(uint32_t element_address, uint8_t *p_updata_data)
{
    uint8_t *p_buff, update_idx, data_len, backup_element_cnt;
    uint32_t read_addr, updata_addr, i, j;
    scene_flash_data_t *p_scene_data, scene_data;

    updata_addr = 0;
    update_idx = GET_SCENE_FALSH_INDEX(element_address);

    p_buff = pvPortMalloc(SCENE_DATA_FLASH_PAGE_SIZE << 1);
    if (p_buff != NULL)
    {
        for (read_addr = SCENE_DATA_START_ADDRESS; read_addr < SCENE_DATA_END_ADDRESS; read_addr += SCENE_DATA_FLASH_PAGE_SIZE)
        {
            //read two pages at a time because we may find the "header" at the end of first page
            while(flash_check_busy());
            flash_read_page((uint32_t)p_buff, read_addr);
            while(flash_check_busy());
            flash_read_page((uint32_t)(p_buff+SCENE_DATA_FLASH_PAGE_SIZE), read_addr + SCENE_DATA_FLASH_PAGE_SIZE);
            while(flash_check_busy());

            //check one page only
            for(i = 0; i < SCENE_DATA_FLASH_PAGE_SIZE; i++)
            {
                p_scene_data = (scene_flash_data_t*)(p_buff + i);
                if ((p_scene_data->header == SCENE_DATA_HEADER))
                {
                    if (p_scene_data->crc32 != scene_data_crc32((uint8_t*)&p_scene_data->data, sizeof(scene_state_t)))         
                    {
                        printf("crc fail 0x%08x, 0x%08xd\n", p_scene_data->crc32, scene_data_crc32((uint8_t*)&p_scene_data->data, sizeof(scene_state_t)));
                        //invalid data
                        updata_addr = 0;
                        read_addr = SCENE_DATA_END_ADDRESS;
                        break;
                    }
                    else
                    {
                        //vaild data found, move update address to the end of valid data
                        updata_addr = read_addr + i + sizeof(scene_flash_data_t);
                    } 
                }
            }
        }
        vPortFree(p_buff);
    }

    if ((updata_addr == 0) || ((updata_addr + sizeof(scene_flash_data_t)) >= SCENE_DATA_END_ADDRESS))
    {
        p_buff = pvPortMalloc(sizeof(scene_flash_data_t) * pib_element_count_get());
        if (p_buff != NULL)
        {
            backup_element_cnt = 0;
            p_scene_data = (scene_flash_data_t*)p_buff;
            for (i = pib_primary_address_get(); i < (pib_element_count_get() + pib_primary_address_get()); i++)
            {
                if (i != element_address)
                {
                    scene_data_read(i, (uint8_t*)&p_scene_data->data, &data_len); 
                    if(data_len > 0)
                    {
                        //printf("updata_addr elem_idx %d, len %d\n", i, sizeof(scene_state_t));
                        backup_element_cnt++;
                        p_scene_data->header = SCENE_DATA_HEADER;
                        p_scene_data->element_idx = GET_SCENE_FALSH_INDEX(i);
                        p_scene_data->crc32 = scene_data_crc32((uint8_t*)&p_scene_data->data, sizeof(scene_state_t));
                        p_scene_data = (scene_flash_data_t*)(p_buff+(backup_element_cnt*sizeof(scene_flash_data_t)));
                        p_scene_data += sizeof(scene_flash_data_t);
                    }
                }     
            }
            scene_data_remove_all();
            updata_addr = SCENE_DATA_START_ADDRESS;

            for (i = 0; i < backup_element_cnt; i++)
            {
                for(j = 0; j < sizeof(scene_flash_data_t); j++)
                {
                    while(flash_check_busy());
                    flash_write_byte(updata_addr + j, p_buff[i*sizeof(scene_flash_data_t)+j]);
                    //printf("%02x ",p_buff[i*sizeof(scene_flash_data_t)+j]);
                    while(flash_check_busy());
                    
                }
                //printf("\n");
                updata_addr += sizeof(scene_flash_data_t);
            }

            vPortFree(p_buff);
        }
    }

    p_buff = pvPortMalloc( sizeof(scene_flash_data_t));

    if (p_buff != NULL)
    {
        p_scene_data = (scene_flash_data_t*)p_buff;
        p_scene_data->header = SCENE_DATA_HEADER;
        p_scene_data->element_idx = update_idx;
        memcpy((uint8_t*)&p_scene_data->data, p_updata_data, sizeof(scene_state_t));
        p_scene_data->crc32 = scene_data_crc32((uint8_t*)&p_scene_data->data, sizeof(scene_state_t));
        for(i = 0; i < sizeof(scene_flash_data_t); i++)
        {
            while(flash_check_busy());
            flash_write_byte(updata_addr + i, p_buff[i]);
            while(flash_check_busy());
        }
        vPortFree(p_buff);
        //printf("updata_addr 0x%08x, len %d\n", updata_addr, sizeof(scene_state_t));
    }
}

void scene_info_reset(void)
{
    scene_data_remove_all();
}

void mmdl_scene_sr_init(ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    scene_state_t  *p_scene_state = (scene_state_t *)p_model->p_state_var;
    uint8_t        data_len;

    scene_data_read(p_element->element_address, (uint8_t *)p_scene_state, &data_len);
    if (data_len == 0)
    {
        memset(p_scene_state, 0, sizeof(scene_state_t));
    }
    //printf("EleAddr 0x%04x DataLen %d\n", p_element->element_address, data_len);
}

void mmdl_scene_sr_handler(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t is_broadcast)
{
    /* store the data in private variables */
    ttl = 30;

    /* process the message by different opcode */
    switch (LE2BE16(p_evt_msg->opcode))
    {
    case MMDL_SCENE_GET_OPCODE:
        mmdl_scene_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCENE_RECALL_OPCODE:
        mmdl_scene_recall(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCENE_RECALL_NO_ACK_OPCODE:
        mmdl_scene_recall(FALSE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCENE_REGISTER_GET_OPCODE:
        mmdl_scene_register_get(p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCENE_STORE_OPCODE:
        mmdl_scene_store(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCENE_STORE_NO_ACK_OPCODE:
        mmdl_scene_store(FALSE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCENE_DELETE_OPCODE:
        mmdl_scene_delete(TRUE, p_evt_msg, p_element, p_model);
        break;

    case MMDL_SCENE_DELETE_NO_ACK_OPCODE:
        mmdl_scene_delete(FALSE, p_evt_msg, p_element, p_model);
        break;
    }
}

static void mmdl_scene_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_scene_send_status(p_evt_msg, p_element, p_model, MMDL_SCENE_SUCCESS);
}

static void mmdl_scene_recall(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    scene_recall_msg_t  *p_scene_recall_msg = (scene_recall_msg_t *)p_evt_msg->parameter;
    scene_state_t       *p_scene_state   = (scene_state_t *)p_model->p_state_var;
    void                *p_extend_set_func = NULL;
    uint8_t             status = MMDL_SCENE_NOT_FOUND, i;

    if (tid == p_scene_recall_msg->tid)
    {
        return;
    }

    if (p_scene_recall_msg->scene_num == 0x0000) /*The value 0x0000 is Prohibited*/
    {
        return;
    }

    for (i = 0; i < RAF_BLE_MESH_SCENE_ENTRY_COUNT; i++)
    {
        if (p_scene_state->scene_register[i].scene_number == p_scene_recall_msg->scene_num)
        {
            status = MMDL_SCENE_SUCCESS;
            p_scene_state->current_scene = p_scene_recall_msg->scene_num;
            p_scene_state->target_scene = p_scene_recall_msg->scene_num;
            break;
        }
    }

    if (ack_flag)
    {
        mmdl_scene_send_status(p_evt_msg, p_element, p_model, status);
    }

    if (status == MMDL_SCENE_SUCCESS)
    {
        ((model_scene_call_back)(p_model->p_user_call_back))
        (p_element->element_address, SCENE_ACTION_RECALL, &(p_scene_state->scene_register[i].scene_state), &p_extend_set_func);

        if (p_extend_set_func != NULL)
        {
            ((model_scene_set_extend)p_extend_set_func)(p_element);
        }
    }
}

static void mmdl_scene_register_get(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    mmdl_scene_send_register_status(p_evt_msg, p_element, p_model, MMDL_SCENE_SUCCESS);
}

static void mmdl_scene_store(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    scene_store_msg_t  *p_scene_store_msg = (scene_store_msg_t *)p_evt_msg->parameter;
    scene_state_t      *p_scene_state   = (scene_state_t *)p_model->p_state_var;
    uint8_t             status = MMDL_SCENE_REGISTER_FULL, i;

    if (p_scene_store_msg->scene_num == 0x0000) /*The value 0x0000 is Prohibited*/
    {
        return;
    }

    for (i = 0; i < RAF_BLE_MESH_SCENE_ENTRY_COUNT; i++)
    {
        if (p_scene_state->scene_register[i].scene_number == p_scene_store_msg->scene_num)
        {
            p_scene_state->current_scene = p_scene_store_msg->scene_num;
            status = MMDL_SCENE_SUCCESS;
            break;
        }
    }

    if (i == RAF_BLE_MESH_SCENE_ENTRY_COUNT)
    {
        for (i = 0; i < RAF_BLE_MESH_SCENE_ENTRY_COUNT; i++)
        {
            if (p_scene_state->scene_register[i].scene_number == 0x0000)
            {
                p_scene_state->scene_register[i].scene_number = p_scene_store_msg->scene_num;
                p_scene_state->current_scene = p_scene_store_msg->scene_num;
                status = MMDL_SCENE_SUCCESS;
                break;
            }
        }
    }

    if (ack_flag)
    {
        mmdl_scene_send_register_status(p_evt_msg, p_element, p_model, status);
    }

    if (status == MMDL_SCENE_SUCCESS)
    {
        ((model_scene_call_back)(p_model->p_user_call_back)) (p_element->element_address,
                SCENE_ACTION_STORE, &(p_scene_state->scene_register[i].scene_state), NULL);
        scene_data_update(p_element->element_address, (uint8_t *)p_model->p_state_var);
    }
}

static void mmdl_scene_delete(uint32_t ack_flag, mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model)
{
    scene_delete_msg_t  *p_scene_delete_msg = (scene_delete_msg_t *)p_evt_msg->parameter;
    scene_state_t       *p_scene_state   = (scene_state_t *)p_model->p_state_var;
    uint8_t             status = MMDL_SCENE_NOT_FOUND, i;

    if (p_scene_delete_msg->scene_num == 0x0000) /*The value 0x0000 is Prohibited*/
    {
        return;
    }

    for (i = 0; i < RAF_BLE_MESH_SCENE_ENTRY_COUNT; i++)
    {
        if (p_scene_state->scene_register[i].scene_number == p_scene_delete_msg->scene_num)
        {
            p_scene_state->scene_register[i].scene_number = 0x0000;
            if (p_scene_state->current_scene == p_scene_delete_msg->scene_num)
            {
                p_scene_state->current_scene = 0x0000;
            }
            status = MMDL_SCENE_SUCCESS;
            break;
        }
    }

    if (ack_flag)
    {
        mmdl_scene_send_register_status(p_evt_msg, p_element, p_model, status);
    }

    if (status == MMDL_SCENE_SUCCESS)
    {
        ((model_scene_call_back)(p_model->p_user_call_back)) (p_element->element_address,
                SCENE_ACTION_DELETE, &(p_scene_state->scene_register[i].scene_state), NULL);
        scene_data_update(p_element->element_address, (uint8_t *)p_model->p_state_var);
    }
}

static void mmdl_scene_send_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t status_code)
{
    mesh_app_mdl_evt_msg_req_t  *pt_req;
    mesh_tlv_t                  *pt_tlv;
    scene_state_t               *p_scene_state   = (scene_state_t *)p_model->p_state_var;
    scene_status_msg_t          *p_scene_status_msg;

    do
    {
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(scene_status_msg_t));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(scene_status_msg_t);

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;
        pt_req->opcode_invalid = 0;

        pt_req->opcode = MMDL_SCENE_STATUS_OPCODE;
        pt_req->parameter_len = sizeof(scene_status_msg_t);
        p_scene_status_msg = (scene_status_msg_t *)pt_req->parameter;
        p_scene_status_msg->status_code = status_code;
        p_scene_status_msg->current_scene = p_scene_state->current_scene;
        /* TODO: remaining time must be processed */
        //p_scene_status_msg->remaining_time = 0;

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

static void mmdl_scene_send_register_status(mesh_app_mdl_evt_msg_idc_t *p_evt_msg, ble_mesh_element_param_t *p_element, ble_mesh_model_param_t *p_model, uint8_t status_code)
{
    mesh_app_mdl_evt_msg_req_t   *pt_req;
    mesh_tlv_t                   *pt_tlv;
    scene_state_t                *p_scene_state   = (scene_state_t *)p_model->p_state_var;
    scene_register_status_msg_t  *p_scene_reg_status_msg;
    uint8_t                      i = 0, valid_scene = 0;
    uint16_t                     scene_number[RAF_BLE_MESH_SCENE_ENTRY_COUNT];

    do
    {
        for (i = 0; i < RAF_BLE_MESH_SCENE_ENTRY_COUNT; i++)
        {
            if (p_scene_state->scene_register[i].scene_number != 0x0000)
            {
                scene_number[valid_scene] = p_scene_state->scene_register[i].scene_number;
                valid_scene++;
            }
        }
        pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(mesh_app_mdl_evt_msg_req_t) +
                              sizeof(scene_register_status_msg_t) + (valid_scene * sizeof(uint16_t)));
        if (pt_tlv == NULL)
        {
            break;
        }

        pt_tlv->type = TYPE_MESH_APP_MDL_EVT_MSG_REQ;
        pt_tlv->length = sizeof(mesh_app_mdl_evt_msg_req_t) + sizeof(scene_register_status_msg_t) + (valid_scene * sizeof(uint16_t));

        pt_req = (mesh_app_mdl_evt_msg_req_t *)pt_tlv->value;

        pt_req->dst_addr = p_evt_msg->src_addr;
        pt_req->src_addr = p_element->element_address;;
        pt_req->appkey_index = p_evt_msg->appkey_index;
        pt_req->ttl = ttl;
        pt_req->opcode_invalid = 0;

        pt_req->opcode = BE2LE16(MMDL_SCENE_REGISTER_STATUS_OPCODE);
        pt_req->parameter_len = sizeof(scene_register_status_msg_t) + (valid_scene * sizeof(uint16_t));
        p_scene_reg_status_msg = (scene_register_status_msg_t *)pt_req->parameter;
        p_scene_reg_status_msg->status_code = status_code;
        p_scene_reg_status_msg->current_scene = p_scene_state->current_scene;
        memcpy((uint8_t *)p_scene_reg_status_msg->scene_list, (uint8_t *)scene_number, sizeof(uint16_t)*valid_scene);
        /* TODO: remaining time must be processed */

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

