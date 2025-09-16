/******************************************************************************
*
* @File         ruci_pci_fsk_cmd.c
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
#include "ruci_pci_fsk_cmd.h"

#if (RUCI_ENDIAN_INVERSE)
#if (RUCI_ENABLE_PCI)

/******************************************************************************
* GLOBAL PARAMETERS
******************************************************************************/
// RUCI: initiate_fsk ----------------------------------------------------------
const uint8_t ruci_elmt_type_initiate_fsk[] =
{
    1, 1, 1, 1
};
const uint8_t ruci_elmt_num_initiate_fsk[] =
{
    1, 1, 1, 1
};

// RUCI: set_fsk_modem ---------------------------------------------------------
const uint8_t ruci_elmt_type_set_fsk_modem[] =
{
    1, 1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set_fsk_modem[] =
{
    1, 1, 1, 1, 1
};

// RUCI: set_fsk_mac -----------------------------------------------------------
const uint8_t ruci_elmt_type_set_fsk_mac[] =
{
    1, 1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set_fsk_mac[] =
{
    1, 1, 1, 1, 1
};

// RUCI: set_fsk_preamble ------------------------------------------------------
const uint8_t ruci_elmt_type_set_fsk_preamble[] =
{
    1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set_fsk_preamble[] =
{
    1, 1, 1, 1
};

// RUCI: set_fsk_sfd -----------------------------------------------------------
const uint8_t ruci_elmt_type_set_fsk_sfd[] =
{
    1, 1, 1, 4
};
const uint8_t ruci_elmt_num_set_fsk_sfd[] =
{
    1, 1, 1, 1
};

// RUCI: set_fsk_type ----------------------------------------------------------
const uint8_t ruci_elmt_type_set_fsk_type[] =
{
    1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set_fsk_type[] =
{
    1, 1, 1, 1
};

#endif /* RUCI_ENABLE_PCI */
#endif /* RUCI_ENDIAN_INVERSE */
