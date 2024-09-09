/**************************************************************************//**
* @file     rt569mp_fw.c
* @version
* @brief    RF MCU FW initialization

******************************************************************************/

#include "mcu.h"
#if 0
#include "chip_define.h"
#endif
#include "rf_mcu.h"
#include "rf_mcu_chip.h"
#include "rf_common_init.h"


/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/
#if (RF_MCU_CHIP_MODEL == RF_MCU_CHIP_569S)
/* Use top level CHIP_VERSION */

#if ((CHIP_VERSION == RT58X_MPA) || (CHIP_VERSION == RT58X_SHUTTLE))
#if (RF_FW_INCLUDE_PCI == TRUE)
#if (RF_MCU_CHIP_TYPE == RF_MCU_TYPE_FPGA_RFT3)
#include "prg_pci_s_fpgat3_fw.h"
#else
#include "prg_pci_sa_asic_fw.h"
#endif
const uint32_t firmware_size_ruci = sizeof(firmware_program_ruci);
#else
const uint8_t firmware_program_ruci[] = {0};
const uint32_t firmware_size_ruci = 0;
#endif

#if (RF_FW_INCLUDE_BLE == TRUE)
#if (RF_MCU_CHIP_TYPE == RF_MCU_TYPE_FPGA_RFT3)
#include "prg_ble_s_fpgat3_fw.h"
#else
#include "prg_ble_sa_asic_fw.h"
#endif
const uint32_t firmware_size_ble = sizeof(firmware_program_ble);
#else
const uint8_t firmware_program_ble[] = {0};
const uint32_t firmware_size_ble = 0;
#endif

#if (RF_FW_INCLUDE_MULTI_2P4G == TRUE)
#include "prg_multi_mp_asic_fw.h"
const uint32_t firmware_size_multi = sizeof(firmware_program_multi);
#else
const uint8_t firmware_program_multi[] = {0};
const uint32_t firmware_size_multi = 0;
#endif

#if (RF_CAL_TYPE & RF_CAL_PWR_ON_MODE)
#if (RF_FW_INCLUDE_PCI == TRUE)
const uint8_t *firmware_program_rfk = firmware_program_ruci;
const uint32_t firmware_size_rfk = sizeof(firmware_program_ruci);
#else
#error "Enable RF_FW_INCLUDE_PCI for RF_CAL_PWR_ON_MODE"
#endif
#else
const uint8_t firmware_program_rfk[] = {0};
const uint32_t firmware_size_rfk = 0;
#endif

#elif (CHIP_VERSION == RT58X_MPB)
#if (RF_FW_INCLUDE_PCI == TRUE)
#include "prg_pci_sb_asic_fw.h"
const uint32_t firmware_size_ruci = sizeof(firmware_program_ruci);
#else
const uint8_t firmware_program_ruci[] = {0};
const uint32_t firmware_size_ruci = 0;
#endif

#if (RF_FW_INCLUDE_BLE == TRUE)
#include "prg_ble_sb_asic_fw.h"
const uint32_t firmware_size_ble = sizeof(firmware_program_ble);
#else
const uint8_t firmware_program_ble[] = {0};
const uint32_t firmware_size_ble = 0;
#endif

#if (RF_FW_INCLUDE_MULTI_2P4G == TRUE)
#include "prg_multi_sb_asic_fw.h"
const uint32_t firmware_size_multi = sizeof(firmware_program_multi);
#else
const uint8_t firmware_program_multi[] = {0};
const uint32_t firmware_size_multi = 0;
#endif

#if (RF_FW_INCLUDE_INTERNAL_TEST == TRUE)
const uint8_t firmware_program_it[] = {0};
const uint32_t firmware_size_it = 0;
#if (RF_MCU_CONST_LOAD_SUPPORTED)
const uint8_t firmware_const_it[] = {0};
const uint32_t const_size_it = 0;
#endif
#endif


#if (RF_CAL_TYPE & RF_CAL_PWR_ON_MODE)
#if (RF_FW_INCLUDE_PCI == TRUE)
const uint8_t *firmware_program_rfk = firmware_program_ruci;
const uint32_t firmware_size_rfk = sizeof(firmware_program_ruci);
#else
#error "Enable RF_FW_INCLUDE_PCI for RF_CAL_PWR_ON_MODE"
#endif
#else
const uint8_t firmware_program_rfk[] = {0};
const uint32_t firmware_size_rfk = 0;
#endif

#else
#error "Error: Invalid RF chip version!!"
#endif

#endif

