/**
 * \file            hosal_sadc.h
 * \brief           Hosal SADC driver header file
 *          
 */
/*
 * Copyright (c) year FirstName LASTNAME
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Author:          Kc.tseng
 */

#ifndef HOSAL_SADC_H
#define HOSAL_SADC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "sadc.h"

/**
 * \brief           hosal sadc value
 */
typedef int32_t hosal_sadc_value_t;

/**
 * \brief           hosal sadc convert status types
 */
typedef enum {
    HOSAL_SADC_CONVERT_IDLE = 0,
    HOSAL_SADC_CONVERT_START,
    HOSAL_SADC_CONVERT_DONE,
} hosal_sadc_convert_state_t;

/**
 * \brief           hosal sadc resolution bit setting
 */
typedef enum {
    HOSAL_SADC_RES_8BIT = 0,                    /*!< 8 bit resolution. */
    HOSAL_SADC_RES_10BIT = 1,                   /*!< 10 bit resolution. */
    HOSAL_SADC_RES_12BIT = 2,                   /*!< 12 bit resolution. */
    HOSAL_SADC_RES_14BIT = 3,                   /*!< 14 bit resolution. */
} hosal_sadc_config_resolution_t;

/**
 * \brief           hosal sadc oversampleing rate setting
 */
typedef enum {
    HOSAL_SADC_OVERSAMPLE_0 = 0,                /*!< No oversampling*/
    HOSAL_SADC_OVERSAMPLE_2 = 1,                /*!< Oversampling ratio multiple 2*/
    HOSAL_SADC_OVERSAMPLE_4 = 2,                /*!< Oversampling ratio multiple 4*/
    HOSAL_SADC_OVERSAMPLE_8 = 3,                /*!< Oversampling ratio multiple 8*/
    HOSAL_SADC_OVERSAMPLE_16 = 4,               /*!< Oversampling ratio multiple 16*/
    HOSAL_SADC_OVERSAMPLE_32 = 5,               /*!< Oversampling ratio multiple 32*/
    HOSAL_SADC_OVERSAMPLE_64 = 6,               /*!< Oversampling ratio multiple 64*/
    HOSAL_SADC_OVERSAMPLE_128 = 7,              /*!< Oversampling ratio multiple 128*/
    HOSAL_SADC_OVERSAMPLE_256 = 8,              /*!< Oversampling ratio multiple 256*/
} hosal_sadc_config_oversample_t;

/**
 * \brief           hosal sadc channel input selection of the analog-to-digital converter.
 */
typedef enum {
    HOSAL_SADC_CH_AIN0 = 0,                     /*!< Input AIO0. */
    HOSAL_SADC_CH_AIN1 = 1,                     /*!< Input AIO1. */
    HOSAL_SADC_CH_AIN2 = 2,                     /*!< Input AIO2. */
    HOSAL_SADC_CH_AIN3 = 3,                     /*!< Input AIO3. */
    HOSAL_SADC_CH_AIN4 = 4,                     /*!< Input AIO4. */
    HOSAL_SADC_CH_AIN5 = 5,                     /*!< Input AIO5. */
    HOSAL_SADC_CH_AIN6 = 6,                     /*!< Input AIO6. */
    HOSAL_SADC_CH_AIN7 = 7,                     /*!< Input AIO7. */
    HOSAL_SADC_CH_VBAT = 8,                     /*!< Input VBAT. */
    HOSAL_SADC_CH_NC = 13,                      /*!< Input No Connect. */
    HOSAL_SADC_COMP_VBAT = 9,                   /*!< Input VBAT. */
    HOSAL_SADC_COMP_TEMPERATURE = 10,           /*!< Input TEMPERATURE. */
    HOSAL_SADC_COMP_VCM = 11,                   /*!< Input VCM. */
    HOSAL_SADC_0VADC = 12,                      /*!< Input VBAT 0V. */
} hosal_sadc_input_ch_t;

/**
 * \brief           hosal sadc callback types
 */
typedef enum {
    HOSAL_SADC_CB_DONE,                         /*!< CB generated when the buffer is filled with samples. */
    HOSAL_SADC_CB_SAMPLE,                       /*!< CB generated when the requested channel is sampled. */
} hosal_sadc_cb_type_t;

/**
 * \brief           hosal sadc analog-to-digital converter driver DONE cb
 */
typedef struct {
    hosal_sadc_value_t* p_buffer;               /*!< Pointer to buffer with converted samples. */
    uint16_t size;                              /*!< Number of samples in the buffer. */
} hosal_sadc_done_cb_t;

/**
 * \brief           hosal sadc analog-to-digital converter driver raw SAMPLE cb
 */
typedef struct {
    hosal_sadc_value_t conversion_value;        /*!< Converted sample. */
    hosal_sadc_value_t compensation_value;      /*!< Compensation sample. */
    hosal_sadc_value_t calibration_value;       /*!< Calibration sample. */
} hosal_sadc_raw_cb_t;

/**
 * \brief           hosal sadc analog-to-digital converter driver SAMPLE cb
 */
typedef struct {
    sadc_value_t value;                         /*!< Converted sample. */
    uint32_t channel;                           /*!< Converted channel. */
} hosal_sadc_sample_cb_t;

/**
 * \brief           hosal sadc analog-to-digital converter driver cb
 */
typedef struct {
    hosal_sadc_cb_type_t type; /**< CB type. */
    hosal_sadc_raw_cb_t raw;

    union {
        hosal_sadc_done_cb_t done;               /*!< Data for DONE cb. */
        hosal_sadc_sample_cb_t sample;           /*!< Data for SAMPLE cb. */
    } data;
} hosal_sadc_cb_t;

/**
 * @brief sadc config structure.
 */
typedef struct
{
    hosal_sadc_config_resolution_t resolution;
    hosal_sadc_config_oversample_t oversample;

} hosal_sadc_config_t;

/**
 * @brief sadc read channel config structure.
 */
typedef struct
{
    hosal_sadc_input_ch_t channel;

} hosal_sadc_channel_config_t;

/**
 * \brief           sadc interrupt service routine callback for user application.
 * \param[in]       p_cb: CB
 */
typedef void (*hosal_sadc_cb_fn)(hosal_sadc_cb_t* p_cb);

/**
 * @brief hosal SADC channel configuration, enable, and register the sadc interrupt callback service routine function
 * @param[in]  SADC configuration
 * @param[in] sadc_int_callback SADC interrupt callback service routine function configuration
 * @return None
 */
void hosal_sadc_config_enable(hosal_sadc_config_t sadc_cfg, void* sadc_int_callback);

/**
 * @brief Trigger to read SADC channel
 * @param[in] ch SADC channel assignment
 *         \arg SADC_CH_AIN0 Read SADC channel AIO0
 *         \arg SADC_CH_AIN1 Read SADC channel AIO1
 *         \arg SADC_CH_AIN2 Read SADC channel AIO2
 *         \arg SADC_CH_AIN3 Read SADC channel AIO3
 *         \arg SADC_CH_AIN4 Read SADC channel AIO4
 *         \arg SADC_CH_AIN5 Read SADC channel AIO5
 *         \arg SADC_CH_AIN6 Read SADC channel AIO6
 *         \arg SADC_CH_AIN7 Read SADC channel AIO7
 *         \arg SADC_CH_VBAT Read SADC channel VBAT
 * @return return SADC channel read status
 *         \arg STATUS_EBUSY Read SADC channel is busy and invalid
 *         \arg STATUS_SUCCESS Read SADC channel is successful
 */
uint32_t hosal_sadc_channel_read(hosal_sadc_channel_config_t ch);

/**
 * @brief hosal SADC Compensation Initialization to enable SW timer for periodic compensation
 * @param[in] xPeriodicTimeInSec Periodic time in second for SADC compensation
 * @return None
 *
 */
void hosal_sadc_compensation_init(uint32_t xPeriodicTimeInSec);




#ifdef __cplusplus
}
#endif

#endif /* End of HOSAL_SADC_H */
