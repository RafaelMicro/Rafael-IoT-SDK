/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/* PURPOSE: ERL device common defintions
*/

#include "zcl/zb_zcl_calendar.h"
#include "zcl/zb_zcl_identify.h"
#include "zcl/zb_zcl_el_measurement.h"
#include "zcl/zb_zcl_meter_identification.h"
#include "zcl/zb_zcl_price.h"


/** @cond DOXYGEN_ERL_SECTION */


/** @defgroup ERL_BASIC_CLUSTER_DEFINITIONS Basic cluster definitions for ERL device
 *  @{
 */

/** @brief Declare attribute list for Basic cluster
 * @param[in] attr_list - attribute list variable name
 * @param[in] zcl_version - pointer to variable to store @ref ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID
 * @param[in] application_version - pointer to variable to store @ref ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID
 * @param[in] stack_version - pointer to variable to store @ref ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID
 * @param[in] hw_version - pointer to variable to store @ref ZB_ZCL_ATTR_BASIC_HW_VERSION_ID
 * @param[in] manufacture_name - pointer to variable to store @ref ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID
 * @param[in] model_identifier - pointer to variable to store @ref ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID
 * @param[in] date_code - pointer to variable to store @ref ZB_ZCL_ATTR_BASIC_DATE_CODE_ID
 * @param[in] power_source - pointer to variable to store @ref ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID
 */
#define ERL_DECLARE_BASIC_ATTRIB_LIST(attr_list, zcl_version, application_version, stack_version, \
                                      hw_version, manufacture_name, model_identifier, date_code,  \
                                      power_source)                                               \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                     \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, (zcl_version), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, (application_version), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID, (stack_version), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_BASIC_HW_VERSION_ID, (hw_version), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, (manufacture_name), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, (model_identifier), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_BASIC_DATE_CODE_ID, (date_code), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, (power_source), ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
 * Basic cluster attributes
 */
typedef struct erl_basic_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID
   * @see ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID
   */
  zb_uint8_t zcl_version;
  /** @copydoc ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID
   * @see ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID
   */
  zb_uint8_t application_version;
  /** @copydoc ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID
   * @see ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID
   */
  zb_uint8_t stack_version;
  /** @copydoc ZB_ZCL_ATTR_BASIC_HW_VERSION_ID
   * @see ZB_ZCL_ATTR_BASIC_HW_VERSION_ID
   */
  zb_uint8_t hw_version;
  /** @copydoc ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID
   * @see ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID
   */
  zb_uint8_t manufacture_name[32];
  /** @copydoc ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID
   * @see ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID
   */
  zb_uint8_t model_identifier[32];
  /** @copydoc ZB_ZCL_ATTR_BASIC_DATE_CODE_ID
   * @see ZB_ZCL_ATTR_BASIC_DATE_CODE_ID
   */
  zb_uint8_t date_code[16];
  /** @copydoc ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID
   * @see ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID
   */
  zb_uint8_t power_source;
} erl_basic_server_attrs_t;


/*FIXME: change default value in zb_zcl_basic.h to correct value */
#define ZB_ZCL_ATTR_BASIC_ZCL_VERSION_DEFAULT 0x03
#define ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_DEFAULT 0x00
#define ZB_ZCL_ATTR_BASIC_STACK_VERSION_DEFAULT 0x00
#define ZB_ZCL_ATTR_BASIC_HW_VERSION_DEFAULT 0x00
#define ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_DEFAULT {0}
#define ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_DEFAULT {0}
#define ZB_ZCL_ATTR_BASIC_DATE_CODE_DEFAULT {0}
#define ZB_ZCL_ATTR_BASIC_POWER_SOURCE_DEFAULT 0x00


/** Initialize @ref erl_basic_server_attrs_s Basic cluster's attributes */
#define ERL_BASIC_ATTR_LIST_INIT    \
  (erl_basic_server_attrs_t)        \
  { .zcl_version = ZB_ZCL_ATTR_BASIC_ZCL_VERSION_DEFAULT,                 \
    .application_version = ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_DEFAULT, \
    .stack_version = ZB_ZCL_ATTR_BASIC_STACK_VERSION_DEFAULT,             \
    .hw_version = ZB_ZCL_ATTR_BASIC_HW_VERSION_DEFAULT,                   \
    .manufacture_name = ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_DEFAULT,      \
    .model_identifier = ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_DEFAULT,       \
    .date_code = ZB_ZCL_ATTR_BASIC_DATE_CODE_DEFAULT,                     \
    .power_source = ZB_ZCL_ATTR_BASIC_POWER_SOURCE_DEFAULT}



/** @brief Declare attribute list for Basic cluster
*  @param[in] attr_list - attribute list variable name
*  @param[in] attrs - pointer to @ref erl_basic_server_attrs_s structure
*/
#define ERL_DECLARE_BASIC_ATTR_LIST(attr_list, attrs)  \
  ERL_DECLARE_BASIC_ATTRIB_LIST(attr_list, &attrs.zcl_version, &attrs.application_version,        \
                                &attrs.stack_version, &attrs.hw_version, &attrs.manufacture_name, \
                                &attrs.model_identifier, &attrs.date_code, &attrs.power_source)


/** @} */ /* ERL_BASIC_CLUSTER_DEFINITIONS */


/** @defgroup ERL_CALENDAR_CLUSTER_DEFINITIONS Calendar cluster definitions for ERL device
 *  @{
 */

/** @brief Declare attribute list for Calendar cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] aux_switch_1_label  - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_1_LABEL
 *  @param[in] aux_switch_2_label  - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_2_LABEL
 *  @param[in] aux_switch_3_label  - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_3_LABEL
 *  @param[in] aux_switch_4_label  - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_4_LABEL
 *  @param[in] aux_switch_5_label  - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_5_LABEL
 *  @param[in] aux_switch_6_label  - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_6_LABEL
 *  @param[in] aux_switch_7_label  - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_7_LABEL
 *  @param[in] aux_switch_8_label  - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_8_LABEL
 *  @param[in] aux_load_switch_state - pointer to variable to store @ref ZB_ZCL_ATTR_CALENDAR_AUX_LOAD_SWITCH_STATE
 */
#define ERL_DECLARE_CALENDAR_ATTRIB_LIST(attr_list, aux_switch_1_label, aux_switch_2_label,         \
                                        aux_switch_3_label, aux_switch_4_label, aux_switch_5_label, \
                                        aux_switch_6_label, aux_switch_7_label, aux_switch_8_label, \
                                        aux_load_switch_state)                                      \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                       \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_1_LABEL, (aux_switch_1_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_2_LABEL, (aux_switch_2_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_3_LABEL, (aux_switch_3_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_4_LABEL, (aux_switch_4_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_5_LABEL, (aux_switch_5_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_6_LABEL, (aux_switch_6_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_7_LABEL, (aux_switch_7_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_8_LABEL, (aux_switch_8_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_CALENDAR_AUX_LOAD_SWITCH_STATE, (aux_load_switch_state), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
 * Calendar cluster attributes
 */
typedef struct erl_calendar_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_1_LABEL
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_1_LABEL
   */
  zb_uint8_t aux_switch_1_label[23];
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_2_LABEL
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_2_LABEL
   */
  zb_uint8_t aux_switch_2_label[23];
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_3_LABEL
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_3_LABEL
   */
  zb_uint8_t aux_switch_3_label[23];
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_4_LABEL
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_4_LABEL
   */
  zb_uint8_t aux_switch_4_label[23];
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_5_LABEL
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_5_LABEL
   */
  zb_uint8_t aux_switch_5_label[23];
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_6_LABEL
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_6_LABEL
   */
  zb_uint8_t aux_switch_6_label[23];
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_7_LABEL
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_7_LABEL
   */
  zb_uint8_t aux_switch_7_label[23];
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_8_LABEL
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_SWITCH_8_LABEL
   */
  zb_uint8_t aux_switch_8_label[23];
  /** @copydoc ZB_ZCL_ATTR_CALENDAR_AUX_LOAD_SWITCH_STATE
   * @see ZB_ZCL_ATTR_CALENDAR_AUX_LOAD_SWITCH_STATE
   */
  zb_uint8_t aux_load_switch_state;
} erl_calendar_server_attrs_t;


/** @ref ZB_ZCL_ATTR_CALENDAR_AUX_LOAD_SWITCH_STATE "AuxiliaryLoadSwitchState" attribute default value */
#define ZB_ZCL_ATTR_CALENDAR_AUX_LOAD_SWITCH_STATE_DEFAULT 0x00


#define ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(n_label) \
  {sizeof("Auxiliary 0") - 1, 'A', 'u', 'x', 'i', 'l', 'i', 'a', 'r', 'y', ' ', '0' + n_label}

/** Initialize @ref erl_calendar_server_attrs_s Calendar cluster's attributes */
#define ERL_CALENDAR_ATTR_LIST_INIT            \
        (erl_calendar_server_attrs_t)           \
        { .aux_switch_1_label = ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(1), \
          .aux_switch_2_label = ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(2), \
          .aux_switch_3_label = ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(3), \
          .aux_switch_4_label = ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(4), \
          .aux_switch_5_label = ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(5), \
          .aux_switch_6_label = ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(6), \
          .aux_switch_7_label = ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(7), \
          .aux_switch_8_label = ERL_AUX_SWITCH_N_LABEL_ATTR_INIT(8), \
          .aux_load_switch_state = ZB_ZCL_ATTR_CALENDAR_AUX_LOAD_SWITCH_STATE_DEFAULT}


/** @brief Declare attribute list for Calendar cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] attrs - pointer to @ref erl_calendar_server_attrs_s structure
 */
#define ERL_DECLARE_CALENDAR_ATTR_LIST(attr_list, attrs)  \
  ERL_DECLARE_CALENDAR_ATTRIB_LIST(attr_list, &attrs.aux_switch_1_label, &attrs.aux_switch_2_label, \
                                   &attrs.aux_switch_3_label, &attrs.aux_switch_4_label, &attrs.aux_switch_5_label, \
                                   &attrs.aux_switch_6_label, &attrs.aux_switch_7_label, &attrs.aux_switch_8_label, \
                                   &attrs.aux_load_switch_state)

/** @} */ /* ERL_CALENDAR_CLUSTER_DEFINITIONS */


/** @defgroup ERL_IDENTIFY_CLUSTER_DEFINITIONS Identify cluster definitions for ERL device
 *  @{
 */

/** @brief Declare attribute list for Identify cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] identify_time  - pointer to variable to store @ref ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID
 */
#define ERL_DECLARE_IDENTIFY_ATTRIB_LIST(attr_list, identify_time)                                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                       \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, (identify_time), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
* Identify cluster attributes
*/
typedef struct erl_identify_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID
  * @see ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID
  */
  zb_uint16_t identify_time;
} erl_identify_server_attrs_t;


/** @ref ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID "IdentifyTime" attribute default value */
#define ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_DEFAULT 0x0000


/** Initialize @ref erl_identify_server_attrs_s Identify cluster's attributes */
#define ERL_IDENTIFY_ATTR_LIST_INIT            \
  (erl_identify_server_attrs_t)           \
  { .identify_time = ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_DEFAULT}


/** @brief Declare attribute list for Identify cluster
*  @param[in] attr_list - attribute list variable name
*  @param[in] attrs - pointer to @ref erl_identify_server_attrs_s structure
*/
#define ERL_DECLARE_IDENTIFY_ATTR_LIST(attr_list, attrs)  \
  ERL_DECLARE_IDENTIFY_ATTRIB_LIST(attr_list, &attrs.identify_time)

/** @} */ /* ERL_IDENTIFY_CLUSTER_DEFINITIONS */


/** @defgroup ERL_ELECTRICAL_MEASUREMENT_CLUSTER_DEFINITIONS Electrical Measurement cluster definitions for ERL device
 *  @{
 */


/** @brief Declare attribute list for Electrical Measurement cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] measurement_type - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID
 *  @param[in] total_active_power - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_ACTIVE_POWER_ID
 *  @param[in] total_apparent_power - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_APPARENT_POWER_ID
 *  @param[in] rms_voltage - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID
 *  @param[in] rms_current - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID
 *  @param[in] active_power - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID
 *  @param[in] apparent_power - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_ID
 *  @param[in] avrg_rmsvoltage_measur_period - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_ID
 *  @param[in] rms_voltage_phb - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHB_ID
 *  @param[in] rms_current_phb - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHB_ID
 *  @param[in] active_power_phb - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHB_ID
 *  @param[in] apparent_power_phb - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHB_ID
 *  @param[in] avrg_rmsvoltage_measur_period_phb - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_ID
 *  @param[in] rms_voltage_phc - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHC_ID
 *  @param[in] rms_current_phc - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHC_ID
 *  @param[in] active_power_phc - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHC_ID
 *  @param[in] apparent_power_phc - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHC_ID
 *  @param[in] avrg_rmsvoltage_measur_period_phc - pointer to variable to store @ref ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_ID
 */
#define ERL_DECLARE_EL_MEASUREMENT_ATTRIB_LIST(attr_list, measurement_type, total_active_power,             \
                                               total_apparent_power, rms_voltage, rms_current,              \
                                               active_power, apparent_power, avrg_rmsvoltage_measur_period, \
                                               rms_voltage_phb, rms_current_phb, active_power_phb,          \
                                               apparent_power_phb, avrg_rmsvoltage_measur_period_phb,       \
                                               rms_voltage_phc, rms_current_phc, active_power_phc,          \
                                               apparent_power_phc, avrg_rmsvoltage_measur_period_phc)       \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                               \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID, (measurement_type), ZB_ZCL_ATTR_TYPE_32BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY ) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_ACTIVE_POWER_ID, (total_active_power), ZB_ZCL_ATTR_TYPE_S32, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_APPARENT_POWER_ID, (total_apparent_power), ZB_ZCL_ATTR_TYPE_U32, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID, (rms_voltage), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID, (rms_current), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, (active_power), ZB_ZCL_ATTR_TYPE_S16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_ID, (apparent_power), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_ID, (avrg_rmsvoltage_measur_period), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHB_ID, (rms_voltage_phb), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHB_ID, (rms_current_phb), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHB_ID, (active_power_phb), ZB_ZCL_ATTR_TYPE_S16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHB_ID, (apparent_power_phb), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_ID, (avrg_rmsvoltage_measur_period_phb), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHC_ID, (rms_voltage_phc), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHC_ID, (rms_current_phc), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHC_ID, (active_power_phc),  ZB_ZCL_ATTR_TYPE_S16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHC_ID, (apparent_power_phc), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_ID, (avrg_rmsvoltage_measur_period_phc), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
 * Electric Measurement cluster attributes
 */
typedef struct erl_el_measurement_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID
   */
  zb_uint32_t measurement_type;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_ACTIVE_POWER_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_ACTIVE_POWER_ID
   */
  zb_int32_t total_active_power;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_APPARENT_POWER_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_TOTAL_APPARENT_POWER_ID
   */
  zb_uint32_t total_apparent_power;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_ID
   */
  zb_uint16_t rms_voltage;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_ID
   */
  zb_uint16_t rms_current;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID
   */
  zb_int16_t  active_power;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_ID
   */
  zb_uint16_t apparent_power;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_ID
   */
  zb_uint16_t avrg_rmsvoltage_measur_period;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHB_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHB_ID
   */
  zb_uint16_t rms_voltage_phb;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHB_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHB_ID
   */
  zb_uint16_t rms_current_phb;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHB_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHB_ID
   */
  zb_int16_t  active_power_phb;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHB_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHB_ID
   */
  zb_uint16_t apparent_power_phb;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_ID
   */
  zb_uint16_t avrg_rmsvoltage_measur_period_phb;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHC_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSVOLTAGE_PHC_ID
   */
  zb_uint16_t rms_voltage_phc;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHC_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_RMSCURRENT_PHC_ID
   */
  zb_uint16_t rms_current_phc;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHC_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_PHC_ID
   */
  zb_int16_t  active_power_phc;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHC_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_APPARENT_POWER_PHC_ID
   */
  zb_uint16_t apparent_power_phc;
  /** @copydoc ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_ID
   * @see ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_ID
   */
  zb_uint16_t avrg_rmsvoltage_measur_period_phc;
} erl_el_measurement_server_attrs_t;


#define ZB_ZCL_ATTR_EL_MEASUR_MEASUREMENT_TYPE_DEFAULT                          0x00000000
#define ZB_ZCL_ATTR_EL_MEASUR_RMSVOLTAGE_DEFAULT                                0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_RMSCURRENT_DEFAULT                                0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_ACTIVE_POWER_DEFAULT                              (zb_int16_t)0x8000
#define ZB_ZCL_ATTR_EL_MEASUR_APPARENT_POWER_DEFAULT                            0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_DEFAULT     0x0000
#define ZB_ZCL_ATTR_EL_MEASUR_RMSVOLTAGE_PHB_DEFAULT                            0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_RMSCURRENT_PHB_DEFAULT                            0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_ACTIVE_POWER_PHB_DEFAULT                          (zb_int16_t)0x8000
#define ZB_ZCL_ATTR_EL_MEASUR_APPARENT_POWER_PHB_DEFAULT                        0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_DEFAULT 0x0000
#define ZB_ZCL_ATTR_EL_MEASUR_RMSVOLTAGE_PHC_DEFAULT                            0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_RMSCURRENT_PHC_DEFAULT                            0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_ACTIVE_POWER_PHC_DEFAULT                          (zb_int16_t)0x8000
#define ZB_ZCL_ATTR_EL_MEASUR_APPARENT_POWER_PHC_DEFAULT                        0xffff
#define ZB_ZCL_ATTR_EL_MEASUR_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_DEFAULT 0x0000


/** Initialize @ref erl_el_measurement_server_attrs_s
 * Electrical Measurement cluster's attributes
 */
#define ERL_EL_MEASUREMENT_ATTR_LIST_INIT \
        (erl_el_measurement_server_attrs_t)     \
        { .measurement_type                  = ZB_ZCL_ATTR_EL_MEASUR_MEASUREMENT_TYPE_DEFAULT,                          \
          .rms_voltage                       = ZB_ZCL_ATTR_EL_MEASUR_RMSVOLTAGE_DEFAULT,                                \
          .rms_current                       = ZB_ZCL_ATTR_EL_MEASUR_RMSCURRENT_DEFAULT,                                \
          .active_power                      = ZB_ZCL_ATTR_EL_MEASUR_ACTIVE_POWER_DEFAULT,                              \
          .apparent_power                    = ZB_ZCL_ATTR_EL_MEASUR_APPARENT_POWER_DEFAULT,                            \
          .avrg_rmsvoltage_measur_period     = ZB_ZCL_ATTR_EL_MEASUR_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_DEFAULT,     \
          .rms_voltage_phb                   = ZB_ZCL_ATTR_EL_MEASUR_RMSVOLTAGE_PHB_DEFAULT,                            \
          .rms_current_phb                   = ZB_ZCL_ATTR_EL_MEASUR_RMSCURRENT_PHB_DEFAULT,                            \
          .active_power_phb                  = ZB_ZCL_ATTR_EL_MEASUR_ACTIVE_POWER_PHB_DEFAULT,                          \
          .apparent_power_phb                = ZB_ZCL_ATTR_EL_MEASUR_APPARENT_POWER_PHB_DEFAULT,                        \
          .avrg_rmsvoltage_measur_period_phb = ZB_ZCL_ATTR_EL_MEASUR_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHB_DEFAULT, \
          .rms_voltage_phc                   = ZB_ZCL_ATTR_EL_MEASUR_RMSVOLTAGE_PHC_DEFAULT,                            \
          .rms_current_phc                   = ZB_ZCL_ATTR_EL_MEASUR_RMSCURRENT_PHC_DEFAULT,                            \
          .active_power_phc                  = ZB_ZCL_ATTR_EL_MEASUR_ACTIVE_POWER_PHC_DEFAULT,                          \
          .apparent_power_phc                = ZB_ZCL_ATTR_EL_MEASUR_APPARENT_POWER_PHC_DEFAULT,                        \
          .avrg_rmsvoltage_measur_period_phc = ZB_ZCL_ATTR_EL_MEASUR_AVERAGE_RMSVOLTAGE_MEASUREMENT_PERIOD_PHC_DEFAULT}


/** @brief Declare attribute list for Electrical Measurement cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] attrs - pointer to @ref erl_el_measurement_server_attrs_s structure
 */
#define ERL_DECLARE_EL_MEASUREMENT_ATTR_LIST(attr_list, attrs)  \
  ERL_DECLARE_EL_MEASUREMENT_ATTRIB_LIST(attr_list, &attrs.measurement_type, &attrs.total_active_power,                         \
                                   &attrs.total_apparent_power, &attrs.rms_voltage, &attrs.rms_current,                         \
                                   &attrs.active_power, &attrs.apparent_power, &attrs.avrg_rmsvoltage_measur_period,            \
                                   &attrs.rms_voltage_phb, &attrs.rms_current_phb, &attrs.active_power_phb,                     \
                                   &attrs.apparent_power_phb, &attrs.avrg_rmsvoltage_measur_period_phb, &attrs.rms_voltage_phc, \
                                   &attrs.rms_current_phc, &attrs.active_power_phc, &attrs.apparent_power_phc,                  \
                                   &attrs.avrg_rmsvoltage_measur_period_phc)

/** @} */ /* ERL_ELECTRICAL_MEASUREMENT_CLUSTER_DEFINITIONS */


/** @defgroup ERL_METER_IDENTIFICATION_CLUSTER_DEFINITIONS  Meter Identification cluster definitions for ERL device
 *  @{
 */

/** @brief Declare attribute list for Meter Identification cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] company_name - pointer to variable to store @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_COMPANY_NAME
 *  @param[in] meter_type_id - pointer to variable to store @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_METER_TYPE_ID
 *  @param[in] data_quality_id - pointer to variable to store @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID
 *  @param[in] model - pointer to variable to store @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_MODEL
 *  @param[in] pod - pointer to variable to store @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_POD
 *  @param[in] available_power - pointer to variable to store @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER
 *  @param[in] power_threshold - pointer to variable to store @ref ZB_ZCl_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD
 *
 */
#define ERL_DECLARE_METER_IDENTIFICATION_ATTRIB_LIST(attr_list, company_name, meter_type_id, data_quality_id, \
                                                     model, pod, available_power, power_threshold)            \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                                 \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCl_ATTR_METER_IDENTIFICATION_COMPANY_NAME, (company_name), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY ) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCl_ATTR_METER_IDENTIFICATION_METER_TYPE_ID, (meter_type_id), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCl_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID, (data_quality_id), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY ) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCl_ATTR_METER_IDENTIFICATION_MODEL, (model), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCl_ATTR_METER_IDENTIFICATION_POD, (pod), ZB_ZCL_ATTR_TYPE_CHAR_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCl_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER, (available_power), ZB_ZCL_ATTR_TYPE_S24, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCl_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD, (power_threshold), ZB_ZCL_ATTR_TYPE_S24, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
 * Meter Identification cluster attributes
 */
typedef struct erl_meter_identification_server_attrs_s
{
  /** @copydoc ZB_ZCl_ATTR_METER_IDENTIFICATION_COMPANY_NAME
   * @see ZB_ZCl_ATTR_METER_IDENTIFICATION_COMPANY_NAME
   */
  zb_uint8_t company_name[1 + 16];
  /** @copydoc ZB_ZCl_ATTR_METER_IDENTIFICATION_METER_TYPE_ID
   * @see ZB_ZCl_ATTR_METER_IDENTIFICATION_METER_TYPE_ID
   */
  zb_uint16_t meter_type_id;
  /** @copydoc ZB_ZCl_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID
   * @see ZB_ZCl_ATTR_METER_IDENTIFICATION_DATA_QUALITY_ID
   */
  zb_uint16_t data_quality_id;
  /** @copydoc ZB_ZCl_ATTR_METER_IDENTIFICATION_MODEL
   * @see ZB_ZCl_ATTR_METER_IDENTIFICATION_MODEL
   */
  zb_uint8_t model[1 + 16];
  /** @copydoc ZB_ZCl_ATTR_METER_IDENTIFICATION_POD
   * @see ZB_ZCl_ATTR_METER_IDENTIFICATION_POD
   */
  zb_uint8_t pod[1 + 16];
  /** @copydoc ZB_ZCl_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER
   * @see ZB_ZCl_ATTR_METER_IDENTIFICATION_AVAILABLE_POWER
   */
  zb_int24_t available_power;
  /** @copydoc ZB_ZCl_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD
   * @see ZB_ZCl_ATTR_METER_IDENTIFICATION_POWER_THRESHOLD
   */
  zb_int24_t power_threshold;
} erl_meter_identification_server_attrs_t;


/** @brief Declare attribute list for Meter Identification cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] attrs - pointer to @ref erl_meter_identification_server_attrs_s structure
 */
#define ERL_DECLARE_METER_IDENTIFICATION_ATTR_LIST(attr_list, attrs)  \
  ERL_DECLARE_METER_IDENTIFICATION_ATTRIB_LIST(attr_list, &attrs.company_name, &attrs.meter_type_id,  \
                                               &attrs.data_quality_id, &attrs.model, &attrs.pod,      \
                                               &attrs.available_power, &attrs.power_threshold)


/** @} */ /* ERL_METER_IDENTIFICATION_CLUSTER_DEFINITIONS */


/** @defgroup ERL_PRICE_CLUSTER_DEFINITIONS  Price cluster definitions for ERL device
 *  @{
 */

/** @brief Declare attribute list for Price cluster
 * @param[in] attr_list - attribute list variable name
 * @param[in] tier1_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier2_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier3_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier4_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier5_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier6_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier7_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier8_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier9_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tier10_price_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
 * @param[in] tariff_label - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL
 */
#define ERL_DECLARE_PRICE_ATTRIB_LIST(attr_list, tier1_price_label, tier2_price_label,         \
                                      tier3_price_label, tier4_price_label, tier5_price_label, \
                                      tier6_price_label, tier7_price_label, tier8_price_label, \
                                      tier9_price_label, tier10_price_label, tariff_label)     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                  \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL, (tier1_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER2_PRICE_LABEL, (tier2_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER3_PRICE_LABEL, (tier3_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER4_PRICE_LABEL, (tier4_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER5_PRICE_LABEL, (tier5_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER6_PRICE_LABEL, (tier6_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER7_PRICE_LABEL, (tier7_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER8_PRICE_LABEL, (tier8_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER9_PRICE_LABEL, (tier9_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING)   \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TIER10_PRICE_LABEL, (tier10_price_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL, (tariff_label), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
 * Price cluster attributes
 */
typedef struct erl_price_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   */
  zb_uint8_t tier1_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER_PRICE_LABEL
   */
  zb_uint8_t tier2_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER_PRICE_LABEL
   */
  zb_uint8_t tier3_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER_PRICE_LABEL
   */
  zb_uint8_t tier4_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER_PRICE_LABEL
   */
  zb_uint8_t tier5_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER_PRICE_LABEL
   */
  zb_uint8_t tier6_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER_PRICE_LABEL
   */
  zb_uint8_t tier7_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER_PRICE_LABEL
   */
  zb_uint8_t tier8_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER_PRICE_LABEL
   */
  zb_uint8_t tier9_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TIER10_PRICE_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TIER10_PRICE_LABEL
   */
  zb_uint8_t tier10_price_label[1 + 12];
  /** @copydoc ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL
   * @see ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL
   */
  zb_uint8_t tariff_label[1 + 24];
} erl_price_server_attrs_t;


/** @ref ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL "TariffLabel" attribute default value */
#define ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL_DEFAULT 0


#define ERL_TIERN_PRICE_LABEL_ATTR_INIT(n_label) \
  {sizeof("Tier 0") - 1, 'T', 'i', 'e', 'r', ' ', '0' + n_label}

/** Initialize @ref erl_price_server_attrs_s Price cluster's attributes */
#define ERL_PRICE_ATTR_LIST_INIT            \
        (erl_price_server_attrs_t)           \
        { .tier1_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(1), \
          .tier2_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(2), \
          .tier3_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(3), \
          .tier4_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(4), \
          .tier5_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(5), \
          .tier6_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(6), \
          .tier7_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(7), \
          .tier8_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(8), \
          .tier9_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(9), \
          .tier10_price_label = ERL_TIERN_PRICE_LABEL_ATTR_INIT(10), \
          .tariff_label = ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL_DEFAULT}


/** @brief Declare attribute list for Price cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] attrs - pointer to @ref erl_price_server_attrs_s structure
 */
#define ERL_DECLARE_PRICE_ATTR_LIST(attr_list, attrs)  \
  ERL_DECLARE_PRICE_ATTRIB_LIST(attr_list, &attrs.tier1_price_label, &attrs.tier2_price_label,  \
                                &attrs.tier3_price_label, &attrs.tier4_price_label,             \
                                &attrs.tier5_price_label, &attrs.tier6_price_label,             \
                                &attrs.tier7_price_label, &attrs.tier8_price_label,             \
                                &attrs.tier9_price_label, &attrs.tier10_price_label,            \
                                &attrs.tariff_label)


/** @} */ /* ERL_PRICE_CLUSTER_DEFINITIONS */


/** @defgroup ERL_METERING_CLUSTER_DEFINITIONS  Metering cluster definitions for ERL device
 *  @{
 */


/** @brief Declare attribute list for Metering cluster
 * @param[in] attr_list - attribute list variable name
 * @param[in] curr_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID
 * @param[in] curr_summ_received - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID
 * @param[in] fast_poll_update_period - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_FAST_POLL_UPDATE_PERIOD_ID
 * @param[in] active_register_tier_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_ACTIVE_REGISTER_TIER_DELIVERED_ID
 * @param[in] curr_tier1_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier1_summ_received - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID
 * @param[in] curr_tier2_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier3_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier4_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier5_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier6_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier7_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier8_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier9_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_DELIVERED_ID
 * @param[in] curr_tier10_summ_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_DELIVERED_ID
 * @param[in] status - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_STATUS_ID
 * @param[in] extended_status - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID
 * @param[in] curr_meter - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID
 * @param[in] service_disconnect_reason - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID
 * @param[in] linky_mode_of_operation - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID
 * @param[in] unit_of_measure - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
 * @param[in] multiplier - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_MULTIPLIER_ID
 * @param[in] divisor - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_DIVISOR_ID
 * @param[in] summation_formatting - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID
 * @param[in] demand_format - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID
 * @param[in] historical_consumption_format - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID
 * @param[in] device_type - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
 * @param[in] site - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_SITE_ID_ID
 * @param[in] meter_serial_number - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID
 * @param[in] module_serial_number - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID
 * @param[in] instantaneous_demand - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID
 * @param[in] curr_day_max_demand_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID
 * @param[in] curr_day_max_demand_delivered_time - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID
 * @param[in] curr_day_max_demand_received - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_ID
 * @param[in] curr_day_max_demand_received_time - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_TIME_ID
 * @param[in] prev_day_max_demand_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID
 * @param[in] prev_day_max_demand_delivered_time - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID
 * @param[in] prev_day_max_demand_received - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_ID
 * @param[in] prev_day_max_demand_received_time - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_TIME_ID
 * @param[in] max_number_of_periods_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_ID
 * @param[in] generic_alarm_mask - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID
 * @param[in] electricity_alarm_mask - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_ELECTRICITY_ALARM_MASK_ID
 * @param[in] generic_flow_pressure_alarm_mask - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_ID
 * @param[in] water_specific_alarm_mask - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_WATER_SPECIFIC_ALARM_MASK_ID
 * @param[in] heat_and_cooling_specific_alarm_mask - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_ID
 * @param[in] gas_specific_alarm_mask - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_GAS_SPECIFIC_ALARM_MASK_ID
 * @param[in] extended_generic_alarm_mask - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_EXTENDED_GENERIC_ALARM_MASK_ID
 * @param[in] manuf_alarm_mask - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_MANUFACTURER_ALARM_MASK_ID
 * @param[in] curr_reactive_summ_q1 - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q1_ID
 * @param[in] curr_reactive_summ_q2 - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q2_ID
 * @param[in] curr_reactive_summ_q3 - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q3_ID
 * @param[in] curr_reactive_summ_q4 - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q4_ID
 */
#define ERL_INTERFACE_DECLARE_METERING_ATTRIB_LIST(attr_list, curr_summ_delivered, curr_summ_received, fast_poll_update_period,             \
                                                   active_register_tier_delivered, curr_tier1_summ_delivered, curr_tier1_summ_received,         \
                                                   curr_tier2_summ_delivered, curr_tier3_summ_delivered, curr_tier4_summ_delivered,                 \
                                                   curr_tier5_summ_delivered, curr_tier6_summ_delivered, curr_tier7_summ_delivered,                 \
                                                   curr_tier8_summ_delivered, curr_tier9_summ_delivered, curr_tier10_summ_delivered,                \
                                                   status, extended_status, curr_meter, service_disconnect_reason,                 \
                                                   linky_mode_of_operation, unit_of_measure, multiplier, divisor,                 \
                                                   summation_formatting, demand_format, historical_consumption_format,                      \
                                                   device_type, site, meter_serial_number, module_serial_number,         \
                                                   instantaneous_demand, curr_day_max_demand_delivered, curr_day_max_demand_delivered_time, \
                                                   curr_day_max_demand_received, curr_day_max_demand_received_time,                     \
                                                   prev_day_max_demand_delivered, prev_day_max_demand_delivered_time,                     \
                                                   prev_day_max_demand_received, prev_day_max_demand_received_time,                   \
                                                   max_number_of_periods_delivered, generic_alarm_mask, electricity_alarm_mask,       \
                                                   generic_flow_pressure_alarm_mask, water_specific_alarm_mask,                   \
                                                   heat_and_cooling_specific_alarm_mask, gas_specific_alarm_mask,                 \
                                                   extended_generic_alarm_mask, manuf_alarm_mask, curr_reactive_summ_q1,            \
                                                   curr_reactive_summ_q2, curr_reactive_summ_q3, curr_reactive_summ_q4)                 \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                                                     \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, (curr_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID, (curr_summ_received), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_FAST_POLL_UPDATE_PERIOD_ID, (fast_poll_update_period), ZB_ZCL_ATTR_TYPE_U8, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_ACTIVE_REGISTER_TIER_DELIVERED_ID, (active_register_tier_delivered), ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID, (curr_tier1_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID, (curr_tier1_summ_received), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_DELIVERED_ID, (curr_tier2_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_DELIVERED_ID, (curr_tier3_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_DELIVERED_ID, (curr_tier4_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_DELIVERED_ID, (curr_tier5_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_DELIVERED_ID, (curr_tier6_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_DELIVERED_ID, (curr_tier7_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_DELIVERED_ID, (curr_tier8_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_DELIVERED_ID, (curr_tier9_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_DELIVERED_ID, (curr_tier10_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_STATUS_ID, (status), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID, (extended_status), ZB_ZCL_ATTR_TYPE_64BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID, (curr_meter), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID, (service_disconnect_reason), ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID, (linky_mode_of_operation), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID, (unit_of_measure), ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, (multiplier), ZB_ZCL_ATTR_TYPE_U24, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_DIVISOR_ID, (divisor), ZB_ZCL_ATTR_TYPE_U24, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, (summation_formatting), ZB_ZCL_ATTR_TYPE_8BITMAP,ZB_ZCL_ATTR_ACCESS_READ_ONLY ) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID, (demand_format), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID, (historical_consumption_format), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID, (device_type), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_SITE_ID_ID, (site), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID, (meter_serial_number), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID, (module_serial_number), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID, (instantaneous_demand), ZB_ZCL_ATTR_TYPE_S24, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID, (curr_day_max_demand_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID, (curr_day_max_demand_delivered_time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_ID, (curr_day_max_demand_received), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_TIME_ID, (curr_day_max_demand_received_time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID, (prev_day_max_demand_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID, (prev_day_max_demand_delivered_time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_ID, (prev_day_max_demand_received), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_TIME_ID, (prev_day_max_demand_received_time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_ID, (max_number_of_periods_delivered), ZB_ZCL_ATTR_TYPE_U8, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID, (generic_alarm_mask), ZB_ZCL_ATTR_TYPE_16BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_ELECTRICITY_ALARM_MASK_ID, (electricity_alarm_mask), ZB_ZCL_ATTR_TYPE_32BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_ID, (generic_flow_pressure_alarm_mask), ZB_ZCL_ATTR_TYPE_16BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_WATER_SPECIFIC_ALARM_MASK_ID, (water_specific_alarm_mask), ZB_ZCL_ATTR_TYPE_16BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_ID, (heat_and_cooling_specific_alarm_mask), ZB_ZCL_ATTR_TYPE_16BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_GAS_SPECIFIC_ALARM_MASK_ID, (gas_specific_alarm_mask), ZB_ZCL_ATTR_TYPE_16BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_EXTENDED_GENERIC_ALARM_MASK_ID, (extended_generic_alarm_mask), ZB_ZCL_ATTR_TYPE_48BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_MANUFACTURER_ALARM_MASK_ID, (manuf_alarm_mask), ZB_ZCL_ATTR_TYPE_16BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q1_ID, (curr_reactive_summ_q1), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q2_ID, (curr_reactive_summ_q2), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q3_ID, (curr_reactive_summ_q3), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q4_ID, (curr_reactive_summ_q4), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


#define ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_MAX_LENGTH 20

/**
 * Metering cluster attributes
 */
typedef struct erl_interface_metering_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID
   */
  zb_uint48_t curr_summ_received;
  /** @copydoc ZB_ZCL_ATTR_METERING_FAST_POLL_UPDATE_PERIOD_ID
   * @see ZB_ZCL_ATTR_METERING_FAST_POLL_UPDATE_PERIOD_ID
   */
  zb_uint8_t fast_poll_update_period;
  /** @copydoc ZB_ZCL_ATTR_METERING_ACTIVE_REGISTER_TIER_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_ACTIVE_REGISTER_TIER_DELIVERED_ID
   */
  zb_uint8_t active_register_tier_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier1_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID
   */
  zb_uint48_t curr_tier1_summ_received;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier2_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier3_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier4_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier5_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier6_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier7_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier8_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier9_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_tier10_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_STATUS_ID
   * @see ZB_ZCL_ATTR_METERING_STATUS_ID
   */
  zb_uint8_t status;
  /** @copydoc ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID
   * @see ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID
   */
  zb_uint64_t extended_status;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID
   */
  zb_uint8_t curr_meter[ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_MAX_LENGTH];
  /** @copydoc ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID
   * @see ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID
   */
  zb_uint8_t service_disconnect_reason;
  /** @copydoc ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID
   * @see ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID
   */
  zb_uint8_t linky_mode_of_operation;
  /** @copydoc ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
   * @see ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
   */
  zb_uint8_t unit_of_measure;
  /** @copydoc ZB_ZCL_ATTR_METERING_MULTIPLIER_ID
   * @see ZB_ZCL_ATTR_METERING_MULTIPLIER_ID
   */
  zb_uint24_t multiplier;
  /** @copydoc ZB_ZCL_ATTR_METERING_DIVISOR_ID
   * @see ZB_ZCL_ATTR_METERING_DIVISOR_ID
   */
  zb_uint24_t divisor;
  /** @copydoc ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID
   * @see ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID
   */
  zb_uint8_t summation_formatting;
  /** @copydoc ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID
   * @see ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID
   */
  zb_uint8_t demand_format;
  /** @copydoc ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID
   * @see ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID
   */
  zb_uint8_t historical_consumption_format;
  /** @copydoc ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
   * @see ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
   */
  zb_uint8_t device_type;
  /** @copydoc ZB_ZCL_ATTR_METERING_SITE_ID_ID
   * @see ZB_ZCL_ATTR_METERING_SITE_ID_ID
   */
  zb_uint8_t site[1 + 32];
  /** @copydoc ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID
   * @see ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID
   */
  zb_uint8_t meter_serial_number[1 + 24];
  /** @copydoc ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID
   * @see ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID
   */
  zb_uint8_t module_serial_number[1 + 24];
  /** @copydoc ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID
   * @see ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID
   */
  zb_int24_t instantaneous_demand;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID
   */
  zb_uint48_t curr_day_max_demand_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID
   */
  zb_uint32_t curr_day_max_demand_delivered_time;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_ID
   */
  zb_uint48_t curr_day_max_demand_received;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_TIME_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_TIME_ID
   */
  zb_uint32_t curr_day_max_demand_received_time;
  /** @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID
   */
  zb_uint48_t prev_day_max_demand_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID
   * @see ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID
   */
  zb_uint32_t prev_day_max_demand_delivered_time;
  /** @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_ID
   * @see ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_ID
   */
  zb_uint48_t prev_day_max_demand_received;
  /** @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_TIME_ID
   * @see ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_TIME_ID
   */
  zb_uint32_t prev_day_max_demand_received_time;
  /** @copydoc ZB_ZCL_ATTR_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_ID
   */
  zb_uint8_t max_number_of_periods_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID
   */
  zb_uint16_t generic_alarm_mask;
  /** @copydoc ZB_ZCL_ATTR_METERING_ELECTRICITY_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_METERING_ELECTRICITY_ALARM_MASK_ID
   */
  zb_uint32_t electricity_alarm_mask;
  /** @copydoc ZB_ZCL_ATTR_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_ID
   */
  zb_uint16_t generic_flow_pressure_alarm_mask;
  /** @copydoc ZB_ZCL_ATTR_METERING_WATER_SPECIFIC_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_METERING_WATER_SPECIFIC_ALARM_MASK_ID
   */
  zb_uint16_t water_specific_alarm_mask;
  /** @copydoc ZB_ZCL_ATTR_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_ID
   */
  zb_uint16_t heat_and_cooling_specific_alarm_mask;
  /** @copydoc ZB_ZCL_ATTR_METERING_GAS_SPECIFIC_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_METERING_GAS_SPECIFIC_ALARM_MASK_ID
   */
  zb_uint16_t gas_specific_alarm_mask;
  /** @copydoc ZB_ZCL_ATTR_METERING_EXTENDED_GENERIC_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_METERING_EXTENDED_GENERIC_ALARM_MASK_ID
   */
  zb_uint48_t extended_generic_alarm_mask;
  /** @copydoc ZB_ZCL_ATTR_METERING_MANUFACTURER_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_METERING_MANUFACTURER_ALARM_MASK_ID
   */
  zb_uint16_t manuf_alarm_mask;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q1_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q1_ID
   */
  zb_uint48_t curr_reactive_summ_q1;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q2_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q2_ID
   */
  zb_uint48_t curr_reactive_summ_q2;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q3_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q3_ID
   */
  zb_uint48_t curr_reactive_summ_q3;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q4_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q4_ID
   */
  zb_uint48_t curr_reactive_summ_q4;
} erl_interface_metering_server_attrs_t;

#define ZB_ZCL_ATTR_METERING_FAST_POLL_UPDATE_PERIOD_DEFAULT 0x05
#define ZB_ZCL_ATTR_METERING_STATUS_DEFAULT 0x00
#define ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_DEFAULT 0x00
#define ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_DEFAULT 0x00
#define ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_DEFAULT ZB_INIT_UINT24(0x00, 0x0000)
#define ZB_ZCL_ATTR_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_DEFAULT 0x18
#define ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_DEFAULT 0xffff
#define ZB_ZCL_ATTR_METERING_ELECTRICITY_ALARM_MASK_DEFAULT 0xffffffff
#define ZB_ZCL_ATTR_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_DEFAULT 0xffff
#define ZB_ZCL_ATTR_METERING_WATER_SPECIFIC_ALARM_MASK_DEFAULT 0xffff
#define ZB_ZCL_ATTR_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_DEFAULT 0xffff
#define ZB_ZCL_ATTR_METERING_GAS_SPECIFIC_ALARM_MASK_DEFAULT 0xffff
#define ZB_ZCL_ATTR_METERING_EXTENDED_GENERIC_ALARM_MASK_DEFAULT ZB_INIT_UINT48(0xffff, 0xffffffff)
#define ZB_ZCL_ATTR_METERING_MANUFACTURER_ALARM_MASK_DEFAULT 0xffff


/** Initialize @ref erl_interface_metering_server_attrs_s Metering cluster's attributes */
#define ERL_INTERFACE_METERING_ATTR_LIST_INIT    \
      (erl_interface_metering_server_attrs_t)    \
      {                                          \
        .fast_poll_update_period              =  ZB_ZCL_ATTR_METERING_FAST_POLL_UPDATE_PERIOD_DEFAULT,              \
        .status                               =  ZB_ZCL_ATTR_METERING_STATUS_DEFAULT,                               \
        .linky_mode_of_operation              =  ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_DEFAULT,              \
        .unit_of_measure                      =  ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_DEFAULT,                      \
        .instantaneous_demand                 =  ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_DEFAULT,                 \
        .max_number_of_periods_delivered      =  ZB_ZCL_ATTR_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_DEFAULT,      \
        .generic_alarm_mask                   =  ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_DEFAULT,                   \
        .electricity_alarm_mask               =  ZB_ZCL_ATTR_METERING_ELECTRICITY_ALARM_MASK_DEFAULT,               \
        .generic_flow_pressure_alarm_mask     =  ZB_ZCL_ATTR_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_DEFAULT,     \
        .water_specific_alarm_mask            =  ZB_ZCL_ATTR_METERING_WATER_SPECIFIC_ALARM_MASK_DEFAULT,            \
        .heat_and_cooling_specific_alarm_mask =  ZB_ZCL_ATTR_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_DEFAULT, \
        .gas_specific_alarm_mask              =  ZB_ZCL_ATTR_METERING_GAS_SPECIFIC_ALARM_MASK_DEFAULT,              \
        .extended_generic_alarm_mask          =  ZB_ZCL_ATTR_METERING_EXTENDED_GENERIC_ALARM_MASK_DEFAULT,          \
        .manuf_alarm_mask                     =  ZB_ZCL_ATTR_METERING_MANUFACTURER_ALARM_MASK_DEFAULT}


/** @brief Declare attribute list for Metering cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] attrs - pointer to @ref erl_interface_metering_server_attrs_s structure
 */
#define ERL_INTERFACE_DECLARE_METERING_ATTR_LIST(attr_list, attrs)  \
  ERL_INTERFACE_DECLARE_METERING_ATTRIB_LIST(attr_list, &attrs.curr_summ_delivered, &attrs.curr_summ_received, &attrs.fast_poll_update_period,             \
                                             &attrs.active_register_tier_delivered, &attrs.curr_tier1_summ_delivered, &attrs.curr_tier1_summ_received,         \
                                             &attrs.curr_tier2_summ_delivered, &attrs.curr_tier3_summ_delivered, &attrs.curr_tier4_summ_delivered,                 \
                                             &attrs.curr_tier5_summ_delivered, &attrs.curr_tier6_summ_delivered, &attrs.curr_tier7_summ_delivered,                 \
                                             &attrs.curr_tier8_summ_delivered, &attrs.curr_tier9_summ_delivered, &attrs.curr_tier10_summ_delivered,                \
                                             &attrs.status, &attrs.extended_status, &attrs.curr_meter, &attrs.service_disconnect_reason,          \
                                             &attrs.linky_mode_of_operation, &attrs.unit_of_measure, &attrs.multiplier, &attrs.divisor,          \
                                             &attrs.summation_formatting, &attrs.demand_format, &attrs.historical_consumption_format,                      \
                                             &attrs.device_type, &attrs.site, &attrs.meter_serial_number, &attrs.module_serial_number,  \
                                             &attrs.instantaneous_demand, &attrs.curr_day_max_demand_delivered, &attrs.curr_day_max_demand_delivered_time, \
                                             &attrs.curr_day_max_demand_received, &attrs.curr_day_max_demand_received_time,                            \
                                             &attrs.prev_day_max_demand_delivered, &attrs.prev_day_max_demand_delivered_time,                            \
                                             &attrs.prev_day_max_demand_received, &attrs.prev_day_max_demand_received_time,                          \
                                             &attrs.max_number_of_periods_delivered, &attrs.generic_alarm_mask, &attrs.electricity_alarm_mask,       \
                                             &attrs.generic_flow_pressure_alarm_mask, &attrs.water_specific_alarm_mask,                          \
                                             &attrs.heat_and_cooling_specific_alarm_mask, &attrs.gas_specific_alarm_mask,                        \
                                             &attrs.extended_generic_alarm_mask, &attrs.manuf_alarm_mask, &attrs.curr_reactive_summ_q1,            \
                                             &attrs.curr_reactive_summ_q2, &attrs.curr_reactive_summ_q3, &attrs.curr_reactive_summ_q4)


/* Attribute list declaration for ERL Interface Phase Metering Element */

/** @brief Declare attribute list for Metering cluster
* @param[in] attr_list - attribute list variable name
* @param[in] curr_summ_delivered pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID
* @param[in] status - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_STATUS_ID
* @param[in] extended_status - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID
* @param[in] curr_meter - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID
* @param[in] service_disconnect_reason - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID
* @param[in] linky_mode_of_operation - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID
* @param[in] unit_of_measure - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
* @param[in] multiplier - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_MULTIPLIER_ID
* @param[in] divisor - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_DIVISOR_ID
* @param[in] summation_formatting - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID
* @param[in] demand_format - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID
* @param[in] device_type - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
* @param[in] site - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_SITE_ID_ID
* @param[in] meter_serial_number - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID
* @param[in] module_serial_number - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID
* @param[in] instantaneous_demand - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID
* @param[in] curr_day_max_demand_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID
* @param[in] curr_day_max_demand_delivered_time - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID
* @param[in] prev_day_max_demand_delivered - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID
* @param[in] prev_day_max_demand_delivered_time - pointer to variable to store @ref ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID
*/
#define ERL_INTERFACE_PHASE_DECLARE_METERING_ATTRIB_LIST(attr_list, curr_summ_delivered, status, extended_status, curr_meter, \
                                                         service_disconnect_reason, linky_mode_of_operation, unit_of_measure, \
                                                         multiplier, divisor, summation_formatting, demand_format, device_type, \
                                                         site, meter_serial_number, module_serial_number, instantaneous_demand, \
                                                         curr_day_max_demand_delivered, curr_day_max_demand_delivered_time, \
                                                         prev_day_max_demand_delivered, prev_day_max_demand_delivered_time) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, (curr_summ_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_STATUS_ID, (status), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID, (extended_status), ZB_ZCL_ATTR_TYPE_64BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID, (curr_meter), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID, (service_disconnect_reason), ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID, (linky_mode_of_operation), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID, (unit_of_measure), ZB_ZCL_ATTR_TYPE_8BIT_ENUM, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, (multiplier), ZB_ZCL_ATTR_TYPE_U24, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_DIVISOR_ID, (divisor), ZB_ZCL_ATTR_TYPE_U24, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, (summation_formatting), ZB_ZCL_ATTR_TYPE_8BITMAP,ZB_ZCL_ATTR_ACCESS_READ_ONLY ) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID, (demand_format), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID, (device_type), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_SITE_ID_ID, (site), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID, (meter_serial_number), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID, (module_serial_number), ZB_ZCL_ATTR_TYPE_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID, (instantaneous_demand), ZB_ZCL_ATTR_TYPE_S24, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID, (curr_day_max_demand_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID, (curr_day_max_demand_delivered_time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID, (prev_day_max_demand_delivered), ZB_ZCL_ATTR_TYPE_U48, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID, (prev_day_max_demand_delivered_time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
 * Metering cluster attributes
 */
typedef struct erl_interface_phase_metering_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_summ_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID
   */
  zb_uint8_t status;
  /** @copydoc ZB_ZCL_ATTR_METERING_STATUS_ID
   * @see ZB_ZCL_ATTR_METERING_STATUS_ID
   */
  zb_uint64_t extended_status;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID
   */
  zb_uint8_t curr_meter[ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_MAX_LENGTH];
  /** @copydoc ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID
   * @see ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID
   */
  zb_uint8_t service_disconnect_reason;
  /** @copydoc ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID
   * @see ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID
   */
  zb_uint8_t linky_mode_of_operation;
  /** @copydoc ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
   * @see ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
   */
  zb_uint8_t unit_of_measure;
  /** @copydoc ZB_ZCL_ATTR_METERING_MULTIPLIER_ID
   * @see ZB_ZCL_ATTR_METERING_MULTIPLIER_ID
   */
  zb_uint24_t multiplier;
  /** @copydoc ZB_ZCL_ATTR_METERING_DIVISOR_ID
   * @see ZB_ZCL_ATTR_METERING_DIVISOR_ID
   */
  zb_uint24_t divisor;
  /** @copydoc ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID
   * @see ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID
   */
  zb_uint8_t summation_formatting;
  /** @copydoc ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID
   * @see ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID
   */
  zb_uint8_t demand_format;
  /** @copydoc ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
   * @see ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
   */
  zb_uint8_t device_type;
  /** @copydoc ZB_ZCL_ATTR_METERING_SITE_ID_ID
   * @see ZB_ZCL_ATTR_METERING_SITE_ID_ID
   */
  zb_uint8_t site[1 + 32];
  /** @copydoc ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID
   * @see ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID
   */
  zb_uint8_t meter_serial_number[1 + 24];
  /** @copydoc ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID
   * @see ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID
   */
  zb_uint8_t module_serial_number[1 + 24];
  /** @copydoc ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID
   * @see ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID
   */
  zb_int24_t instantaneous_demand;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID
   */
  zb_uint48_t curr_day_max_demand_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID
   */
  zb_uint32_t curr_day_max_demand_delivered_time;
  /** @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID
   */
  zb_uint48_t prev_day_max_demand_delivered;
  /** @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID
   * @see ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID
   */
  zb_uint32_t prev_day_max_demand_delivered_time;
  /** @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_ID
   * @see ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_ID
   */
} erl_interface_phase_metering_server_attrs_t;


/** Initialize @ref erl_interface_phase_metering_server_attrs_s Metering cluster's attributes */
#define ERL_INTERFACE_PHASE_METERING_ATTR_LIST_INIT    \
        (erl_interface_phase_metering_server_attrs_t)  \
        {                              \
          .status                               =  0x00,           \
          .linky_mode_of_operation              =  0x00,           \
          .unit_of_measure                      =  0x00,           \
          .instantaneous_demand                 =  0x00,           \
          .extended_generic_alarm_mask          =  0xffffffffffff}


/** @brief Declare attribute list for Metering cluster
 *  @param[in] attr_list - attribute list variable name
 *  @param[in] attrs - pointer to @ref erl_interface_phase_metering_server_attrs_s structure
 */
#define ERL_INTERFACE_PHASE_DECLARE_METERING_ATTR_LIST(attr_list, attrs)  \
  ERL_INTERFACE_PHASE_DECLARE_METERING_ATTRIB_LIST(attr_list, &attrs.curr_summ_delivered, &attrs.status, \
                                                   &attrs.extended_status, &attrs.curr_meter, \
                                                   &attrs.service_disconnect_reason, &attrs.linky_mode_of_operation, \
                                                   &attrs.unit_of_measure, &attrs.multiplier, &attrs.divisor, \
                                                   &attrs.summation_formatting, &attrs.demand_format, &attrs.device_type, \
                                                   &attrs.site, &attrs.meter_serial_number, &attrs.module_serial_number, \
                                                   &attrs.instantaneous_demand, &attrs.curr_day_max_demand_delivered, \
                                                   &attrs.curr_day_max_demand_delivered_time, &attrs.prev_day_max_demand_delivered, \
                                                   &attrs.prev_day_max_demand_delivered_time)

/** @} */ /* ERL_METERING_CLUSTER_DEFINITIONS */


/** @defgroup ERL_TIME_CLUSTER_DEFINITIONS Time cluster definitions for ERL device
 *  @{
 */

/** @brief Declare attribute list for Time cluster
 * @param[in] attr_list - attribute list variable name
 * @param[in] time - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_TIME_ID
 * @param[in] time_status - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_TIME_STATUS_ID
 * @param[in] time_zone - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_TIME_ZONE_ID
 * @param[in] dst_start - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_DST_START_ID
 * @param[in] dst_end - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_DST_END_ID
 * @param[in] dst_shift - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_DST_SHIFT_ID
 * @param[in] standard_time - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID
 * @param[in] local_time - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID
 * @param[in] last_set_time - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID
 * @param[in] valid_until_time - pointer to variable to store @ref ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID
 */
/**FIXME: in TIME cluster TimeStatus attribute is declarated incorrect. Correct type is ZB_ZCL_ATTR_TYPE_8BITMAP
 * FIXME: ZB_ZCL_ATTR_TIME_DST_SHIFT_ID attribute is also affected
*/
#define ERL_DECLARE_TIME_ATTRIB_LIST(attr_list, zcl_version, application_version, stack_version, \
                                     hw_version, manufacture_name, model_identifier, date_code,  \
                                     power_source)                                               \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                    \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_TIME_ID, (time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_TIME_STATUS_ID, (time_status), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_TIME_ZONE_ID, (time_zone), ZB_ZCL_ATTR_TYPE_S32, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_DST_START_ID, (dst_start), ZB_ZCL_ATTR_TYPE_U32, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_DST_END_ID, (dst_end), ZB_ZCL_ATTR_TYPE_U32, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_DST_SHIFT_ID, (dst_shift), ZB_ZCL_ATTR_TYPE_S32, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID, (standard_time), ZB_ZCL_ATTR_TYPE_U32, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID, (local_time), ZB_ZCL_ATTR_TYPE_U32, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID, (last_set_time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID, (valid_until_time), ZB_ZCL_ATTR_TYPE_UTC_TIME, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
 * Time cluster attributes
 */
typedef struct erl_time_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_TIME_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_TIME_ID
   */
  zb_uint32_t time;
  /** @copydoc ZB_ZCL_ATTR_TIME_TIME_STATUS_ID
   * @see ZB_ZCL_ATTR_TIME_TIME_STATUS_ID
   */
  zb_uint8_t time_status;
  /** @copydoc ZB_ZCL_ATTR_TIME_TIME_ZONE_ID
   * @see ZB_ZCL_ATTR_TIME_TIME_ZONE_ID
   */
  zb_int32_t time_zone;
  /** @copydoc ZB_ZCL_ATTR_TIME_DST_START_ID
   * @see ZB_ZCL_ATTR_TIME_DST_START_ID
   */
  zb_uint32_t dst_start;
  /** @copydoc ZB_ZCL_ATTR_TIME_DST_END_ID
   * @see ZB_ZCL_ATTR_TIME_DST_END_ID
   */
  zb_uint32_t dst_end;
  /** @copydoc ZB_ZCL_ATTR_TIME_DST_SHIFT_ID
   * @see ZB_ZCL_ATTR_TIME_DST_SHIFT_ID
   */
  zb_int32_t dst_shift;
  /** @copydoc ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID
   */
  zb_uint32_t standard_time;
  /** @copydoc ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID
   */
  zb_uint32_t local_time;
  /** @copydoc ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID
   */
  zb_uint32_t last_set_time;
  /** @copydoc ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID
   */
  zb_uint32_t valid_until_time;
} erl_time_server_attrs_t;


#define ZB_ZCL_ATTR_TIME_TIME_DEFAULT 0xffffffff
#define ZB_ZCL_ATTR_TIME_TIME_STATUS_DEFAULT 0x00
#define ZB_ZCL_ATTR_TIME_TIME_ZONE_DEFAULT 0x00000000
#define ZB_ZCL_ATTR_TIME_DST_START_DEFAULT 0xffffffff
#define ZB_ZCL_ATTR_TIME_DST_END_DEFAULT 0xffffffff
#define ZB_ZCL_ATTR_TIME_DST_SHIFT_DEFAULT 0x00000000
#define ZB_ZCL_ATTR_TIME_STANDARD_TIME_DEFAULT 0xffffffff
#define ZB_ZCL_ATTR_TIME_LOCAL_TIME_DEFAULT 0xffffffff
#define ZB_ZCL_ATTR_TIME_LAST_SET_TIME_DEFAULT 0xffffffff
#define ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_DEFAULT 0xffffffff


/** Initialize @ref erl_time_server_attrs_s Time cluster's attributes */
#define ERL_TIME_ATTR_LIST_INIT    \
  (erl_time_server_attrs_t)         \
  { .time = ZB_ZCL_ATTR_TIME_TIME_DEFAULT, \
    .time_status = ZB_ZCL_ATTR_TIME_TIME_STATUS_DEFAULT, \
    .time_zone = ZB_ZCL_ATTR_TIME_TIME_ZONE_DEFAULT, \
    .dst_start = ZB_ZCL_ATTR_TIME_DST_START_DEFAULT, \
    .dst_end = ZB_ZCL_ATTR_TIME_DST_END_DEFAULT, \
    .dst_shift = ZB_ZCL_ATTR_TIME_DST_SHIFT_DEFAULT, \
    .standard_time = ZB_ZCL_ATTR_TIME_STANDARD_TIME_DEFAULT, \
    .local_time = ZB_ZCL_ATTR_TIME_LOCAL_TIME_DEFAULT, \
    .last_set_time = ZB_ZCL_ATTR_TIME_LAST_SET_TIME_DEFAULT, \
    .valid_until_time = ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_DEFAULT}


/** @brief Declare attribute list for Time cluster
*  @param[in] attr_list - attribute list variable name
*  @param[in] attrs - pointer to @ref erl_time_server_attrs_s structure
*/
#define ERL_DECLARE_TIME_ATTR_LIST(attr_list, attrs)  \
  ERL_DECLARE_TIME_ATTRIB_LIST(attr_list, &attrs.time, &attrs.time_status, &attrs.time_zone, \
                                &attrs.dst_start, &attrs.dst_end, &attrs.dst_shift, \
                                &attrs.standard_time, &attrs.local_time, &attrs.last_set_time, \
                                &attrs.valid_until_time)


/** @} */ /* ERL_TIME_CLUSTER_DEFINITIONS */


/** @defgroup ERL_DIAGNOSTICS_CLUSTER_DEFINITIONS Diagnostics cluster definitions for ERL device
 *  @{
 */

/** @brief Declare attribute list for Diagnostics cluster
 * @param[in] attr_list - attribute list variable name
 * @param[in] number_of_resets - pointer to variable to store @ref ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID
 * @param[in] last_msg_lqi - pointer to variable to store @ref ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID
 * @param[in] last_msg_rssi - pointer to variable to store @ref ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID
 */
/* FIXME: For ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID attribute type is changed to zb_uint8_t */
#define ERL_DECLARE_DIAGNOSTICS_ATTRIB_LIST(attr_list, number_of_resets, last_msg_lqi, last_msg_rssi) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                         \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID, (number_of_resets), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID, (last_msg_lqi), ZB_ZCL_ATTR_TYPE_U8, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID, (last_msg_rssi), ZB_ZCL_ATTR_TYPE_U8, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/**
* Diagnostics cluster attributes
*/
typedef struct erl_diagnostics_server_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID
   * @see ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_ID
   */
  zb_uint16_t number_of_resets;
  /** @copydoc ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID
   * @see ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_ID
   */
  zb_uint8_t last_msg_lqi;
  /** @copydoc ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID
   * @see ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_ID
   */
  zb_uint8_t last_msg_rssi;

} erl_diagnostics_server_attrs_t;


#define ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_DEFAULT 0x00000000
#define ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_DEFAULT 0
#define ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_DEFAULT 0


/** Initialize @ref erl_diagnostics_server_attrs_s Diagnostics cluster's attributes */
#define ERL_DIAGNOSTICS_ATTR_LIST_INIT    \
  (erl_diagnostics_server_attrs_t)            \
  { .number_of_resets = ZB_ZCL_ATTR_DIAGNOSTICS_NUMBER_OF_RESETS_DEFAULT, \
    .last_msg_lqi = ZB_ZCL_ATTR_DIAGNOSTICS_LAST_LQI_DEFAULT,             \
    .last_msg_rssi = ZB_ZCL_ATTR_DIAGNOSTICS_LAST_RSSI_DEFAULT}


/** @brief Declare attribute list for Diagnostics cluster
* @param[in] attr_list - attribute list variable name
* @param[in] attrs - pointer to @ref erl_diagnostics_server_attrs_s structure
*/
#define ERL_DECLARE_DIAGNOSTICS_ATTR_LIST(attr_list, attrs)  \
  ERL_DECLARE_DIAGNOSTICS_ATTRIB_LIST(attr_list, &attrs.number_of_resets, &attrs.last_msg_lqi, &attrs.last_msg_rssi)


/** @} */ /* ERL_DIAGNOSTICS_CLUSTER_DEFINITIONS */


/** @endcond */ /* DOXYGEN_ERL_SECTION */
