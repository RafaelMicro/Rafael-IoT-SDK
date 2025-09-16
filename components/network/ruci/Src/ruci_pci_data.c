/******************************************************************************
*
* @File         ruci_pci_data.c
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
#include "ruci_pci_data.h"

#if (RUCI_ENDIAN_INVERSE)
#if (RUCI_ENABLE_PCI)

/******************************************************************************
* GLOBAL PARAMETERS
******************************************************************************/
// RUCI: set_tx_control_field --------------------------------------------------
const uint8_t ruci_elmt_type_set_tx_control_field[] =
{
    1, 1, 2, 1, 1
};
const uint8_t ruci_elmt_num_set_tx_control_field[] =
{
    1, 1, 1, 1, 1
};

// RUCI: rx_control_field ------------------------------------------------------
const uint8_t ruci_elmt_type_rx_control_field[] =
{
    1, 1, 2, 1, 1, 1
};
const uint8_t ruci_elmt_num_rx_control_field[] =
{
    1, 1, 1, 1, 1, 1
};

#endif /* RUCI_ENABLE_PCI */
#endif /* RUCI_ENDIAN_INVERSE */
