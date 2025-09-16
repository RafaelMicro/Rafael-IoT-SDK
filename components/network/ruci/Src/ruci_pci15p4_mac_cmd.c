/******************************************************************************
*
* @File         ruci_pci15p4_mac_cmd.c
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
#include "ruci_pci15p4_mac_cmd.h"

#if (RUCI_ENDIAN_INVERSE)
#if (RUCI_ENABLE_PCI)

/******************************************************************************
* GLOBAL PARAMETERS
******************************************************************************/
// RUCI: initiate_zigbee -------------------------------------------------------
const uint8_t ruci_elmt_type_initiate_zigbee[] =
{
    1, 1, 1
};
const uint8_t ruci_elmt_num_initiate_zigbee[] =
{
    1, 1, 1
};

// RUCI: set15p4_address_filter ------------------------------------------------
const uint8_t ruci_elmt_type_set15p4_address_filter[] =
{
    1, 1, 1, 1, 2, 4, 2, 1
};
const uint8_t ruci_elmt_num_set15p4_address_filter[] =
{
    1, 1, 1, 1, 1, 2, 1, 1
};

// RUCI: set15p4_mac_pib -------------------------------------------------------
const uint8_t ruci_elmt_type_set15p4_mac_pib[] =
{
    1, 1, 1, 4, 4, 1, 1, 4, 1, 1
};
const uint8_t ruci_elmt_num_set15p4_mac_pib[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

// RUCI: set15p4_auto_ack ------------------------------------------------------
const uint8_t ruci_elmt_type_set15p4_auto_ack[] =
{
    1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set15p4_auto_ack[] =
{
    1, 1, 1, 1
};

// RUCI: set15p4_phy_pib -------------------------------------------------------
const uint8_t ruci_elmt_type_set15p4_phy_pib[] =
{
    1, 1, 1, 2, 1, 1, 2
};
const uint8_t ruci_elmt_num_set15p4_phy_pib[] =
{
    1, 1, 1, 1, 1, 1, 1
};

// RUCI: set15p4_pending_bit ---------------------------------------------------
const uint8_t ruci_elmt_type_set15p4_pending_bit[] =
{
    1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set15p4_pending_bit[] =
{
    1, 1, 1, 1
};

// RUCI: set_src_match_enable --------------------------------------------------
const uint8_t ruci_elmt_type_set_src_match_enable[] =
{
    1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set_src_match_enable[] =
{
    1, 1, 1, 1
};

// RUCI: set_src_match_short_entry_ctrl ----------------------------------------
const uint8_t ruci_elmt_type_set_src_match_short_entry_ctrl[] =
{
    1, 1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set_src_match_short_entry_ctrl[] =
{
    1, 1, 1, 1, 2
};

// RUCI: set_src_match_extended_entry_ctrl -------------------------------------
const uint8_t ruci_elmt_type_set_src_match_extended_entry_ctrl[] =
{
    1, 1, 1, 1, 1
};
const uint8_t ruci_elmt_num_set_src_match_extended_entry_ctrl[] =
{
    1, 1, 1, 1, 8
};

// RUCI: set_csl_receiver_ctrl -------------------------------------------------
const uint8_t ruci_elmt_type_set_csl_receiver_ctrl[] =
{
    1, 1, 1, 1, 4
};
const uint8_t ruci_elmt_num_set_csl_receiver_ctrl[] =
{
    1, 1, 1, 1, 1
};

// RUCI: get_csl_accuracy ------------------------------------------------------
const uint8_t ruci_elmt_type_get_csl_accuracy[] =
{
    1, 1, 1
};
const uint8_t ruci_elmt_num_get_csl_accuracy[] =
{
    1, 1, 1
};

// RUCI: get_csl_uncertainty ---------------------------------------------------
const uint8_t ruci_elmt_type_get_csl_uncertainty[] =
{
    1, 1, 1
};
const uint8_t ruci_elmt_num_get_csl_uncertainty[] =
{
    1, 1, 1
};

// RUCI: update_csl_sample_time ------------------------------------------------
const uint8_t ruci_elmt_type_update_csl_sample_time[] =
{
    1, 1, 1, 4
};
const uint8_t ruci_elmt_num_update_csl_sample_time[] =
{
    1, 1, 1, 1
};

#endif /* RUCI_ENABLE_PCI */
#endif /* RUCI_ENDIAN_INVERSE */
