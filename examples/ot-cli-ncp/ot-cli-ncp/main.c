#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "EnhancedFlashDataset.h"
#include "FreeRTOS.h"
#include "cli.h"
#include "hosal_rf.h"
#include "lmac15p4.h"
#include "log.h"
#include "main.h"
#include "mcu.h"
#include "openthread_port.h"
#include "task.h"
#include "uart.h"

#define PHY_PIB_TURNAROUND_TIMER    192
#define PHY_PIB_CCA_DETECTED_TIME   128 // 8 symbols
#define PHY_PIB_CCA_DETECT_MODE     0
#define PHY_PIB_CCA_THRESHOLD       75
#define MAC_PIB_UNIT_BACKOFF_PERIOD 320
#define MAC_PIB_MAC_ACK_WAIT_DURATION                                          \
    544 // non-beacon mode; 864 for beacon mode
#define MAC_PIB_MAC_MAX_BE                    8
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME 16416
#define MAC_PIB_MAC_MAX_FRAME_RETRIES         4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS       10
#define MAC_PIB_MAC_MIN_BE                    3

int main(void) {
    enhanced_flash_dataset_init();
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);

    log_info("Thread 2.4G\r\n");
    lmac15p4_init(LMAC15P4_2P4G_OQPSK, 0);

    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE,
                         PHY_PIB_CCA_THRESHOLD, PHY_PIB_CCA_DETECTED_TIME);

    lmac15p4_mac_pib_set(MAC_PIB_UNIT_BACKOFF_PERIOD,
                         MAC_PIB_MAC_ACK_WAIT_DURATION, MAC_PIB_MAC_MAX_BE,
                         MAC_PIB_MAC_MAX_CSMACA_BACKOFFS,
                         MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME,
                         MAC_PIB_MAC_MAX_FRAME_RETRIES, MAC_PIB_MAC_MIN_BE);

    cli_init();

    otrStart();
    // app_task();

    return 0;
}
