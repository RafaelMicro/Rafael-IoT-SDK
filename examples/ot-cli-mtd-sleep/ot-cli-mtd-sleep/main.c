#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config/platform.h>
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

#ifdef CONFIG_BUILD_COMPONENT_SUBG_CTRL
#include "subg_ctrl.h"
#endif

#if OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT
#define PHY_PIB_TURNAROUND_TIMER    192
#define PHY_PIB_CCA_DETECTED_TIME   128 // 8 symbols
#define PHY_PIB_CCA_DETECT_MODE     0
#define PHY_PIB_CCA_THRESHOLD       85
#define MAC_PIB_UNIT_BACKOFF_PERIOD 320
#define MAC_PIB_MAC_ACK_WAIT_DURATION                                          \
    544 // non-beacon mode; 864 for beacon mode
#define MAC_PIB_MAC_MAX_BE                    8
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME 16416
#define MAC_PIB_MAC_MAX_FRAME_RETRIES         4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS       10
#define MAC_PIB_MAC_MIN_BE                    5
#elif CONFIG_BUILD_COMPONENT_OPENTHREAD_SUBG
#define PHY_PIB_TURNAROUND_TIMER  1000
#define PHY_PIB_CCA_DETECTED_TIME 640 // 8 symbols for 50 kbps-data rate
#define PHY_PIB_CCA_DETECT_MODE   0
#define PHY_PIB_CCA_THRESHOLD     75
#define MAC_PIB_UNIT_BACKOFF_PERIOD                                            \
    1640 // PHY_PIB_TURNAROUND_TIMER + PHY_PIB_CCA_DETECTED_TIME
#define MAC_PIB_MAC_ACK_WAIT_DURATION         2000 // oqpsk neee more then 2000
#define MAC_PIB_MAC_MAX_BE                    5
#define MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME 82080 // for 50 kbps-data rate
#define MAC_PIB_MAC_MAX_FRAME_RETRIES         4
#define MAC_PIB_MAC_MAX_CSMACA_BACKOFFS       4
#define MAC_PIB_MAC_MIN_BE                    2
static uint16_t cca_duration_table[] = {0, 0, 0, 160, 320, 640, 128, 213};
static uint32_t frame_total_wait_time_table[] = {0,     0,     0,      20520,
                                                 41040, 82080, 164160, 273600};
uint16_t cca_duration = 0;
uint16_t frame_total_wait_time = 0;
uint32_t backof_period = 0;
#endif
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_915M;
static uint8_t sPhyPowerIndex = 30;
static uint32_t sPhyFrequency = CONFIG_APP_SUBG_FREQUENCY;
static uint32_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_50K;

int main(void) {
    enhanced_flash_dataset_init();
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);

#if CONFIG_BUILD_COMPONENT_OPENTHREAD_SUBG

    log_info("Sub-G Thread  433 \r\n");

    cca_duration = cca_duration_table[sPhyDataRate];
    frame_total_wait_time = frame_total_wait_time_table[sPhyDataRate];
    backof_period = PHY_PIB_TURNAROUND_TIMER + cca_duration;

    lmac15p4_init(LMAC15P4_SUBG_FSK, HOSAL_RF_BAND_SUBG_433M);

    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE,
                         PHY_PIB_CCA_THRESHOLD, cca_duration);

    lmac15p4_mac_pib_set(backof_period, MAC_PIB_MAC_ACK_WAIT_DURATION,
                         MAC_PIB_MAC_MAX_BE, MAC_PIB_MAC_MAX_CSMACA_BACKOFFS,
                         MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME,
                         MAC_PIB_MAC_MAX_FRAME_RETRIES, MAC_PIB_MAC_MIN_BE);

    subg_ctrl_sleep_set(false);
    subg_ctrl_idle_set();
    subg_ctrl_modem_config_set(SUBG_CTRL_MODU_FSK, sPhyDataRate,
                               SUBG_CTRL_FSK_MOD_1);
    subg_ctrl_mac_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_CRC_TYPE_16,
                      SUBG_CTRL_WHITEN_DISABLE);
    subg_ctrl_preamble_set(SUBG_CTRL_MODU_FSK, 8);
    subg_ctrl_sfd_set(SUBG_CTRL_MODU_FSK, 0x00007209);
    subg_ctrl_filter_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_FILTER_TYPE_GFSK);
#elif CONFIG_BUILD_COMPONENT_OPENTHREAD
    log_info("Thread 2.4G\r\n");
    lmac15p4_init(LMAC15P4_2P4G_OQPSK, 0);
    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE,
                         PHY_PIB_CCA_THRESHOLD, PHY_PIB_CCA_DETECTED_TIME);

    lmac15p4_mac_pib_set(MAC_PIB_UNIT_BACKOFF_PERIOD,
                         MAC_PIB_MAC_ACK_WAIT_DURATION, MAC_PIB_MAC_MAX_BE,
                         MAC_PIB_MAC_MAX_CSMACA_BACKOFFS,
                         MAC_PIB_MAC_MAX_FRAME_TOTAL_WAIT_TIME,
                         MAC_PIB_MAC_MAX_FRAME_RETRIES, MAC_PIB_MAC_MIN_BE);

#endif

    // cli_init();

    otrStart();
    // app_task();

    return 0;
}