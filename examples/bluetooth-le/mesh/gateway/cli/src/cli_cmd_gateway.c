/** @file cli_cmd_gateway.c
 *
 * @author Rex
 * @version 0.1
 * @date 2022/01/17
 * @license
 * @description
 */


#if (SUPPORT_DEBUG_CONSOLE_CLI == 1)

#include "types.h"
#include "util_string.h"

#include "cli.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"

#include "cfgmdl_client.h"
#include "ble_mesh_lib_api.h"

#include "mmdl_common.h"
#include "mmdl_client.h"

#include "mmdl_gen_onoff_common.h"
#include "mmdl_gen_level_common.h"

//=============================================================================
//                  Constant Definition
//=============================================================================

//=============================================================================
//                  Macro Definition
//=============================================================================
#define MAX_UNPROV_DEVICE_NUM  10
#define MAX_PROV_DEVICE_NUM    50

typedef uint8_t device_state_t;
#define DEVICE_INVALID                           0x00
#define DEVICE_PROVISION_START                   0x01
#define DEVICE_PROVISION_WAITING                 0x02

//=============================================================================
//                  Structure Definition
//=============================================================================
typedef struct
{
    device_state_t state;
    uint8_t        device_uuid[16];
} unprov_device;

typedef struct
{
    uint16_t     primary_addr;
    uint8_t      device_key[16];
} prov_device;

//=============================================================================
//                  Global Data Definition
//=============================================================================

uint8_t auto_provision = false;

unprov_device unprov_device_list[MAX_UNPROV_DEVICE_NUM];
prov_device prov_device_list[MAX_PROV_DEVICE_NUM] = {0};

//=============================================================================
//                  Private Function Definition
//=============================================================================

static void _memcpy_inv(uint8_t *pDst, uint8_t *pSrc, uint16_t len)
{
    uint16_t i;

    if (len)
    {
        i = 0;
        do
        {
            len--;
            *(pDst + len) = *(pSrc + i);
            i++;
        } while (len != 0);
    }
}

static int _cli_cmd_provision_all_device(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint8_t enable;

    do
    {
        if (argc < 2)
        {
            printf("Invalid parameter\n");
            break;
        }

        enable = (*(argv[1] + 1) == 'x')
                 ? utility_strtox(argv[1] + 2, 0, 2)
                 : utility_strtol(argv[1], 0);
        if (enable)
        {
            ble_mesh_find_unprov_device_start();
            auto_provision = true;
            memset(unprov_device_list, 0, sizeof(unprov_device_list));

            printf("Auto provision device start\n");
        }
        else
        {
            ble_mesh_find_unprov_device_stop();
            auto_provision = false;
            printf("Auto provision device stop\n");
        }
    } while (0);

    return 0;
}

static int _cli_cmd_device_key_set(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t address, i;
    uint32_t key1, key2, key3, key4;
    do
    {
        if (argc < 6)
        {
            printf("Invalid parameter\n");
            break;
        }

        address = (*(argv[1] + 1) == 'x')
                  ? utility_strtox(argv[1] + 2, 0, 8)
                  : utility_strtol(argv[1], 0);

        key1 = (*(argv[2] + 1) == 'x')
               ? utility_strtox(argv[2] + 2, 0, 8)
               : utility_strtol(argv[2], 0);

        key2 = (*(argv[3] + 1) == 'x')
               ? utility_strtox(argv[3] + 2, 0, 8)
               : utility_strtol(argv[3], 0);

        key3 = (*(argv[4] + 1) == 'x')
               ? utility_strtox(argv[4] + 2, 0, 8)
               : utility_strtol(argv[4], 0);

        key4 = (*(argv[5] + 1) == 'x')
               ? utility_strtox(argv[5] + 2, 0, 8)
               : utility_strtol(argv[5], 0);
        for (i = 0; i < MAX_PROV_DEVICE_NUM; i++)
        {
            if (prov_device_list[i].primary_addr == 0)
            {
                prov_device_list[i].primary_addr = address;
                _memcpy_inv(prov_device_list[i].device_key, (uint8_t *)&key1, sizeof(key1));
                _memcpy_inv(prov_device_list[i].device_key + 4, (uint8_t *)&key2, sizeof(key2));
                _memcpy_inv(prov_device_list[i].device_key + 8, (uint8_t *)&key3, sizeof(key3));
                _memcpy_inv(prov_device_list[i].device_key + 12, (uint8_t *)&key4, sizeof(key4));
                printf("device key set success\n");
                break;
            }
        }
        if (i == MAX_PROV_DEVICE_NUM)
        {
            printf("no space to set device key\n");
        }
    } while (0);

    return 0;
}

static int _cli_cmd_device_composition_data_get(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t address, i;
    uint8_t page_num;

    do
    {
        if (argc < 3)
        {
            printf("Invalid parameter\n");
            break;
        }

        address = (*(argv[1] + 1) == 'x')
                  ? utility_strtox(argv[1] + 2, 0, 4)
                  : utility_strtol(argv[1], 0);

        page_num = (*(argv[2] + 1) == 'x')
                   ? utility_strtox(argv[2] + 2, 0, 2)
                   : utility_strtol(argv[2], 0);

        printf("Get node 0x%04x composition data\n", address);
        for (i = 0; i < MAX_PROV_DEVICE_NUM; i++)
        {
            if (prov_device_list[i].primary_addr == address)
            {
                pib_device_key_set(prov_device_list[i].device_key);

                cfgmdl_client_send(CONFIG_COMPOSITION_DATA_GET, address, &page_num, sizeof(uint8_t));
                break;
            }
        }

        if (i == MAX_PROV_DEVICE_NUM)
        {
            printf("device key not found\n");
        }
    } while (0);

    return 0;
}

static int _cli_cmd_device_subscribe_list_delete(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t primary_address, ele_address, address, i;
    uint32_t model;
    uint8_t sub_list_del[8], mdl_len = 2;

    do
    {
        if (argc < 5)
        {
            printf("Invalid parameter\n");
            break;
        }

        primary_address = (*(argv[1] + 1) == 'x')
                          ? utility_strtox(argv[1] + 2, 0, 4)
                          : utility_strtol(argv[1], 0);

        ele_address = (*(argv[2] + 1) == 'x')
                      ? utility_strtox(argv[2] + 2, 0, 4)
                      : utility_strtol(argv[2], 0);

        address = (*(argv[3] + 1) == 'x')
                  ? utility_strtox(argv[3] + 2, 0, 4)
                  : utility_strtol(argv[3], 0);

        model = (*(argv[4] + 1) == 'x')
                ? utility_strtox(argv[4] + 2, 0, 8)
                : utility_strtol(argv[4], 0);

        if (model > 0xFFFF)
        {
            mdl_len = 4;
        }
        printf("Model 0x%08x of element 0x%04x delete subscribe address 0x%04x\n", model, ele_address, address);
        memcpy(sub_list_del, (uint8_t *)&ele_address, sizeof(uint16_t));
        memcpy(sub_list_del + 2, (uint8_t *)&address, sizeof(uint16_t));
        memcpy(sub_list_del + 4, (uint8_t *)&model, mdl_len);
        for (i = 0; i < MAX_PROV_DEVICE_NUM; i++)
        {
            if (prov_device_list[i].primary_addr == primary_address)
            {
                pib_device_key_set(prov_device_list[i].device_key);

                cfgmdl_client_send(CONFIG_MDL_SUBSCRIPTION_DELETE, primary_address, sub_list_del, 4 + mdl_len);
                break;
            }
        }

        if (i == MAX_PROV_DEVICE_NUM)
        {
            printf("device key not found\n");
        }
    } while (0);

    return 0;
}

static int _cli_cmd_device_subscribe_list_add(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t primary_address, ele_address, address, i;
    uint32_t model;
    uint8_t sub_list_add[8], mdl_len = 2;

    do
    {
        if (argc < 5)
        {
            printf("Invalid parameter\n");
            break;
        }

        primary_address = (*(argv[1] + 1) == 'x')
                          ? utility_strtox(argv[1] + 2, 0, 4)
                          : utility_strtol(argv[1], 0);

        ele_address = (*(argv[2] + 1) == 'x')
                      ? utility_strtox(argv[2] + 2, 0, 4)
                      : utility_strtol(argv[2], 0);

        address = (*(argv[3] + 1) == 'x')
                  ? utility_strtox(argv[3] + 2, 0, 4)
                  : utility_strtol(argv[3], 0);

        model = (*(argv[4] + 1) == 'x')
                ? utility_strtox(argv[4] + 2, 0, 8)
                : utility_strtol(argv[4], 0);

        if (model > 0xFFFF)
        {
            mdl_len = 4;
        }
        printf("Model 0x%08x of element 0x%04x add subscribe address 0x%04x\n", model, ele_address, address);
        memcpy(sub_list_add, (uint8_t *)&ele_address, sizeof(uint16_t));
        memcpy(sub_list_add + 2, (uint8_t *)&address, sizeof(uint16_t));
        memcpy(sub_list_add + 4, (uint8_t *)&model, mdl_len);
        for (i = 0; i < MAX_PROV_DEVICE_NUM; i++)
        {
            if (prov_device_list[i].primary_addr == primary_address)
            {
                pib_device_key_set(prov_device_list[i].device_key);

                cfgmdl_client_send(CONFIG_MDL_SUBSCRIPTION_ADD, primary_address, sub_list_add, 4 + mdl_len);
                break;
            }
        }

        if (i == MAX_PROV_DEVICE_NUM)
        {
            printf("device key not found\n");
        }
    } while (0);

    return 0;
}

static int _cli_cmd_app_key_add(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t address, i;
    uint32_t index = 0;
    uint8_t app_key_add[19];

    do
    {
        if (argc < 2)
        {
            printf("Invalid parameter\n");
            break;
        }

        address = (*(argv[1] + 1) == 'x')
                  ? utility_strtox(argv[1] + 2, 0, 4)
                  : utility_strtol(argv[1], 0);

        printf("add app key to node 0x%04x\n", address);
        for (i = 0; i < MAX_PROV_DEVICE_NUM; i++)
        {
            if (prov_device_list[i].primary_addr == address)
            {
                pib_device_key_set(prov_device_list[i].device_key);

                if (pib_local_app_key_get_by_idx(index, &app_key_add[3]) == true)
                {
                    memcpy(app_key_add, (uint8_t *)&index, 3);
                    cfgmdl_client_send(CONFIG_APPKEY_ADD, address, app_key_add, sizeof(app_key_add));
                }
                else
                {
                    printf("GW need to add app key index %d\n", index);
                }
                break;
            }
        }

        if (i == MAX_PROV_DEVICE_NUM)
        {
            printf("device key not found\n");
        }

    } while (0);

    return 0;
}



static int _cli_cmd_model_app_bind(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t address, i;
    uint32_t index = 0, model_id;
    uint8_t *p_data, model_len = 2;

    do
    {
        if (argc < 3)
        {
            printf("Invalid parameter\n");
            break;
        }

        address = (*(argv[1] + 1) == 'x')
                  ? utility_strtox(argv[1] + 2, 0, 4)
                  : utility_strtol(argv[1], 0);

        model_id = (*(argv[2] + 1) == 'x')
                   ? utility_strtox(argv[2] + 2, 0, 8)
                   : utility_strtol(argv[2], 0);

        if (model_id > 0xFFFF)
        {
            model_len = 4;
        }
        p_data = pvPortMalloc(4 + model_len);

        memcpy(p_data, (uint8_t *)&address, 2);
        memcpy(p_data + 2, (uint8_t *)&index, 2);
        memcpy(p_data + 4, (uint8_t *)&model_id, model_len);

        printf("bind device 0x%04x's app key to model 0x%04x\n", address, model_id);

        for (i = 0; i < MAX_PROV_DEVICE_NUM; i++)
        {
            if (prov_device_list[i].primary_addr == address)
            {
                pib_device_key_set(prov_device_list[i].device_key);
                cfgmdl_client_send(CONFIG_MDL_APP_BIND, address, p_data, 4 + model_len);
                break;
            }
        }

        if (i == MAX_PROV_DEVICE_NUM)
        {
            printf("device key not found\n");
        }
        vPortFree(p_data);
    } while (0);

    return 0;
}

static int _cli_cmd_onoff_set(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t address;
    uint8_t onoff, data[2], key_idx = 0;
    static uint8_t tid = 1;

    do
    {
        if (argc < 3)
        {
            printf("Invalid parameter\n");
            break;
        }

        address = (*(argv[1] + 1) == 'x')
                  ? utility_strtox(argv[1] + 2, 0, 4)
                  : utility_strtol(argv[1], 0);

        onoff = (*(argv[2] + 1) == 'x')
                ? utility_strtox(argv[2] + 2, 0, 2)
                : utility_strtol(argv[2], 0);

        data[0] = onoff;
        data[1] = tid;
        tid++;
        if (tid == 0)
        {
            tid = 1;
        }

        printf("set onoff status %d to node 0x%04x\n", onoff, address);
        mmdl_client_send(address, pib_primary_address_get(), BE2LE16(MMDL_GEN_ONOFF_SET_OPCODE), key_idx, sizeof(data), data);
    } while (0);

    return 0;
}


static int _cli_cmd_level_set(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t address, level;
    uint8_t data[3], key_idx = 0;
    static uint8_t tid = 1;

    do
    {
        if (argc < 3)
        {
            printf("Invalid parameter\n");
            break;
        }

        address = (*(argv[1] + 1) == 'x')
                  ? utility_strtox(argv[1] + 2, 0, 4)
                  : utility_strtol(argv[1], 0);

        level = (*(argv[2] + 1) == 'x')
                ? utility_strtox(argv[2] + 2, 0, 4)
                : utility_strtol(argv[2], 0);

        data[0] = (level & 0xFF);
        data[1] = ((level >> 8) & 0xFF);
        data[2] = tid;
        tid++;
        if (tid == 0)
        {
            tid = 1;
        }
        printf("set level status %d to node 0x%04x\n", level, address);
        mmdl_client_send(address, pib_primary_address_get(), BE2LE16(MMDL_GEN_LEVEL_SET_OPCODE), key_idx, sizeof(data), data);
    } while (0);

    return 0;
}


static int _cli_cmd_node_reset(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    uint16_t address, i;

    do
    {
        if (argc < 2)
        {
            printf("Invalid parameter\n");
            break;
        }

        address = (*(argv[1] + 1) == 'x')
                  ? utility_strtox(argv[1] + 2, 0, 4)
                  : utility_strtol(argv[1], 0);

        printf("Node reset command send to 0x%04x\n", address);
        for (i = 0; i < MAX_PROV_DEVICE_NUM; i++)
        {
            if (prov_device_list[i].primary_addr == address)
            {
                pib_device_key_set(prov_device_list[i].device_key);

                cfgmdl_client_send(CONFIG_NODE_RESET, address, NULL, 0);
                prov_device_list[i].primary_addr = 0;
                break;
            }
        }

        if (i == MAX_PROV_DEVICE_NUM)
        {
            printf("device key not found\n");
        }
    } while (0);
    return 0;
}


//=============================================================================
//                  Public Function Definition
//=============================================================================

const sh_cmd_t  g_cli_cmd_provision_all STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "provisionall",
    .cmd_exec       = _cli_cmd_provision_all_device,
    .pDescription   = "start/stop provision all device\n"
    "  usage: provisionall [start]\n"
    "    e.g. provisionall 1\n",
};

const sh_cmd_t  g_cli_cmd_device_key_set STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "devkeyset",
    .cmd_exec       = _cli_cmd_device_key_set,
    .pDescription   = "set device key into gw\n"
    "  usage: devkeyset [address] [key] [key] [key] [key]\n"
    "    e.g. devkeyset 0x0123 0x1234 0x5678 0x90ab 0xcdef\n",
};


const sh_cmd_t  g_cli_cmd_composition_data_get STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "compget",
    .cmd_exec       = _cli_cmd_device_composition_data_get,
    .pDescription   = "get device's composition data\n"
    "  usage: compget [address] [page num]\n"
    "    e.g. compget 0x0123 0\n",
};

const sh_cmd_t  g_cli_app_key_add STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "appkeyadd",
    .cmd_exec       = _cli_cmd_app_key_add,
    .pDescription   = "add app key to device with index 0\n"
    "  usage: appkeyadd [address]\n"
    "    e.g. appkeyadd 0x0123\n",
};

const sh_cmd_t  g_cli_model_app_bind STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "modelappbind",
    .cmd_exec       = _cli_cmd_model_app_bind,
    .pDescription   = "bind app key index 0 to specific model\n"
    "  usage: modelappbind [address] [model id]\n"
    "    e.g. modelappbind 0x0123 0x1000(onoff server model:0x1000, level server model:0x1002)\n",
};

const sh_cmd_t  g_cli_cmd_subscribe_list_del STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "subdel",
    .cmd_exec       = _cli_cmd_device_subscribe_list_delete,
    .pDescription   = "delete specific model to device's subscribe list\n"
    "  usage: subdel [dst address] [element address] [address] [model ID]\n"
    "    e.g. subdel 0x0123 0x0123 0xc001 0x08310864\n",
};

const sh_cmd_t  g_cli_cmd_subscribe_list_add STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "subadd",
    .cmd_exec       = _cli_cmd_device_subscribe_list_add,
    .pDescription   = "add specific model to device's subscribe list\n"
    "  usage: subadd [dst address] [element address] [address] [model ID]\n"
    "    e.g. subadd 0x0123 0x0123 0xc001 0x08310864\n",
};


const sh_cmd_t  g_cli_onoff_set STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "onoff",
    .cmd_exec       = _cli_cmd_onoff_set,
    .pDescription   = "set device onoff status\n"
    "  usage: onoff [address] [onoffstatus]\n"
    "    e.g. onoff 0x0123 0(0:off/1:on)\n",
};

const sh_cmd_t  g_cli_level_set STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "level",
    .cmd_exec       = _cli_cmd_level_set,
    .pDescription   = "set device level status\n"
    "  usage: level [address] [levelstatus]\n"
    "    e.g. level 0x0123 0x8000(0x8000-0x7fff)\n",
};

const sh_cmd_t  g_cli_node_reset STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name      = "nodereset",
    .cmd_exec       = _cli_cmd_node_reset,
    .pDescription   = "reset device to factory new\n"
    "  usage: nodereset [address]\n"
    "    e.g. nodereset 0x0123\n",
};

void app_model_evt_parse(mesh_app_mdl_evt_msg_idc_t *pt_msg_idc)
{
    uint32_t opcode_len, opcode;

    opcode_len = (pt_msg_idc->opcode & 0xFFFF0000) ? 4 :
                 (pt_msg_idc->opcode & 0xFF00) ? 2 : 1;

    opcode = (opcode_len == 4) ? BE2LE32(pt_msg_idc->opcode) :
             (opcode_len == 2) ? BE2LE16(pt_msg_idc->opcode) : pt_msg_idc->opcode;

    switch (opcode)
    {
    case MMDL_GEN_ONOFF_STATUS_OPCODE:
    {
        gen_on_off_status_msg_t *p_gen_onoff_status = (gen_on_off_status_msg_t *)pt_msg_idc->parameter;
        printf("Generic onoff status [src address: 0x%04x]\nApp key index: %d\n",
                   pt_msg_idc->src_addr, pt_msg_idc->appkey_index);
        printf("present onoff: %d\n", p_gen_onoff_status->present_onoff);

    }
    break;

    case MMDL_GEN_LEVEL_STATUS_OPCODE:
    {
        gen_level_status_msg_t *p_gen_level_status = (gen_level_status_msg_t *)pt_msg_idc->parameter;
        printf("Generic level status [src address: 0x%04x]\nApp key index: %d\n",
                   pt_msg_idc->src_addr, pt_msg_idc->appkey_index);
        printf("present level: %d\n", p_gen_level_status->present_level);
    }
    break;

    default:
        break;
    }
}

void cfg_model_evt_parse(mesh_cfg_mdl_evt_msg_idc_t *pt_msg_idc)
{

    switch (pt_msg_idc->opcode)
    {
    case CONFIG_COMPOSITION_DATA_STATUS:
    {
        cfg_comp_data_status *p_comp_data_status = (cfg_comp_data_status *)pt_msg_idc->parameter;
        element_format *p_element;
        uint32_t vendor_model;
        uint16_t sig_model;
        uint8_t parse_len = 0, element_idx = 0, i, *p_model, elements_parse_len = 0;

        parse_len += sizeof(uint8_t);

        if (p_comp_data_status->page_num == 0)
        {
            parse_len += sizeof(comp_data_page_0);

            printf("config composition data status [src address: 0x%04x]\n", pt_msg_idc->src_addr);
            while (parse_len < pt_msg_idc->parameter_len)
            {
                p_element = (element_format *)(p_comp_data_status->data.comp_data.elements + elements_parse_len);
                parse_len += sizeof(element_format);
                elements_parse_len += sizeof(element_format);
                p_model = p_element->models;

                printf("Element[%d]: %d SIG model & %d vendor model\n", element_idx, p_element->num_s, p_element->num_v);

                if (p_element->num_s > 0)
                {
                    printf("SIG models:\n");
                    for (i = 0; i < p_element->num_s; i++)
                    {
                        memcpy((uint8_t *)&sig_model, p_model, sizeof(uint16_t));
                        printf("[%d] 0x%04x\n", i, sig_model);
                        p_model += sizeof(uint16_t);
                        parse_len += sizeof(uint16_t);
                        elements_parse_len += sizeof(uint16_t);
                    }
                }

                if (p_element->num_v > 0)
                {
                    printf("Vendor models:\n");
                    for (i = 0; i < p_element->num_v; i++)
                    {
                        memcpy((uint8_t *)&vendor_model, p_model, sizeof(uint32_t));
                        printf("[%d] Company ID 0x%04x ", i, (vendor_model & 0xffff));
                        printf("Vendor model ID 0x%04x\n", (vendor_model >> 16) & 0xffff);
                        p_model += sizeof(uint32_t);
                        parse_len += sizeof(uint32_t);
                        elements_parse_len += sizeof(uint32_t);
                    }
                }

                element_idx ++;
            }

        }
        else
        {
            printf("Reserved page number: %d\n", p_comp_data_status->page_num);
        }
    }
    break;

    case CONFIG_APPKEY_STATUS:
    {
        cfg_appkey_status *p_app_key_status = (cfg_appkey_status *)pt_msg_idc->parameter;

        printf("Config appkey status [src address: 0x%04x]\nstatus 0x%02x, index: %d\n",
                   pt_msg_idc->src_addr, p_app_key_status->status, p_app_key_status->key_index);
    }
    break;

    case CONFIG_MDL_APP_STATUS:
    {
        cfg_model_app_status *p_app_status = (cfg_model_app_status *)pt_msg_idc->parameter;


        printf("Config model app status [src address: 0x%04x]\nstatus 0x%02x, element_address 0x%04x, appkey index: %d ",
                   pt_msg_idc->src_addr, p_app_status->status, p_app_status->element_address, p_app_status->appkey_index);

        if (pt_msg_idc->parameter_len == sizeof(cfg_model_app_status))
        {
            printf("vendor model ID 0x%08x\n", p_app_status->model_id.vendor_model);
        }
        else
        {
            printf("sig model ID 0x%04x\n", p_app_status->model_id.sig_model);
        }
    }
    break;

    case CONFIG_NODE_RESET_STATUS:
    {
        printf("Config node reset status [src address: 0x%04x]\n", pt_msg_idc->src_addr);
    }
    break;

    default:
        break;
    }
}

bool auto_prov_started(void)
{
    uint8_t i;
    for (i = 0; i < MAX_UNPROV_DEVICE_NUM ; i++)
    {
        if (unprov_device_list[i].state == DEVICE_PROVISION_START)
        {
            return true;
        }
    }
    return false;
}

void auto_prov_device_start(uint16_t primary_addr, uint8_t *p_device_uuid)
{
    uint8_t i;

    if (auto_provision == true)
    {
        for (i = 0; i < MAX_UNPROV_DEVICE_NUM; i++)
        {
            if ((unprov_device_list[i].state != DEVICE_INVALID) &&
                    (memcmp(unprov_device_list[i].device_uuid, p_device_uuid, 16)) == 0)
            {
                break;
            }
        }

        if (i == MAX_UNPROV_DEVICE_NUM)
        {
            for (i = 0; i < MAX_UNPROV_DEVICE_NUM ; i++)
            {
                if (unprov_device_list[i].state == DEVICE_INVALID)
                {
                    if (auto_prov_started() == false)
                    {
                        ble_mesh_select_unprovisioned_device(primary_addr, p_device_uuid);
                        printf("Start provision device: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                                   p_device_uuid[0], p_device_uuid[1], p_device_uuid[2], p_device_uuid[3],
                                   p_device_uuid[4], p_device_uuid[5], p_device_uuid[6], p_device_uuid[7],
                                   p_device_uuid[8], p_device_uuid[9], p_device_uuid[10], p_device_uuid[11],
                                   p_device_uuid[12], p_device_uuid[13], p_device_uuid[14], p_device_uuid[15]);
                        unprov_device_list[i].state = DEVICE_PROVISION_START;
                    }
                    else
                    {
                        unprov_device_list[i].state = DEVICE_PROVISION_WAITING;
                        printf("Pending provision device: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                                   p_device_uuid[0], p_device_uuid[1], p_device_uuid[2], p_device_uuid[3],
                                   p_device_uuid[4], p_device_uuid[5], p_device_uuid[6], p_device_uuid[7],
                                   p_device_uuid[8], p_device_uuid[9], p_device_uuid[10], p_device_uuid[11],
                                   p_device_uuid[12], p_device_uuid[13], p_device_uuid[14], p_device_uuid[15]);
                    }
                    memcpy(unprov_device_list[i].device_uuid, p_device_uuid, 16);
                    break;
                }
            }
        }
    }
}

void auto_prov_device_complete(uint16_t *p_primary_addr, mesh_prov_complete_idc_t *p_prov_complete_idc)
{
    uint8_t i;

    if (auto_provision == true)
    {
        if (p_prov_complete_idc->status == 0)
        {
            printf("Provision success\n");
            printf("device uuid: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                       p_prov_complete_idc->device_uuid[0], p_prov_complete_idc->device_uuid[1], p_prov_complete_idc->device_uuid[2], p_prov_complete_idc->device_uuid[3],
                       p_prov_complete_idc->device_uuid[4], p_prov_complete_idc->device_uuid[5], p_prov_complete_idc->device_uuid[6], p_prov_complete_idc->device_uuid[7],
                       p_prov_complete_idc->device_uuid[8], p_prov_complete_idc->device_uuid[9], p_prov_complete_idc->device_uuid[10], p_prov_complete_idc->device_uuid[11],
                       p_prov_complete_idc->device_uuid[12], p_prov_complete_idc->device_uuid[13], p_prov_complete_idc->device_uuid[14], p_prov_complete_idc->device_uuid[15]);
            printf("device key: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                       p_prov_complete_idc->device_key[0], p_prov_complete_idc->device_key[1], p_prov_complete_idc->device_key[2], p_prov_complete_idc->device_key[3],
                       p_prov_complete_idc->device_key[4], p_prov_complete_idc->device_key[5], p_prov_complete_idc->device_key[6], p_prov_complete_idc->device_key[7],
                       p_prov_complete_idc->device_key[8], p_prov_complete_idc->device_key[9], p_prov_complete_idc->device_key[10], p_prov_complete_idc->device_key[11],
                       p_prov_complete_idc->device_key[12], p_prov_complete_idc->device_key[13], p_prov_complete_idc->device_key[14], p_prov_complete_idc->device_key[15]);
            printf("element address 0x%04x\n", p_prov_complete_idc->element_address);
            printf("element count %d\n", p_prov_complete_idc->element_count);
            *p_primary_addr = (p_prov_complete_idc->element_address + p_prov_complete_idc->element_count);

            for (i = 0; i < MAX_PROV_DEVICE_NUM; i++)
            {
                if (prov_device_list[i].primary_addr == 0)
                {
                    prov_device_list[i].primary_addr = p_prov_complete_idc->element_address;
                    memcpy(prov_device_list[i].device_key, p_prov_complete_idc->device_key, 16);
                    break;
                }
            }

            if (i == MAX_PROV_DEVICE_NUM)
            {
                printf("No resource to save device key\n");
            }

        }
        else
        {
            printf("Provision fail\n");
            printf("device uuid: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                       p_prov_complete_idc->device_uuid[0], p_prov_complete_idc->device_uuid[1], p_prov_complete_idc->device_uuid[2], p_prov_complete_idc->device_uuid[3],
                       p_prov_complete_idc->device_uuid[4], p_prov_complete_idc->device_uuid[5], p_prov_complete_idc->device_uuid[6], p_prov_complete_idc->device_uuid[7],
                       p_prov_complete_idc->device_uuid[8], p_prov_complete_idc->device_uuid[9], p_prov_complete_idc->device_uuid[10], p_prov_complete_idc->device_uuid[11],
                       p_prov_complete_idc->device_uuid[12], p_prov_complete_idc->device_uuid[13], p_prov_complete_idc->device_uuid[14], p_prov_complete_idc->device_uuid[15]);

        }

        for (i = 0; i < MAX_UNPROV_DEVICE_NUM; i++)
        {
            if ((unprov_device_list[i].state == DEVICE_PROVISION_START) &&
                    (memcmp(unprov_device_list[i].device_uuid, p_prov_complete_idc->device_uuid, 16)) == 0)
            {
                unprov_device_list[i].state = DEVICE_INVALID;
                break;
            }
        }

        for (i = 0; i < MAX_UNPROV_DEVICE_NUM; i++)
        {
            if (unprov_device_list[i].state == DEVICE_PROVISION_WAITING)
            {
                unprov_device_list[i].state = DEVICE_PROVISION_START;
                ble_mesh_select_unprovisioned_device(*p_primary_addr, unprov_device_list[i].device_uuid);
                printf("Start provision device: 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                           unprov_device_list[i].device_uuid[0], unprov_device_list[i].device_uuid[1], unprov_device_list[i].device_uuid[2], unprov_device_list[i].device_uuid[3],
                           unprov_device_list[i].device_uuid[4], unprov_device_list[i].device_uuid[5], unprov_device_list[i].device_uuid[6], unprov_device_list[i].device_uuid[7],
                           unprov_device_list[i].device_uuid[8], unprov_device_list[i].device_uuid[9], unprov_device_list[i].device_uuid[10], unprov_device_list[i].device_uuid[11],
                           unprov_device_list[i].device_uuid[12], unprov_device_list[i].device_uuid[13], unprov_device_list[i].device_uuid[14], unprov_device_list[i].device_uuid[15]);
                break;
            }
        }

    }

}
#endif

