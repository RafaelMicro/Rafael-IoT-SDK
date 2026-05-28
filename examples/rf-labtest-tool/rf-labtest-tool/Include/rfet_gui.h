/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

/**
 * @file rfet_gui.h
 * @author
 * @date
 * @brief Brief single line description use for indexing
 *
 * More detailed description can go here
 *
 *
 * @see http://
 */
#ifndef _RFET_GUI_H_
#define _RFET_GUI_H_

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "rf_common_init.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/

/**************************************************************************************************
*    TYPEDEFS
*************************************************************************************************/
typedef uint8_t RFET_GUI_STS;
#define RFET_GUI_STS_SUCCESS                ((RFET_GUI_STS)(0x00))
#define RFET_GUI_STS_FAIL                   ((RFET_GUI_STS)(0x01))
#define RFET_GUI_STS_SLEEP                  ((RFET_GUI_STS)(0x10))

typedef uint8_t RFET_GUI_CTRL;
#define RFET_GUI_CTRL_NONE                  ((RFET_GUI_CTRL)(0x00))
#define RFET_GUI_CTRL_READ_CMDQ             ((RFET_GUI_CTRL)(0x01))
#define RFET_GUI_CTRL_READ_DATAQ            ((RFET_GUI_CTRL)(0x02))
#define RFET_GUI_CTRL_READ_MCU_STATE        ((RFET_GUI_CTRL)(0x04))
/**************************************************************************************************
 *    Global Prototypes
 *************************************************************************************************/
void rfet_gui_command_control(uint8_t *rx_data_ptr, uint16_t rx_len);
void rfet_gui_cc_default_entry(uint8_t *rx_data_ptr, uint16_t rx_len);
bool rfet_gui_init(RF_FW_LOAD_SELECT fw_select);
bool rfet_gui_is_busy(void);
void rfet_gui_task(void);
void rfet_gui_mutex_unlock(void);

#endif  //_RFET_GUI_H_

