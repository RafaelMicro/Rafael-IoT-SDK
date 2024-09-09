/**
 * @file at_cmd.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <sys/errno.h>

#include <stdlib.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <log.h>

#include "main.h"
#include <openthread/coap.h>
#include <openthread/dataset.h>
#include <openthread/dataset_ftd.h>
#include <openthread/dataset_updater.h>
#include "util_string.h"

//=============================================================================
//                  Constant Definition
//=============================================================================
typedef uint8_t (*atcmd_fn)(uint8_t *args, uint16_t args_lens, uint8_t type);
#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
//=============================================================================
//                  Macro Definition
//=============================================================================
#define AT_CMD_CHAR_0                  '0'
#define AT_CMD_CHAR_9                  '9'
#define AT_CMD_QUESTION_MARK           '?'
#define AT_CMD_EQUAL_MARK              '='
#define AT_CMD_L_SQ_BRACKET            '['
#define AT_CMD_R_SQ_BRACKET            ']'
#define AT_CMD_L_ANGLE_BRACKET         '<'
#define AT_CMD_R_ANGLE_BRACKET         '>'
#define AT_CMD_COMMA_MARK              ','
#define AT_CMD_SEMICOLON               ';'
#define AT_CMD_CR                      '\r'
#define AT_CMD_LF                      '\n'
#define AT_CMD_NULL                    '\0'

#define AT_CMD_NAME_LEN                16


#define OT_DEFAULT_COAP_PORT 5683 ///< Default CoAP port, as specified in RFC 7252
#define RAFAEL_OTA_URL_DATA "ota/data"
#define IP_NONE "0:0:0:0:0:0:0:0"
#define OTA_SEGMENTS_MAX_SIZE 256
//=============================================================================
//                  Structure Definition
//=============================================================================
typedef enum{
    AT_CMD_TEST_TYPE = 0x0,
    AT_CMD_QUERY_TYPE,
    AT_CMD_SETTING_TYPE,
    AT_CMD_EXECUTE_TYPE,
    AT_CMD_UNKNOW_TYPE
}at_cmd_type_t;

typedef struct 
{
    char command[AT_CMD_NAME_LEN];
    atcmd_fn cmd_fn;
}at_cmd_t;

typedef struct
{
    uint32_t version;
    uint32_t size;
    uint32_t crc;
    uint16_t seq;
    uint16_t segments;
    uint16_t intervel;
    bool is_unicast;
    uint8_t data[OTA_SEGMENTS_MAX_SIZE];
} __attribute__((packed)) ota_data_t;
//=============================================================================
//                  Global Data Definition
//=============================================================================
static bool at_hex_mode = false;
static bool at_ota_binded = false;
static uint32_t at_ota_version = 0;
static uint32_t at_ota_size = 0;
static uint32_t at_ota_crc = 0;
static uint16_t at_ota_segments = 0;
static char at_ota_dst_addr_str[OT_IP6_ADDRESS_STRING_SIZE] = IP_NONE;
static otCoapResource at_ota_data_resource;
const char at_ota_data_Uri_Path[] = RAFAEL_OTA_URL_DATA;

/*at command parse use*/
static char *argv[256];
static int argv_cnt= 0;

uint16_t at_udp_port = 5678;
otOperationalDataset at_dataset;   
//=============================================================================
//                  Private Function Definition
//=============================================================================
static void at_req_parse_args(const char *req_args, const char *req_expr, ...)
{
    va_list args;
    int req_args_num = 0;

    va_start(args, req_expr);

    req_args_num = vsscanf(req_args, req_expr, args);

    va_end(args);
}

/*NCP-AT Unsolicited Messages Lists*/
static void ot_stateChangeCallback(uint32_t flags, void * p_context) 
{
    char states[5][10] = {"disabled", "detached", "child", "router", "leader"};
    otInstance *instance = (otInstance *)p_context;
    uint8_t *p;
    uint8_t *state_data = NULL;
    uint16_t len;

    if (flags & OT_CHANGED_THREAD_ROLE)
    {
        uint32_t role = otThreadGetDeviceRole(p_context);
        switch(role)
        {
            case OT_DEVICE_ROLE_CHILD:
                break;
            case OT_DEVICE_ROLE_ROUTER:
                p = (uint8_t *)(otThreadGetRloc(instance)->mFields.m8);
                len = 79;
                state_data = pvPortMalloc(len);
                if(state_data)
                {
                    snprintf(state_data, len, "+URC:\"Network State\",\"ROUTER\",\"RLOC\"=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
                                    p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

                }                 
                break;
            case OT_DEVICE_ROLE_LEADER:
                p = (uint8_t *)(otThreadGetRloc(instance)->mFields.m8);
                len = 79;
                state_data = pvPortMalloc(len);
                if(state_data)
                {
                    snprintf(state_data, len, "+URC:\"Network State\",\"LEADER\",\"RLOC\"=%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
                                    p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
                }    
                break;

            case OT_DEVICE_ROLE_DISABLED:
            case OT_DEVICE_ROLE_DETACHED:
                // +URC: "Network State", "DETACHED"
                len = 36;
                state_data = pvPortMalloc(len);
                if(state_data)
                {
                    snprintf(state_data, len, "+URC:\"Network State\",\"DETACHED\"\r\n");

                } 
            default:
                break;
        }
    }

    if(state_data)
    {
        cp_sub_cmd_send(state_data, len);
        vPortFree(state_data);
    }    
}
static void ot_neighborChangeCallback (otNeighborTableEvent aEvent, const otNeighborTableEntryInfo *aEntryInfo) 
{
    uint8_t *nei_data = NULL;
    uint16_t len = 0;

    switch (aEvent) {
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED:
            len = 38;
            nei_data = pvPortMalloc(len);
            snprintf(nei_data, len, "+URC:\"Child Added\":%llx%c%c", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8, 0x0a, 0x0d);

            log_info("child added %llx", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED:
            len = 40;
            nei_data = pvPortMalloc(len);
            snprintf(nei_data, len, "+URC:\"Child Removed\":%llx%c%c", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8, 0x0a, 0x0d);

            log_info("child removed %llx", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;

        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_ADDED:
            len = 39;
            nei_data = pvPortMalloc(len);
            snprintf(nei_data, len, "+URC:\"Router Added\":%llx%c%c", *(uint64_t *)aEntryInfo->mInfo.mRouter.mExtAddress.m8, 0x0a, 0x0d);

            log_info("router added %llx", *(uint64_t *)aEntryInfo->mInfo.mRouter.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_REMOVED:
            len = 41;
            nei_data = pvPortMalloc(len);
            snprintf(nei_data, len, "+URC:\" Router Removed\":%llx%c%c", *(uint64_t *)aEntryInfo->mInfo.mRouter.mExtAddress.m8, 0x0a, 0x0d);

            log_info("router removed %llx", *(uint64_t *)aEntryInfo->mInfo.mRouter.mExtAddress.m8);
            break;
    }
    if(nei_data)
    {
        cp_sub_cmd_send(nei_data, len);
        vPortFree(nei_data);
    }
}

static void __ota_data_piece(uint8_t *payload, uint16_t *payloadlength, void *data)
{
    ota_data_t *ota_data = NULL;
    uint16_t ota_data_lens = 0;
    uint32_t toatol_num = 0;

    uint8_t *tmp = payload;

    ota_data = (ota_data_t *)data;
    memcpy(tmp, &ota_data->version, 4);
    tmp += 4;
    memcpy(tmp, &ota_data->size, 4);
    tmp += 4;
    memcpy(tmp, &ota_data->crc, 4);
    tmp += 4;
    memcpy(tmp, &ota_data->seq, 2);
    tmp += 2;
    memcpy(tmp, &ota_data->segments, 2);
    tmp += 2;
    memcpy(tmp, &ota_data->intervel, 2);
    tmp += 2;
    *tmp++ = ota_data->is_unicast;
    toatol_num = (ota_data->size / ota_data->segments);
    if (ota_data->size % ota_data->segments)
    {
        ++toatol_num;
    }
    if (ota_data->seq == (toatol_num - 1))
    {
        ota_data_lens = ota_data->size % ota_data->segments;
    }
    else
    {
        ota_data_lens = ota_data->segments;
    }

    memcpy(tmp, &ota_data->data, ota_data_lens);
    tmp += ota_data_lens;
    *payloadlength = (tmp - payload);
}

static void __ota_coap_ack_cb(void *aContext, otMessage *otMsg, const otMessageInfo *otInfo, otError aResult)
{
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    uint16_t length;
    otMessage *responseMessage = NULL;
    otCoapCode responseCode = OT_COAP_CODE_EMPTY;
    otError       error   = OT_ERROR_NONE;
    uint8_t *coap_data = NULL;
    otIp6AddressToString(&otInfo->mPeerAddr, string, sizeof(string));
    length = otMessageGetLength(otMsg) - otMessageGetOffset(otMsg);
    
    do
    {
        if (length > 0)
        {
            coap_data = pvPortMalloc(length + 63);
            if(coap_data == NULL)
            {
                break;
            }
            snprintf(coap_data, 15, "+URC:\"OTAAck\":");
            otMessageRead(otMsg, otMessageGetOffset(otMsg), &coap_data[16], length);
            snprintf(&coap_data[length + 15] , 48, ",\"IP\":%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n", 
                        otInfo->mPeerAddr.mFields.m8[0], otInfo->mPeerAddr.mFields.m8[1], otInfo->mPeerAddr.mFields.m8[2], otInfo->mPeerAddr.mFields.m8[3],
                        otInfo->mPeerAddr.mFields.m8[4], otInfo->mPeerAddr.mFields.m8[5], otInfo->mPeerAddr.mFields.m8[6], otInfo->mPeerAddr.mFields.m8[7],
                        otInfo->mPeerAddr.mFields.m8[8], otInfo->mPeerAddr.mFields.m8[9], otInfo->mPeerAddr.mFields.m8[10], otInfo->mPeerAddr.mFields.m8[11],
                        otInfo->mPeerAddr.mFields.m8[12], otInfo->mPeerAddr.mFields.m8[13], otInfo->mPeerAddr.mFields.m8[14], otInfo->mPeerAddr.mFields.m8[15]);

            cp_sub_cmd_send(coap_data, (length + 63));

            vPortFree(coap_data);
        }
            
    } while (0);
    if (error != OT_ERROR_NONE && responseMessage != NULL)
    {
        otMessageFree(responseMessage);
    }
}

otError __ota_coap_request(otCoapCode aCoapCode, otIp6Address coapDestinationIp, otCoapType coapType, uint8_t *payload, uint16_t payloadLength, const char *coap_Path)
{
    otError       error   = OT_ERROR_NONE;
    otMessage    *message = NULL;
    otMessageInfo messageInfo;

    // Default parameters

    do
    {
        otInstance *instance = otrGetInstance();
        if(!instance)
        {
            break;
        }
        message = otCoapNewMessage(instance, NULL);
        if (NULL == message)
        {
            error = OT_ERROR_NO_BUFS;
            break;
        }
        otCoapMessageInit(message, coapType, aCoapCode);
        otCoapMessageGenerateToken(message, OT_COAP_DEFAULT_TOKEN_LENGTH);

        error = otCoapMessageAppendUriPathOptions(message, coap_Path);
        if (OT_ERROR_NONE != error)
        {
            break;
        }

        if (payloadLength > 0)
        {
            error = otCoapMessageSetPayloadMarker(message);
            if (OT_ERROR_NONE != error)
            {
                break;
            }
        }

        // Embed content into message if given
        if (payloadLength > 0)
        {
            error = otMessageAppend(message, payload, payloadLength);
            if (OT_ERROR_NONE != error)
            {
                break;
            }
        }

        memset(&messageInfo, 0, sizeof(messageInfo));
        messageInfo.mPeerAddr = coapDestinationIp;
        messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

        if ((coapType == OT_COAP_TYPE_CONFIRMABLE) || (aCoapCode == OT_COAP_CODE_GET))
        {
            error = otCoapSendRequestWithParameters(instance, message, &messageInfo, &__ota_coap_ack_cb,
                                                    NULL, NULL);
        }
        else
        {
            error = otCoapSendRequestWithParameters(instance, message, &messageInfo, NULL, NULL, NULL);
        }
    } while (0);

    if ((error != OT_ERROR_NONE) && (message != NULL))
    {
        otMessageFree(message);
    }
    log_info("error %u ",error);
    return error;
}

void __udp_cb(otMessage *otMsg, const otMessageInfo *otInfo)
{
    uint8_t *udp_data = NULL;
    uint16_t len;

    len = otMessageGetLength(otMsg) - otMessageGetOffset(otMsg);
    udp_data = pvPortMalloc(len + 61);

    do
    {
        if(udp_data == NULL)
            break;
        
        snprintf(udp_data, 13, "+URC:\"Data\":");
        otMessageRead(otMsg, otMessageGetOffset(otMsg), &udp_data[13], len);
        snprintf(&udp_data[len + 13] , 48, ",\"IP\":%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n", 
                    otInfo->mPeerAddr.mFields.m8[0], otInfo->mPeerAddr.mFields.m8[1], otInfo->mPeerAddr.mFields.m8[2], otInfo->mPeerAddr.mFields.m8[3],
                    otInfo->mPeerAddr.mFields.m8[4], otInfo->mPeerAddr.mFields.m8[5], otInfo->mPeerAddr.mFields.m8[6], otInfo->mPeerAddr.mFields.m8[7],
                    otInfo->mPeerAddr.mFields.m8[8], otInfo->mPeerAddr.mFields.m8[9], otInfo->mPeerAddr.mFields.m8[10], otInfo->mPeerAddr.mFields.m8[11],
                    otInfo->mPeerAddr.mFields.m8[12], otInfo->mPeerAddr.mFields.m8[13], otInfo->mPeerAddr.mFields.m8[14], otInfo->mPeerAddr.mFields.m8[15]);

        cp_sub_cmd_send(udp_data, (len + 61));

        vPortFree(udp_data);
    } while (0);
    return NULL;
}

/*NCP-AT command Lists*/
static uint8_t __udpsend(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint16_t udp_port = 0, udplen = 0;
    uint8_t *udp_data =NULL;
    otIp6Address ip6_addr;
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    switch(type)
    {
        case AT_CMD_TEST_TYPE:     
            snprintf(rsp_data, 49, "+UDPSEND:<ipaddr>,<port>,<data lengths>,<data>\r\n");      
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            break;
        case AT_CMD_SETTING_TYPE:
             OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    char *parm = NULL;
                    argv_cnt= 0;
                    for(parm = args; (parm-(char*)args) < args_lens; parm++)
                    {
                        if ((*parm != AT_CMD_COMMA_MARK) && ((argv_cnt == 0) || (*(parm - 1) == AT_CMD_COMMA_MARK)))
                        {
                            argv[argv_cnt++] = parm;
                            if(*(parm - 1) == 0x2C)
                            {
                                *(parm - 1) = '\0';
                            }
                        }
                    }     
                    ret = otIp6AddressFromString(argv[0], &ip6_addr);
                    udp_port = utility_strtox(argv[1], 0, 4);
                    udplen = utility_strtox(argv[2], 0, 4);
                    udp_data = argv[3];
                        
                    log_info_hexdump("ip", ip6_addr.mFields.m8, OT_IP6_ADDRESS_SIZE);
                    log_info("port : 0x%04X", udp_port);
                    log_info_hexdump("data",  udp_data, udplen);
                    ret = app_udpSend(instance, &ip6_addr, udp_data, udplen, udp_port);  
                }                  
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}

static uint8_t __udpbind(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    switch(type)
    {
        case AT_CMD_TEST_TYPE:     
            snprintf(rsp_data, 24, "+UDPBIND:<0x0~0xFFFF>\r\n");      
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            snprintf(rsp_data, 16, "+UDPBIND:%04X\r\n",at_udp_port);      
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_SETTING_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    if((args[0] == '0' && args[1] == 'x') ||
                    (args[0] == '0' && args[1] == 'X'))
                    {
                        at_udp_port = utility_strtox(&args[2], 0, 4);
                        ret = app_sockInit(instance, __udp_cb, at_udp_port);
                    }
                }   
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}

static uint8_t __version(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    switch(type)
    {
        case AT_CMD_TEST_TYPE:
            break;
        case AT_CMD_QUERY_TYPE:
            snprintf(rsp_data, 12+20+12+9, "+VERSION:%s,%s,%s\r\n",RAFAEL_SDK_VER,__DATE__,__TIME__);      
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_SETTING_TYPE:
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}

static uint8_t __start(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    switch(type)
    {
        case AT_CMD_TEST_TYPE:
            break;
        case AT_CMD_QUERY_TYPE:
            break;
        case AT_CMD_SETTING_TYPE:
            break;
        case AT_CMD_EXECUTE_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    otSetStateChangedCallback(instance, ot_stateChangeCallback, instance);
                    otThreadRegisterNeighborTableCallback(instance, ot_neighborChangeCallback);

                    ret = otIp6SetEnabled(instance, true);

                    ret = otThreadSetEnabled(instance, true);
                }
            )
            break;
        default:
            break;
    }
    return ret;
}

static uint8_t __stop(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    switch(type)
    {
        case AT_CMD_TEST_TYPE:
            break;
        case AT_CMD_QUERY_TYPE:
            break;
        case AT_CMD_SETTING_TYPE:
            break;
        case AT_CMD_EXECUTE_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    ret = otIp6SetEnabled(instance, false);

                    ret = otThreadSetEnabled(instance, false);
                }
            )
            break;
        default:
            break;
    }
    return ret;
}

static uint8_t __state(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    switch(type)
    {
        case AT_CMD_TEST_TYPE:
            break;
        case AT_CMD_QUERY_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    const char *state_str = otThreadDeviceRoleToString(otThreadGetDeviceRole(instance));
                    snprintf(rsp_data, 10+strlen(state_str), "+STATE:%s\r\n", state_str);
                    cp_sub_cmd_send(rsp_data, strlen(rsp_data));
                    ret = OT_ERROR_NONE;
                }
            )
            break;
        case AT_CMD_SETTING_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    if (memcmp(args,"LEADER", strlen(args)) == 0)
                    {
                        ret = otThreadBecomeLeader(instance);
                    }
                }
            )
            break;
        case AT_CMD_EXECUTE_TYPE:            
            break;
        default:
            break;
    }
    return ret;
}

static uint8_t __set_netname(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    switch(type)
    {
        case AT_CMD_TEST_TYPE:     
            snprintf(rsp_data, 27, "+NETNAME:<16 CHARACTERS>\r\n");      
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    const char *netname = otThreadGetNetworkName(instance);
                    snprintf(rsp_data, 28, "+NETNAME:%s\r\n", netname);
                    cp_sub_cmd_send(rsp_data, strlen(rsp_data));
                    ret = OT_ERROR_NONE;
                }
            )
            break;
        case AT_CMD_SETTING_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    ret =otDatasetGetActive(instance, &at_dataset);
                    memcpy(at_dataset.mNetworkName.m8, args, strlen(args));
                    at_dataset.mComponents.mIsNetworkNamePresent = true;
                    ret =otDatasetSetActive(instance, &at_dataset);
                }   
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}

static uint8_t __set_panid(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    otPanId panid;
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    switch(type)
    {
        case AT_CMD_TEST_TYPE:     
            snprintf(rsp_data, 22, "+PANID:<0x1~0xFFFE>\r\n");      
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    panid = otLinkGetPanId(instance);
                    snprintf(rsp_data, 14, "+PANID:%04X\r\n", panid);
                    cp_sub_cmd_send(rsp_data, strlen(rsp_data));
                    ret = OT_ERROR_NONE;
                }
            )
            break;
        case AT_CMD_SETTING_TYPE:
            panid = utility_strtox(args, 0, 4);
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    ret =otDatasetGetActive(instance, &at_dataset);
                    at_dataset.mPanId = (otPanId)panid;
                    at_dataset.mComponents.mIsPanIdPresent = true;
                    ret =otDatasetSetActive(instance, &at_dataset);
                }
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}

static uint8_t __set_channel(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    uint8_t channel = 0xFF;

    switch(type)
    {
        case AT_CMD_TEST_TYPE:     
            snprintf(rsp_data, 13, "+CH:<1~10>\r\n");      
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    uint8_t ch_hex = otLinkGetChannel(instance);
                    snprintf(rsp_data, 9, "+CH:%02X\r\n", ch_hex);
                    cp_sub_cmd_send(rsp_data, strlen(rsp_data));
                    ret = OT_ERROR_NONE;
                }
            )
            break;
        case AT_CMD_SETTING_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    ret =otDatasetGetActive(instance, &at_dataset);    
                    at_dataset.mChannel = utility_strtoul(args, 0);
                    at_dataset.mComponents.mIsChannelPresent = true;
                    ret =otDatasetSetActive(instance, &at_dataset);
                }
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}

static uint8_t __set_networkkey(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    otNetworkKey netkey;
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);

    switch(type)
    {
        case AT_CMD_TEST_TYPE:      
            snprintf(rsp_data, 17, "+NETKEY:<0~16>\r\n");   
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    otThreadGetNetworkKey(instance, &netkey);
                    snprintf(rsp_data, 43, 
                        "+NETKEY:%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                        netkey.m8[0], netkey.m8[1], netkey.m8[2], netkey.m8[3], netkey.m8[4], netkey.m8[5],
                        netkey.m8[6], netkey.m8[7], netkey.m8[8], netkey.m8[9], netkey.m8[10], netkey.m8[11],
                        netkey.m8[12], netkey.m8[13], netkey.m8[14], netkey.m8[15]);               
                    
                    cp_sub_cmd_send(rsp_data, strlen(rsp_data));
                    ret = OT_ERROR_NONE;
                }
            )            
            break;
        case AT_CMD_SETTING_TYPE:
            memset(netkey.m8,0x0,OT_NETWORK_KEY_SIZE);
            for(int i=0;i<(strlen(args)/2);i++)
            {
                netkey.m8[i]=(uint8_t)utility_strtox(&args[i*2], 0, 2);
            }
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {   
                    ret =otDatasetGetActive(instance, &at_dataset);    
                    memcpy(at_dataset.mNetworkKey.m8, netkey.m8, OT_NETWORK_KEY_SIZE);
                    at_dataset.mComponents.mIsNetworkKeyPresent = true;
                    ret =otDatasetSetActive(instance, &at_dataset);
                }
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}


static uint8_t __extaddr(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    otExtAddress extAddress;
    const otExtAddress *paddr;
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);

    switch(type)
    {
        case AT_CMD_TEST_TYPE:      
            snprintf(rsp_data, 16, "+EXTADDR:<0~8>\r\n");   
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    paddr = otLinkGetExtendedAddress(instance);
                    snprintf(rsp_data, 27, 
                        "+EXTADDR:%02X%02X%02X%02X%02X%02X%02X%02X\r\n",
                        paddr->m8[0], paddr->m8[1], paddr->m8[2], paddr->m8[3], 
                        paddr->m8[4], paddr->m8[5], paddr->m8[6], paddr->m8[7]);               
                    
                    cp_sub_cmd_send(rsp_data, strlen(rsp_data));
                    ret = OT_ERROR_NONE;
                }
            )            
            break;
        case AT_CMD_SETTING_TYPE:
            for(int i=0;i<8;i++)
            {
                extAddress.m8[i]=(uint8_t)utility_strtox(&args[i*2], 0, 2);
            }
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {   
                    ret = otLinkSetExtendedAddress(instance, &extAddress);
                }
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}
static uint8_t __hexmode(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    switch(type)
    {
        case AT_CMD_TEST_TYPE:      
            snprintf(rsp_data, 28, "+HEXMODE:<0:close,1:open>\r\n");   
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            snprintf(rsp_data, 13, 
                "+HEXMODE:%u\r\n",at_hex_mode);
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_SETTING_TYPE:      
            at_hex_mode =  utility_strtoul(args, 0) == 0 ? false: true;
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}
static uint8_t __otabind(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);

    switch(type)
    {
        case AT_CMD_TEST_TYPE:      
            snprintf(rsp_data, 18, "+OTABIND:<ipv6>\r\n");   
            cp_sub_cmd_send(rsp_data, strlen(rsp_data));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            snprintf(rsp_data, 12+strlen(at_ota_dst_addr_str), 
                "+OTABIND:%s\r\n",at_ota_dst_addr_str);
            cp_sub_cmd_send(rsp_data, (12+strlen(at_ota_dst_addr_str))-1);
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_SETTING_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {   
                    if(at_ota_binded == false)
                    {
                        ret = otCoapStart(instance, OT_DEFAULT_COAP_PORT);                        
                    }                    
                }
                memcpy(at_ota_dst_addr_str,args,args_lens);
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}

static uint8_t __otasend(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *buf = pvPortMalloc(256);
    ota_data_t ota_data;
    otIp6Address coapDestinationIp;
    otCoapCode CoapCode = OT_COAP_CODE_POST;
    otCoapType coapType = OT_COAP_TYPE_CONFIRMABLE;
    uint8_t *payload = NULL;
    uint16_t payloadlength = 0, sequence_index = 0, bin_lens = 0;
    switch(type)
    {
        case AT_CMD_TEST_TYPE:      
            snprintf(buf, 47, "+OTASEND:<index>,<Hex_Lens:1~256>,<Hex_Data>\r\n");   
            cp_sub_cmd_send(buf, strlen(buf));
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_QUERY_TYPE:
            break;
        case AT_CMD_SETTING_TYPE:
            ret = otIp6AddressFromString(at_ota_dst_addr_str, &coapDestinationIp);
            if(ret == OT_ERROR_NONE)
            {
                /*sequence 4 byte*/
                sequence_index = args[1] << 8 & 0xff00 | args[0];
                /*bin_lens 2 byte*/
                bin_lens = args[6] << 8 & 0xff00 | args[5];
                if(bin_lens <= 256)
                {
                    memcpy(buf,&args[8],bin_lens);
                    if(sequence_index == 0)
                    {                        
                        at_ota_version = SWAP_UINT32(*(uint32_t *)buf);
                        at_ota_size = SWAP_UINT32(*(uint32_t *)(buf + 24)) + 0x20;
                        at_ota_crc = SWAP_UINT32(*(uint32_t *)(buf + 16));
                        at_ota_segments = bin_lens;                
                    }
                    ota_data.version = at_ota_version;
                    ota_data.size = at_ota_size;
                    ota_data.crc = at_ota_crc;
                    ota_data.seq = sequence_index;
                    ota_data.segments = at_ota_segments;
                    ota_data.intervel = 500;
                    ota_data.is_unicast = true;
                    memcpy(ota_data.data, buf, bin_lens);
                    payload = pvPortMalloc(sizeof(ota_data_t));
                    if(payload)
                    {
                        __ota_data_piece(payload, &payloadlength, &ota_data);
                        ret = __ota_coap_request(CoapCode, coapDestinationIp, coapType, payload, payloadlength, RAFAEL_OTA_URL_DATA);
                    }     
                }
            }                       
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(buf) vPortFree(buf);
    if (payload) vPortFree(payload);
    return ret;
}

static uint8_t __dataset(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    char *rsp_data = pvPortMalloc(256);
    char *parm = NULL;
    switch(type)
    {
        case AT_CMD_TEST_TYPE:
            break;
        case AT_CMD_QUERY_TYPE:            
            snprintf(rsp_data,29,"+DATASET,ACTIVETIME,%08d", (uint32_t)at_dataset.mActiveTimestamp.mSeconds);

            snprintf(&rsp_data[28],7,",CH,%02x", at_dataset.mChannel);
            
            snprintf(&rsp_data[34],30,",EXTEEDPANID,%02X%02X%02X%02X%02X%02X%02X%02X", 
                    at_dataset.mExtendedPanId.m8[0],at_dataset.mExtendedPanId.m8[1],
                    at_dataset.mExtendedPanId.m8[2],at_dataset.mExtendedPanId.m8[3],
                    at_dataset.mExtendedPanId.m8[4],at_dataset.mExtendedPanId.m8[5],
                    at_dataset.mExtendedPanId.m8[6],at_dataset.mExtendedPanId.m8[7]);

            snprintf(&rsp_data[63],29,",MESHPREFIX,%02X%02X%02X%02X%02X%02X%02X%02X", 
                    at_dataset.mMeshLocalPrefix.m8[0],at_dataset.mMeshLocalPrefix.m8[1],
                    at_dataset.mMeshLocalPrefix.m8[2],at_dataset.mMeshLocalPrefix.m8[3],
                    at_dataset.mMeshLocalPrefix.m8[4],at_dataset.mMeshLocalPrefix.m8[5],
                    at_dataset.mMeshLocalPrefix.m8[6],at_dataset.mMeshLocalPrefix.m8[7]);

            snprintf(&rsp_data[91],41,",NETKEY,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
                    at_dataset.mNetworkKey.m8[0],at_dataset.mNetworkKey.m8[1],
                    at_dataset.mNetworkKey.m8[2],at_dataset.mNetworkKey.m8[3],
                    at_dataset.mNetworkKey.m8[4],at_dataset.mNetworkKey.m8[5],
                    at_dataset.mNetworkKey.m8[6],at_dataset.mNetworkKey.m8[7],
                    at_dataset.mNetworkKey.m8[8],at_dataset.mNetworkKey.m8[9],
                    at_dataset.mNetworkKey.m8[10],at_dataset.mNetworkKey.m8[11],
                    at_dataset.mNetworkKey.m8[12],at_dataset.mNetworkKey.m8[13],
                    at_dataset.mNetworkKey.m8[14],at_dataset.mNetworkKey.m8[15]);

            snprintf(&rsp_data[131],27,",NETNAME,%s",at_dataset.mNetworkName.m8);

            snprintf(&rsp_data[157],14,",PANID,%04X\r\n",at_dataset.mPanId);
            cp_sub_cmd_send(rsp_data, 170);
            ret = OT_ERROR_NONE;
            break;
        case AT_CMD_SETTING_TYPE:                
                
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    argv_cnt= 0;
                    for(parm = args; (parm-(char*)args) < args_lens; parm++)
                    {
                        if ((*parm != AT_CMD_COMMA_MARK) && ((argv_cnt == 0) || (*(parm - 1) == AT_CMD_COMMA_MARK)))
                        {
                            argv[argv_cnt++] = parm;
                            if(*(parm - 1) == 0x2C)
                            {
                                *(parm - 1) = '\0';
                            }
                        }
                    }                
                    if(memcmp(argv[0],"NEW",strlen(argv[0])) == 0)
                    {
                        ret = otDatasetCreateNewNetwork(instance, &at_dataset);
                    }
                    else if(memcmp(argv[0],"CH",strlen(argv[0])) == 0)
                    {
                        at_dataset.mChannel = utility_strtoul(argv[1], 0);
                        at_dataset.mComponents.mIsChannelPresent = true;
                        ret = OT_ERROR_NONE;
                    }
                    else if(memcmp(argv[0],"ACTIVE",strlen(argv[0])) == 0)
                    {
                        ret = otDatasetSetActive(instance, &at_dataset);
                    }
                    else if(memcmp(argv[0],"NETKEY",strlen(argv[0])) == 0)
                    {
                        memset(at_dataset.mNetworkKey.m8,0x0,OT_NETWORK_KEY_SIZE);
                        for(int i=0; i< (strlen(argv[1])/2); i++)
                        {
                            at_dataset.mNetworkKey.m8[i]=(uint8_t)utility_strtox(&argv[1][i*2], 0, 2);
                        }
                        at_dataset.mComponents.mIsNetworkKeyPresent = true;
                        ret = OT_ERROR_NONE;
                    }
                    else if(memcmp(argv[0],"NETNAME",strlen(argv[0])) == 0)
                    {
                        otNetworkNameFromString(&at_dataset.mNetworkName, argv[1]);
                        at_dataset.mComponents.mIsNetworkNamePresent = true;
                        at_dataset.mComponents.mIsNetworkKeyPresent = true;
                        ret = OT_ERROR_NONE;
                    }
                    else if(memcmp(argv[0],"PANID",strlen(argv[0])) == 0)
                    {
                        at_dataset.mPanId = utility_strtox(argv[1], 0, 4);
                        at_dataset.mComponents.mIsPanIdPresent = true;
                        ret = OT_ERROR_NONE;
                    }
                }
            )
            break;
        case AT_CMD_EXECUTE_TYPE:
            break;
        default:
            break;
    }
    if(rsp_data) vPortFree(rsp_data);
    return ret;
}

static uint8_t __reset(uint8_t *args, uint16_t args_lens, uint8_t type)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;

    switch(type)
    {
        case AT_CMD_TEST_TYPE:
            break;
        case AT_CMD_QUERY_TYPE:
            break;
        case AT_CMD_SETTING_TYPE:
            break;
        case AT_CMD_EXECUTE_TYPE:
            OT_THREAD_SAFE(
                otInstance *instance = otrGetInstance();
                if(instance)
                {
                    otInstanceReset(instance);
                }
            )
            break;
        default:
            break;
    }
    return ret;
}

static const at_cmd_t at_cmd_list[] =
{
    {
        .command = "VERSION",
        .cmd_fn = __version,
    },
    {
        .command = "NETKEY",
        .cmd_fn = __set_networkkey,
    },
    {
        .command = "CH",
        .cmd_fn = __set_channel,
    },
    {
        .command = "PANID",
        .cmd_fn = __set_panid,
    },
    {
        .command = "NETNAME",
        .cmd_fn = __set_netname,
    },
    {
        .command = "START",
        .cmd_fn = __start,
    },
    {
        .command = "STOP",
        .cmd_fn = __stop,
    },
    {
        .command = "STATE",
        .cmd_fn = __state,
    },
    {
        .command = "UDPBIND",
        .cmd_fn = __udpbind,
    },
    {
        .command = "UDPSEND",
        .cmd_fn = __udpsend,
    },
    {
        .command = "EXTADDR",
        .cmd_fn = __extaddr,
    },
    {
        .command = "HEXMODE",
        .cmd_fn = __hexmode,
    },
    {
        .command = "OTABIND",
        .cmd_fn = __otabind,
    },
    {
        .command = "OTASEND",
        .cmd_fn = __otasend,
    },
    {
        .command = "DATASET",
        .cmd_fn = __dataset,
    },
    {
        .command = "RESET",
        .cmd_fn = __reset,
    },
};

static uint8_t at_cmd_process(at_cmd_t *cmd, uint8_t *cmd_args, uint16_t cmd_lens)
{
    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    uint8_t cmd_type = AT_CMD_UNKNOW_TYPE;
    do
    {
        if(!cmd) break;
        if(!cmd_args) break;

        if (cmd_args[0] == AT_CMD_EQUAL_MARK && cmd_args[1] == AT_CMD_QUESTION_MARK && cmd_args[2] == AT_CMD_CR)
        {
            cmd_type = AT_CMD_TEST_TYPE;
        }
        else if (cmd_args[0] == AT_CMD_QUESTION_MARK && cmd_args[1] == AT_CMD_CR)
        {
            cmd_type = AT_CMD_QUERY_TYPE;
        }
        else if (cmd_args[0] == AT_CMD_EQUAL_MARK
                || (cmd_args[0] >= AT_CMD_CHAR_0 && cmd_args[0] <= AT_CMD_CHAR_9 && cmd_args[1] == AT_CMD_CR))
        {
            cmd_type = AT_CMD_SETTING_TYPE;
        }
        else if (cmd_args[0] == AT_CMD_CR)
        {
            cmd_type = AT_CMD_EXECUTE_TYPE;
        }
        else
        {
            break;
        }
        
        if(cmd_args[cmd_lens-1] == 0x0A || cmd_args[cmd_lens-1] == 0x0D)
        {
            cmd_args[cmd_lens-1] = '\0';
        }
        if(cmd_type == AT_CMD_SETTING_TYPE)
        {
            ret = cmd->cmd_fn((cmd_args+1), (cmd_lens-1), cmd_type);
        }
        else
        {
            ret = cmd->cmd_fn(cmd_args, cmd_lens, cmd_type);
        }
    } while (0);    

    return ret;
}

static at_cmd_t *at_find_cmd(const char *cmd)
{
    int i = 0;
    int cmd_num = sizeof(at_cmd_list)/sizeof(at_cmd_t);
    at_cmd_t *find_cmd =NULL;

    do
    {
        if(!cmd) break;
        if(!at_cmd_list) break;

        for (i = 0; i < cmd_num; i++)
        {
            if (!strcasecmp(cmd, at_cmd_list[i].command))
            {
                find_cmd = (at_cmd_t*)&at_cmd_list[i];
                break;
            }
        }
    }while (0);    
    
    return find_cmd;
}

static uint8_t at_cmd_get_name(const char *cmd_buffer, uint16_t cmd_buffer_lens, char *cmd_name)
{
    int cmd_name_len = 0, i = 0;
    uint8_t ret = OT_ERROR_INVALID_COMMAND;

    do
    {
        if(!cmd_buffer) break;
        if(!cmd_name) break;

        for (i = 0; i < cmd_buffer_lens; i++)
        {
            if (*(cmd_buffer + i) == AT_CMD_QUESTION_MARK || *(cmd_buffer + i) == AT_CMD_EQUAL_MARK
                || *(cmd_buffer + i) == AT_CMD_CR
                || (*(cmd_buffer + i) >= AT_CMD_CHAR_0 && *(cmd_buffer + i) <= AT_CMD_CHAR_9))
            {
                cmd_name_len = i;
                memcpy(cmd_name, cmd_buffer, cmd_name_len);
                *(cmd_name + cmd_name_len) = '\0';
                
                ret = OT_ERROR_NONE;
                break;
            }   
        }
    } while (0);

    return ret;
}


void at_cmd_proc(uint8_t *pdata, uint16_t len)
{

    uint8_t ret = OT_ERROR_INVALID_COMMAND;
    const uint8_t rsp_ok[] = "OK\r\n";
    const uint8_t rsp_error[] = "ERROR\r\n";
    uint8_t cme_error[16];

    uint8_t *rsp_data = NULL;
    uint16_t rsp_len = 0;
    
    log_info("AT command in");
    log_info_hexdump("", pdata, len);  

    uint8_t *buffer = pdata+3; //AT+
    uint16_t buffer_lens = len-3; //AT+
    char cur_cmd_name[AT_CMD_NAME_LEN] = { 0 };
    at_cmd_t *cur_cmd = NULL;
    uint8_t *cur_cmd_args = NULL;
    uint16_t cur_cmd_arg_len = 0;

    do
    {
        ret = at_cmd_get_name(buffer, buffer_lens, cur_cmd_name);
        if(ret != OT_ERROR_NONE)
        {
            break;
        }

        cur_cmd = at_find_cmd(cur_cmd_name);
        if (!cur_cmd)
        {
            break;
        }

        cur_cmd_args = buffer + strlen(cur_cmd_name);
        cur_cmd_arg_len = buffer_lens - strlen(cur_cmd_name);

        ret = at_cmd_process(cur_cmd, cur_cmd_args, cur_cmd_arg_len);
        if(ret != OT_ERROR_NONE)
        {
            break;
        }

    } while (0);
    
    if(ret == OT_ERROR_NONE)
    {
        rsp_data = (uint8_t *)rsp_ok;
        rsp_len = sizeof(rsp_ok);
    }
    else
    {
        rsp_data = (uint8_t *)rsp_error;
        rsp_len = sizeof(rsp_error);
    }

    cp_sub_cmd_send(rsp_data,rsp_len);

    if(ret != OT_ERROR_NONE)
    {
        snprintf(cme_error, 17, "+CME ERROR:%03u\r\n", ret);
        cp_sub_cmd_send(cme_error,sizeof(cme_error));
    }
}