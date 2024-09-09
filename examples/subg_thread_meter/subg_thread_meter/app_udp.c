/**
 * @file app_udp.c
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief
 * @version 0.1
 * @date 2023-10-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <openthread/udp.h>
#include <openthread/thread.h>

#include "FreeRTOS.h"

#include <main.h>
#include "log.h"
#include "cli.h"
#include "util_string.h"

static otUdpSocket          appSock;
static void (*app_udpHandler) (otMessage *, const otMessageInfo *);
static uint16_t appUdpPort = THREAD_UDP_PORT;

static QueueHandle_t app_udp_handle;

typedef struct
{
    uint16_t dlen;
    uint8_t *pdata;
} _app_udp_data_t ;

static void otUdpReceive_handler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    //log_info("\nUPD Packet received, port: %d, len: %d", aMessageInfo->mPeerPort, otMessageGetLength(aMessage));

    if (app_udpHandler)
    {
        app_udpHandler(aMessage, aMessageInfo);
    }
}

otError app_udpSend(uint16_t PeerPort, otIp6Address PeerAddr, uint8_t *data,
                    uint16_t data_lens)
{
    otError error = OT_ERROR_NONE;
    otMessage *message = NULL;
    otMessageInfo messageInfo;
    otMessageSettings messageSettings = {true, OT_MESSAGE_PRIORITY_NORMAL};

    do
    {
        memset(&messageInfo, 0, sizeof(messageInfo));

        messageInfo.mPeerPort = PeerPort;
        messageInfo.mPeerAddr = PeerAddr;

        otInstance *instance = otrGetInstance();
        if (instance)
        {
            message = otUdpNewMessage(instance, &messageSettings);
            if (message == NULL)
            {
                error = OT_ERROR_NO_BUFS;
                break;
            }
            error = otMessageAppend(message, data, data_lens);
            if (error != OT_ERROR_NONE)
            {
                break;
            }

            error = otUdpSend(instance, &appSock, message, &messageInfo);
            if (error != OT_ERROR_NONE)
            {
                break;
            }
        }
        message = NULL;

    } while (0);

    if (message != NULL)
    {
        otMessageFree(message);
    }
    return error;
}

void app_udp_received_queue_push(uint8_t *data, uint16_t data_lens)
{
    _app_udp_data_t u_data;

    u_data.pdata =  pvPortMalloc(data_lens);
    if (u_data.pdata)
    {
        memcpy(u_data.pdata, data, data_lens);
        u_data.dlen = data_lens;
        if (xQueueSend(app_udp_handle, (void *)&u_data, 0) != pdPASS)
        {
            vPortFree(u_data.pdata);
        }
        else
        {
            APP_EVENT_NOTIFY(EVENT_UDP_RECEIVED);
        }
    }
}

void __udp_task(app_task_event_t sevent)
{
    if ((EVENT_UDP_RECEIVED & sevent))
    {
        _app_udp_data_t u_data;
        if (xQueueReceive(app_udp_handle, (void *)&u_data, 0) == pdPASS)
        {
            log_info_hexdump("Received Message ", u_data.pdata, u_data.dlen);

            if (u_data.pdata)
            {
                vPortFree(u_data.pdata);
            }
        }
    }
}

uint8_t app_sockInit(otInstance *instance, void (*handler)(otMessage *, const otMessageInfo *), uint16_t udp_port)
{
    otSockAddr sockAddr;
    BaseType_t xReturned;

    uint8_t ret;

    memset(&appSock, 0, sizeof(otUdpSocket));
    memset(&sockAddr, 0, sizeof(otSockAddr));

    ret = otUdpOpen(instance, &appSock, otUdpReceive_handler, instance);

    if (OT_ERROR_NONE == ret)
    {
        app_udpHandler = handler;
        sockAddr.mPort = udp_port;
        ret = otUdpBind(instance, &appSock, &sockAddr, OT_NETIF_THREAD);
        if (OT_ERROR_NONE == ret)
        {
            log_info("UDP PORT           : 0x%x", udp_port);
        }
    }

    app_udp_handle = xQueueCreate(5, sizeof(_app_udp_data_t));

    return ret;
}

static int _cli_cmd_udpsend(int argc, char **argv, cb_shell_out_t log_out, void *pExtra)
{
    otIp6Address PeerAddr;
    uint16_t payload_len;
    uint8_t *payload = NULL;
    if (argc > 2)
    {
        otIp6AddressFromString(argv[1], &PeerAddr);
        payload_len = utility_strtol(argv[2], 0) ;
        log_info("udpsend len %u \r\n", payload_len);
        payload = pvPortMalloc(payload_len);
        if (payload)
        {
            memset(payload, 0xfe, payload_len);
            app_udpSend(THREAD_UDP_PORT, PeerAddr, payload, payload_len);
            vPortFree(payload);
        }
    }
    return 0;
}

const sh_cmd_t g_cli_cmd_udpsend STATIC_CLI_CMD_ATTRIBUTE =
{
    .pCmd_name    = "udpsend",
    .pDescription = "udp send <ip> <lens>",
    .cmd_exec     = _cli_cmd_udpsend,
};
