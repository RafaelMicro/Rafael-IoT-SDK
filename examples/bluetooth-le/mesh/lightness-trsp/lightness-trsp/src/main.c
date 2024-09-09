/** @file
 *
 * @brief FreeRTOSbySystem example file.
 *
 */


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/

#include "project_config.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "mesh_mdl_handler.h"
#include "mesh_app.h"
#include "ble_mesh_element.h"
#include "hosal_rf.h"
#include "log.h"


#include "mmdl_opcodes.h"
#include "mmdl_common.h"
#include "mmdl_defs.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"

#include "ble_mesh_lib_api.h"
#include "ble_host_cmd.h"
#include "mesh_task.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define PWM_PWM0_COUNTER_MODE   PWM_COUNTER_MODE_UP         /*UP is up counter Mode ;UD is up and down Mode*/
#define PWM_PWM0_SEQ_ORDER      PWM_SEQ_ORDER_R             /*Rseq or Tseq Selection or both*/
#define PWM_PWM0_ELEMENT_NUM    1                           /*genrator Pulse number*/
#define PWM_PWM0_REPEAT_NUM     0                           /*Repeat Pulse number*/
#define PWM_PWM0_DLY_NUM        0                           /*Pulse delay number*/
#define PWM_PWM0_CNT_END_VAL    10000                       /*Count end Value*/
#define PWM_PWM0_CLK_DIV        PWM_CLK_DIV_4               /*PWM Input Clock Div*/
#define PWM_PWM0_TRIG_SRC       PWM_TRIGGER_SRC_SELF        /*PWM Trigger Source by self or PWM1~PWM4*/
#define PWM_PWM0_SEQ_MODE       PWM_SEQ_MODE_CONTINUOUS     /*Continuous and Noncontinuous mode*/
#define PWM_PWM0_PLAY_CNT       0                           /*0:is infinite*/
#define PWM_PWM0_SEQ_NUM        PWM_SEQ_NUM_1               /*use rdma single simple or two simple*/
#define PWM_PWM0_DMA_SML_FMT    PWM_DMA_SMP_FMT_1           /*Pwm DMA Simple Format 0 or 1*/

#define MAX_TRSP_DATA_LEN           (377)

#define INVALID_DST_ADDRESS         (0x0000)

// Advertising device name
#define DEVICE_NAME                 'B', 'L', 'E', '_', 'M', 'E', 'S', 'H'
/**************************************************************************************************
 *    TYPEDEFS
 *************************************************************************************************/
typedef struct
{
    const char          *p_cmd_example;
    const char          *p_cmd_header;
    void                (*p_cmd_process)(uint8_t *p_data);
} user_cmd_t;

typedef enum
{
    APP_GATT_PROVISION_EVT,
    APP_MESH_PROVISION_EVT,
    APP_PROVISIONING_EVT,
    APP_PROVISION_DONE_EVT,
    APP_IDLE_EVT,
} app_main_evt_t;
/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
pwm_seq_para_head_t pwm_para_config[3];

uint32_t pwm_rdma0_addr_temp[3];
uint32_t pwm_rdma1_addr_temp[3];
uint16_t pwm_count_end_val[3] = {PWM_PWM0_CNT_END_VAL, PWM_PWM0_CNT_END_VAL, PWM_PWM0_CNT_END_VAL};
pwm_clk_div_t pwm_clk_div[3] = {PWM_PWM0_CLK_DIV, PWM_PWM0_CLK_DIV, PWM_PWM0_CLK_DIV};
pwm_seq_order_t pwm_seq_order[3] = {PWM_PWM0_SEQ_ORDER, PWM_PWM0_SEQ_ORDER, PWM_PWM0_SEQ_ORDER};
pwm_trigger_src_t pwm_trigger_src[3] = {PWM_PWM0_TRIG_SRC, PWM_PWM0_TRIG_SRC, PWM_PWM0_TRIG_SRC};
uint16_t pwm_play_cnt[3] = {PWM_PWM0_PLAY_CNT, PWM_PWM0_PLAY_CNT, PWM_PWM0_PLAY_CNT};
pwm_seq_num_t pwm_seq_num[3] = {PWM_PWM0_SEQ_NUM, PWM_PWM0_SEQ_NUM, PWM_PWM0_SEQ_NUM};
pwm_seq_mode_t pwm_seq_mode[3] = {PWM_PWM0_SEQ_MODE, PWM_PWM0_SEQ_MODE, PWM_PWM0_SEQ_MODE};
pwm_counter_mode_t pwm_counter_mode[3] = {PWM_PWM0_COUNTER_MODE, PWM_PWM0_COUNTER_MODE, PWM_PWM0_COUNTER_MODE};
pwm_dma_smp_fmt_t pwm_dma_smp_fmt[3] = {PWM_PWM0_DMA_SML_FMT, PWM_PWM0_DMA_SML_FMT, PWM_PWM0_DMA_SML_FMT};
uint8_t pwm_element_arr[3] = {PWM_PWM0_ELEMENT_NUM, PWM_PWM0_ELEMENT_NUM, PWM_PWM0_ELEMENT_NUM};
uint8_t pwm_rep_arr[3] = {PWM_PWM0_REPEAT_NUM, PWM_PWM0_REPEAT_NUM, PWM_PWM0_REPEAT_NUM};
uint8_t pwm_dly_arr[3] = {PWM_PWM0_DLY_NUM, PWM_PWM0_DLY_NUM, PWM_PWM0_DLY_NUM};

extern ble_mesh_element_param_t g_element_info[];


/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
extern int retarget_stdout_string(char *str, int length, UART_T *p_csr);
extern int retarget_stdout_char(int ch, UART_T *p_csr);
extern void app_init(void);


/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
static uint8_t app_mesh_event_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv);
static uint8_t app_button_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv);
static uint8_t app_uart_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv);
static void user_data_dst_address_set(uint8_t *p_rx_data);
static void user_data_tx_ack_set(uint8_t *p_rx_data);
/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/

static mesh_app_cfg_t gt_app_cfg;
uint8_t (* const app_event_handler[])(uint32_t data, mesh_tlv_t *p_mesh_tlv) =
{
    app_mesh_event_handler,
    app_button_handler,
    app_uart_handler,
};

static user_cmd_t g_user_cmd_list[] =
{
    {
        .p_cmd_example = "DstAddr0x001c",
        .p_cmd_header =  "DstAddr0x",
        .p_cmd_process = user_data_dst_address_set,
    },
    {
        .p_cmd_example = "TxAckEnable0",
        .p_cmd_header =  "TxAckEnable",
        .p_cmd_process = user_data_tx_ack_set,
    },
};


static uint32_t app_main_event = APP_GATT_PROVISION_EVT;




static uint16_t user_data_dst_addr = INVALID_DST_ADDRESS;
static uint16_t user_data_appkey_idx = 0xFFFF;
static uint8_t user_data_req_ack = 0;

static uint8_t user_cmd_num = (sizeof(g_user_cmd_list) / sizeof(user_cmd_t));


static const uint8_t         DEVICE_NAME_STR[] = {DEVICE_NAME};

static TimerHandle_t unprov_idc_tmr;
xQueueHandle app_msg_q;
//static SemaphoreHandle_t semaphore_cb;

/*this is pin mux setting*/
static void init_default_pin_mux(void)
{
    int i;

    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++)
    {
#if 0
        if (i == 20)
        {
            pin_set_mode(i, MODE_PWM0);
        }
        else if (i == 21)
        {
            pin_set_mode(i, MODE_PWM1);
        }
        else if (i == 22)
        {
            pin_set_mode(i, MODE_PWM2);
        }
        else 
#endif
        if ((i != 16) && (i != 17))
        {
            pin_set_mode(i, MODE_GPIO);
        }
    }
    gpio_cfg_output(23);
    gpio_pin_set(23);


    return;
}

void pwm_init_parameter(pwm_id_t id, pwm_seq_para_head_t *pwm_para_config)
{
    pwm_seq_para_t *pwm_seq = NULL;

    pwm_para_config->pwm_id            = id;
    pwm_para_config->pwm_play_cnt      = pwm_play_cnt[id]; //0 means continuous
    pwm_para_config->pwm_seq_order     = pwm_seq_order[id];
    pwm_para_config->pwm_seq_num       = pwm_seq_num[id];
    pwm_para_config->pwm_seq_mode      = pwm_seq_mode[id];
    pwm_para_config->pwm_triggered_src = pwm_trigger_src[id];
    pwm_para_config->pwm_clk_div       = pwm_clk_div[id];
    pwm_para_config->pwm_counter_mode  = pwm_counter_mode[id];
    pwm_para_config->pwm_dma_smp_fmt   = pwm_dma_smp_fmt[id];

    if (pwm_para_config->pwm_seq_num == PWM_SEQ_NUM_2)
    {
        pwm_seq = &(pwm_para_config->pwm_seq0);
        pwm_seq->element_num    = pwm_element_arr[id];
        pwm_seq->repeat_num     = pwm_rep_arr[id];
        pwm_seq->delay_num      = pwm_dly_arr[id];
        pwm_seq->rdma_addr      = (uint32_t)&pwm_rdma0_addr_temp[id];

        pwm_seq = &(pwm_para_config->pwm_seq1);
        pwm_seq->element_num    = pwm_element_arr[id];
        pwm_seq->repeat_num     = pwm_rep_arr[id];
        pwm_seq->delay_num      = pwm_dly_arr[id];
        pwm_seq->rdma_addr      = (uint32_t)&pwm_rdma1_addr_temp[id];
    }
    else if (pwm_para_config->pwm_seq_num == PWM_SEQ_NUM_1)
    {
        if (pwm_para_config->pwm_seq_order == PWM_SEQ_ORDER_R)
        {
            pwm_seq = &(pwm_para_config->pwm_seq0);
            pwm_seq->rdma_addr  = (uint32_t)&pwm_rdma0_addr_temp[id];
        }
        else if (pwm_para_config->pwm_seq_order == PWM_SEQ_ORDER_T)
        {
            pwm_seq = &(pwm_para_config->pwm_seq1);
            pwm_seq->rdma_addr  = (uint32_t)&pwm_rdma1_addr_temp[id];
        }

        pwm_seq->element_num    = pwm_element_arr[id];
        pwm_seq->repeat_num     = pwm_rep_arr[id];
        pwm_seq->delay_num      = pwm_dly_arr[id];
    }

    if (pwm_para_config->pwm_dma_smp_fmt == PWM_DMA_SMP_FMT_0)
    {
        pwm_para_config->pwm_count_end_val = pwm_count_end_val[id];
    }
    else
    {
        pwm_para_config->pwm_count_end_val = 0x00;
    }

}

void set_duty_cycle(pwm_seq_para_head_t *pwm_para_config, uint16_t current_lv)
{
    uint32_t dutycycle ;
    dutycycle = (4000 * current_lv) / 65535;

    pwm_seq_para_t *pwm_seq;
    uint32_t *rdma_addr;

    pwm_seq = &pwm_para_config->pwm_seq0;
    rdma_addr = (uint32_t *)pwm_seq->rdma_addr;
    *(rdma_addr)  = PWM_FILL_SAMPLE_DATA_MODE1(0, 4000 - dutycycle, 4000);

    Pwm_Start(pwm_para_config);
}

static uint8_t parse_hex_digit(char c)
{
    uint8_t result = 0xff;

    if ((c >= '0') && (c <= '9'))
    {
        result = c - '0';
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        result = c - 'a' + 10;
    }
    else if ((c >= 'A') && (c <= 'F'))
    {
        result = c - 'A' + 10;
    }

    return result;
}

static void unprov_tmr_cb(TimerHandle_t t_timer)
{
    //extern void set_duty_cycle(pwm_seq_para_head_t *pwm_para_config, uint16_t current_lv);
    extern pwm_seq_para_head_t pwm_para_config[3];
    extern light_lightness_state_t  el0_light_lightness_state;
    static uint8_t toggle;

    if (pib_is_provisioned())
    {
        //set_duty_cycle(&pwm_para_config[0], el0_light_lightness_state.lightness_actual);
        //set_duty_cycle(&pwm_para_config[1], el0_light_lightness_state.lightness_actual);
        //set_duty_cycle(&pwm_para_config[2], el0_light_lightness_state.lightness_actual);
        hosal_gpio_pin_write(20, 0);
        xTimerStop(unprov_idc_tmr, 0);
    }
    else
    {
        hosal_gpio_pin_toggle(20);
        hosal_gpio_pin_toggle(21);
        #if 0
        toggle ^= 1;
        if (toggle)
        {
            //set_duty_cycle(&pwm_para_config[0], 0xFFFF);
            //set_duty_cycle(&pwm_para_config[1], 0xFFFF);
            //set_duty_cycle(&pwm_para_config[2], 0xFFFF);
            hosal_gpio_pin_write(20, 0);
        }
        else
        {
            //set_duty_cycle(&pwm_para_config[0], 0);
            //set_duty_cycle(&pwm_para_config[1], 0);
            //set_duty_cycle(&pwm_para_config[2], 0);
            hosal_gpio_pin_write(20, 1);
        }
        #endif
    }
}



static void user_data_dst_address_set(uint8_t *p_rx_data)
{
    uint8_t dst_addr[4];
    dst_addr[0] = parse_hex_digit((char)p_rx_data[0]);
    dst_addr[1] = parse_hex_digit((char)p_rx_data[1]);
    dst_addr[2] = parse_hex_digit((char)p_rx_data[2]);
    dst_addr[3] = parse_hex_digit((char)p_rx_data[3]);

    user_data_dst_addr = (((dst_addr[0] & 0x0F) << 12) |
                          ((dst_addr[1] & 0x0F) << 8) |
                          ((dst_addr[2] & 0x0F) << 4) | (dst_addr[3] & 0x0F));

    printf("destination address for uart data: 0x%04x\n", user_data_dst_addr);
}

static void user_data_tx_ack_set(uint8_t *p_rx_data)
{
    user_data_req_ack = parse_hex_digit((char)p_rx_data[0]);

    printf("uart data tx with ack %d\n", user_data_req_ack);
}

static void uart_data_handler(char ch)
{
    static uint8_t rx_buffer[MAX_TRSP_DATA_LEN + 1];
    static uint16_t index = 0;
    uint16_t i, cmd_processed = false;
    mmdl_transmit_info_t tx_info;
    raf_trsp_set_msg_t *p_raf_trsp_set_msg;
    int status;

    rx_buffer[index++] = ch;

    if (index > (MAX_TRSP_DATA_LEN + 1)) /* (+1) length '\r' or '\n' */
    {
        printf("uart data out of memory %d\n", index - 1);
        index = 0;
    }
    else if ((ch == '\n') || (ch == '\r'))
    {
        for (i = 0 ; i < user_cmd_num; i++)
        {
            if ((index == strlen(g_user_cmd_list[i].p_cmd_example) + 1/* (+1) length '\r' or '\n' */) &&
                    (strncmp((char *)rx_buffer, g_user_cmd_list[i].p_cmd_header, strlen(g_user_cmd_list[i].p_cmd_header)) == 0))
            {
                g_user_cmd_list[i].p_cmd_process(rx_buffer + strlen(g_user_cmd_list[i].p_cmd_header));
                cmd_processed = true;
            }
        }

        if (cmd_processed == false)
        {
            if (user_data_dst_addr != INVALID_DST_ADDRESS)
            {
                if (user_data_appkey_idx == 0xFFFF)
                {
                    printf("no APP key bind to Rafael TRSP client model\n");
                }
                else if (pib_is_provisioned())
                {
                    printf("Send Rafael TRSP set, dst addr 0x%04x\n", user_data_dst_addr);

                    for (i = 0; i < (index - 1); i++)
                    {
                        printf("%02x ", rx_buffer[i]);
                    }
                    printf("\n... " );


                    p_raf_trsp_set_msg = pvPortMalloc(sizeof(raf_trsp_set_msg_t) + (index - 1/*(-1) removed '\n' or '\r'*/));
                    if (p_raf_trsp_set_msg == NULL)
                    {
                        printf("fail (no memory)\n");
                    }
                    else
                    {
                        tx_info.dst_addr = user_data_dst_addr;
                        tx_info.src_addr = pib_primary_address_get();
                        tx_info.appkey_index = user_data_appkey_idx;
                        p_raf_trsp_set_msg->data_len = (index - 1/*(-1) removed '\n' or '\r'*/);
                        memcpy(p_raf_trsp_set_msg->data, rx_buffer, p_raf_trsp_set_msg->data_len);

                        if (user_data_req_ack)
                        {
                            status = mmdl_rafael_trsp_send_set(tx_info, p_raf_trsp_set_msg);
                        }
                        else
                        {
                            status = mmdl_rafael_trsp_send_unack_set(tx_info, p_raf_trsp_set_msg);

                        }
                        printf("result %d\n", status);
                        vPortFree(p_raf_trsp_set_msg);
                    }
                }
                else
                {
                    printf("device was not provision\n");
                }

            }
            else
            {
                printf("destination address invalid, uart data ignore\n");
                printf("command example:\n");
                for (i = 0 ; i < user_cmd_num; i++)
                {
                    printf("%s\n", g_user_cmd_list[i].p_cmd_example);
                }
            }
        }
        index = 0;

    }
}

static uint8_t app_button_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
    uint8_t memory_free = false;

    switch (data)
    {
    case 0: //Mesh provsion start
    case 1: //Gatt provsion start
        if (app_main_event == APP_PROVISIONING_EVT)
        {
            ble_mesh_provision_stop();
        }

        if (pib_is_provisioned())
        {
            ble_mesh_proxy_disable();
        }
        element_info_init();
        pib_provision_info_local_reset();
        app_main_event = (data == 0) ? APP_MESH_PROVISION_EVT : APP_GATT_PROVISION_EVT;
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        break;
    }

    return memory_free;
}

static uint8_t app_uart_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
    uint8_t memory_free = false;
    char ch;
#if 0
    while (bsp_console_stdin_str(&ch, 1) > 0)
    {
        uart_data_handler(ch);
    }
    return memory_free;
#endif
}

static void ble_trsps_evt_msg_handler(uint16_t len, uint8_t *p_trsps_data)
{
    uint16_t i;
    bool tx_success;

    printf("Receive BLE TRSPS data:\n");
    for (i = 0; i < len ; i++)
    {
        printf("%02x ", p_trsps_data[i]);
    }
    printf("\n");

    tx_success = ble_trsps_data_set(len, p_trsps_data);

    printf("Loopback received BLE TRSPS data %d:\n", tx_success);
}

static uint8_t app_mesh_event_handler(uint32_t data, mesh_tlv_t *p_mesh_tlv)
{
    mesh_prov_start_cfm_t *p_start_cfm;
    uint8_t memory_free = true, i;

    switch (p_mesh_tlv->type)
    {
    case TYPE_MESH_PROV_START_CFM:
    {
        p_start_cfm = (mesh_prov_start_cfm_t *)p_mesh_tlv->value;
        if (p_start_cfm->status != 0)
        {
            printf("Provision timeout!\n");
            app_main_event = APP_GATT_PROVISION_EVT;
        }
        else
        {
            printf("Provision done! address 0x%04x\n", p_start_cfm->element_address);
            for (i = 0 ; i < ble_mesh_element_count ; i++)
            {
                g_element_info[i].element_address = p_start_cfm->element_address + i;
            }
            app_main_event = APP_PROVISION_DONE_EVT;
        }
    }
    break;

    case TYPE_MESH_NODE_RESET_IDC:
    {
        printf("Recv CfgNodeRest ... \n");
        element_info_init();
        app_main_event = APP_GATT_PROVISION_EVT;
    }
    break;

    case TYPE_MESH_BLE_SVC_TRSPS_WRITE_IDC:
    {
        ble_trsps_evt_msg_handler(p_mesh_tlv->length, p_mesh_tlv->value);
    }
    break;

    case TYPE_MESH_APP_MDL_EVT_MSG_IDC:
    {
        mmdl_evt_msg_cb((mesh_app_mdl_evt_msg_idc_t *)p_mesh_tlv->value);
    }
    break;

    case TYPE_MESH_CFG_MDL_APP_BIND_IDC:
    {
        mesh_cfg_mdl_app_bind_idc_t *p_app_bind_idc;
        p_app_bind_idc = (mesh_cfg_mdl_app_bind_idc_t *)p_mesh_tlv->value;

        if (p_app_bind_idc->model_id == MMDL_RAFAEL_TRSP_SR_MDL_ID)
        {
            printf("Appkey index %d bind to Rafael TRSP server model\n", p_app_bind_idc->appkey_index);
        }
        else if (p_app_bind_idc->model_id == MMDL_RAFAEL_TRSP_CL_MDL_ID)
        {
            printf("Appkey index %d bind to Rafael TRSP client model\n", p_app_bind_idc->appkey_index);
            user_data_appkey_idx = p_app_bind_idc->appkey_index;
        }
    }
    break;

    case TYPE_MESH_PROVISION_COMPLETE_IDC:
    {
        mesh_prov_complete_idc_t *p_prov_device;

        p_prov_device = (mesh_prov_complete_idc_t *)p_mesh_tlv->value;
        if (p_prov_device->status == 1)
        {
            printf("Device provision fail  ... \n");
        }
    }
    break;

    case TYPE_MESH_PROV_ADV_ENABLED_IDC:
    {
        mesh_prov_adv_enable_idc_t *p_adv_enable_status;
        p_adv_enable_status = (mesh_prov_adv_enable_idc_t *)p_mesh_tlv->value;
        printf("Provision ADV enable result: %d\n", p_adv_enable_status->status);
    }
    break;

    case TYPE_MESH_PROXY_ENABLE_IDC:
    {
        mesh_proxy_adv_enable_idc_t *p_adv_enable_status;
        p_adv_enable_status = (mesh_proxy_adv_enable_idc_t *)p_mesh_tlv->value;
        printf("Proxy ADV enable result: %d\n", p_adv_enable_status->status);
    }
    break;

    default:
        break;
    }
    return memory_free;
}

static void app_main_loop(void)
{
    switch (app_main_event)
    {
    case APP_MESH_PROVISION_EVT:
        if (pib_is_provisioned() == 0)
        {
            ble_mesh_provision_start(0);
            printf("Start mesh provision!\n");
            app_main_event = APP_PROVISIONING_EVT;
            xTimerStart(unprov_idc_tmr, 0);
        }
        else
        {
            app_main_event = APP_IDLE_EVT;
        }
        break;

    case APP_GATT_PROVISION_EVT:
        if (pib_is_provisioned() == 0)
        {
            ble_mesh_provision_start(1);
            printf("Start gatt provision!\n");
            app_main_event = APP_PROVISIONING_EVT;
            xTimerStart(unprov_idc_tmr, 0);
        }
        else
        {
            app_main_event = APP_IDLE_EVT;
        }
        break;

    case APP_PROVISIONING_EVT:
        break;

    case APP_PROVISION_DONE_EVT:
        printf("Provision Complete!!\n");

        ble_mesh_proxy_enable();
        app_main_event = APP_IDLE_EVT;
        break;

    case APP_IDLE_EVT:
        break;

    default:
        break;
    }
}

static void app_main_task(void)
{
    ble_mesh_model_param_t  *p_model;
    app_queue_t app_q;
    uint8_t i, j;

    ble_mesh_device_name_set(sizeof(DEVICE_NAME_STR), (uint8_t *)DEVICE_NAME_STR);
    if (pib_is_provisioned())
    {
        for (i = 0 ; i < ble_mesh_element_count ; i++)
        {
            g_element_info[i].element_address = pib_primary_address_get() + i;

            //search the appkey index which bind to model "MMDL_RAFAEL_TRSP_CL_MDL_ID"
            if ((user_data_appkey_idx == 0xFFFF) &&
                    (search_model(g_element_info + i, MMDL_RAFAEL_TRSP_CL_MDL_ID, &p_model)))
            {
                for (j = 0 ; j < RAF_BLE_MESH_MODEL_BIND_LIST_SIZE ; j++)
                {
                    if (p_model->binding_list[j] != 0xFFFF)
                    {
                        user_data_appkey_idx = p_model->binding_list[j];
                        break;
                    }
                }
            }
        }

        ble_mesh_proxy_enable();
        printf("Device provisioned ... primary address 0x%04x\n", pib_primary_address_get());
    }

    for (;;)
    {
        app_main_loop();

        if (xQueueReceive(app_msg_q, &app_q, 0) == pdTRUE)
        {
            if (app_event_handler[app_q.event](app_q.data, app_q.pt_tlv) == true)
            {
                vPortFree(app_q.pt_tlv);
            }
        }
    }
}


static ble_err_t mesh_app_evt_indication_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t app_q;

    status = BLE_ERR_OK;
    
    do
    {
        if (p_param == NULL)
        {
            printf("[%s] null point get!\n", __func__);
            status = BLE_ERR_INVALID_PARAMETER;
            break;
        }
        app_q.pt_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + ((mesh_tlv_t*)p_param)->length);
        if (app_q.pt_tlv == NULL)
        {
            status = BLE_ERR_ALLOC_MEMORY_FAIL;
            break;
        }
        app_q.event = APP_BLE_MESH_EVT;
        app_q.pt_tlv->length =  ((mesh_tlv_t*)p_param)->length;
        app_q.pt_tlv->type =  ((mesh_tlv_t*)p_param)->type;
        memcpy(app_q.pt_tlv->value,  ((mesh_tlv_t*)p_param)->value,  ((mesh_tlv_t*)p_param)->length);
        while (xQueueSendToBack(app_msg_q, &app_q, 20) != pdPASS)
        {
            printf("[%s] send fail\n", __func__);
            status = BLE_BUSY;
        };

        vPortFree(p_param);

    } while (0);

    return status;
}

static ble_err_t ble_app_event_cb(void *p_param)
{
    ble_err_t status;
    mesh_tlv_t *p_tlv;
    status = BLE_ERR_OK;

    
    do {
        p_tlv = pvPortMalloc(sizeof(mesh_tlv_t) + sizeof(ble_evt_param_t));
        if (p_tlv == NULL)
        {
            status = BLE_ERR_DATA_MALLOC_FAIL;
            break;
        }

        p_tlv->type = MSG_TAG_BEARER_HCI_EVENT;
        memcpy(p_tlv->value, p_param, sizeof(ble_evt_param_t));

        if (mesh_queue_sendto(MESH_LAYER_BEARER, p_tlv) != MESH_SUCCESS)
        {
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}

static ble_err_t ble_service_data_cb(void *p_param)
{
    ble_err_t status;
    ble_evt_att_param_t *p_evt_att;
    mesh_tlv_t *p_tlv;
    status = BLE_ERR_OK;
    do {
        {
            p_evt_att = p_param;
            p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_att_param_t) + p_evt_att->length);
            if (p_tlv == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                //xSemaphoreGive(semaphore_cb);
                break;
            }
            p_tlv->type = MSG_TAG_BEARER_HCI_ACL_DATA;
            memcpy(p_tlv->value, p_param, sizeof(ble_evt_att_param_t) + p_evt_att->length);

            if (mesh_queue_sendto(MESH_LAYER_BEARER, p_tlv) != MESH_SUCCESS)
            {
                status = BLE_BUSY;
                //xSemaphoreGive(semaphore_cb);
            }
        }

    } while (0);

    return status;
}

void app_init(void)
{
    mesh_task_priority_t task_priority_cfg;
    ble_task_priority_t ble_task_level;


    pib_init(NULL, g_element_info, ble_mesh_element_count, NULL);

    mmdl_init();

    app_msg_q = xQueueCreate(16, sizeof(app_queue_t));

    unprov_idc_tmr = xTimerCreate("100ms", pdMS_TO_TICKS(100), pdTRUE, (void *)0, unprov_tmr_cb);

    if (ble_host_callback_set(APP_GENERAL_EVENT, ble_app_event_cb) != BLE_ERR_OK)
    {
        printf("ble_host_callback_set(APP_GENERAL_EVENT) fail...\n");
    }

    if (ble_host_callback_set(APP_SERVICE_EVENT, ble_service_data_cb) != BLE_ERR_OK)
    {
        printf("ble_host_callback_set(APP_SERVICE_EVENT) fail...\n");
    }


    ble_task_level.hci_tx_level = configMAX_PRIORITIES - 6;
    ble_task_level.ble_host_level = configMAX_PRIORITIES - 7;    
    if (ble_host_stack_init(&ble_task_level) == 0) {
        printf("BLE stack initial success...\n");
    }
    else {
        printf("BLE stack initial fail...\n");
    }

    task_priority_cfg.bearer_level = configMAX_PRIORITIES - 8;
    task_priority_cfg.mesh_level = configMAX_PRIORITIES - 9;
    if (mesh_stack_init(&task_priority_cfg, mesh_app_evt_indication_cb) == MESH_TASK_ERR_OK)
    {
        printf("BLE Mesh stack initial success...\n");
    }
    else {
        printf("BLE Mesh stack initial fail...\n");
    }


    
}

#if 0
void app_bsp_isr_callback(bsp_event_t event)
{
    BaseType_t context_switch = pdFALSE;
    app_queue_t t_app_q;

    switch (event)
    {
    case BSP_EVENT_BUTTONS_0:
    case BSP_EVENT_BUTTONS_1:
    case BSP_EVENT_BUTTONS_2:
    case BSP_EVENT_BUTTONS_3:
    case BSP_EVENT_BUTTONS_4:
        t_app_q.event = APP_BUTTON_EVT;
        t_app_q.data = event;

        xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
        break;
    case BSP_EVENT_UART_RX_RECV:
    case BSP_EVENT_UART_RX_DONE:
    {
        t_app_q.event = APP_UART_EVT;

        xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
    }
    break;

    default:
        break;
    }

}
#endif 
void button_cb(uint32_t pin, void* isr_param) 
{
    BaseType_t context_switch = pdFALSE;
    app_queue_t t_app_q;

    printf("GPIO%d=%d\r\n", pin, hosal_gpio_pin_get(pin));
    switch (pin)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        t_app_q.event = APP_BUTTON_EVT;
        t_app_q.data = pin;

        xQueueSendToBackFromISR(app_msg_q, &t_app_q, &context_switch);
        break;

    default:
        break;
    }

    return;
}

void button_init(void)
{
    uint8_t i = 0;
    hosal_gpio_input_config_t pin_cfg;
    /* gpio0 pin setting */
    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_RISING;
    pin_cfg.usr_cb = button_cb;

    hosal_gpio_set_debounce_time(DEBOUNCE_SLOWCLOCKS_1024);
    NVIC_SetPriority(Gpio_IRQn, 7);
    for (i = 0; i < 5; i++)
    {
        hosal_gpio_debounce_enable(i);
        hosal_pin_set_pullopt(i, HOSAL_PULL_UP_100K);
        hosal_gpio_cfg_input(i, pin_cfg);    
        hosal_gpio_int_enable(i);
    }

}
int32_t main(void)
{

    init_default_pin_mux();
    Delay_Init();
    hosal_rf_init(HOSAL_RF_MODE_BLE_CONTROLLER);
    #if 0
    pwm_init_parameter((pwm_id_t)0, &pwm_para_config[0]);
    Pwm_Init(&pwm_para_config[0]);
    pwm_init_parameter((pwm_id_t)1, &pwm_para_config[1]);
    Pwm_Init(&pwm_para_config[1]);
    pwm_init_parameter((pwm_id_t)2, &pwm_para_config[2]);
    Pwm_Init(&pwm_para_config[2]);
    set_duty_cycle(&pwm_para_config[0], 0xFFFF);
    set_duty_cycle(&pwm_para_config[1], 0xFFFF);
    set_duty_cycle(&pwm_para_config[2], 0xFFFF);
    #endif
    button_init();

    app_init();
    app_main_task();
    while (1)
    {
    }
}

