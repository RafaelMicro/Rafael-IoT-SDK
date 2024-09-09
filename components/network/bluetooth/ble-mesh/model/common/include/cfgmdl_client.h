
#ifndef CFGMDL_CLIENT_H
#define CFGMDL_CLIENT_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "mesh_api.h"
#include "pib.h"
#include "mmdl_opcodes.h"
#include "mmdl_defs.h"


#define CONFIG_APPKEY_ADD                           0x00
#define CONFIG_APPKEY_UPDATE                        0x01
#define CONFIG_COMPOSITION_DATA_STATUS              0x02
#define CONFIG_PUBLICATION_SET                      0x03

#define CONFIG_APPKEY_DELETE                        0x0080
#define CONFIG_APPKEY_GET                           0x0180
#define CONFIG_APPKEY_LIST                          0x0280
#define CONFIG_APPKEY_STATUS                        0x0380

#define CONFIG_COMPOSITION_DATA_GET                 0x0880
#define CONFIG_MDL_SUBSCRIPTION_ADD                     0x1B80
#define CONFIG_MDL_SUBSCRIPTION_DELETE                  0x1C80
#define CONFIG_MDL_SUBSCRIPTION_DELETE_ALL              0x1D80
#define CONFIG_MDL_SUBSCRIPTION_OVERWRITE               0x1E80
#define CONFIG_MDL_SUBSCRIPTION_STATUS                  0x1F80
#define CONFIG_SIG_MODEL_SUBSCRIPTION_GET           0x2980
#define CONFIG_SIG_MODEL_SUBSCRIPTION_LIST          0x2A80
#define CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET        0x2B80
#define CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST       0x2C80
#define CONFIG_MDL_APP_BIND                             0x3D80
#define CONFIG_MDL_APP_STATUS                           0x3E80
#define CONFIG_MDL_APP_UNBIND                           0x3F80
#define CONFIG_NETKEY_ADD                           0x4080
#define CONFIG_NETKEY_DELETE                        0x4180
#define CONFIG_NETKEY_GET                           0x4280
#define CONFIG_NETKEY_LIST                          0x4380
#define CONFIG_NETKEY_STATUS                        0x4480
#define CONFIG_NETKEY_UPDATE                        0x4580
#define CONFIG_NODE_RESET                           0x4980
#define CONFIG_NODE_RESET_STATUS                    0x4A80
#define CONFIG_SIG_MODEL_APP_GET                    0x4B80
#define CONFIG_SIG_MODEL_APP_LIST                   0x4C80
#define CONFIG_VENDOR_MODEL_APP_GET                 0x4D80
#define CONFIG_VENDOR_MODEL_APP_LIST                0x4E80


typedef struct __attribute__((packed))
{
    uint16_t loc;
    uint8_t  num_s;    /*Contains a count of SIG Model IDs in this element*/
    uint8_t  num_v;    /*Contains a count of Vendor Model IDs in this element*/
    uint8_t  models[];
}
element_format;

typedef struct __attribute__((packed))
{
    uint16_t  cid;   /*company identifier assigned by the Bluetooth SIG*/
    uint16_t  pid;   /*vendor-assigned product identifier*/
    uint16_t  vid;
    uint16_t  crpl;
    uint16_t  relay: 1,     /*Features*/
              proxy: 1,     /*Features*/
              friend: 1,    /*Features*/
              low_power: 1, /*Features*/
              rfu: 12;      /*Features*/
    uint8_t   elements[];
}
comp_data_page_0;

typedef struct __attribute__((packed))
{
    uint8_t   page_num;
    __packed union
    {
        comp_data_page_0 comp_data;
    } data;
}
cfg_comp_data_status;


typedef struct
{
    uint32_t  status: 8;
    uint32_t  key_index: 24;
} cfg_appkey_status;

typedef struct __attribute__((packed))
{
    uint8_t   status;
    uint16_t  element_address;
    uint16_t  appkey_index;
    __packed union
    {
        uint16_t sig_model;
        uint32_t vendor_model;
    } model_id;
}
cfg_model_app_status;

void cfgmdl_client_send(uint32_t opcode, uint16_t dst, uint8_t *p_param, uint16_t pram_len);


#ifdef __cplusplus
}
#endif

#endif /* CFGMDL_CLIENT_H */
