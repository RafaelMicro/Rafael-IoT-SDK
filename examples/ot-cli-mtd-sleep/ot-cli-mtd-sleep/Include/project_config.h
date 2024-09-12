#include "chip_define.h"
#include "mcu.h"


#define MODULE_ENABLE(module) (module > 0)

/*System use UART0 */
#define SUPPORT_UART0                      1

/*System use UART1 */
#define SUPPORT_UART1                      1
#define SUPPORT_UART1_TX_DMA               1
#define SUPPORT_UART1_RX_DMA               1

#define SUPPORT_QSPI_DMA                   1

/*Support AES  */
#define CRYPTO_AES_ENABLE                  1


#define SET_SYS_CLK    SYS_CLK_64MHZ
#define RF_FW_INCLUDE_PCI           (1)
#define RF_FW_INCLUDE_BLE           (0)
#define RF_FW_INCLUDE_MULTI_2P4G    (0)

#if  MODULE_ENABLE(RF_FW_INCLUDE_MULTI_2P4G)
#define SUPPORT_BLE                 (1)
#define SUPPORT_15P4                (1)
#define RFB_ZIGBEE_ENABLED          (0)
#define RFB_MULTI_ENABLED           (1)
#define RF_MCU_CONST_LOAD_SUPPORTED (0x01)
#endif

#if  MODULE_ENABLE(RF_FW_INCLUDE_BLE)
#define SUPPORT_BLE                 (1)
#define SUPPORT_15P4                (0)
#define RFB_ZIGBEE_ENABLED          (0)
#define RFB_MULTI_ENABLED           (0)
#endif

#if  MODULE_ENABLE(RF_FW_INCLUDE_PCI)
#define SUPPORT_BLE                 (0)
#define SUPPORT_15P4                (1)
#define RFB_ZIGBEE_ENABLED          (0)
#define RFB_MULTI_ENABLED           (0)
#endif
#define SUPPORT_MULTITASKING        (TRUE)
#define CRYPTO_SECP256R1_ENABLE     (TRUE)

#define __reloc __attribute__ ((used, section("reloc_text")))