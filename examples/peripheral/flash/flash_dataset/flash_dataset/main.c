/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 * 
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_flash.h"
#include "uart_stdio.h"
#include "EnhancedFlashDataset.h"


int main(void) {
    
    uint32_t reboot_count = 0;
    size_t actual_len = 0;
    EfErrCode status;

    /* Initialize UART for logging */
    uart_stdio_init();
    
    /**
     * EFD (Enhanced Flash Dataset) Recorder Start Address Configuration
     * 
     * Target: RT58x Series
     * - Flash 1MB: 0x000F0000
     * - Flash 2MB: 0x001F0000
     * 
     * Target: RT584 Series
     * - Flash 1MB: 0x100F0000 
     * - Flash 2MB: 0x101F0000  
     * - Flash 4MB: 0x103F0000 
     * 
     * Note: 
     * 1. Ensure the start address is aligned with EFD_ERASE_MIN_SIZE (4KB).
     * 2. Verify that this area does not overlap with code or other data sections 
     *    in the Linker Script (.ld).
     */
    
    printf("========================================\n");
    printf("EFD Data Persistence Demo \n");
    printf("========================================\n");
    /* 
     * Step 1: Initialize EFD Module
     * This will verify Flash partitions, initialize GC (Garbage Collection),
     * and load the Environment Variable database from Flash.
     */
    printf("EFD Initialization ... \n");
    status = enhanced_flash_dataset_init();
    if (status != EFD_NO_ERR) {
        printf("FAIL (Error Code: %d)\n", status);
        /* If initialization fails, check EFD_START_ADDR and Flash hardware */
        while (1); 
    }
    printf("OK\n");

    /* 
     * Step 2: Read Data from Flash
     * Try to retrieve the value of "reboot" from the KV (Key-Value) database.
     * efd_get_env_blob returns the actual data length found in Flash.
     */
    actual_len = 0;
    efd_get_env_blob("reboot", &reboot_count, sizeof(uint32_t), &actual_len);

    if (actual_len == 0) {
        /* 
         * If length is 0, the key doesn't exist (First boot or Flash erased).
         * Set initial value to 1.
         */
        reboot_count = 1;
        printf("Key 'reboot' not found. Initializing count to 1...\n");
    } else {
        /* 
         * Key exists. Increment the counter.
         */
        reboot_count += 1;
        printf("Retrieved 'reboot' count: %u\n", reboot_count);
    }

    /* 
     * Step 3: Write Updated Data Back to Flash
     * EFD handles sector erasing and data verification (CRC) automatically.
     * It also performs Wear Leveling to extend Flash lifespan.
     */
    printf("Updating Flash record ... \n");
    status = efd_set_env_blob("reboot", &reboot_count, sizeof(uint32_t));
    
    if (status == EFD_NO_ERR) {
        printf("OK\n");
    } else {
        printf("FAIL (Save error: %d)\n", status);
    }

    /* 
     * Step 4: Verification and Final Result
     */
    printf("========================================\n");
    printf("EFD Persistent Test Completed Successfully\n");
    printf("Next boot count will be: %u\n", reboot_count + 1);
    printf("========================================\n");

    /* Enter main loop or continue application logic */
    while (1) {
        /* Infinite loop */
    }
}
