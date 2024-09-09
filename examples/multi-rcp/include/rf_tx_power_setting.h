#ifndef __RF_TX_POWER_SETTING_H__
#define __RF_TX_POWER_SETTING_H__

#include <stdint.h>

typedef struct {
    uint8_t tbl_rf_tx_power_stage[4];
    uint8_t tbl_rf_tx_power_segment[3];
} rf_tx_power_setting_t;

const rf_tx_power_setting_t fcc_rf_tx_power_setting_ble = {
    .tbl_rf_tx_power_stage = {31, 28, 28, 24},
    .tbl_rf_tx_power_segment = {38, 38, 39}};

const rf_tx_power_setting_t fcc_rf_tx_power_setting_15p4 = {
    .tbl_rf_tx_power_stage = {31, 31, 31, 18},
    .tbl_rf_tx_power_segment = {8, 26, 39}};

const rf_tx_power_setting_t ce_rf_tx_power_setting_ble = {
    .tbl_rf_tx_power_stage = {29, 30, 30, 30},
    .tbl_rf_tx_power_segment = {1, 38, 39}};

const rf_tx_power_setting_t ce_rf_tx_power_setting_15p4 = {
    .tbl_rf_tx_power_stage = {28, 29, 29, 29},
    .tbl_rf_tx_power_segment = {8, 26, 34}};

const rf_tx_power_setting_t jp_rf_tx_power_setting_ble = {
    .tbl_rf_tx_power_stage = {31, 28, 25, 19},
    .tbl_rf_tx_power_segment = {35, 38, 39}};

const rf_tx_power_setting_t jp_rf_tx_power_setting_15p4 = {
    .tbl_rf_tx_power_stage = {31, 31, 31, 10},
    .tbl_rf_tx_power_segment = {13, 26, 34}};

#endif // __RF_TX_POWER_SETTING_H__