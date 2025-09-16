#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config/platform.h>
#include "EnhancedFlashDataset.h"
#include "FreeRTOS.h"
#include "app_hooks.h"
#include "cli.h"
#include "hosal_dma.h"
#include "hosal_rf.h"
#include "lmac15p4.h"
#include "log.h"
#include "main.h"
#include "mcu.h"
#include "miu_port.h"
#include "subg_ctrl.h"
#include "task.h"
#include "uart_stdio.h"

#ifndef CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE
#define CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE 8192
#endif

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
static const char* const data_rate_str[] = {
    "2M", "1M", "500K", "200K", "100K", "50K", "300K", "150K", "75K"};
static const char* const band_str[] = {"SubG_915M", "2P4G",      "SubG_868M",
                                       "SubG_433M", "SubG_315M", "SubG_470M"};
uint16_t cca_duration = 0;
uint16_t frame_total_wait_time = 0;
uint32_t backof_period = 0;
/*set subg frequencydatarate*/
#if CONFIG_SUBG_FREQUENCY_BAND_915
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_915M;
#elif CONFIG_SUBG_FREQUENCY_BAND_868
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_868M;
#elif CONFIG_SUBG_FREQUENCY_BAND_470
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_470M;
#elif CONFIG_SUBG_FREQUENCY_BAND_433
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_433M;
#else
static uint8_t sPhyFrequencyBand = HOSAL_RF_BAND_SUBG_915M;
#endif
// static uint8_t sPhyPowerIndex = 30;
/*set subg datarate*/
#if CONFIG_SUBG_DATA_RATE_FSK_300K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_300K;
#elif CONFIG_SUBG_DATA_RATE_FSK_200K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_200K;
#elif CONFIG_SUBG_DATA_RATE_FSK_100K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_100K;
#elif CONFIG_SUBG_DATA_RATE_FSK_50K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_50K;
#elif CONFIG_SUBG_DATA_RATE_OQPSK_25K
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_25K;
#else
static uint8_t sPhyDataRate = HOSAL_RF_PHY_DATA_RATE_300K;
#endif

static void pin_mux_init(void) {
    int i;

    /*set all pin to gpio, except GPIO16, GPIO17 */
    for (i = 0; i < 32; i++) {
        pin_set_mode(i, MODE_GPIO);
    }
    return;
}

#ifdef CONFIG_HOSAL_SOC_IDLE_SLEEP
static void __init_sleep() {
    //Initialize low power wakeup source, like uart.
    // lpm_enable_low_power_wakeup(LOW_POWER_WAKEUP_UART0_RX);
    printf("__init_sleep:%d \r\n", CONFIG_HOSAL_SOC_SLEEP_TIMER_ID);
}
#endif

static void app_task_entry(void* pvParameters) {
    enhanced_flash_dataset_init();
    hosal_rf_init(HOSAL_RF_MODE_RUCI_CMD);

    log_info("Mesh It Up Sniffer Band: %s Data Rate: %s \r\n",
             band_str[sPhyFrequencyBand], data_rate_str[sPhyDataRate]);

    cca_duration = cca_duration_table[sPhyDataRate];
    frame_total_wait_time = frame_total_wait_time_table[sPhyDataRate];
    backof_period = PHY_PIB_TURNAROUND_TIMER + cca_duration;

#if CONFIG_SUBG_DATA_RATE_OQPSK_25K
    lmac15p4_init(LMAC15P4_SUBG_OQPSK, sPhyFrequencyBand);
#else
    lmac15p4_init(LMAC15P4_SUBG_FSK, sPhyFrequencyBand);
#endif

    lmac15p4_phy_pib_set(PHY_PIB_TURNAROUND_TIMER, PHY_PIB_CCA_DETECT_MODE,
                         PHY_PIB_CCA_THRESHOLD, cca_duration);

    lmac15p4_mac_pib_set(backof_period, MAC_PIB_MAC_ACK_WAIT_DURATION,
                         MAC_PIB_MAC_MAX_BE, MAC_PIB_MAC_MAX_CSMACA_BACKOFFS,
                         frame_total_wait_time, MAC_PIB_MAC_MAX_FRAME_RETRIES,
                         MAC_PIB_MAC_MIN_BE);

    subg_ctrl_sleep_set(false);
    subg_ctrl_idle_set();
#if CONFIG_SUBG_DATA_RATE_OQPSK_25K
    subg_ctrl_modem_config_set(LMAC15P4_SUBG_OQPSK, sPhyDataRate,
                               SUBG_CTRL_FSK_MOD_1);
    subg_ctrl_mac_set(LMAC15P4_SUBG_OQPSK, SUBG_CTRL_CRC_TYPE_16,
                      SUBG_CTRL_WHITEN_DISABLE);
    subg_ctrl_preamble_set(LMAC15P4_SUBG_OQPSK, 8);
    subg_ctrl_sfd_set(LMAC15P4_SUBG_OQPSK, 0x00007209);
    subg_ctrl_filter_set(LMAC15P4_SUBG_OQPSK, SUBG_CTRL_FILTER_TYPE_GFSK);
#else
    subg_ctrl_modem_config_set(SUBG_CTRL_MODU_FSK, sPhyDataRate,
                               SUBG_CTRL_FSK_MOD_1);
    subg_ctrl_mac_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_CRC_TYPE_16,
                      SUBG_CTRL_WHITEN_DISABLE);
    subg_ctrl_preamble_set(SUBG_CTRL_MODU_FSK, 8);
    subg_ctrl_sfd_set(SUBG_CTRL_MODU_FSK, 0x00007209);
    subg_ctrl_filter_set(SUBG_CTRL_MODU_FSK, SUBG_CTRL_FILTER_TYPE_GFSK);
#endif

    cli_init();

    miuStart();

    vTaskDelete(NULL);
}

int main(void) {
    /*gpio init*/
    pin_mux_init();

    /*freertos heap init*/
    vHeapRegionsInt();

    /*debug uart init*/
    uart_stdio_init();

    /*dma init; RT58x need call*/
    hosal_dma_init();

    /*application task start for customer*/
    if (xTaskCreate(app_task_entry, (char*)"main",
                    CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE, NULL,
                    E_TASK_PRIORITY_APP, NULL)
        != pdPASS) {
        puts("Task create fail....\r\n");
    }

    vTaskStartScheduler();
    while (1) {}

    return 0;
}