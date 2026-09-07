/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/** @file
 *
 * @brief BLE ESL peripheral role demo.
 *
 */

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "ble_profile.h"
#include "ble_app.h"
#include "ble_event.h"
#include "ble_l2cap.h"
#include "ble_profile.h"
#include "hosal_rf.h"
#include "ble_api.h"
#include "ble_host_cmd.h"
#include "hosal_uart.h"
#include "hosal_gpio.h"
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"
#include "rt_aes.h"
#include "flashctl.h"
#include "sysctrl.h"
#include "app_hooks.h"
#include "dump_boot_info.h"
#include "epd_driver.h"
#include "hosal_flash.h"
#include "rt_crypto_util.h"
#include "ble_fota.h"

/**************************************************************************************************
 *    MACROS
 *************************************************************************************************/


/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define BLE_APP_CB_QUEUE_SIZE           32
#define APP_ISR_QUEUE_SIZE              2
#define APP_REQ_QUEUE_SIZE              6
#define DISPLAY_QUEUE_SIZE              2
#define APP_QUEUE_SIZE                  (BLE_APP_CB_QUEUE_SIZE + APP_ISR_QUEUE_SIZE + APP_REQ_QUEUE_SIZE + DISPLAY_QUEUE_SIZE)

#define APP_ESL_P_HOST_ID              0

// MTU size
#define DEFAULT_MTU                     BLE_GATT_ATT_MTU_MAX

// Advertising device name
#define DEVICE_NAME                     "ESL_DEMO"

// Advertising parameters
#define APP_ADV_INTERVAL_MIN            160U      // 160*0.625ms=100ms
#define APP_ADV_INTERVAL_MAX            160U      // 160*0.625ms=100ms

#define U16_HIGHBYTE(x)                 (uint8_t)((x >> 8) & 0xFF) 
#define U16_LOWBYTE(x)                  (uint8_t)(x & 0xFF)
// GAP device name
static const uint8_t         DEVICE_NAME_STR[] = {DEVICE_NAME};

// Device BLE Address
static ble_gap_addr_t  DEVICE_ADDR = {.addr_type = RANDOM_STATIC_ADDR,
                                            .addr = {0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xC6 }
                                           };

#define UART0_OPERATION_PORT            0
HOSAL_UART_DEV_DECL(uart0_dev, UART0_OPERATION_PORT, CONFIG_UART_STDIO_TX_PIN, CONFIG_UART_STDIO_RX_PIN, UART_BAUDRATE_115200)

#define GPIO_WAKE_UP_PIN                0

#define ESL_DISPLAY_NUM                 2
#define ESL_IMAGE_NUM                   2
#define ESL_SENSOR_NUM                  8
#define ESL_LED_NUM                     1

#define ESL_LED_PIN                     21

#define ESL_ENADV_MIC_SIZE                0x04
#define ESL_ENADV_CCM_HEADER              0xEA

#define PADV_SYNC_TIMEOUT                 (1000) //unit: 10ms => 10s 

#define ESL_RSP_DATA_HEADER(rsp, adv, slot, rsp_len) rsp->sync_handle = adv->sync_handle;\
                                      rsp->req_event = adv->periodic_evt_cnt;\
                                      rsp->req_subevent = adv->subevent;\
                                      rsp->rsp_subevent = adv->subevent;\
                                      rsp->rsp_slot = slot;\
                                      ((esls_padv_enc_hdr_param_t*)(rsp->rsp_data))->length = 0x0A+rsp_len;\
                                      ((esls_padv_enc_hdr_param_t*)(rsp->rsp_data))->type = GAP_AD_TYPE_ENCRYPTED_ADVERTISING_DATA;\
                                      ((esls_padv_enc_hdr_param_t*)(rsp->rsp_data))->randomizer[0] = g_app_rand_value;\
                                      *((uint32_t*)(((esls_padv_enc_hdr_param_t*)(rsp->rsp_data))->randomizer+1)) = xTaskGetTickCount();\
                                      rsp->rsp_data_len = 7
                                

#define MESH_DEVICE_DEVICE_DATE_OF_MANUFACTURE                         0x000C
#define MESH_DEVICE_DEVICE_FIRMWAR_REVISION                            0x000E
#define MESH_DEVICE_DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION   0x0013
#define MESH_DEVICE_PRESENT_AMBIENT_TEMPERATURE                        0x004F
#define MESH_DEVICE_PRESENT_INPUT_VOLTAGE                              0x0059
#define MESH_DEVICE_PRESENT_DEVICE_OPERATING_TEMPERATURE               0x0054


//-------------------// flash start address 

//  image 0 Mono(4K) //
//  image 0 red (4K) //
//  image 1 Mono(4K) //
//  image 1 red (4K) //
//     Bond 2 (4K)   //
//     Bond 1 (4K)   //
//-------------------// flash end address 
#define EPD_IMAGE_FLASH_ADDRESS (uint8_t *)(FLASH_END_ADDR(( (flash_get_deviceinfo() >> FLASH_SIZE_ID_SHIFT) & 0xFF )) - (4096 * 6))  

#define SEND_ERR_RSP(p_rsp, rsp_len, p_err, err_code)   rsp_len = sizeof(esls_ctrl_point_rsp_err_param_t); \
                                                        p_err = (esls_ctrl_point_rsp_err_param_t *)(p_rsp); \
                                                        p_err->opcode = ESL_RSP_ERROR;  \
                                                        p_err->error_code = err_code
/**
 * @brief Per-display hardware information, as reported to the AP via the
 *        ESL_DISPLAY_INFORMATION characteristic.
 * @details One entry of esl_hw_info.display_info[].
 */
typedef struct __attribute__((packed)) ble_esl_display_info_s
{
    uint16_t width;         /**< Display width, in pixels. */
    uint16_t height;        /**< Display height, in pixels. */
    uint8_t  display_type;  /**< Vendor-specific display type identifier. */
} ble_esl_display_info_t;

/**
 * @brief Image storage information, as reported to the AP via the ESL_IMAGE_INFORMATION
 *        characteristic.
 */
typedef struct __attribute__((packed)) ble_esl_image_info_s
{
    uint8_t max_image_index;   /**< Highest valid image index that can be stored/displayed. */
} ble_esl_image_info_t;

/**
 * @brief Per-sensor descriptor entry, as reported to the AP via the ESL_SENSOR_INFORMATION
 *        characteristic.
 * @details One entry of esl_hw_info.sensor_info[]; size selects which member of sensor_type
 *          is valid.
 */
typedef struct __attribute__((packed)) ble_esl_sensor_info_s
{
    uint8_t  size;    /**< 0 selects the 2-byte standard sensor type ID (sensor_type.stanard);
                            any other value selects the 4-byte vendor-specific sensor type ID
                            (sensor_type.vendor_specific). */
    union
    {
        uint32_t vendor_specific;  /**< 4-byte vendor-specific sensor type ID, valid when size != 0. */
        uint16_t stanard;          /**< 2-byte standard (Bluetooth SIG) sensor type ID, valid when size == 0. */
    }sensor_type;   /**< Sensor type identifier; see size for which member is valid. */
} ble_esl_sensor_info_t;


/**
 * @brief Per-LED hardware capability entry, as reported to the AP via the
 *        ESL_LED_INFORMATION characteristic.
 * @details One entry of esl_hw_info.led_info[].
 */
typedef struct __attribute__((packed)) ble_svcs_esl_led_info_s
{
    uint8_t  red:2,       /**< Red color capability field (2 bits). */
             green:2,     /**< Green color capability field (2 bits). */
             blue:2,      /**< Blue color capability field (2 bits). */
             led_type:2;  /**< LED type field (2 bits). */
} ble_esl_led_info_t;


/**
 * @brief Aggregate ESL hardware capability/information table exposed to the AP.
 * @details Populated once at build time (see the esl_hw_info instance) and returned verbatim
 *          in response to ESL_DISPLAY_INFORMATION / ESL_IMAGE_INFORMATION /
 *          ESL_SENSOR_INFORMATION / ESL_LED_INFORMATION characteristic reads.
 */
typedef struct ble_svcs_esl_hw_info_s
{
    ble_esl_display_info_t display_info[ESL_DISPLAY_NUM];  /**< Per-display hardware information. */
    ble_esl_image_info_t   image_info;                     /**< Image storage information. */
    ble_esl_sensor_info_t  sensor_info[ESL_SENSOR_NUM];    /**< Per-sensor descriptor table. */
    ble_esl_led_info_t     led_info[ESL_LED_NUM];          /**< Per-LED hardware capability information. */
} ble_esl_hw_info_t;

/**
 * @brief Runtime ESL control/association state maintained for this tag.
 * @details Holds the ESL/group address assigned by the AP, the periodic advertising sync
 *          handle and AP/ESL AES-CCM key material, the current absolute time, the status
 *          flags reported in ESL control point basic-state responses, and the most recently
 *          addressed image/display/LED indices. Backing instance: esl_ctrl_info.
 */
typedef struct ble_svcs_esl_ctrl_info_s
{
    uint16_t  esl_id:8,      /**< ESL ID assigned by the AP via the ESL_ADDRESS characteristic write
                                   (0xFF = unassigned/broadcast). */
              group_id:7,    /**< Group ID assigned by the AP via the ESL_ADDRESS characteristic write. */
              rfu:1;         /**< Reserved for future use. */
    uint16_t  sync_handle;   /**< Periodic advertising sync handle while synchronized to the AP (0xFFFF = none). */
    uint8_t   ap_sync_key[16];    /**< AP sync AES-CCM key, stored byte-reversed from the AP_SYNC_KEY_MATERIAL write. */
    uint8_t   ap_sync_nonce[8];   /**< AP sync AES-CCM nonce, copied as-is from the AP_SYNC_KEY_MATERIAL write. */
    uint8_t   esl_rsp_key[16];    /**< ESL response AES-CCM key, stored byte-reversed from the ESL_RSP_KEY_MATERIAL write. */
    uint8_t   esl_rsp_nonce[8];   /**< ESL response AES-CCM nonce, copied as-is from the ESL_RSP_KEY_MATERIAL write. */
    uint32_t  current_time;  /**< Current absolute time, from the ESL_CURRENT_ABSOLUTE_TIME characteristic write. */
    uint16_t  service_need:1,            /**< 1 if the ESL requires servicing (basic state response). */
              synchronized:1,            /**< 1 if the ESL is synchronized to the AP's periodic advertising. */
              active_led:1,              /**< 1 if any LED is currently active. */
              pending_led_update:1,      /**< 1 if an LED update is pending (basic state response). */
              pending_display_update:1,  /**< 1 if a display update is pending (basic state response). */
              rfu_2:11;                  /**< Reserved for future use. */
    uint8_t image_idx;      /**< Index of the image most recently selected for display. */
    uint8_t display_idx;    /**< Index of the display most recently addressed. */
    uint8_t led_idx;        /**< Index of the LED most recently addressed. */
} ble_esl_ctrl_info_t;


/**
 * @brief 48-bit (6-byte) value split into a 32-bit low part and a 16-bit high part.
 * @details Used to interpret a 6-byte OTS object ID (see the OLCP "Go To" handling in
 *          @ref ble_svcs_ots_evt_handler).
 */
typedef struct __attribute__((packed))
{
    uint32_t low;   /**< Low 32 bits (bytes 0-3) of the value. */
    uint16_t high;  /**< High 16 bits (bytes 4-5) of the value. */
}ble_uint48_t;

/**
 * @brief State of an in-progress OTS object transfer.
 * @details Backing instance: esl_object_info. Populated when the OACP "Write" request is
 *          processed and filled in by @ref ots_data_received() as object data arrives over
 *          L2CAP CoC.
 */
typedef struct ble_object_info_s
{
    uint16_t object_size;       /**< Negotiated total object size, in bytes (from the OACP Write request length). */
    uint16_t l2cap_data_size;   /**< Total payload length declared in the 2-byte length prefix of the first L2CAP data chunk. */
    uint16_t current_size;      /**< Number of object bytes received so far. */
    uint8_t *p_object_data;     /**< Buffer accumulating the received object data; NULL when no transfer is in progress. */

} ble_object_info_t;


/**
 * @brief Parameters for decrypting an AES-CCM encrypted periodic advertising data payload.
 * @details Used by @ref app_decrypt_adv_data().
 */
typedef struct __attribute__((packed)) decrypt_eadv_param_s
{
    uint8_t *p_rand;        /**< Randomizer bytes from the encrypted AD header; forms part of the CCM nonce. */
    uint8_t *p_enc_data;    /**< Encrypted payload (ciphertext followed by MIC). */
    uint8_t enc_data_len;   /**< Length of p_enc_data in bytes, including the MIC. */
    uint8_t *p_dec_data;    /**< Output buffer to receive the decrypted plaintext. */
    uint8_t *p_dec_key;     /**< AES-CCM decryption key. */
    uint8_t *p_dec_nonce;   /**< Fixed part of the AES-CCM nonce (combined with p_rand). */
} decrypt_eadv_param_t;

/** @brief Action selector for @ref esl_image_action_t / @ref esl_epd_image_process(). */
typedef uint8_t ESL_IMAGE_ACTION;
#define ESL_IMAGE_DATA_ERASE        ((ESL_IMAGE_ACTION)0x01)   /**< Erase the flash sector for an image index/part. */
#define ESL_IMAGE_DATA_UPDATE       ((ESL_IMAGE_ACTION)0x02)   /**< Write new image data into the most recently erased flash sector. */
#define ESL_IMAGE_DATA_DISPLAY      ((ESL_IMAGE_ACTION)0x03)   /**< Read a stored image back from flash for display. */
/**
 * @brief Parameters for @ref esl_epd_image_process(), selecting an erase/update/display
 *        action on EPD image flash storage.
 */
typedef struct esl_image_action_s
{
    ESL_IMAGE_ACTION action;   /**< Selects the operation to perform; one of ESL_IMAGE_DATA_ERASE/UPDATE/DISPLAY. */
    union
    {
        struct
        {
            uint8_t image_index;   /**< Image index to erase. */
            uint8_t part;          /**< Image part to erase (0 = black/white plane, 1 = red plane). */
        } erase;
        struct
        {
            uint16_t image_len;        /**< Length of the image data to write, in bytes. */
            uint8_t *p_image_data;     /**< Image data to write into the most recently erased sector. */
        } update;
        struct
        {
            uint8_t image_index;       /**< Image index to read back. */
            uint8_t part;              /**< Image part to read back (0 = black/white plane, 1 = red plane). */
            uint16_t image_len;        /**< Output: length of the image data read back, in bytes (0 if none stored). */
            uint8_t *p_image_data;     /**< Output: newly allocated buffer with the image data; caller must free it. */
        } display;
    }u;     /**< Action-specific parameters; see action. */
} esl_image_action_t;

/**************************************************************************************************
 *    LOCAL VARIABLES
 *************************************************************************************************/
static QueueHandle_t g_app_msg_q;
static SemaphoreHandle_t semaphore_cb;
static SemaphoreHandle_t semaphore_isr;
static SemaphoreHandle_t semaphore_app;
static SemaphoreHandle_t semaphore_display;

static uint8_t g_advertising_host_id = BLE_HOSTID_RESERVED;

static uint8_t g_app_rand_value = 0;
static TimerHandle_t g_absolute_time_timer;
static TimerHandle_t  g_fota_timer;

static uint32_t g_absolute_time = 0;

//esl_app_info_t esl_app_info;

ble_object_info_t esl_object_info =
{
    .object_size = 0,
    .current_size = 0,
    .p_object_data = NULL,
};


ble_esl_hw_info_t esl_hw_info = 
{
    { //display info
        0x0080,   //display1 info, width
        0x0080,   //display1 info, height
        0x01,     //display1 info, display type
        0x0080,   //display2 info, width
        0x0080,   //display2 info, height
        0x01,     //display2 info, display type
    },
    {  //image info
        0x02,  //max_image_index
    },
    {  //sensor info
        0x00,       //size
        {
            MESH_DEVICE_PRESENT_INPUT_VOLTAGE,     //stanard sensor id
        },
        0x00,       //size
        {
            MESH_DEVICE_PRESENT_DEVICE_OPERATING_TEMPERATURE,     //stanard sensor id
        },
        0x00,       //size
        {
            MESH_DEVICE_DEVICE_FIRMWAR_REVISION,     //stanard sensor id
        },
        0x00,       //size
        {
            MESH_DEVICE_DEVICE_DATE_OF_MANUFACTURE,     //stanard sensor id
        },
        0x00,       //size
        {
            MESH_DEVICE_DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION,     //stanard sensor id
        },
        0x01,       //size
        {
            0xCAFE02FF, //vendor specific sensor id
        },
        0x00,       //size
        {
            MESH_DEVICE_PRESENT_AMBIENT_TEMPERATURE,     //stanard sensor id
        },
        0x01,       //size
        {
            0xC0DE02FF, //vendor specific sensor id
        },
    },
    {  //LED info
        3, //red
        2, //green
        0, //blue
        0, //LED type
    }
};


ble_esl_ctrl_info_t esl_ctrl_info = 
{
    .esl_id = 0xFF,
    .sync_handle = 0xFFFF,
    .service_need = 0,
    .synchronized = 0,
    .active_led = 0,
    .pending_led_update = 0,
    .pending_display_update = 0,
    .rfu = 0x00,
};



/**************************************************************************************************
 *    FUNCTION DECLARATION
 *************************************************************************************************/
static void ble_app_main(app_req_param_t *p_param);
static ble_err_t ble_init(void);
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data);
static ble_err_t adv_init(void);
static ble_err_t adv_enable(uint8_t host_id);
static bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr);
static bool fota_sw_timer_start(void);

/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
/* ------------------------------
 *  Handler
 * ------------------------------
 */
/**
 * @brief GPIO interrupt handler for the wake-up pin.
 *
 * This function is invoked from ISR context when the configured wake-up GPIO edge is
 * detected. It masks the BLE application task's low power mode so the task can wake up and
 * process pending work.
 *
 * @param pin Pin number that triggered the interrupt (unused).
 * @param isr_param User parameter registered with the GPIO interrupt (unused).
 * @return void This function does not return a value.
 * @note Called from ISR context.
 */
static void app_gpio_handler(uint32_t pin, void *isr_param)
{
    hosal_lpm_ioctrl(HOSAL_LPM_MASK, HOSAL_LOW_POWER_MASK_BIT_TASK_BLE_APP);
}

/**
 * @brief Turns the ESL LED on.
 *
 * This function drives the LED GPIO pin low to turn the (active-low) LED on.
 *
 * @return void This function does not return a value.
 */
static void led_on(void) { hosal_gpio_pin_write(ESL_LED_PIN, 0); }

/**
 * @brief Turns the ESL LED off.
 *
 * This function drives the LED GPIO pin high to turn the (active-low) LED off.
 *
 * @return void This function does not return a value.
 */
static void led_off(void) { hosal_gpio_pin_write(ESL_LED_PIN, 1); }

/**
 * @brief Performs flash erase/update/display operations for EPD image data.
 *
 * Depending on p_update->action:
 * - @ref ESL_IMAGE_DATA_ERASE erases the flash sector for the given image index/part (if the
 *   image index is valid) and remembers it, via static state, as the target for a subsequent
 *   update.
 * - @ref ESL_IMAGE_DATA_UPDATE writes the supplied image data into the flash sector most
 *   recently erased.
 * - @ref ESL_IMAGE_DATA_DISPLAY reads the stored image back from flash into a newly allocated
 *   buffer, writing the buffer pointer and length into p_update->u.display.
 *
 * @param p_update Image action descriptor: selects the action and carries its parameters; for
 *                 the display action, the resulting buffer/length are written back into it.
 * @return void This function does not return a value.
 * @note For the display action, the caller is responsible for freeing
 *       p_update->u.display.p_image_data.
 */
static void esl_epd_image_process(esl_image_action_t *p_update)
{
    uint32_t curr_len, remain_len;
    uint8_t *p_esl_epd_image, *p_flash_address, temp_buff[256];
    static uint8_t last_image_idx = 0xff;
    static uint8_t last_image_part = 0xff;

    p_esl_epd_image = EPD_IMAGE_FLASH_ADDRESS;

    if (p_update->action == ESL_IMAGE_DATA_ERASE)
    {
        p_flash_address = (p_esl_epd_image + (p_update->u.erase.image_index * 8192) + (p_update->u.erase.part * 4096));
        if (p_update->u.erase.image_index < 2)
        {
            printf("erase addr 0x%08x\n", (uint32_t)(p_flash_address));
            hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, (uint32_t)(p_flash_address));
            last_image_idx = p_update->u.erase.image_index;
            last_image_part = p_update->u.erase.part;
        }
        else
        {
            printf("invalid image index %d\n", p_update->u.erase.image_index);
        }
    }
    else if (p_update->action == ESL_IMAGE_DATA_UPDATE)
    {
         if (last_image_idx < 2)
         {
            curr_len = 0;
            remain_len = p_update->u.update.image_len;
            if (p_update->u.update.image_len < 256)
            {
                printf("invalid image size %d\n", p_update->u.update.image_len);
                return;
            }
            p_flash_address = (p_esl_epd_image + (last_image_idx * 8192) + (last_image_part * 4096));

            while (remain_len > 0)
            {
                if (curr_len == 0) 
                {
                    temp_buff[0] = (p_update->u.update.image_len & 0xFF);
                    temp_buff[1] = ((p_update->u.update.image_len >> 8) & 0xFF);
                    memcpy(temp_buff + 2, p_update->u.update.p_image_data, 254);
                    hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, (uint32_t)(p_flash_address + curr_len), temp_buff);
                    remain_len -= 254;
                    curr_len += 256;
                }
                else if (remain_len > 256)
                {
                    hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, (uint32_t)(p_flash_address + curr_len),
                                      p_update->u.update.p_image_data + curr_len - 2);
                    remain_len -= 256;
                    curr_len += 256;
                }
                else
                {
                    while ( hosal_flash_write(HOSAL_FLASH_WRITE_BYTE, (uint32_t)(p_flash_address + curr_len),
                                      p_update->u.update.p_image_data + curr_len - 2) != 0);

                    remain_len -= 1;
                    curr_len += 1;
                }
            }

         }
         else
        {
            printf("incorrect image index %d\n", last_image_idx);
        }
    }
    else if (p_update->action == ESL_IMAGE_DATA_DISPLAY)
    {
        
        p_flash_address = (p_esl_epd_image + (p_update->u.display.image_index * 8192) + (p_update->u.display.part * 4096));
        hosal_flash_read(HOSAL_FLASH_READ_PAGE, (uint32_t)(p_flash_address), temp_buff);
        p_update->u.display.image_len = (temp_buff[0] | (temp_buff[1] << 8));

        if (p_update->u.display.image_len == 0xFFFF)
        {
            p_update->u.display.image_len = 0;
        }

        if (p_update->u.display.image_len != 0)
        {
            p_update->u.display.p_image_data = pvPortMalloc(p_update->u.display.image_len);
            if (p_update->u.display.p_image_data != NULL)
            {
                memcpy(p_update->u.display.p_image_data, temp_buff + 2, 254);
                remain_len = p_update->u.display.image_len - 254;
                curr_len = 256;
                while (remain_len > 0)
                {
                    if (remain_len >= 256)
                    {
                        hosal_flash_read(HOSAL_FLASH_READ_PAGE, (uint32_t)(p_flash_address + curr_len),
                                          p_update->u.display.p_image_data + curr_len - 2);
                        remain_len -= 256;
                        curr_len += 256;
                    }
                    else
                    {
                        hosal_flash_read(HOSAL_FLASH_READ_BYTE, (uint32_t)(p_flash_address + curr_len),
                                          p_update->u.display.p_image_data + curr_len - 2);
                        remain_len -= 1;
                        curr_len += 1;
                    }
    
                    hosal_flash_ioctrl(HOSAL_FLASH_CACHE, NULL);
                }
            }
            else
            {
                p_update->u.display.image_len = 0;
            }
        }

    }
}

/**
 * @brief Decrypts an AES-CCM encrypted advertising data payload.
 *
 * This function initializes the AES engine with the supplied decryption key, builds the CCM
 * nonce from the payload's randomizer and the supplied nonce, and runs CCM
 * decryption/verification over the encrypted payload described by p_decrypt_parm.
 *
 * @param p_decrypt_parm Decryption parameters: randomizer, encrypted data and its length,
 *                        decryption key/nonce, and the output buffer to receive the
 *                        decrypted data.
 * @return The number of decrypted bytes written to p_decrypt_parm->p_dec_data on success,
 *         or 0 if the payload buffer allocation or CCM decryption/verification failed.
 */
static uint32_t app_decrypt_adv_data(decrypt_eadv_param_t *p_decrypt_parm)
{
    struct aes_ctx ctx;
    struct aes_ccm_decryption_packet  ccm_payload_packet;
    uint8_t data_len,  nonce[13];
    uint32_t status, outbuf_length;
    
    crypto_lib_init();

    aes_acquire(&ctx);

    aes_key_init(&ctx, p_decrypt_parm->p_dec_key, AES_KEY128);

    /*load key to secure engine*/
    aes_load_round_key(&ctx);

    ccm_payload_packet.payload_buf = pvPortMalloc(p_decrypt_parm->enc_data_len + 1); //1:ESL_ENADV_CCM_HEADER

    if (ccm_payload_packet.payload_buf != NULL)
    {
        ccm_payload_packet.payload_buf[0] = ESL_ENADV_CCM_HEADER;
        memcpy(ccm_payload_packet.payload_buf + 1, p_decrypt_parm->p_enc_data, p_decrypt_parm->enc_data_len);

        memcpy(nonce, p_decrypt_parm->p_rand, 5);
        memcpy(nonce + 5, p_decrypt_parm->p_dec_nonce, 8);
        ccm_payload_packet.nonce = (uint8_t *) nonce;

        ccm_payload_packet.hdr_len = 1;
        ccm_payload_packet.mlen = ESL_ENADV_MIC_SIZE;
        ccm_payload_packet.data_len = (p_decrypt_parm->enc_data_len - ccm_payload_packet.mlen);   /*Notice: we don't count mlen for data length!*/

        outbuf_length = ccm_payload_packet.data_len;    
        ccm_payload_packet.out_buf = p_decrypt_parm->p_dec_data;
        ccm_payload_packet.out_buf_len = &outbuf_length;

        status = aes_ccm_decryption_verification( &ccm_payload_packet);

        vPortFree(ccm_payload_packet.payload_buf);
    }
    else
    {
        status = STATUS_ERROR;
    }
    aes_release(&ctx);

    if (status == STATUS_SUCCESS)
    {
        return outbuf_length;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief Encrypts an ESL control point response payload with AES-CCM.
 *
 * This function initializes the AES engine with the ESL response key (esl_ctrl_info.esl_rsp_key),
 * builds the CCM nonce from p_rand and the stored ESL response nonce (esl_ctrl_info.esl_rsp_nonce),
 * and runs CCM encryption over p_data.
 *
 * @param p_rand Randomizer bytes used as the first part of the CCM nonce.
 * @param p_data Plaintext response data to encrypt.
 * @param data_len Length of p_data in bytes.
 * @param p_enc_data Output buffer that receives the CCM header byte, ciphertext, and MIC.
 * @return The number of bytes written to p_enc_data (data length + MIC size + header length)
 *         on success, or 0 if CCM encryption failed.
 */
static uint32_t app_enc_adv_data(uint8_t *p_rand, uint8_t *p_data, uint8_t data_len, uint8_t *p_enc_data)
{
    struct aes_ctx ctx;
    struct aes_ccm_encryption_packet  ccm_packet;
    uint8_t nonce[13], headr;
    uint32_t status, outbuf_length;

    aes_acquire(&ctx);
    aes_key_init(&ctx, esl_ctrl_info.esl_rsp_key, AES_KEY128);

    /*load key to secure engine*/
    aes_load_round_key(&ctx);

    memcpy(nonce, p_rand, 5);
    memcpy(nonce + 5, esl_ctrl_info.esl_rsp_nonce, 8);
    ccm_packet.nonce = (uint8_t *) nonce;
    ccm_packet.hdr_len = 1;

    headr = ESL_ENADV_CCM_HEADER;
    ccm_packet.hdr = &headr;
    ccm_packet.data_len = data_len;
    ccm_packet.data = p_data;
    ccm_packet.mlen = ESL_ENADV_MIC_SIZE;
    ccm_packet.out_buf = p_enc_data;

    outbuf_length = data_len + ccm_packet.mlen + ccm_packet.hdr_len;
    ccm_packet.out_buf_len = &outbuf_length;

    status = aes_ccm_encryption( &ccm_packet);
    aes_release(&ctx);

    if (status == STATUS_SUCCESS)
    {
        return outbuf_length;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief Builds and transmits an encrypted periodic advertising response subevent.
 *
 * This function allocates a response buffer, fills the ESL periodic advertising response
 * header (sync handle, request/response event and subevent, response slot, and a fresh
 * randomizer), encrypts p_rsp_data with @ref app_enc_adv_data, and hands the resulting
 * response to the BLE stack via ble_cmd_periodic_adv_rsp_data_set().
 *
 * @param p_adv_data_info Periodic advertising report the response corresponds to; provides
 *                         the sync handle, event counter, and subevent used in the header.
 * @param rsp_slot Response subevent slot to transmit in.
 * @param p_rsp_data Plaintext ESL control point response payload to encrypt and send.
 * @param rsp_len Length of p_rsp_data in bytes.
 * @return TRUE if the response buffers were allocated and the response was submitted to the
 *         BLE stack, FALSE if a buffer allocation failed.
 */
static uint8_t app_esl_send_adv_response(ble_evt_padv_padv_report_v2_t *p_adv_data_info, uint8_t rsp_slot, uint8_t *p_rsp_data, uint8_t rsp_len)
{
    ble_periodic_adv_rsp_data_set_param_t *p_adv_rsp_data;
    uint8_t *p_enc_payload, rsp_send, enc_len;

    rsp_send = FALSE;
    p_adv_rsp_data = pvPortMalloc(sizeof(ble_periodic_adv_rsp_data_set_param_t) + 
                                 sizeof(esls_padv_enc_hdr_param_t) + ESL_ENADV_MIC_SIZE + rsp_len);

    if (p_adv_rsp_data != NULL)
    { 
        ESL_RSP_DATA_HEADER(p_adv_rsp_data, p_adv_data_info, rsp_slot, rsp_len);

        p_enc_payload = pvPortMalloc(rsp_len + ESL_ENADV_MIC_SIZE + 0x01/*addition data "0xEA"*/);

        if (p_enc_payload == NULL)
        { 
            vPortFree(p_adv_rsp_data);
            return rsp_send;
        }

        enc_len = app_enc_adv_data(((esls_padv_enc_hdr_param_t*)(p_adv_rsp_data->rsp_data))->randomizer, 
                                   p_rsp_data, rsp_len,  p_enc_payload);
        memcpy(((esls_padv_enc_hdr_param_t*)(p_adv_rsp_data->rsp_data))->enc_data, p_enc_payload + 1, (enc_len - 1));//shift one byte addition data "0xEA""
                p_adv_rsp_data->rsp_data_len += (enc_len - 1);
        ble_cmd_periodic_adv_rsp_data_set(p_adv_rsp_data);

        vPortFree(p_enc_payload);
        vPortFree(p_adv_rsp_data);
        rsp_send = TRUE;
    }
    return rsp_send; 
}

/**
 * @brief Parses and executes a single ESL control point command.
 *
 * This function is only processed if the command's esl_id matches this ESL's assigned ID
 * (esl_ctrl_info.esl_id) or is the broadcast value 0xff. It dispatches on
 * p_ctrl_point_cmd->cmd_header.opcode (PING, UNASSOCIATE_FROM_AP, SERVICE_RESET,
 * FACTORY_RESET, UPDATE_COMPLETE, READ_SENSOR_DATA, REFRESH_DISPLAY, DISPLAY_IMAGE,
 * DISPLAY_TIMED_IMAGE, LED_CONTROL, LED_TIMED_CONTROL), updating the ESL control state
 * (esl_ctrl_info) and LED/EPD hardware as needed, and queuing EPD display requests to the
 * application task where applicable.
 *
 * For most opcodes, unless the command's esl_id is the broadcast value 0xff, a response is
 * generated: on success the matching state response (basic/display/sensor/LED state) is
 * allocated into *pp_rsp_data with its length written to *p_rsp_data_len; on failure (invalid
 * opcode, invalid parameters/image index, or allocation failure) an ESL_RSP_ERROR response is
 * built instead. FACTORY_RESET and UPDATE_COMPLETE never generate a response regardless of
 * esl_id, and READ_SENSOR_DATA generates its success response even when esl_id is the
 * broadcast value.
 *
 * @param p_ctrl_point_cmd Decoded ESL control point command to execute.
 * @param pp_rsp_data Output: set to a newly allocated response buffer when a response is
 *                    generated. The caller owns and must free this buffer.
 * @param p_rsp_data_len Output: always initialized to 0; updated to the length of the
 *                        response written to *pp_rsp_data if a response was generated.
 * @return void This function does not return a value.
 */
static void app_esl_cmd_parsing(ble_esl_control_point_cmd_t *p_ctrl_point_cmd, uint8_t **pp_rsp_data, uint8_t *p_rsp_data_len)
{
    uint8_t error_code = ESL_RSP_ERROR_CODE_RFU;
    esls_ctrl_point_rsp_err_param_t *p_err;
    esls_ctrl_point_rsp_display_state_param_t *p_display_state;
    display_req_display_change_param_t *p_display_change_param;
    app_queue_t app_q;
    static uint8_t last_image_index = 0xff;
    *p_rsp_data_len = 0;

    if ((p_ctrl_point_cmd->cmd_header.esl_id == esl_ctrl_info.esl_id) ||
        (p_ctrl_point_cmd->cmd_header.esl_id == 0xff))
    {
        switch (p_ctrl_point_cmd->cmd_header.opcode)
        {                        
            case ESL_CMD_UNASSOCIATE_FROM_AP:
            case ESL_CMD_SERVICE_RESET:
            case ESL_CMD_PING:
            {
                esls_ctrl_point_rsp_basic_state_param_t *p_basic_state;

                if (p_ctrl_point_cmd->cmd_header.opcode == ESL_CMD_UNASSOCIATE_FROM_AP)
                {
                    esl_ctrl_info.synchronized = 0;
                    if (app_request_set(APP_ESL_P_HOST_ID, APP_REQUEST_SYNC_TERMINATE, false) == false)
                    {
                        printf("unassociate from ap fail\n");
                    }
                    printf("unassociate from ap\n");
                }
                else if (p_ctrl_point_cmd->cmd_header.opcode == ESL_CMD_SERVICE_RESET)
                {
                    esl_ctrl_info.service_need = 0;
                    printf("service reset\n");
                }
                else
                {
                    printf("ping\n");
                }

                if (p_ctrl_point_cmd->cmd_header.esl_id != 0xff)
                {
                    *pp_rsp_data = pvPortMalloc(sizeof(esls_ctrl_point_rsp_basic_state_param_t));
                    if (*pp_rsp_data != NULL)
                    {
                        *p_rsp_data_len = sizeof(esls_ctrl_point_rsp_basic_state_param_t);
                        p_basic_state = (esls_ctrl_point_rsp_basic_state_param_t *)(*pp_rsp_data);

                        p_basic_state->opcode = ESL_RSP_BASIC_STATE; // opcode
                        p_basic_state->service_need = esl_ctrl_info.service_need;
                        p_basic_state->synchronized = esl_ctrl_info.synchronized;
                        p_basic_state->active_led = esl_ctrl_info.active_led; 
                        p_basic_state->pending_led_update = esl_ctrl_info.pending_led_update;
                        p_basic_state->pending_display_update = esl_ctrl_info.pending_display_update;
                        p_basic_state->rfu = esl_ctrl_info.rfu;
                    }
                    else
                    {
                        error_code = ESL_RSP_ERROR_CODE_INSUFFICIENT_RESOURCES;
                    }
                }
            }
            break;

            case ESL_CMD_FACTORY_RESET:
                if (ble_app_link_info[APP_ESL_P_HOST_ID].state == STATE_CONNECTED)
                {
                     sys_software_reset();
                }
            break;

            case ESL_CMD_UPDATE_COMPLETE:
                printf("update complete %d\n", esl_ctrl_info.synchronized);
                if (esl_ctrl_info.synchronized == 1)
                {
                    ble_cmd_conn_terminate(0);
                }
            break;

            case ESL_CMD_READ_SENSOR_DATA:
            {
                esls_ctrl_point_rsp_sensor_state_param_t *p_sensor_state;
                static uint8_t sensor_data_len_for_test = 1;

                printf("read sensor data index %d\n", p_ctrl_point_cmd->read_sensor_data.sensor_index);

                if (p_ctrl_point_cmd->read_sensor_data.sensor_index >= ESL_SENSOR_NUM)
                {
                    error_code = ESL_RSP_ERROR_CODE_INVALID_PARAMETERS;
                }
                else
                {
                    *pp_rsp_data = pvPortMalloc(sizeof(esls_ctrl_point_rsp_sensor_state_param_t) + (sensor_data_len_for_test & 0x0f));
                    if (*pp_rsp_data != NULL)
                    {
                        *p_rsp_data_len = sizeof(esls_ctrl_point_rsp_sensor_state_param_t) + (sensor_data_len_for_test & 0x0f);
                        p_sensor_state = (esls_ctrl_point_rsp_sensor_state_param_t *)(*pp_rsp_data);

                        p_sensor_state->opcode = ESL_RSP_SENSOR_VALUE; // opcode
                        p_sensor_state->opcode |= ((sensor_data_len_for_test & 0x0f) << 4);
                        p_sensor_state->sensor_index = p_ctrl_point_cmd->read_sensor_data.sensor_index;
                        for (uint8_t i = 0; i < (sensor_data_len_for_test & 0x0f); i++)
                        {
                            p_sensor_state->sensor_data[i] = i;
                        }
                    }
                    else
                    {
                        error_code = ESL_RSP_ERROR_CODE_INSUFFICIENT_RESOURCES;
                    }
                    sensor_data_len_for_test++;
                }
            }
            break;

            case ESL_CMD_REFRESH_DISPLAY:
            {
                printf("display image, display_index %d %d\n", 
                       p_ctrl_point_cmd->refresh_display.display_index, last_image_index);
                if (p_ctrl_point_cmd->refresh_display.display_index >= ESL_DISPLAY_NUM)
                {
                    error_code = ESL_RSP_ERROR_CODE_INVALID_PARAMETERS;
                }
                else
                {
                    display_req_display_refresh_param_t *p_display_refresh_param;

                    esl_ctrl_info.display_idx = p_ctrl_point_cmd->refresh_display.display_index;

                    if (p_ctrl_point_cmd->cmd_header.esl_id != 0xff)
                    {
                        if (last_image_index != 0xFF)
                        {
                            *pp_rsp_data = pvPortMalloc(sizeof(esls_ctrl_point_rsp_display_state_param_t));
                            if (*pp_rsp_data != NULL)
                            {
                                *p_rsp_data_len = sizeof(esls_ctrl_point_rsp_display_state_param_t);
                                p_display_state = (esls_ctrl_point_rsp_display_state_param_t *)(*pp_rsp_data);
                        
                                p_display_state->opcode = ESL_RSP_DISPLAY_STATE; // opcode
                                p_display_state->display_index = esl_ctrl_info.display_idx;
                                p_display_state->image_index = last_image_index;
                            }
                            else
                            {
                                error_code = ESL_RSP_ERROR_CODE_INSUFFICIENT_RESOURCES;
                            }
                        }
                        else
                        {
                            //send error response
                            error_code = ESL_RSP_ERROR_CODE_INVALID_PARAMETERS;
                        }
                    }

                    if (last_image_index != 0xFF)
                    {
                        app_q.event = 0; // from BLE
                        app_q.param_type = QUEUE_TYPE_DISPLAY_REQ;
                        app_q.param.pt_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(display_req_display_change_param_t));

                        if (app_q.param.pt_tlv != NULL)
                        {
                            app_q.param.pt_tlv->type = DISPLAY_REQ_DISPLAY_REFRESH;
                            p_display_refresh_param = (display_req_display_refresh_param_t *)app_q.param.pt_tlv->value;
                            p_display_refresh_param->display_index = esl_ctrl_info.display_idx;
                            p_display_refresh_param->image_index = last_image_index;
                            if (xSemaphoreTake(semaphore_display, 0) == pdTRUE)
                            {
                                if (xQueueSendToBack(g_app_msg_q, &app_q, 0) != pdTRUE)
                                {
                                    // send error
                                    vPortFree(app_q.param.pt_tlv);
                                    xSemaphoreGive(semaphore_display);
                                }
                            }
                            else
                            {
                                vPortFree(app_q.param.pt_tlv);
                            }
                        }
                    }
                }

            }
            break;

            case ESL_CMD_DISPLAY_IMAGE:
            {
                printf("display image, display_index %d image_index %d\n", 
                        p_ctrl_point_cmd->display_image.display_index, p_ctrl_point_cmd->display_image.image_index);

                if (p_ctrl_point_cmd->display_image.display_index >= ESL_DISPLAY_NUM)
                {
                    error_code = ESL_RSP_ERROR_CODE_INVALID_PARAMETERS;
                }
                else if (p_ctrl_point_cmd->display_image.image_index >= ESL_IMAGE_NUM)
                { 
                    error_code = ESL_RSP_ERROR_CODE_INVALID_IMAGE_INDEX;
                }
                else
                {
                    esl_ctrl_info.display_idx = p_ctrl_point_cmd->display_image.display_index;
                    esl_ctrl_info.image_idx = p_ctrl_point_cmd->display_image.image_index;
                    last_image_index = esl_ctrl_info.image_idx;
                    if (p_ctrl_point_cmd->cmd_header.esl_id != 0xff)
                    {
                        *pp_rsp_data = pvPortMalloc(sizeof(esls_ctrl_point_rsp_display_state_param_t));
                        if (*pp_rsp_data != NULL)
                        {
                            *p_rsp_data_len = sizeof(esls_ctrl_point_rsp_display_state_param_t);
                            p_display_state = (esls_ctrl_point_rsp_display_state_param_t *)(*pp_rsp_data);
                        
                            p_display_state->opcode = ESL_RSP_DISPLAY_STATE; // opcode
                            p_display_state->display_index = esl_ctrl_info.display_idx;
                            p_display_state->image_index = esl_ctrl_info.image_idx;
                        }
                        else
                        {
                            error_code = ESL_RSP_ERROR_CODE_INSUFFICIENT_RESOURCES;
                        }
                    }

                    app_q.event = 0; // from BLE
                    app_q.param_type = QUEUE_TYPE_DISPLAY_REQ;
                    app_q.param.pt_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(display_req_display_change_param_t));

                    if (app_q.param.pt_tlv != NULL)
                    {
                        app_q.param.pt_tlv->type = DISPLAY_REQ_DISPLAY_CHANGE;
                        p_display_change_param = (display_req_display_change_param_t *)app_q.param.pt_tlv->value;
                        p_display_change_param->display_index = esl_ctrl_info.display_idx;
                        p_display_change_param->image_index = esl_ctrl_info.image_idx;
                        if (xSemaphoreTake(semaphore_display, 0) == pdTRUE)
                        {
                            if (xQueueSendToBack(g_app_msg_q, &app_q, 0) != pdTRUE)
                            {
                                // send error
                                xSemaphoreGive(semaphore_display);
                                vPortFree(app_q.param.pt_tlv);
                            }
                        }
                        else
                        {
                            vPortFree(app_q.param.pt_tlv);
                        }

                    }



                }
            }
            break;

            case ESL_CMD_DISPLAY_TIMED_IMAGE:
            {
                printf("display timed image, display_index %d image_index %d\n", 
                        p_ctrl_point_cmd->display_timed_image.display_index, p_ctrl_point_cmd->display_timed_image.image_index);

                if (p_ctrl_point_cmd->display_timed_image.display_index >= ESL_DISPLAY_NUM)
                {
                    error_code = ESL_RSP_ERROR_CODE_INVALID_PARAMETERS;
                }
                else if(p_ctrl_point_cmd->display_timed_image.image_index >= ESL_IMAGE_NUM)
                { 
                    error_code = ESL_RSP_ERROR_CODE_INVALID_IMAGE_INDEX;
                }
                else
                {
                    esl_ctrl_info.display_idx = p_ctrl_point_cmd->display_timed_image.display_index;
                    esl_ctrl_info.image_idx = p_ctrl_point_cmd->display_timed_image.image_index;

                    if (p_ctrl_point_cmd->cmd_header.esl_id != 0xff)
                    {
                        *pp_rsp_data = pvPortMalloc(sizeof(esls_ctrl_point_rsp_display_state_param_t));
                        if (*pp_rsp_data != NULL)
                        {
                            *p_rsp_data_len = sizeof(esls_ctrl_point_rsp_display_state_param_t);
                            p_display_state = (esls_ctrl_point_rsp_display_state_param_t *)(*pp_rsp_data);
                        
                            p_display_state->opcode = ESL_RSP_DISPLAY_STATE; // opcode
                            p_display_state->display_index = esl_ctrl_info.display_idx;
                            p_display_state->image_index = esl_ctrl_info.image_idx;
                        }
                        else
                        {
                            error_code = ESL_RSP_ERROR_CODE_INSUFFICIENT_RESOURCES;
                        }
                    }
                    app_q.event = 0; // from BLE
                    app_q.param_type = QUEUE_TYPE_DISPLAY_REQ;
                    app_q.param.pt_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(display_req_display_change_param_t));

                    if (app_q.param.pt_tlv != NULL)
                    {
                        app_q.param.pt_tlv->type = DISPLAY_REQ_DISPLAY_CHANGE;
                        p_display_change_param = (display_req_display_change_param_t *)app_q.param.pt_tlv->value;
                        p_display_change_param->display_index = esl_ctrl_info.display_idx;
                        p_display_change_param->image_index = esl_ctrl_info.image_idx;
                        if (xSemaphoreTake(semaphore_display, 0) == pdTRUE)
                        {
                            if (xQueueSendToBack(g_app_msg_q, &app_q, 0) != pdTRUE)
                            {
                                // send error
                                xSemaphoreGive(semaphore_display);
                                vPortFree(app_q.param.pt_tlv);
                            }
                        }
                        else
                        {
                            vPortFree(app_q.param.pt_tlv);
                        }

                    }
                }
            }
            break;

            case ESL_CMD_LED_CONTROL:
            {
                printf("led control, led index %d\n", p_ctrl_point_cmd->led_ctrl.led_index);

                if (p_ctrl_point_cmd->led_ctrl.led_index >= ESL_LED_NUM)
                {
                    error_code = ESL_RSP_ERROR_CODE_INVALID_PARAMETERS;
                }
                else
                {
                    esls_ctrl_point_rsp_led_state_param_t *p_led_state;
                    esl_ctrl_info.led_idx = p_ctrl_point_cmd->led_ctrl.led_index;

                    if (p_ctrl_point_cmd->led_ctrl.repeat_duration == 0)
                    {
                        if (p_ctrl_point_cmd->led_ctrl.repeat_type == 1)
                        {
                            led_on();//led on
                            printf("LED ON\n");
                            esl_ctrl_info.active_led = 1;
                        } 
                        else
                        {
                            led_off();//led off
                            printf("LED OFF\n");
                            esl_ctrl_info.active_led = 0;
                        }
                    }

                    if (p_ctrl_point_cmd->cmd_header.esl_id != 0xff)
                    {
                        *pp_rsp_data = pvPortMalloc(sizeof(esls_ctrl_point_rsp_led_state_param_t));
                        if (*pp_rsp_data != NULL)
                        {
                            *p_rsp_data_len = sizeof(esls_ctrl_point_rsp_led_state_param_t);
                            p_led_state = (esls_ctrl_point_rsp_led_state_param_t *)(*pp_rsp_data);

                            p_led_state->opcode = ESL_RSP_LED_STATE; // opcode
                            p_led_state->led_index = esl_ctrl_info.led_idx;
                        }
                        else
                        {
                            error_code = ESL_RSP_ERROR_CODE_INSUFFICIENT_RESOURCES;
                        }
                    }
                }

            }
            break;

            case ESL_CMD_LED_TIMED_CONTROL:
            {
                printf("led control, led timed index %d\n", p_ctrl_point_cmd->led_timed_ctrl.led_index);

                if (p_ctrl_point_cmd->led_ctrl.led_index >= ESL_LED_NUM)
                {
                    error_code = ESL_RSP_ERROR_CODE_INVALID_PARAMETERS;
                }
                else
                {
                    esls_ctrl_point_rsp_led_state_param_t *p_led_state;

                    esl_ctrl_info.led_idx =  p_ctrl_point_cmd->led_timed_ctrl.led_index;

                    if (p_ctrl_point_cmd->cmd_header.esl_id != 0xff)
                    {
                        *pp_rsp_data = pvPortMalloc(sizeof(esls_ctrl_point_rsp_led_state_param_t));
                        if (*pp_rsp_data != NULL)
                        {
                            *p_rsp_data_len = sizeof(esls_ctrl_point_rsp_led_state_param_t);
                            p_led_state = (esls_ctrl_point_rsp_led_state_param_t *)(*pp_rsp_data);

                            p_led_state->opcode = ESL_RSP_LED_STATE; // opcode
                            p_led_state->led_index = esl_ctrl_info.led_idx;
                        }
                        else
                        {
                            error_code = ESL_RSP_ERROR_CODE_INSUFFICIENT_RESOURCES;
                        }
                    }
                }
            }
            break;

            default:
                error_code = ESL_RSP_ERROR_CODE_INVALID_OPCODE;
            break;
        }
        printf("error_code 0x%02x\n", error_code);
        if (error_code != ESL_RSP_ERROR_CODE_RFU)
        {
            if (p_ctrl_point_cmd->cmd_header.esl_id != 0xff)
            {
                *pp_rsp_data = pvPortMalloc(sizeof(esls_ctrl_point_rsp_err_param_t));
                if (*pp_rsp_data != NULL)
                {
                    SEND_ERR_RSP(*pp_rsp_data, *p_rsp_data_len, p_err, error_code);
                }
                else
                {
                    //TBD..
                }
            }
        }
    }
}

/**
 * @brief Parses decrypted ESL periodic advertising data and executes any control point commands.
 *
 * This function verifies the payload's AD type is Electronic Shelf Label and that its
 * group_id matches this ESL's assigned group (esl_ctrl_info.group_id) before iterating over
 * the concatenated ESL control point commands packed in the payload. Each command is executed
 * via @ref app_esl_cmd_parsing; if a command produces a response, it is wrapped in an ESL
 * response AD structure and transmitted via @ref app_esl_send_adv_response() in the response
 * subevent slot matching that command's position (0-based) in the command list.
 *
 * @param p_adv_data_info Periodic advertising report the decrypted payload came from; passed
 *                         through to @ref app_esl_send_adv_response() for response addressing.
 * @param p_data Decrypted ESL advertising data (AD tag, group_id, and packed commands).
 * @param data_len Length of p_data in bytes (unused; the command list length is instead taken
 *                 from the length field embedded in p_data).
 * @return void This function does not return a value.
 */
static void app_esl_adv_data_parsing(ble_evt_padv_padv_report_v2_t *p_adv_data_info, uint8_t *p_data, uint8_t data_len)
{
    esls_padv_enc_data_param_t *p_adv_packet;
    uint8_t parsing_len, *p_enc_payload,  rsp_slot, *p_rsp_data, rsp_data_len;
    ble_esl_control_point_cmd_t *p_ctrl_point_cmd;
    esls_padv_rsp_param_t *p_padv_rsp_param;

    p_adv_packet = (esls_padv_enc_data_param_t*)p_data;
    rsp_slot = 0;

    if (p_adv_packet->esl_tag == GAP_AD_TYPE_ELECTRONIC_SHELF_LABEL) //Electronic Shelf Label
    {
        if (p_adv_packet->group_id == esl_ctrl_info.group_id)
        {
            parsing_len = 0;
            p_rsp_data = NULL;
            while(parsing_len < (p_adv_packet->len - 2))
            {
                p_ctrl_point_cmd = (ble_esl_control_point_cmd_t*)(p_adv_packet->cmd+parsing_len);
                parsing_len +=1; // shift the size of opcode
                rsp_data_len = 0;
                
                app_esl_cmd_parsing(p_ctrl_point_cmd, &p_rsp_data, &rsp_data_len);
                if ((rsp_data_len > 0) && (p_rsp_data != NULL))
                {
                    p_padv_rsp_param = pvPortMalloc(rsp_data_len + sizeof(esls_padv_rsp_param_t));
                    if (p_padv_rsp_param != NULL)
                    {
                        p_padv_rsp_param->len = (rsp_data_len+1);
                        p_padv_rsp_param->esl_tag = GAP_AD_TYPE_ELECTRONIC_SHELF_LABEL;
                        memcpy(p_padv_rsp_param->rsp, p_rsp_data, rsp_data_len);
                        app_esl_send_adv_response(p_adv_data_info, rsp_slot, (uint8_t*)p_padv_rsp_param, rsp_data_len+2);
                        vPortFree(p_padv_rsp_param);
                    }                
                }

                if (p_rsp_data != NULL)
                {
                    vPortFree(p_rsp_data);
                    p_rsp_data = NULL;
                }
                parsing_len += (((p_ctrl_point_cmd->cmd_header.opcode & 0xF0) >> 4) + 1);
                rsp_slot++;
            }
        }
    }
}
/**
 * @brief Handles FOTA timer expiration.
 *
 * This function is invoked when the FOTA timer expires. It verifies the connection state
 * and processes the FOTA timer expiry event accordingly.
 *
 * @param timer The handle of the expired timer.
 * @return void This function does not return any value.
 */
static void fota_timer_handler(TimerHandle_t timer)
{
    configASSERT(timer);

    // FOTA
    if (ble_app_link_info[APP_ESL_P_HOST_ID].state == STATE_CONNECTED)
    {
        // FOTA timer tick and check if timer is expired
        if (ble_fota_timertick() == EXPIRED)
        {
            app_request_set(APP_ESL_P_HOST_ID, APP_REQUEST_FOTA_TIMER_EXPIRY, false);
        }
    }
}

/**
 * @brief Starts the FOTA software timer.
 *
 * This function ensures that the Firmware Over-The-Air (FOTA) software timer is active.
 * If the timer is not already running, it attempts to start it. The timer triggers
 * periodic FOTA-related tasks by invoking the fota_timer_handler function.
 *
 * @return true if the timer was successfully started or is already active, false otherwise.
 */
static bool fota_sw_timer_start(void)
{
    if ( xTimerIsTimerActive( g_fota_timer ) == pdFALSE )
    {
        if ( xTimerStart( g_fota_timer, 0 ) != pdTRUE )
        {
            // The timer could not be set into the Active state.
            return false;
        }
    }
    return true;
}

// ESL: Peripheral
/**
 * @brief Handles application-level requests for the ESL peripheral link.
 *
 * This function dispatches on p_param->app_req:
 * - @ref APP_REQUEST_ADV_START initializes GATT service data, sets the preferred MTU size,
 *   and (re)starts legacy advertising.
 * - @ref APP_REQUEST_FOTA_TIMER_EXPIRY forwards the FOTA timer expiry to the FOTA module.
 * - @ref APP_REQUEST_SYNC_TERMINATE terminates the active periodic advertising sync
 *   identified by esl_ctrl_info.sync_handle.
 *
 * @param p_param Application request parameters: host id and requested event.
 * @return void This function does not return a value.
 */
static void app_peripheral_handler(app_req_param_t *p_param)
{
    ble_err_t status;
    uint8_t host_id;
    ble_info_link0_t *p_profile_info;
    ble_periodic_advertising_sync_terminate_param_t sync_terminate;

    host_id = p_param->host_id;
    p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    status = BLE_ERR_OK;
    switch (p_param->app_req)
    {
    case APP_REQUEST_ADV_START:
        do
        {
            // service data init
            svcs_gatts_data_init(&p_profile_info->svcs_info_gatts.server_info.data);

            // set preferred MTU size and data length
            status = ble_cmd_default_mtu_size_set(host_id, DEFAULT_MTU);
            if (status != BLE_ERR_OK)
            {
                printf("ble_cmd_default_mtu_size_set() status = %d\n", status);
                break;
            }

            // enable advertising
            status = adv_init();
            if (status != BLE_ERR_OK)
            {
                printf("adv_init() status = %d\n", status);
                break;
            }

            status = adv_enable(host_id);
            if (status != BLE_ERR_OK)
            {
                printf("adv_enable() status = %d\n", status);
                break;
            }
        } while (0);
        break;

    case APP_REQUEST_FOTA_TIMER_EXPIRY:
        // handle FOTA timer expired event
        ble_fota_timerexpiry_handler(host_id);
        break;
    case APP_REQUEST_SYNC_TERMINATE:
        
        sync_terminate.sync_handle = esl_ctrl_info.sync_handle;
        status  = ble_cmd_periodic_advertising_sync_terminate(&sync_terminate);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_periodic_advertising_sync_terminate() status = %d\n", status);
        }
        break;
        //esl_ctrl_info.sync_handle = 0xFFFF;
    default:
        break;
    }
}

/**
 * @brief Handles general BLE stack events dispatched from the application task.
 *
 * This function dispatches on p_param->event to update the advertising/connected link's state
 * in ble_app_link_info (indexed by g_advertising_host_id or the event's own host_id, which this
 * function only reads), print diagnostic information, and drive the ESL periodic advertising
 * sync flow: setting the sync subevent and enabling receive on periodic advertising sync
 * transfer, decrypting and parsing periodic advertising reports (see
 * @ref app_esl_adv_data_parsing), restarting advertising on disconnect/sync loss/sync
 * terminate, and cleaning up any in-progress OTS object transfer on disconnect.
 *
 * @param p_param BLE event parameters, including the event code and its associated data.
 * @return void This function does not return a value.
 */
static void ble_evt_handler(ble_evt_param_t *p_param)
{
    switch (p_param->event)
    {
    case BLE_ADV_EVT_SET_ENABLE:
    {
        ble_evt_adv_set_adv_enable_t *p_adv_enable = (ble_evt_adv_set_adv_enable_t *)&p_param->event_param.ble_evt_adv.param.evt_set_adv_enable;

        if (p_adv_enable->status == BLE_HCI_ERR_CODE_SUCCESS)
        {
            if (p_adv_enable->adv_enabled == true)
            {
                if (g_advertising_host_id != BLE_HOSTID_RESERVED)
                {
                    ble_app_link_info[g_advertising_host_id].state = STATE_ADVERTISING;
                }
                printf("Advertising...\n");
            }
            else
            {
                if (g_advertising_host_id != BLE_HOSTID_RESERVED)
                {
                    ble_app_link_info[g_advertising_host_id].state = STATE_STANDBY;
                }
                printf("Idle.\n");
            }
        }
        else
        {
            printf("Advertising enable failed.\n");
        }
    }
    break;

    case BLE_GAP_EVT_CONN_COMPLETE:
    {
        ble_evt_gap_conn_complete_t *p_conn_param = (ble_evt_gap_conn_complete_t *)&p_param->event_param.ble_evt_gap.param.evt_conn_complete;

        if (p_conn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Connect failed, error code = 0x%02x\n", p_conn_param->status);
        }
        else
        {
            if (p_conn_param->host_id < BLE_SUPPORT_NUM_CONN_MAX)
            {
                ble_app_link_info[p_conn_param->host_id].state = STATE_CONNECTED;
            }
            printf("Connected, ID=%d, Connected to %02x:%02x:%02x:%02x:%02x:%02x\n",
                       p_conn_param->host_id,
                       p_conn_param->peer_addr.addr[5],
                       p_conn_param->peer_addr.addr[4],
                       p_conn_param->peer_addr.addr[3],
                       p_conn_param->peer_addr.addr[2],
                       p_conn_param->peer_addr.addr[1],
                       p_conn_param->peer_addr.addr[0]);
        }
    }
    break;

    case BLE_GAP_EVT_CONN_PARAM_UPDATE:
    {
        ble_evt_gap_conn_param_update_t *p_conn_param = (ble_evt_gap_conn_param_update_t *)&p_param->event_param.ble_evt_gap.param.evt_conn_param_update;

        if (p_conn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Connection update failed, error code = 0x%02x\n", p_conn_param->status);
        }
        else
        {
            printf("Connection updated\n");
            printf("ID: %d, ", p_conn_param->host_id);
            printf("Interval: %d, ", p_conn_param->conn_interval);
            printf("Latency: %d, ", p_conn_param->periph_latency);
            printf("Supervision Timeout: %d\n", p_conn_param->supv_timeout);
        }
    }
    break;

    case BLE_GAP_EVT_PHY_READ:
    case BLE_GAP_EVT_PHY_UPDATE:
    {
        ble_evt_gap_phy_t *p_phy_param = (ble_evt_gap_phy_t *)&p_param->event_param.ble_evt_gap.param.evt_phy;
        if (p_phy_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("PHY update/read failed, error code = 0x%02x\n", p_phy_param->status);
        }
        else
        {
            printf("PHY updated/read, ID: %d, TX PHY: %d, RX PHY: %d\n", p_phy_param->host_id, p_phy_param->tx_phy, p_phy_param->rx_phy);
        }
    }
    break;

    case BLE_ATT_GATT_EVT_MTU_EXCHANGE:
    {
        ble_evt_mtu_t *p_mtu_param = (ble_evt_mtu_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_mtu;
        printf("MTU Exchanged, ID:%d, size: %d\n", p_mtu_param->host_id, p_mtu_param->mtu);
    }
    break;

    case BLE_ATT_GATT_EVT_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH:
    {
        ble_evt_suggest_data_length_set_t *p_data_len_param = (ble_evt_suggest_data_length_set_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_suggest_data_length_set;

        printf("Write default data length, status: %d\n", p_data_len_param->status);
    }
    break;

    case BLE_ATT_GATT_EVT_DATA_LENGTH_CHANGE:
    {
        ble_evt_data_length_change_t *p_data_len_param = (ble_evt_data_length_change_t *)&p_param->event_param.ble_evt_att_gatt.param.ble_evt_data_length_change;
        printf("Data length changed, ID: %d\n", p_data_len_param->host_id);
        printf("MaxTxOctets: %d  MaxTxTime:%d\n", p_data_len_param->max_tx_octets, p_data_len_param->max_tx_time);
        printf("MaxRxOctets: %d  MaxRxTime:%d\n", p_data_len_param->max_rx_octets, p_data_len_param->max_rx_time);
    }
    break;

    case BLE_GAP_EVT_DISCONN_COMPLETE:
    {
        ble_evt_gap_disconn_complete_t *p_disconn_param = (ble_evt_gap_disconn_complete_t *)&p_param->event_param.ble_evt_gap.param.evt_disconn_complete;
        ble_err_t status;
    
        if (p_disconn_param->status != BLE_HCI_ERR_CODE_SUCCESS)
        {
            printf("Disconnect failed, error code = 0x%02x\n", p_disconn_param->status);
        }
        else
        {
            if (p_disconn_param->host_id < BLE_SUPPORT_NUM_CONN_MAX)
            {
                ble_app_link_info[p_disconn_param->host_id].state = STATE_STANDBY;
            }

            if (esl_ctrl_info.synchronized == 0)
            {
                // re-start adv
                if (app_request_set(p_disconn_param->host_id, APP_REQUEST_ADV_START, false) == false)
                {
                // No Application queue buffer. Error.
                }
            }
            else
            {
                status = ble_cmd_default_mtu_size_set(p_disconn_param->host_id, DEFAULT_MTU);
                if (status != BLE_ERR_OK)
                {
                    printf("ble_cmd_default_mtu_size_set() status = %d\n", status);
                }
            }
            printf("Disconnect, ID:%d, Reason:0x%02x\n", p_disconn_param->host_id, p_disconn_param->reason);

            if (esl_object_info.p_object_data != NULL)
            {
                //incomplete object data
                vPortFree(esl_object_info.p_object_data);
                esl_object_info.p_object_data = NULL;
                esl_object_info.current_size = 0;
                esl_object_info.object_size = 0;
            }
            ble_fota_disconnect();
        }
    }
    break;

    case BLE_SM_EVT_STK_GENERATION_METHOD:
    {
        ble_evt_sm_stk_gen_method_t *p_stk_gen_param = (ble_evt_sm_stk_gen_method_t *)&p_param->event_param.ble_evt_sm.param.evt_stk_gen_method;
        printf("key_gen_method %d\n", p_stk_gen_param->key_gen_method);
        if (p_stk_gen_param->key_gen_method == PASSKEY_ENTRY)
        {
            // I/O Capability is keyboard
            // Start scanning user-entered passkey.
        }
        else if (p_stk_gen_param->key_gen_method == PASSKEY_DISPLAY)
        {
            // I/O Capability is display
            // Generate a 6-digit random code and display it for pairing.
        }
    }
    break;

    case BLE_SM_EVT_AUTH_STATUS:
    {
        ble_evt_sm_auth_status_t *p_auth_param = (ble_evt_sm_auth_status_t *)&p_param->event_param.ble_evt_sm.param.evt_auth_status;

        printf("auth status %d\n", p_auth_param->status);
    }
    break;

    case BLE_PADV_EVT_SET_PADV_RECEIVE_ENABLE:
    {
        ble_evt_padv_set_padv_receive_enable_t *p_padv_receive_eanble_param = (ble_evt_padv_set_padv_receive_enable_t *)&p_param->event_param.ble_evt_padv.param.evt_set_padv_receive_enable;
        if (p_padv_receive_eanble_param->status == 0)
        {
            printf("Receiving periodic adv ...\n");
        }
    }
    break;

    case BLE_EADV_EVT_ADV_SET_TERMINATED:  
    {
        //TBD ble_evt_padv_set_padv_receive_enable_t *p_padv_receive_eanble_param = (ble_evt_padv_set_padv_receive_enable_t *)&p_param->event_param.ble_evt_padv.param.evt_set_padv_receive_enable;
    }
    break;

    case BLE_PADV_EVT_PADV_SYNC_TERMINATE:
    {
        ble_evt_padv_terminate_sync_t *p_sync_terminate_param = (ble_evt_padv_terminate_sync_t *)&p_param->event_param.ble_evt_padv.param.evt_padv_sync_terminate;
        printf("sync terminate status %d\n", p_sync_terminate_param->status);
        esl_ctrl_info.synchronized = 0;
        esl_ctrl_info.sync_handle = 0xFFFF;
        if (app_request_set(APP_ESL_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
        {
            // No Application queue buffer. Error.
        }

    }
    break;

    case BLE_PADV_EVT_PADV_SYNC_TRANSFER_RECEIVED_V2:
    {
        ble_evt_padv_padv_sync_transfer_received_v2_t *p_sync_tranfer_rcvd_v2_param = (ble_evt_padv_padv_sync_transfer_received_v2_t *)&p_param->event_param.ble_evt_padv.param.evt_padv_sync_transfer_received_v2;
        ble_periodic_sync_subevent_param_t sync_subevent;

        sync_subevent.sync_handle = p_sync_tranfer_rcvd_v2_param->sync_handle;
        sync_subevent.subevent = esl_ctrl_info.group_id;

        printf("sync_subevent %d\n", sync_subevent.subevent);
        ble_cmd_periodic_sync_subevent_set(&sync_subevent);
        ble_cmd_periodic_adv_receive_enable(p_sync_tranfer_rcvd_v2_param->sync_handle);

        esl_ctrl_info.sync_handle = p_sync_tranfer_rcvd_v2_param->sync_handle;
        esl_ctrl_info.synchronized = 1;
        esl_ctrl_info.active_led = 1;
        led_on();//led on
        ble_cmd_conn_terminate(p_sync_tranfer_rcvd_v2_param->host_id);
    }
    break;

    case BLE_PADV_EVT_PADV_SYNC_LOST:
    {
        ble_evt_padv_padv_sync_lost_t *p_sync_lost_param = (ble_evt_padv_padv_sync_lost_t *)&p_param->event_param.ble_evt_padv.param.evt_padv_sync_lost;

        if(p_sync_lost_param->sync_handle == esl_ctrl_info.sync_handle)
        {
            esl_ctrl_info.synchronized = 0;
            esl_ctrl_info.sync_handle = 0xFFFF;

            if (app_request_set(APP_ESL_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
            {
                // No Application queue buffer. Error.
            }
            printf("lost sync ...\n");
        }
        else
        {
            printf("unknow sync handle 0x%04x 0x%04x\n", p_sync_lost_param->sync_handle, esl_ctrl_info.sync_handle);
        }
    }
    break;

    case BLE_PADV_EVT_PADV_REPORT_V2:
    {
        ble_evt_padv_padv_report_v2_t *p_padv_report =  (ble_evt_padv_padv_report_v2_t *)&p_param->event_param.ble_evt_padv.param.evt_padv_report_v2;
        uint32_t dec_length;
        uint8_t *p_dec_buff, parse_len;
        decrypt_eadv_param_t decrypt_parm;
        esls_padv_enc_hdr_param_t *p_enc_hdr;

        parse_len = 0;
        while (parse_len < p_padv_report->data_length)
        {
            p_enc_hdr = (esls_padv_enc_hdr_param_t *)&p_padv_report->data[parse_len];

            if (p_enc_hdr->type == GAP_AD_TYPE_ENCRYPTED_ADVERTISING_DATA)
            {
                dec_length = (p_enc_hdr->length - sizeof(p_enc_hdr->type) - sizeof(p_enc_hdr->randomizer) - ESL_ENADV_MIC_SIZE);
                if (dec_length == 0)
                {
                    printf("error length %d\n", p_enc_hdr->length);
                }
                else
                {
                    p_dec_buff = pvPortMalloc(dec_length);
                    if (p_dec_buff != NULL)
                    {
                        decrypt_parm.p_rand = p_enc_hdr->randomizer;
                        decrypt_parm.p_enc_data = p_enc_hdr->enc_data;
                        decrypt_parm.enc_data_len = (p_enc_hdr->length - sizeof(p_enc_hdr->type) - sizeof(p_enc_hdr->randomizer));
                        decrypt_parm.p_dec_data = p_dec_buff;
                        decrypt_parm.p_dec_key = esl_ctrl_info.ap_sync_key;
                        decrypt_parm.p_dec_nonce = esl_ctrl_info.ap_sync_nonce;
                        dec_length = app_decrypt_adv_data(&decrypt_parm);

                        if (dec_length != 0)
                        {
                            app_esl_adv_data_parsing(p_padv_report, p_dec_buff, dec_length);   
                        }
                        else
                        {
                            printf("ADV data decrypt fail\n");
                        }                 
                        vPortFree(p_dec_buff);
                    }
                }
            }
            parse_len += (p_enc_hdr->length + sizeof(p_enc_hdr->length));
        }
    }
    break;

    default:
        //printf("default event:0x%02x\n", p_param->event);
        break;
    }
}

/* ------------------------------
 *  Methods
 * ------------------------------
 */
/**
 * @brief Copies a byte buffer while reversing its byte order.
 *
 * This function copies len bytes from pSrc into pDst such that pDst[len-1-i] = pSrc[i], i.e.
 * the byte order is reversed during the copy. It is used to convert the over-the-air AP sync
 * key / ESL response key byte order into the order expected by the AES engine.
 *
 * @param pDst Destination buffer (must be at least len bytes).
 * @param pSrc Source buffer (must be at least len bytes).
 * @param len Number of bytes to copy.
 * @return void This function does not return a value.
 */
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

/**
 * @brief Initializes the advertising parameters and data.
 *
 * This function sets up the advertising parameters, including the advertising type,
 * interval, channel map, and filter policy. It also configures the advertising data
 * and scan response data to be used during BLE advertising.
 *
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code on failure).
 */
static ble_err_t adv_init(void)
{
    ble_err_t status;
    ble_adv_param_t adv_param;
    ble_adv_data_param_t adv_data_param;
    ble_adv_data_param_t adv_scan_data_param;
    ble_gap_addr_t addr_param;
    const uint8_t   SCANRSP_ADLENGTH  = (1) + strlen(DEVICE_NAME_STR); //  1 byte data type

    // adv data
    uint8_t adv_data[] =
    {
        0x02, GAP_AD_TYPE_FLAGS, BLE_GAP_FLAGS_LIMITED_DISCOVERABLE_MODE,
        0x03, GAP_AD_TYPE_SERVICE_CPLT_16B_UUID, U16_LOWBYTE(GATT_SERVICES_ELECTRONIC_SHELF_LABEL), U16_HIGHBYTE(GATT_SERVICES_ELECTRONIC_SHELF_LABEL), 
    };

    // scan response data
    uint8_t adv_scan_rsp_data[2 + (strlen(DEVICE_NAME_STR))];
    adv_scan_rsp_data[0] = SCANRSP_ADLENGTH;                      // AD length
    adv_scan_rsp_data[1] = GAP_AD_TYPE_LOCAL_NAME_COMPLETE;       // AD data type
    memcpy(&adv_scan_rsp_data[2], DEVICE_NAME_STR, strlen(DEVICE_NAME_STR)); // Copy the name

    ble_cmd_device_addr_get(&addr_param);
    do
    {
        adv_param.adv_type = ADV_TYPE_ADV_IND;
        adv_param.own_addr_type = addr_param.addr_type;
        adv_param.adv_interval_min = APP_ADV_INTERVAL_MIN;
        adv_param.adv_interval_max = APP_ADV_INTERVAL_MAX;
        adv_param.adv_channel_map = ADV_CHANNEL_ALL;
        adv_param.adv_filter_policy = ADV_FILTER_POLICY_ACCEPT_ALL;

        // set adv parameter
        status = ble_cmd_adv_param_set(&adv_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_adv_param_set() status = %d\n", status);
            break;
        }

        // set adv data
        adv_data_param.length = sizeof(adv_data);
        memcpy(&adv_data_param.data, &adv_data, sizeof(adv_data));
        status = ble_cmd_adv_data_set(&adv_data_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_adv_data_set() status = %d\n", status);
            break;
        }

        // set scan rsp data
        adv_scan_data_param.length = sizeof(adv_scan_rsp_data);
        memcpy(&adv_scan_data_param.data, &adv_scan_rsp_data, sizeof(adv_scan_rsp_data));
        status = ble_cmd_adv_scan_rsp_set(&adv_scan_data_param);
        if (status != BLE_ERR_OK)
        {
            printf("ble_cmd_adv_scan_rsp_set() status = %d\n", status);
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Enables advertising for the specified host ID.
 *
 * This function starts advertising for the specified host ID. It sends a command to the BLE stack
 * to enable advertising and checks the status of the operation.
 *
 * @param host_id The ID of the host for which to enable advertising.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code on failure).
 */
static ble_err_t adv_enable(uint8_t host_id)
{
    ble_err_t status;

    status = ble_cmd_adv_enable(host_id);
    if (status != BLE_ERR_OK)
    {
        printf("adv_enable() status = %d\n", status);
    }

    return status;
}

/**
 * @brief Sets an application request in the application queue.
 *
 * This function adds a request to the application queue for the specified host ID.
 * It can be called from both normal and ISR contexts. The function handles the
 * synchronization of the queue and semaphore appropriately.
 *
 * @param host_id The ID of the host for which to set the request.
 * @param request The application request to set.
 * @param from_isr Indicates whether the function is called from an ISR context (true) or not (false).
 * @return true if the request was successfully set, false otherwise.
 */
static bool app_request_set(uint8_t host_id, app_request_t request, bool from_isr)
{
    app_queue_t p_app_q;

    p_app_q.event = 0; // from BLE
    p_app_q.param_type = QUEUE_TYPE_APP_REQ;
    p_app_q.param.app_req.host_id = host_id;
    p_app_q.param.app_req.app_req = request;

    if (from_isr == false)
    {
        if (xSemaphoreTake(semaphore_app, 0) == pdTRUE)
        {
            p_app_q.from_isr = false;
            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 0) != pdTRUE)
            {
                // send error
                xSemaphoreGive(semaphore_app);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        BaseType_t context_switch = pdFALSE;

        if (xSemaphoreTakeFromISR(semaphore_isr, &context_switch) == pdTRUE)
        {
            p_app_q.from_isr = true;
            context_switch = pdFALSE;
            if (xQueueSendToBackFromISR(g_app_msg_q, &p_app_q, &context_switch) != pdTRUE)
            {
                context_switch = pdFALSE;
                xSemaphoreGiveFromISR(semaphore_isr, &context_switch);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

/* ------------------------------
 *  Application Task
 * ------------------------------
 */
/**
 * @brief Callback function for BLE application events.
 *
 * This function handles general BLE application events and queues them for processing.
 *
 * @param p_param Pointer to the event parameters.
 * @return ble_err_t Returns BLE_ERR_OK on success, BLE_BUSY if the callback or application
 *                    queue is currently full, or BLE_ERR_DATA_MALLOC_FAIL if the TLV buffer
 *                    could not be allocated.
 */
static ble_err_t ble_app_event_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;

    status = BLE_ERR_OK;
    if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
    {
        p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);
        if (p_tlv != NULL)
        {
            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_GENERAL_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_evt_param_t) + ((ble_evt_param_t *)p_param)->extended_length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                vPortFree(p_tlv);
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_ERR_DATA_MALLOC_FAIL;
            xSemaphoreGive(semaphore_cb);
        }
    }
    else
    {
        status = BLE_BUSY;
    }

    return status;
}

/**
 * @brief Callback function for BLE service data events.
 *
 * This function handles service data events and queues them for processing.
 *
 * @param p_param Pointer to the event parameters.
 * @return ble_err_t Returns BLE_ERR_OK on success, BLE_BUSY if the callback or application
 *                    queue is currently full, or BLE_ERR_DATA_MALLOC_FAIL if the TLV buffer
 *                    could not be allocated.
 */
static ble_err_t ble_service_data_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_evt_att_param_t *p_evt_att;

    status = BLE_ERR_OK;
    do {
        if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
        {
            p_evt_att = p_param;
            p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_evt_att_param_t) + p_evt_att->length);
            if (p_tlv == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                xSemaphoreGive(semaphore_cb);
                break;
            }

            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_SERVICE_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_evt_att_param_t) + p_evt_att->length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                vPortFree(p_tlv);
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}

/**
 * @brief Callback function for BLE L2CAP data events.
 *
 * This function handles L2CAP data events and queues them for processing.
 *
 * @param p_param Pointer to the event parameters.
 * @return ble_err_t Returns BLE_ERR_OK on success, BLE_BUSY if the callback or application
 *                    queue is currently full, or BLE_ERR_DATA_MALLOC_FAIL if the TLV buffer
 *                    could not be allocated.
 */
static ble_err_t ble_l2cap_data_cb(void *p_param)
{
    ble_err_t status;
    app_queue_t p_app_q;
    ble_tlv_t *p_tlv;
    ble_l2cap_evt_param_t *p_evt_l2cap;

    status = BLE_ERR_OK;
    do {
        if (xSemaphoreTake(semaphore_cb, 0) == pdTRUE)
        {
            p_evt_l2cap = p_param;
            p_tlv = pvPortMalloc(sizeof(ble_tlv_t) + sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length);
            if (p_tlv == NULL)
            {
                status = BLE_ERR_DATA_MALLOC_FAIL;
                xSemaphoreGive(semaphore_cb);
                break;
            }

            p_app_q.param_type = QUEUE_TYPE_OTHERS;
            p_app_q.param.pt_tlv = p_tlv;
            p_app_q.param.pt_tlv->type = APP_L2CAP_DATA_EVENT;
            memcpy(p_tlv->value, p_param, sizeof(ble_l2cap_evt_param_t) + p_evt_l2cap->length);

            if (xQueueSendToBack(g_app_msg_q, &p_app_q, 1) != pdTRUE)
            {
                status = BLE_BUSY;
                vPortFree(p_tlv);
                xSemaphoreGive(semaphore_cb);
            }
        }
        else
        {
            status = BLE_BUSY;
        }
    } while (0);

    return status;
}


/**
 * @brief Receives and reassembles OTS object data delivered over L2CAP CoC.
 *
 * This function appends incoming L2CAP data into esl_object_info.p_object_data. The first
 * chunk carries a 2-byte length prefix that is compared against the object size negotiated by
 * the preceding OTS Object Action Control Point write request. Once the accumulated size
 * reaches the negotiated size, and it matches the requested object size, the completed image
 * data is handed to @ref esl_epd_image_process() (ESL_IMAGE_DATA_UPDATE) to be written into
 * flash; the object buffer is then freed. If more data arrives than expected, the in-progress
 * transfer is aborted and its buffer/state reset.
 *
 * @param p_data Received L2CAP data chunk.
 * @param len Length of p_data in bytes.
 * @return void This function does not return a value.
 * @note Does nothing if no OTS object transfer is currently in progress
 *       (esl_object_info.p_object_data == NULL).
 */
void ots_data_received(uint8_t *p_data, uint16_t len)
{
    uint16_t l2cap_data_len = 0;

    if (esl_object_info.p_object_data != NULL)
    {
        if ((esl_object_info.current_size + len) <= esl_object_info.object_size)
        {
            if (esl_object_info.current_size == 0)
            {
                l2cap_data_len = ((p_data[0]) | (p_data[1] << 8));
                //printf("l2cap data len %d\n", l2cap_data_len);

                if (l2cap_data_len != esl_object_info.object_size)
                {
                    //Error, TBD
                    printf("Length mismatch with object size %d\n", esl_object_info.object_size);
                }
                else
                {
                    esl_object_info.l2cap_data_size = l2cap_data_len;
                }
                memcpy(esl_object_info.p_object_data, p_data+2, len-2);
                esl_object_info.current_size = (len-2);
            }
            else
            {
                memcpy(esl_object_info.p_object_data + esl_object_info.current_size, p_data, len);
                esl_object_info.current_size += len;
            }
                                
            if (esl_object_info.current_size == esl_object_info.l2cap_data_size)
            {

                if(esl_object_info.l2cap_data_size == esl_object_info.object_size)
                {
                    esl_image_action_t update;

                    update.action = ESL_IMAGE_DATA_UPDATE;
                    update.u.update.image_len = esl_object_info.object_size;
                    update.u.update.p_image_data = esl_object_info.p_object_data;                    
                    esl_epd_image_process(&update);
                }
                vPortFree(esl_object_info.p_object_data);
                esl_object_info.p_object_data = NULL;

            }
        }
        else
        {
            //error, TBD..
            printf("error size %d %d %d\n", esl_object_info.current_size, len, esl_object_info.object_size);
            vPortFree(esl_object_info.p_object_data);
            esl_object_info.p_object_data = NULL;
            esl_object_info.current_size = 0;
            esl_object_info.object_size = 0;
        }
    }
}

/**
 * @brief Processes a queued display request and drives the EPD hardware.
 *
 * This function extracts the image index from the display TLV payload (refresh or change
 * request), reads the corresponding image data for both the black/white and red planes from
 * flash via @ref esl_epd_image_process(), and drives the EPD to display them.
 *
 * @param pt_tlv Display request TLV: type is @ref DISPLAY_REQ_DISPLAY_REFRESH or
 *               @ref DISPLAY_REQ_DISPLAY_CHANGE, value is the corresponding parameter struct.
 * @return void This function does not return a value.
 */
static void process_display(ble_tlv_t *pt_tlv)
{
    esl_image_action_t update;
    uint8_t image_index = 0xFF;
    if (pt_tlv->type == DISPLAY_REQ_DISPLAY_REFRESH)
    {
        display_req_display_refresh_param_t *p_refresh_param;

        p_refresh_param = (display_req_display_refresh_param_t *)pt_tlv->value;
        image_index = p_refresh_param->image_index;
    }
    else if (pt_tlv->type == DISPLAY_REQ_DISPLAY_CHANGE)
    {
        display_req_display_change_param_t *p_change_param;
        
        p_change_param = (display_req_display_change_param_t *)pt_tlv->value;
        image_index = p_change_param->image_index;
    }
        
    if (image_index != 0xFF)
    {
        update.action = ESL_IMAGE_DATA_DISPLAY;
        update.u.display.image_index = image_index;
        update.u.display.part = 0;
        update.u.display.image_len = 0;

        esl_epd_image_process(&update);

        if (update.u.display.image_len > 0)
        {
            epd_io_init();
            epd_reset_and_init();
            epd_display(LOAD_IMAGE_BW, update.u.display.p_image_data, update.u.display.image_len);
            vPortFree(update.u.display.p_image_data);
        }

        update.action = ESL_IMAGE_DATA_DISPLAY;
        update.u.display.image_index = image_index;
        update.u.display.part = 1;
        update.u.display.image_len = 0;

        esl_epd_image_process(&update);

        if (update.u.display.image_len > 0)
        {
            epd_display(LOAD_IMAGE_RED, update.u.display.p_image_data, update.u.display.image_len);
            epd_refresh();
            epd_sleep();
            vPortFree(update.u.display.p_image_data);
        }
    }
}

/**
 * @brief Main task for the BLE application.
 *
 * This function initializes the BLE stack, starts advertising, and processes incoming
 * events from the application queue. It dispatches application requests (advertising start,
 * FOTA timer expiry, sync terminate), queued display requests, and general BLE/service/L2CAP
 * events to their respective handlers.
 *
 * @return void This function does not return a value.
 * @note Runs an infinite loop and never returns; if BLE stack initialization fails, it
 *       instead blocks forever in a `while (1);` loop.
 */
static void app_main_task(void)
{
    ble_err_t status;
    app_queue_t p_app_q;

    status = BLE_ERR_OK;

    // BLE default setting and profile init
    status = ble_init();
    if (status != BLE_ERR_OK)
    {
        printf("[DEBUG_ERR] ble_init() fail: %d\n", status);
        while (1);
    }

    // start adv
    if (app_request_set(APP_ESL_P_HOST_ID, APP_REQUEST_ADV_START, false) == false)
    {
        // No Application queue buffer. Error.
    }

    for (;;)
    {
        g_app_rand_value++;
        if (xQueueReceive(g_app_msg_q, &p_app_q, portMAX_DELAY) == pdTRUE)
        {
            switch (p_app_q.param_type)
            {
            case QUEUE_TYPE_APP_REQ:
            {
                if (p_app_q.from_isr == true)
                {
                    xSemaphoreGive(semaphore_isr);
                }
                else
                {
                    xSemaphoreGive(semaphore_app);
                }
                ble_app_main(&p_app_q.param.app_req);
            }
            break;

            case QUEUE_TYPE_DISPLAY_REQ:
            {
                process_display(p_app_q.param.pt_tlv);
                vPortFree(p_app_q.param.pt_tlv);
                xSemaphoreGive(semaphore_display);
            }
            break;

            case QUEUE_TYPE_OTHERS:
            {
                if (p_app_q.param.pt_tlv != NULL)
                {
                    switch (p_app_q.param.pt_tlv->type)
                    {
                    case APP_GENERAL_EVENT:
                        ble_evt_handler((ble_evt_param_t *)p_app_q.param.pt_tlv->value);
                        break;

                    case APP_SERVICE_EVENT:
                    {
                        ble_evt_att_param_t *p_svcs_param = (ble_evt_att_param_t *)p_app_q.param.pt_tlv->value;
                        
                        switch (p_svcs_param->gatt_role)
                        {
                        case BLE_GATT_ROLE_CLIENT:
                            att_db_link[p_svcs_param->host_id].p_client_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                            break;

                        case BLE_GATT_ROLE_SERVER:
                            att_db_link[p_svcs_param->host_id].p_server_db[p_svcs_param->cb_index]->att_handler(p_svcs_param);
                            break;

                        default:
                            break;
                        }
                    }
                    break;

                    case APP_L2CAP_DATA_EVENT:
                    {
                        ble_l2cap_evt_param_t *p_l2cap_event = (ble_l2cap_evt_param_t*)(p_app_q.param.pt_tlv->value);
                
                        switch (p_l2cap_event->event)
                        {
                            case LE_L2CAP_EVT_CREDIT_BASED_CONNECT_REQ_RSP:
                                printf("l2cap conn request, host_id %d result %d, dest_id %d\n", p_l2cap_event->evt_param.l2cap_conn_req_rsp.host_id,
                                                   p_l2cap_event->evt_param.l2cap_conn_req_rsp.result, 
                                                   p_l2cap_event->evt_param.l2cap_conn_req_rsp.dest_id);

                            break;

                            case LE_L2CAP_EVT_DATA_RECEIVED:
                            {
                                ble_l2cap_data_param_t *p_l2cap_data;

                                p_l2cap_data = (ble_l2cap_data_param_t *)&p_l2cap_event->evt_param.l2cap_data_param;

                                //printf("APP GET L2CAP DATA, hostID %d, psm 0x%04x\n", p_l2cap_data->host_id, p_l2cap_data->psm);

                                if (p_l2cap_data->psm == BLESERVICE_OTS_PSM)
                                {
                                    ots_data_received(p_l2cap_data->data, p_l2cap_data->length);
                                }
                            }
                            break;
                        }
                    }
                    break;

                    default:
                        break;
                    }

                    // free
                    vPortFree(p_app_q.param.pt_tlv);
                    xSemaphoreGive(semaphore_cb);
                }
            }
            break;

            default:
                break;
            }
        }
    }
}

/**
 * @brief Main application handler for BLE requests.
 *
 * This function forwards the application request to the peripheral link handler
 * (@ref app_peripheral_handler).
 *
 * @param p_param Pointer to the request parameters.
 * @return void This function does not return a value.
 */
static void ble_app_main(app_req_param_t *p_param)
{
    // Link - Peripheral
    app_peripheral_handler(p_param);
}

/* ------------------------------
 *  Application Initializations
 * ------------------------------
 */
/**
 * @brief Initializes the GATT service data.
 *
 * This function initializes the GATT service data structure for the peripheral role.
 * It sets the service changed characteristic client configuration descriptor (CCCD) to 0.
 *
 * @param p_data Pointer to the GATT service data structure.
 * @return void This function does not return a value.
 */
static void svcs_gatts_data_init(ble_svcs_gatts_data_t *p_data)
{
    p_data->service_changed_cccd = 0;
}

/**
 * @brief GATT event handler for the ESL Service (ESLS).
 *
 * This function handles server-side ESLS characteristic write/read events: it stores the ESL
 * address (esl_id/group_id) and, if already synchronized, updates the periodic sync subevent;
 * stores the AP sync key and ESL response key byte-reversed via @ref _memcpy_inv (their
 * accompanying nonces are copied as-is); stores the current absolute time; responds to reads of the display,
 * image, sensor, and LED information characteristics with the static hardware info
 * (esl_hw_info); and parses ESL control point writes via @ref app_esl_cmd_parsing, sending any
 * generated response as a GATT notification.
 *
 * @param p_param BLE ATT/GATT event parameters (event code, host id, handle, and data).
 * @return void This function does not return a value.
 * @note Only server-role events (p_param->gatt_role == BLE_GATT_ROLE_SERVER) are processed.
 */
static void ble_svcs_esls_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        ble_gatt_data_param_t param;

        switch (p_param->event)
        {
            case BLESERVICE_ESLS_ESL_ADDRESS_WRITE_EVENT:
            {
                esl_ctrl_info.esl_id = p_param->data[0];
                esl_ctrl_info.group_id = p_param->data[1];
                printf("esl_id %d group_id %d\n", esl_ctrl_info.esl_id, esl_ctrl_info.group_id);

                if (esl_ctrl_info.synchronized == 1)
                {
                    ble_periodic_sync_subevent_param_t sync_subevent;
                    ble_err_t err;
                    sync_subevent.sync_handle = esl_ctrl_info.sync_handle;
                    sync_subevent.subevent = esl_ctrl_info.group_id;

                    err = ble_cmd_periodic_sync_subevent_set(&sync_subevent);
                    //printf("sync_subevent %d err %d\n", sync_subevent.subevent, err);
                }
            }
            break;

            case BLESERVICE_ESLS_AP_SYNC_KEY_MATERIAL_WRITE_EVENT:
            {
                _memcpy_inv(esl_ctrl_info.ap_sync_key, p_param->data, 16);
                memcpy(esl_ctrl_info.ap_sync_nonce, p_param->data+16, 8);
            }
            break;

            case BLESERVICE_ESLS_ESL_RSP_KEY_MATERIAL_WRITE_EVENT:
                _memcpy_inv(esl_ctrl_info.esl_rsp_key, p_param->data, 16);
                memcpy(esl_ctrl_info.esl_rsp_nonce, p_param->data+16, 8);
            break;

            case BLESERVICE_ESLS_ESL_CURRENT_ABSOLUTE_TIME_WRITE_EVENT:
            {
                memcpy((uint8_t *)&esl_ctrl_info.current_time, p_param->data, sizeof(esl_ctrl_info.current_time));
                printf("absolute time %d\n", esl_ctrl_info.current_time);
                g_absolute_time = esl_ctrl_info.current_time;
            }
            break;

            case BLESERVICE_ESLS_ESL_DISPLAY_INFORMATION_READ_EVENT:
            {
                param.host_id = p_param->host_id;
                param.handle_num = p_param->handle_num;
                param.length = sizeof(esl_hw_info.display_info);
                param.p_data = (uint8_t*)&esl_hw_info.display_info;

                ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &param);
            }
            break;

            case BLESERVICE_ESLS_ESL_IMAGE_INFORMATION_READ_EVENT:
            {
                param.host_id = p_param->host_id;
                param.handle_num = p_param->handle_num;
                param.length = sizeof(esl_hw_info.image_info);
                param.p_data = (uint8_t*)&esl_hw_info.image_info;

                ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &param);
            }
            break;

            case BLESERVICE_ESLS_ESL_SENSOR_INFORMATION_READ_EVENT:
            {
                uint8_t i;

                param.host_id = p_param->host_id;
                param.handle_num = p_param->handle_num;
                param.length = 0;
                param.p_data = pvPortMalloc(sizeof(esl_hw_info.sensor_info));

                if (param.p_data != NULL)
                {
                    for (i = 0; i < ESL_SENSOR_NUM; i++)
                    {
                        param.p_data[param.length++] = esl_hw_info.sensor_info[i].size;

                        if (esl_hw_info.sensor_info[i].size == 0x00)
                        {
                            param.p_data[param.length++] = ((esl_hw_info.sensor_info[i].sensor_type.stanard) & 0xFF);
                            param.p_data[param.length++] = ((esl_hw_info.sensor_info[i].sensor_type.stanard >> 8) & 0xFF);
                        }
                        else
                        {
                            param.p_data[param.length++] = ((esl_hw_info.sensor_info[i].sensor_type.vendor_specific) & 0xFF) ;
                            param.p_data[param.length++] = ((esl_hw_info.sensor_info[i].sensor_type.vendor_specific >> 8) & 0xFF);
                            param.p_data[param.length++] = ((esl_hw_info.sensor_info[i].sensor_type.vendor_specific >> 16) & 0xFF);
                            param.p_data[param.length++] = ((esl_hw_info.sensor_info[i].sensor_type.vendor_specific >> 24) & 0xFF);
                        }
                    }
                    ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &param);
                    vPortFree(param.p_data);
                }
            }
            break;

            case BLESERVICE_ESLS_ESL_LED_INFORMATION_READ_EVENT:
            {
                param.host_id = p_param->host_id;
                param.handle_num = p_param->handle_num;
                param.length = sizeof(esl_hw_info.led_info);
                param.p_data = (uint8_t*)&esl_hw_info.led_info;

                ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &param);
            }
            break;

            case BLESERVICE_ESLS_ESL_CONTROL_POINT_WRITE_EVENT:
            case BLESERVICE_ESLS_ESL_CONTROL_POINT_WRITE_WITHOUT_RSP_EVENT:
            {
                uint8_t *p_rsp_data, rsp_data_len;
                ble_esl_control_point_cmd_t *p_ctrl_point_cmd;

                p_ctrl_point_cmd = (ble_esl_control_point_cmd_t *)(p_param->data);

                //printf("ctrl point opcode 0x%02x\n", p_ctrl_point_cmd->cmd_header.opcode);
                app_esl_cmd_parsing(p_ctrl_point_cmd, &p_rsp_data, &rsp_data_len);
                if ((rsp_data_len > 0) && (p_rsp_data != NULL))
                {
                    param.host_id = p_param->host_id;
                    param.handle_num = p_param->handle_num;
                    param.length = rsp_data_len;
                    param.p_data = p_rsp_data;
                    ble_svcs_data_send(TYPE_BLE_GATT_NOTIFICATION, &param);
                    vPortFree(p_rsp_data);
                }
            }
            break;

            default:
            break;
        }
    }
}

/**
 * @brief GATT event handler for the Object Transfer Service (OTS).
 *
 * This function handles server-side OTS characteristic events. Most read/write events for
 * object metadata characteristics are unhandled placeholders in this demo except:
 * - OBJECT_TYPE_READ, which responds with a fixed 16-byte object type UUID.
 * - OBJECT_ACTION_CONTROL_POINT_WRITE, which processes an OACP "Write" request by allocating
 *   a receive buffer for the object (esl_object_info) sized to the requested length, then
 *   indicates the result (success or insufficient resources); other opcodes are indicated as
 *   not supported.
 * - OBJECT_LIST_CONTROL_POINT_WRITE, which processes an OLCP "Go To" request: if the object ID
 *   encodes an image index/part, it erases the corresponding EPD image flash sector via
 *   @ref esl_epd_image_process(); other opcodes are indicated as not supported.
 * The actual object data is received separately over L2CAP (see @ref ots_data_received).
 *
 * @param p_param BLE ATT/GATT event parameters (event code, host id, handle, and data).
 * @return void This function does not return a value.
 * @note Only server-role events (p_param->gatt_role == BLE_GATT_ROLE_SERVER) are processed.
 */
static void ble_svcs_ots_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        switch (p_param->event)
        {
            case BLESERVICE_OTS_OTS_FEATURE_READ_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_NAME_READ_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_NAME_WRITE_EVENT:
                printf("Object name write\n");
            break;

            case BLESERVICE_OTS_OBJECT_TYPE_READ_EVENT:
            {
                 ble_gatt_data_param_t param;

                param.host_id = p_param->host_id;
                param.handle_num = p_param->handle_num;
                param.length = 16;
                param.p_data = pvPortMalloc(param.length);
                if (param.p_data != NULL)
                {
                    param.p_data[0] = 0xE5; //UUID
                    param.p_data[1] = 0x10;
                    param.p_data[2] = 0x00;
                    param.p_data[3] = 0x01;
                    param.p_data[4] = 0x4B;
                    param.p_data[5] = 0x1D;
                    param.p_data[6] = 0xB1;
                    param.p_data[7] = 0x9B;
                    param.p_data[8] = 0x00;
                    param.p_data[9] = 0xB1;
                    param.p_data[10] = 0xE5;
                    param.p_data[11] = 0x01;
                    param.p_data[12] = 0x8B;
                    param.p_data[13] = 0xAD;
                    param.p_data[14] = 0xF0;
                    param.p_data[15] = 0x0D;
                    
                    ble_svcs_data_send(TYPE_BLE_GATT_READ_RSP, &param);
                    vPortFree(param.p_data);
                }
            }
            break;

            case BLESERVICE_OTS_OBJECT_SIZE_READ_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_FIRST_CREATED_READ_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_FIRST_CREATED_WRITE_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_LAST_MODIFIED_READ_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_LAST_MODIFIED_WRITE_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_ID_READ_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_PROPERTIES_READ_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_PROPERTIES_WRITE_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_WRITE_EVENT:
            {
                ble_gatt_data_param_t param;
                ots_oacp_t *p_ocap = (ots_oacp_t*)p_param->data;
                ots_oacp_response_t ocap_rsp;

                printf("Object action control point\n");
                for(uint8_t i = 0 ; i < p_param->length; i++)
                {
                    printf("%02x ", p_param->data[i]);
                }
                printf("\n");

                ocap_rsp.op_code = OACP_OPCODE_RESPONSE; //Response Code of Object Action Control Point
                ocap_rsp.req_op_code = p_ocap->opcode;
                switch (p_ocap->opcode)
                {
                    case OACP_OPCODE_WRITE:
                    {
                        printf("write offset %d, len %d\n", p_ocap->param.write.offset, p_ocap->param.write.length);
                        if (p_ocap->param.write.offset > 0) 
                        {
                           //TBD...this demo was not process offset field
                        }
            
                        if (esl_object_info.p_object_data != NULL)
                        {
                            //error
                            vPortFree(esl_object_info.p_object_data);
                            esl_object_info.p_object_data = NULL;
                        }

                        esl_object_info.p_object_data = pvPortMalloc(p_ocap->param.write.length);
                        if (esl_object_info.p_object_data == NULL)
                        {
                            ocap_rsp.result_code = OACP_RESULT_CODE_INSUFFICIENT_RESOURCES;
                        }
                        else
                        {
                            esl_object_info.object_size = p_ocap->param.write.length;
                            esl_object_info.current_size = 0;
                            ocap_rsp.result_code = OACP_RESULT_CODE_SUCCESS;
                        }
                    }
                    break;

                    default:
                    {
                        ocap_rsp.result_code = OACP_RESULT_CODE_OPCODE_NOT_SUPPORT;
                    }
                    break;
                }
                param.host_id = p_param->host_id;
                param.handle_num = p_param->handle_num;
                param.length = sizeof(ots_oacp_response_t);
                param.p_data = (uint8_t *)&ocap_rsp;
                ble_svcs_data_send(TYPE_BLE_GATT_INDICATION, &param);
            }
            break;

            case BLESERVICE_OTS_OBJECT_ACTION_CONTROL_POINT_INDICATE_CONFIRM_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_WRITE_EVENT:
            {
                ble_gatt_data_param_t param;
                ots_olcp_t *p_olap = (ots_olcp_t*)p_param->data;
                ots_olcp_response_t  olcp_rsp;

                olcp_rsp.op_code = OLCP_OPCODE_RESPONSE; //Response Code of Control Point
                olcp_rsp.req_op_code = p_olap->opcode;
                switch (p_olap->opcode)
                {
                    case OLCP_OPCODE_GO_TO:
                    {
                        ble_uint48_t object_id;

                        memcpy((uint8_t *)&object_id, p_olap->param.go_to.object_id, sizeof(ble_uint48_t));
                                     
                        //printf("OLCP GoTo, object_id 0x%04x%08x\n", object_id.high, object_id.low);

                        if ((object_id.high == 0) && (object_id.low == 0))
                        {
                            printf("Directory list object\n"); 
                        }
                        else if  ((object_id.high == 0) && (object_id.low > 0x00) && (object_id.low < 0x100))
                        {
                            printf("Reserved object ID\n"); 
                        }
                        else
                        {
                            esl_image_action_t update;
                            //printf("Allocated object ID\n"); 
                            
                            update.action = ESL_IMAGE_DATA_ERASE;
                            update.u.erase.image_index = p_olap->param.go_to.object_id[0];
                            update.u.erase.part = p_olap->param.go_to.object_id[1];
                            esl_epd_image_process(&update);
                            
                        }
                        olcp_rsp.result_code = OLCP_RESULT_CODE_SUCCESS;
                    }   
                    break;

                    default:
                        olcp_rsp.result_code = OLCP_RESULT_CODE_OPCODE_NOT_SUPPORT;
                    break;
                }
                param.host_id = p_param->host_id;
                param.handle_num = p_param->handle_num;
                param.length = sizeof(ots_olcp_response_t);
                param.p_data = (uint8_t *)&olcp_rsp;
                ble_svcs_data_send(TYPE_BLE_GATT_INDICATION, &param); 
            }
            break;

            case BLESERVICE_OTS_OBJECT_LIST_CONTROL_POINT_INDICATE_CONFIRM_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_LIST_FILTER_READ_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_LIST_FILTER_WRITE_EVENT:
            break;

            case BLESERVICE_OTS_OBJECT_CHANGED_INDICATE_CONFIRM_EVENT:
            break;
 
            default:
            break;
        }
    }
}

/**
 * @brief GATT event handler for the FOTA Service (FOTAS).
 *
 * This function forwards incoming FOTA data and command writes to the FOTA module
 * (ble_fota_data() / ble_fota_cmd()) and (re)starts the FOTA software timer so pending FOTA
 * processing continues to be ticked.
 *
 * @param p_param BLE ATT/GATT event parameters (event code, host id, length, and data).
 * @return void This function does not return a value.
 * @note Only server-role events (p_param->gatt_role == BLE_GATT_ROLE_SERVER) are processed.
 */
static void ble_svcs_fotas_evt_handler(ble_evt_att_param_t *p_param)
{
    if (p_param->gatt_role == BLE_GATT_ROLE_SERVER)
    {
        /* ----------------- Handle event from client ----------------- */
        switch (p_param->event)
        {
        case BLESERVICE_FOTAS_DATA_WRITE_WITHOUT_RSP_EVENT:
        {
            ble_fota_data(p_param->host_id, p_param->length, p_param->data);
            fota_sw_timer_start();
        }
        break;

        case BLESERVICE_FOTAS_COMMAND_WRITE_EVENT:
        {
            ble_fota_cmd(p_param->host_id, p_param->length, p_param->data);
            fota_sw_timer_start();
        }

        default:
            break;
        }
    }
}


/**
 * @brief Initializes the server profile for the specified host ID.
 *
 * This function sets the link's state (ble_app_link_info[host_id].state) to STATE_STANDBY and
 * initializes the server profile for the specified host ID, setting up the GAP, GATT, DIS,
 * ESLS, OTS, and FOTAS services.
 *
 * @param host_id The ID of the host for which to initialize the server profile.
 * @return ble_err_t Status of the operation (BLE_ERR_OK on success, error code on failure).
 */
static ble_err_t server_profile_init(uint8_t host_id)
{
    ble_err_t status = BLE_ERR_OK;
    ble_info_link0_t *p_profile_info = (ble_info_link0_t *)ble_app_link_info[host_id].profile_info;

    // set link's state
    ble_app_link_info[host_id].state = STATE_STANDBY;

    do
    {
        // GAP Related
        // -------------------------------------
        status = ble_svcs_gaps_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_gaps), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // set GAP device name
        status = ble_svcs_gaps_device_name_set((uint8_t *)DEVICE_NAME_STR, strlen(DEVICE_NAME_STR));
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // GATT Related
        // -------------------------------------
        status = ble_svcs_gatts_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_gatts), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // DIS Related
        // -------------------------------------
        status = ble_svcs_dis_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_dis), NULL);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // ESL Related
        // -------------------------------------
        status = ble_svcs_esls_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_esls), ble_svcs_esls_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // OTS Related
        // -------------------------------------
        status = ble_svcs_ots_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_ots), ble_svcs_ots_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        // FOTAS Related
        // -------------------------------------
        status = ble_svcs_fotas_init(host_id, BLE_GATT_ROLE_SERVER, &(p_profile_info->svcs_info_fotas), ble_svcs_fotas_evt_handler);
        if (status != BLE_ERR_OK)
        {
            break;
        }
    } while (0);

    return status;
}

/**
 * @brief Initializes the BLE stack and sets up callbacks.
 *
 * This function initializes the BLE stack, sets up necessary callbacks, and configures the
 * device address (from the unique code area if available, otherwise derived from the flash
 * unique ID). It also initializes the PHY controller, the resolvable address, the suggested
 * data length, the default periodic advertising sync transfer parameters, the server profile,
 * and the FOTA module.
 *
 * @return ble_err_t Returns BLE_ERR_OK on success, or an error code on failure.
 */
static ble_err_t ble_init(void)
{
    ble_err_t status;
    ble_unique_code_format_t unique_code_param;
    ble_gap_addr_t device_addr_param;
    ble_default_periodic_adv_sync_transfer_param_t default_param;
    uint8_t flash_id[7];
    status = BLE_ERR_OK;
    do
    {
        status = ble_host_callback_set(APP_GENERAL_EVENT, ble_app_event_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_host_callback_set(APP_SERVICE_EVENT, ble_service_data_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_host_callback_set(APP_L2CAP_DATA_EVENT, ble_l2cap_data_cb);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_cmd_phy_controller_init();
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_cmd_read_unique_code(&unique_code_param);
        if (status == BLE_ERR_OK)
        {
            device_addr_param.addr_type = unique_code_param.addr_type;
            memcpy(&device_addr_param.addr, &unique_code_param.ble_addr, 6);
            status = ble_cmd_device_addr_set((ble_gap_addr_t *)&device_addr_param);
            if (status != BLE_ERR_OK)
            {
                break;
            }
            status = ble_cmd_write_identity_resolving_key((ble_sm_irk_param_t *)&unique_code_param.ble_irk[0]);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }
        else
        {
            flash_get_unique_id((uint32_t)flash_id, sizeof(flash_id));
            DEVICE_ADDR.addr[0] = flash_id[2];
            DEVICE_ADDR.addr[1] = flash_id[3];
            DEVICE_ADDR.addr[2] = flash_id[4];
            DEVICE_ADDR.addr[3] = flash_id[5];
            DEVICE_ADDR.addr[4] = flash_id[6];
            printf("ADDRESS 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", DEVICE_ADDR.addr[5],
                DEVICE_ADDR.addr[4], DEVICE_ADDR.addr[3], DEVICE_ADDR.addr[2], DEVICE_ADDR.addr[1], DEVICE_ADDR.addr[0]);
            status = ble_cmd_device_addr_set((ble_gap_addr_t *)&DEVICE_ADDR);
            if (status != BLE_ERR_OK)
            {
                break;
            }
        }

        status = ble_cmd_resolvable_address_init();
        if (status != BLE_ERR_OK)
        {
            break;
        }

        status = ble_cmd_suggest_data_len_set(BLE_GATT_DATA_LENGTH_MAX);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        default_param.mode = PERIODIC_ADV_EVENT_WITH_DUPLICATE_FILTER;
        default_param.skip = 100;
        default_param.sync_timeout = PADV_SYNC_TIMEOUT;
        status = ble_cmd_default_periodic_adv_sync_transfer_param_set(&default_param);
        if (status != BLE_ERR_OK)
        {
            break;
        }
        // BLE profile init --> only 1 link (host id = 0)
        status = server_profile_init(APP_ESL_P_HOST_ID);
        if (status != BLE_ERR_OK)
        {
            break;
        }

        ble_fota_fw_buffer_flash_check();
        ble_fota_init();
    } while (0);

    return status;
}

/**
 * @brief Periodic timer handler that advances the absolute time counter and blinks the LED.
 *
 * This function is invoked each time the absolute-time timer fires. It advances
 * g_absolute_time by the timer's tick amount, and while the ESL is not synchronized to an AP
 * (esl_ctrl_info.synchronized == 0) it also toggles the status LED on/off to indicate the
 * standby state.
 *
 * @param timer The handle of the expired timer (unused).
 * @return void This function does not return a value.
 */
static void absolute_time_timer_handler(TimerHandle_t timer)
{
    g_absolute_time += 1000;

    if (esl_ctrl_info.synchronized == 0)
    {
        esl_ctrl_info.active_led ^= 1;
        if (esl_ctrl_info.active_led == 1)
        {
            led_on();//led on
        }
        else
        {
            led_off();//led off
        }
    }
}

/**
 * @brief Initializes the application.
 *
 * This function sets up the application by creating the application message queue and
 * semaphores, initializing the BLE stack, configuring the wake-up GPIO interrupt, and
 * creating the absolute-time and FOTA software timers. It also configures and turns off the
 * ESL status LED.
 *
 * @return void This function does not return a value.
 */
static void app_init(void)
{
    ble_task_priority_t ble_task_level;
    hosal_gpio_input_config_t input_cfg;

    // banner
    printf("------------------------------------------\n");
    printf("  BLE ESL (P) demo: start...\n");
    printf("------------------------------------------\n");

    // application queue & semaphore
    g_app_msg_q = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_t));
    semaphore_cb = xSemaphoreCreateCounting(BLE_APP_CB_QUEUE_SIZE, BLE_APP_CB_QUEUE_SIZE);
    semaphore_isr = xSemaphoreCreateCounting(APP_ISR_QUEUE_SIZE, APP_ISR_QUEUE_SIZE);
    semaphore_app = xSemaphoreCreateCounting(APP_REQ_QUEUE_SIZE, APP_REQ_QUEUE_SIZE);
    semaphore_display = xSemaphoreCreateCounting(DISPLAY_QUEUE_SIZE, DISPLAY_QUEUE_SIZE);

    // BLE Stack init  
    ble_task_level.ble_host_level = configMAX_PRIORITIES - 7;
    ble_task_level.hci_tx_level = configMAX_PRIORITIES - 6;
    if (ble_host_stack_init(&ble_task_level) == 0) {
        printf("BLE stack initial success...\n");
    }
    else {
        printf("BLE stack initial fail...\n");
    }

    // wake up pin
    input_cfg.pin_int_mode = HOSAL_GPIO_PIN_INT_EDGE_FALLING;
    input_cfg.usr_cb = app_gpio_handler;
    input_cfg.param = NULL;
    hosal_gpio_cfg_input(GPIO_WAKE_UP_PIN, input_cfg);
    hosal_gpio_debounce_enable(GPIO_WAKE_UP_PIN);
    hosal_gpio_int_enable(GPIO_WAKE_UP_PIN);
    NVIC_EnableIRQ(Gpio_IRQn);
    hosal_lpm_ioctrl(HOSAL_LPM_ENABLE_WAKE_UP_SOURCE, HOSAL_LOW_POWER_WAKEUP_GPIO);

    g_absolute_time_timer = xTimerCreate("absolute_time", pdMS_TO_TICKS(10*1000), pdTRUE, (void*)0, absolute_time_timer_handler);
    xTimerStart(g_absolute_time_timer, 0);

    g_fota_timer = xTimerCreate("FOTA_Timer", pdMS_TO_TICKS(1000), pdTRUE, ( void * ) 0, fota_timer_handler);
    hosal_pin_set_mode(ESL_LED_PIN, HOSAL_MODE_GPIO);
    hosal_gpio_cfg_output(ESL_LED_PIN);
    led_off(); //esl_ctrl_info.active_led = 0;
}

/**
 * @brief Initializes the pin multiplexing.
 *
 * This function sets all GPIO pins to GPIO mode, except for GPIO16 and GPIO17,
 * which are reserved for specific functions.
 *
 * @return void This function does not return a value.
 */
static void pin_mux_init(void) 
{
    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (int i = 0; i < 32; i++) {
        if (i == 16 || i == 17) {
            continue; // Skip GPIO16 and GPIO17
        }
        hosal_pin_set_mode(i, HOSAL_MODE_GPIO);
        hosal_pin_set_pullopt(i, PULLUP_100K);
    }
}

/**
 * @brief Main entry point for the application.
 *
 * This function initializes the RF module, starts the application initialization,
 * and enters an infinite loop to keep the application running.
 *
 * @param pvParameters Pointer to parameters passed to the task (not used).
 * @return void This function does not return a value.
 */
static void app_main_entry(void* pvParameters)
{
    hosal_lpm_init();
    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LOW_POWER_LEVEL_SLEEP0);
    hosal_rf_init(HOSAL_RF_MODE_BLE_CONTROLLER);

    /* application init */
    app_init();
    app_main_task();

    while (1) {
    }
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
/**
 * @brief Application entry point.
 *
 * Initializes hardware, configures peripherals, and starts the main application task.
 *
 * @return int Not used.
 */
 int main(void)
{
    pin_mux_init();
    uart_stdio_init();
    vHeapRegionsInt();
    _dump_boot_info();

    if (xTaskCreate(app_main_entry, (char*)"main",
                    CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE, NULL,
                    E_TASK_PRIORITY_APP, NULL)
        != pdPASS) {
        puts("Task create fail....\r\n");
    }
    puts("[OS] Starting OS Scheduler...\r\n");
    puts("\r\n");
    vTaskStartScheduler();
    while (1) {};

    return 0;
}
