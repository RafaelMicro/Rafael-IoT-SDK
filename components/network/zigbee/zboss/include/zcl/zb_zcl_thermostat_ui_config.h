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
/* PURPOSE: Thermostat UI Configuration cluster defintions
*/

#ifndef ZB_ZCL_THERMOSTAT_UI_CONFIG_H
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/* Cluster ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG */

/*! @addtogroup ZB_ZCL_THERMOSTAT_UI_CONFIG
    @{
    @name Thermostat UI Configuration cluster attributes
    @{
*/

/*! @brief Thermostat UI Configuration cluster attribute identifiers
    @see ZCL spec, subclause 6.6.2.2
*/
enum zb_zcl_thermostat_ui_config_attr_e
{
  /** @brief Temperature Display Mode attribute */
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID        = 0x0000,
  /** @brief Keypad Lockout attribute */
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID                  = 0x0001,
  /** The ScheduleProgrammingVisibility attribute is used to hide the weekly
   *  schedule programming functionality or menu on a thermostat from a user
   *  to prevent local user programming of the weekly schedule. */
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_SCHEDULE_PROGRAMMING_VISIBILITY_ID = 0x0002,
};

/*! @brief Values for Temperature Display Mode attribute,
    @see ZCL spec, subclause 6.6.2.2.1 */
enum zb_zcl_thermostat_ui_config_temperature_display_mode_e
{
  /*! Temperature in C value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_IN_C     = 0x00,
  /*! Temperature in F value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_IN_F     = 0x01,

  ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_RESERVED = 0x02
};

/*! @brief Values for Keypad Lockout attribute,
    @see ZCL spec, subclause 6.6.2.2.2 */
enum zb_zcl_thermostat_ui_config_keypad_lockout_e
{
  /*! No Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_NO_LOCKOUT = 0x00,
  /*! Level 1 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_1    = 0x01,
  /*! Level 2 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_2    = 0x02,
  /*! Level 3 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_3    = 0x03,
  /*! Level 4 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_4    = 0x04,
  /*! Level 5 Lockout value */
  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_LEVEL_5    = 0x05,

  ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_RESERVED   = 0x06
};

/** @brief Default value for Temperature Display Mode attribute */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_DEFAULT_VALUE 0x00

/** @brief Default value for Keypad Lockout attribute */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_DEFAULT_VALUE 0x00

/** @brief Default value for ScheduleProgrammingVisibility attribute */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_SCHEDULE_PROGRAMMING_VISIBILITY_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Declare attribute list for Thermostat UI Configuration cluster
    @param attr_list - attribute list name
    @param temperature_display_mode - pointer to variable to store Temperature Display Mode attribute value
    @param keypad_lockout - pointer to variable to store Keypad Lockout attribute value
*/
#define ZB_ZCL_DECLARE_THERMOSTAT_UI_CONFIG_ATTRIB_LIST(attr_list, temperature_display_mode, keypad_lockout)     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID, (temperature_display_mode)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID, (keypad_lockout))                     \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Thermostat UI Configuration cluster attributes */

/*! @name Thermostat UI Configuration cluster commands
    @{
*/

/*! @} */ /* Thermostat UI Configuration cluster commands */

/*! @cond internals_doc
    @internal @name Thermostat UI Configuration cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID(data_ptr) \
{                                                                                                     \
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_TEMPERATURE_DISPLAY_MODE_ID,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                      \
  (void*) data_ptr                                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID(data_ptr)           \
{                                                                                                     \
  ZB_ZCL_ATTR_THERMOSTAT_UI_CONFIG_KEYPAD_LOCKOUT_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                      \
  (void*) data_ptr                                                                               \
}

/*! @internal Number of attributes mandatory for reporting in Thermostat UI Configuration cluster */
#define ZB_ZCL_THERMOSTAT_UI_CONFIG_REPORT_ATTR_COUNT 0

/*! @}
    @endcond */ /* Thermostat UI Configuration cluster internals */

/*! @} */ /* ZCL HA Thermostat UI Configuration cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_thermostat_ui_config_init_server(void);
void zb_zcl_thermostat_ui_config_init_client(void);
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG_SERVER_ROLE_INIT zb_zcl_thermostat_ui_config_init_server
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG_CLIENT_ROLE_INIT zb_zcl_thermostat_ui_config_init_client

#endif /* ZB_ZCL_THERMOSTAT_UI_CONFIG_H */
