/******************************************************************************
*
* @File         ruci_cmn_event.c
* @Version
* $Revision:6351
* $Date: 2023-11-17
* @Brief
* @Note
* Copyright (C) 2023 Rafael Microelectronics Inc. All rights reserved.
*
*****************************************************************************/

/******************************************************************************
* INCLUDES
******************************************************************************/
#include "ruci_cmn_event.h"

#if (RUCI_ENDIAN_INVERSE)
#if (RUCI_ENABLE_CMN)

/******************************************************************************
* GLOBAL PARAMETERS
******************************************************************************/
// RUCI: cmn_cnf_event ---------------------------------------------------------
const uint8_t ruci_elmt_type_cmn_cnf_event[] =
{
    1, 1, 1, 1, 1, 1
};
const uint8_t ruci_elmt_num_cmn_cnf_event[] =
{
    1, 1, 1, 1, 1, 1
};

// RUCI: get_fw_ver_event ------------------------------------------------------
const uint8_t ruci_elmt_type_get_fw_ver_event[] =
{
    1, 1, 1, 4, 1, 1, 1, 4
};
const uint8_t ruci_elmt_num_get_fw_ver_event[] =
{
    1, 1, 1, 1, 1, 1, 1, 1
};

// RUCI: set_calibration_enable_event ------------------------------------------
const uint8_t ruci_elmt_type_set_calibration_enable_event[] =
{
    1, 1, 1, 1, 1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set_calibration_enable_event[] =
{
    1, 1, 1, 1, 1, 4, 2, 3
};

// RUCI: get_temperature_rpt_event ---------------------------------------------
const uint8_t ruci_elmt_type_get_temperature_rpt_event[] =
{
    1, 1, 1, 4, 1
};
const uint8_t ruci_elmt_num_get_temperature_rpt_event[] =
{
    1, 1, 1, 1, 1
};

// RUCI: get_voltage_rpt_event -------------------------------------------------
const uint8_t ruci_elmt_type_get_voltage_rpt_event[] =
{
    1, 1, 1, 4, 1
};
const uint8_t ruci_elmt_num_get_voltage_rpt_event[] =
{
    1, 1, 1, 1, 1
};

#endif /* RUCI_ENABLE_CMN */
#endif /* RUCI_ENDIAN_INVERSE */
