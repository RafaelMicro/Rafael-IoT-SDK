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
#define SUPPORT_QSPI0_MULTI_CS             0

#define SET_SYS_CLK    SYS_CLK_64MHZ
#define RF_FW_INCLUDE_PCI           (TRUE)
#define RF_FW_INCLUDE_BLE           (FALSE)
#define RF_FW_INCLUDE_MULTI_2P4G    (FALSE)

#define SUPPORT_MULTITASKING        (TRUE)
#define CRYPTO_SECP256R1_ENABLE     (TRUE)


#define __reloc __attribute__ ((used, section("reloc_text")))
