/******************************************************************************
*
* @File         ruci_sf_host_cmd.c
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
#include "ruci_sf_host_cmd.h"

#if (RUCI_ENDIAN_INVERSE)
#if (RUCI_ENABLE_SF)

/******************************************************************************
* GLOBAL PARAMETERS
******************************************************************************/
// RUCI: sfr_read --------------------------------------------------------------
const uint8_t ruci_elmt_type_sfr_read[] =
{
    1, 1, 2, 1
};
const uint8_t ruci_elmt_num_sfr_read[] =
{
    1, 1, 1, 1
};

// RUCI: sfr_write -------------------------------------------------------------
const uint8_t ruci_elmt_type_sfr_write[] =
{
    1, 1, 2, 1, 1
};
const uint8_t ruci_elmt_num_sfr_write[] =
{
    1, 1, 1, 1, 1
};

// RUCI: io_read ---------------------------------------------------------------
const uint8_t ruci_elmt_type_io_read[] =
{
    1, 1, 2, 1, 2
};
const uint8_t ruci_elmt_num_io_read[] =
{
    1, 1, 1, 1, 1
};

// RUCI: io_write --------------------------------------------------------------
const uint8_t ruci_elmt_type_io_write[] =
{
    1, 1, 2, 1, 2, 1
};
const uint8_t ruci_elmt_num_io_write[] =
{
    1, 1, 1, 1, 1, 2110
};

// RUCI: mem_read --------------------------------------------------------------
const uint8_t ruci_elmt_type_mem_read[] =
{
    1, 1, 2, 2, 2
};
const uint8_t ruci_elmt_num_mem_read[] =
{
    1, 1, 1, 1, 1
};

// RUCI: mem_write -------------------------------------------------------------
const uint8_t ruci_elmt_type_mem_write[] =
{
    1, 1, 2, 2, 2, 1
};
const uint8_t ruci_elmt_num_mem_write[] =
{
    1, 1, 1, 1, 1, 2048
};

// RUCI: reg_read --------------------------------------------------------------
const uint8_t ruci_elmt_type_reg_read[] =
{
    1, 1, 2, 4
};
const uint8_t ruci_elmt_num_reg_read[] =
{
    1, 1, 1, 1
};

// RUCI: reg_write -------------------------------------------------------------
const uint8_t ruci_elmt_type_reg_write[] =
{
    1, 1, 2, 4, 4
};
const uint8_t ruci_elmt_num_reg_write[] =
{
    1, 1, 1, 1, 1
};

#endif /* RUCI_ENABLE_SF */
#endif /* RUCI_ENDIAN_INVERSE */
