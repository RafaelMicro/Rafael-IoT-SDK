/*
 * Copyright (c) 2022-2025 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#if CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "FreeRTOSConfig.h"
#endif

#include "uart_stdio.h"
#include "hosal_uart.h"
#include "hosal_sysctrl.h"
#include "hosal_dma.h"
#include "hosal_trng.h"
#include "hosal_flash.h"
#include "hosal_status.h"
#if defined(CONFIG_RT584H) || defined(CONFIG_RT584L) || defined(CONFIG_RT584HA4) || defined(CONFIG_RF1301)
#include "hosal_flash_vbat_detect.h"
#else
#endif
#include "hosal_gpio.h"
#include "app_hooks.h"
/* ----------------------------
 * Per-chip address configuration
 * RF1301 : 1MB  (0x10000000 ~ 0x100FFFFF)
 * RT584H/L : 2MB (0x10100000 ~ 0x102FFFFF)
 * RT584HA4 : 4MB (0x10000000 ~ 0x103FFFFF)
 * Note: keep last 8KB (MP / reserved) not tested
 * ---------------------------- */
#if defined(CONFIG_RF1301)

/* 1MB */
#define FLASH_BASE            (0x10000000u)
#define FLASH_SIZE            (0x00100000u) /* 1MB */
#define FLASH_END_EXCL        (FLASH_BASE + FLASH_SIZE)
#define FLASH_TAIL_END        (FLASH_END_EXCL - 0x2000u) /* keep last 8KB -> 0x100FE000 */

#define TEST_PROTECTED_ADDR   (FLASH_BASE + 0x00010000u) /* 0x10010000 */

/* Default allowed region: 0x100E0000 ~ 0x100F0000 (64KB) */
#define DEFAULT_ALLOWED_START (FLASH_BASE + 0x000E0000u)
#define DEFAULT_ALLOWED_END   (FLASH_BASE + 0x000F0000u)

static uint32_t g_allowed_start = DEFAULT_ALLOWED_START;
static uint32_t g_allowed_end   = DEFAULT_ALLOWED_END;

/* BP cases */
#define BP_CASE0_START        (FLASH_BASE + 0x00020000u)
#define BP_CASE0_END          (FLASH_TAIL_END)

#define BP_CASE1_START        (FLASH_END_EXCL - 0x00020000u) /* last 128KB -> 0x100E0000 */
#define BP_CASE1_END          (FLASH_TAIL_END)

#define BP_CASE2_START        (FLASH_BASE + 0x00020000u)
#define BP_CASE2_END          (FLASH_BASE + 0x00080000u)

#define BP_CASE3_START        (FLASH_BASE + 0x00060000u)
#define BP_CASE3_END          (FLASH_BASE + 0x00080000u)

#define TEST_ALLOWED_ADDR     0x100E0000
#elif defined(CONFIG_RT584H) || defined(CONFIG_RT584L)

/* 2MB */
#define FLASH_BASE            (0x10100000u)
#define FLASH_SIZE            (0x00200000u) /* 2MB */
#define FLASH_END_EXCL        (FLASH_BASE + FLASH_SIZE)
#define FLASH_TAIL_END        (FLASH_END_EXCL - 0x2000u) /* -> 0x102FE000 */

#define TEST_PROTECTED_ADDR   (FLASH_BASE + 0x00010000u) /* 0x10110000 */

/* Default allowed region: last 128KB 的前半段 64KB */
#define DEFAULT_ALLOWED_START (FLASH_END_EXCL - 0x00020000u) /* 0x102E0000 */
#define DEFAULT_ALLOWED_END   (FLASH_END_EXCL - 0x00010000u) /* 0x102F0000 */

static uint32_t g_allowed_start = DEFAULT_ALLOWED_START;
static uint32_t g_allowed_end   = DEFAULT_ALLOWED_END;

/* BP cases */
#define BP_CASE0_START        (FLASH_BASE + 0x00020000u)
#define BP_CASE0_END          (FLASH_TAIL_END)

#define BP_CASE1_START        (FLASH_END_EXCL - 0x00020000u) /* 0x102E0000 */
#define BP_CASE1_END          (FLASH_TAIL_END)               /* 0x102FE000 */

#define BP_CASE2_START        (FLASH_BASE + 0x00020000u)      /* 0x10120000 */
#define BP_CASE2_END          (FLASH_BASE + 0x00080000u)      /* 0x10180000 */

#define BP_CASE3_START        (FLASH_BASE + 0x00060000u)      /* 0x10160000 */
#define BP_CASE3_END          (FLASH_BASE + 0x00080000u)      /* 0x10180000 */

#define TEST_ALLOWED_ADDR     0x102E0000
#elif defined(CONFIG_RT584HA4)

/* 4MB */
#define FLASH_BASE            (0x10000000u)
#define FLASH_SIZE            (0x00400000u) /* 4MB */
#define FLASH_END_EXCL        (FLASH_BASE + FLASH_SIZE)
#define FLASH_TAIL_END        (FLASH_END_EXCL - 0x2000u) /* -> 0x103FE000 */

#define TEST_PROTECTED_ADDR   (FLASH_BASE + 0x00010000u) /* 0x10010000 */

/* Default allowed region: last 128KB 的前半段 64KB */
#define DEFAULT_ALLOWED_START (FLASH_END_EXCL - 0x00020000u) /* 0x103E0000 */
#define DEFAULT_ALLOWED_END   (FLASH_END_EXCL - 0x00010000u) /* 0x103F0000 */

static uint32_t g_allowed_start = DEFAULT_ALLOWED_START;
static uint32_t g_allowed_end   = DEFAULT_ALLOWED_END;

/* BP cases */
#define BP_CASE0_START        (FLASH_BASE + 0x00040000u) /* keep your original style */
#define BP_CASE0_END          (FLASH_TAIL_END)           /* 0x103FE000 */

#define BP_CASE1_START        (FLASH_END_EXCL - 0x00020000u) /* 0x103E0000 */
#define BP_CASE1_END          (FLASH_TAIL_END)

#define BP_CASE2_START        (FLASH_BASE + 0x00200000u)     /* 0x10200000 */
#define BP_CASE2_END          (FLASH_BASE + 0x00300000u)     /* 0x10300000 */

#define BP_CASE3_START        (FLASH_BASE + 0x00100000u)     /* 0x10100000 */
#define BP_CASE3_END          (FLASH_BASE + 0x00120000u)     /* 0x10120000 */

#define TEST_ALLOWED_ADDR     0x103E0000
#else
#error "No supported chip macro defined. Please define CONFIG_RF1301/CONFIG_RT584H/CONFIG_RT584L/CONFIG_RT584HA4."
#endif



/* Pattern / Size */
#define PATTERN_AA            (0xAAu)
#define PATTERN_55            (0x55u)
#define PATTERN_FF            (0xFFu)

#define PAGE_SIZE             (256u)
#define SECTOR_SIZE           (0x1000u)   /* 4KB */

/* GPIO：measure verify on/off  write timing (option) */
#define GPIO_TIMING_PIN       (20)

/* ============================================================
 * 2) Test Log 
 * ============================================================ */

static uint32_t g_test_pass = 0;
static uint32_t g_test_fail = 0;

#define TEST_PRINT(fmt, ...)  printf("[TEST] " fmt "\n", ##__VA_ARGS__)
#define TEST_OK(msg)          do { printf("  OK   %s\n", msg); g_test_pass++; } while(0)
#define TEST_FAIL(msg)        do { printf("  FAIL %s\n", msg); g_test_fail++; } while(0)

/* ============================================================
 * 3) ：fill / verify pattern
 * ============================================================ */

static void fill_pattern(uint8_t *buf, uint32_t size, uint8_t pattern)
{
    for (uint32_t i = 0; i < size; i++) {
        buf[i] = pattern;
    }
}

static bool check_pattern(const uint8_t *buf, uint32_t size, uint8_t pattern)
{
    for (uint32_t i = 0; i < size; i++) {
        if (buf[i] != pattern) {
            printf("    Mismatch at [%u]: expect=0x%02X, actual=0x%02X\n",
                   i, pattern, buf[i]);
            return false;
        }
    }
    return true;
}

/* ============================================================
 * 4) HOSAL Flash base examples
 *    - erase -> write -> read -> verify
 * ============================================================ */

static void example_write_read_byte(uint32_t addr, uint8_t v)
{
    int rc;
    uint8_t r = 0;

    TEST_PRINT("Example: Write/Read BYTE @ 0x%08X val=0x%02X", addr, v);

    rc = hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, addr);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("Erase sector failed");
        return;
    }

    rc = hosal_flash_write(HOSAL_FLASH_WRITE_BYTE, addr, &v);
    if (rc != HOSAL_STATUS_SUCCESS) {
        printf("    rc=%d\n", rc);
        TEST_FAIL("Write byte failed");
        return;
    }

    rc = hosal_flash_read(HOSAL_FLASH_READ_BYTE, addr, &r);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("Read byte failed");
        return;
    }

    if (r == v) {
        TEST_OK("Byte write/read verified");
    } else {
        printf("    Expected 0x%02X, Got 0x%02X\n", v, r);
        TEST_FAIL("Byte verify failed");
    }
}

static void example_write_read_page(uint32_t addr, uint8_t pattern)
{
    int rc;
    static uint8_t wbuf[PAGE_SIZE] __attribute__((aligned(4)));
    static uint8_t rbuf[PAGE_SIZE] __attribute__((aligned(4)));

    TEST_PRINT("Example: Write/Read PAGE @ 0x%08X pattern=0x%02X", addr, pattern);

    fill_pattern(wbuf, PAGE_SIZE, pattern);

    rc = hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, addr);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("Erase sector failed");
        return;
    }

    rc = hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, addr, wbuf);
    if (rc != HOSAL_STATUS_SUCCESS) {
        printf("    rc=%d\n", rc);
        TEST_FAIL("Write page failed");
        return;
    }

    rc = hosal_flash_read(HOSAL_FLASH_READ_PAGE, addr, rbuf);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("Read page failed");
        return;
    }

    if (check_pattern(rbuf, PAGE_SIZE, pattern)) {
        TEST_OK("Page write/read verified");
    } else {
        TEST_FAIL("Page verify failed");
    }
}

static void example_erase_verify_ff(uint32_t addr)
{
    int rc;
    static uint8_t buf[PAGE_SIZE] __attribute__((aligned(4)));

    TEST_PRINT("Example: Erase verify (0xFF) @ 0x%08X", addr);

    /* write page data, avoid 0xFF can't compare*/
    fill_pattern(buf, PAGE_SIZE, PATTERN_AA);
    rc = hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, addr, buf);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("Setup write failed");
        return;
    }

    rc = hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, addr);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("Erase sector failed");
        return;
    }

    memset(buf, 0, sizeof(buf));
    rc = hosal_flash_read_n_bytes(addr, buf, PAGE_SIZE);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("Read after erase failed");
        return;
    }

    if (check_pattern(buf, PAGE_SIZE, PATTERN_FF)) {
        TEST_OK("Erase verified (all 0xFF)");
    } else {
        TEST_FAIL("Erase incomplete");
    }
}

/* ============================================================
 * 5) Write + Verify function
 * ============================================================ */

static void example_write_with_verify(uint32_t addr, bool verify_enable, bool write_page)
{
    int rc;
    static uint8_t buf[PAGE_SIZE] __attribute__((aligned(4)));

    TEST_PRINT("Example: Write with %s (mode=%s) @ 0x%08X",
               verify_enable ? "VERIFY-ON" : "VERIFY-OFF",
               write_page ? "PAGE" : "BYTE",
               addr);

    rc = hosal_flash_ioctrl(verify_enable ? HOSAL_FLASH_ENABLE_VERIFY
                                         : HOSAL_FLASH_DISABLE_VERIFY,
                            NULL);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("ioctrl verify toggle failed");
        return;
    }

    rc = hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, addr);
    if (rc != HOSAL_STATUS_SUCCESS) {
        (void)hosal_flash_ioctrl(HOSAL_FLASH_DISABLE_VERIFY, NULL);
        TEST_FAIL("Erase sector failed");
        return;
    }

    fill_pattern(buf, PAGE_SIZE, PATTERN_55);

    hosal_gpio_pin_set(GPIO_TIMING_PIN);

    if (write_page) {
        rc = hosal_flash_write(HOSAL_FLASH_WRITE_PAGE, addr, buf);
    } else {
        uint8_t v = buf[0];
        rc = hosal_flash_write(HOSAL_FLASH_WRITE_BYTE, addr, &v);
    }

    hosal_gpio_pin_clear(GPIO_TIMING_PIN);

    (void)hosal_flash_ioctrl(HOSAL_FLASH_DISABLE_VERIFY, NULL);

    if (rc == HOSAL_STATUS_SUCCESS) {
        TEST_OK("Write OK");
    } else if (rc == HOSAL_STATUS_VERIFY_FAIL) {
        TEST_FAIL("Write verify failed");
    } else {
        printf("    rc=%d\n", rc);
        TEST_FAIL("Write failed");
    }
}

/* ============================================================
 * 6) BP / Allowed Region setting examples
 * ============================================================ */

typedef struct {
    const char *name;
    uint32_t start;
    uint32_t end;
    uint32_t allowed_probe;
    uint32_t outside_probe;
} bp_case_t;

/* depend on：region 0x10040000 ~ 0x103FE000 */
static const bp_case_t g_bp_cases[] = {
    {
        .name = "ALLOW_FULL_FLASH",
        .start = BP_CASE0_START,
        .end   = BP_CASE0_END,
        .allowed_probe = (BP_CASE0_START + 0x1000u),
        .outside_probe = BP_CASE0_END,
    },
    {
        .name = "ALLOW_TAIL",
        .start = BP_CASE1_START,
        .end   = BP_CASE1_END,
        .allowed_probe = (BP_CASE1_START + 0x1000u),
        .outside_probe = (BP_CASE1_START - 0x1000u),
    },
    {
        .name = "ALLOW_MID",
        .start = BP_CASE2_START,
        .end   = BP_CASE2_END,
        .allowed_probe = (BP_CASE2_START + 0x1000u),
        .outside_probe = BP_CASE2_END,
    },
    {
        .name = "ALLOW_HEAD",
        .start = BP_CASE3_START,
        .end   = BP_CASE3_END,
        .allowed_probe = (BP_CASE3_START + 0x1000u),
        .outside_probe = BP_CASE3_END,
    },
};
static bool example_bp_set_allowed_region(uint32_t start, uint32_t end)
{
    int rc;

    TEST_PRINT("BP Example: Set Allowed Region [0x%08X, 0x%08X)", start, end);

    /* 1) Clear All BP */
    rc = hosal_flash_ioctrl(HOSAL_FLASH_BP_REMOVE_ALL, NULL);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("BP_REMOVE_ALL failed");
        return false;
    }

    /* 2) set allow region */
    hosal_flash_allowed_region_t region = {
        .start = start,
        .end   = end,
    };
    rc = hosal_flash_ioctrl(HOSAL_FLASH_SET_ALLOWED_REGION, &region);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("SET_ALLOWED_REGION failed");
        return false;
    }

    /* 3) read protect config */
    flash_protection_config_t cfg;
    rc = hosal_flash_ioctrl(HOSAL_FLASH_GET_PROTECTION_CONFIG, &cfg);
    if (rc != HOSAL_STATUS_SUCCESS) {
        TEST_FAIL("GET_PROTECTION_CONFIG failed");
        return false;
    }

    printf("    Protection enabled:   %s\n", cfg.protection_enabled ? "YES" : "NO");
    printf("    Bootloader end:       0x%08X\n", cfg.bootloader_end);
    printf("    App image size:       %u KB (0x%X)\n", (cfg.app_image_size / 1024), cfg.app_image_size);
    printf("    Allowed region:       [0x%08X, 0x%08X)\n", cfg.allowed_start, cfg.allowed_end);
    printf("    BP=%u, CMP=%u\n", cfg.current_bp, cfg.current_cmp);

    hosal_flash_sr12_t sr = {0};
    rc = hosal_flash_ioctrl(HOSAL_FLASH_GET_STATUS_SR12, &sr);
    if (rc == HOSAL_STATUS_SUCCESS) {
        printf("    SR1=0x%02X SR2=0x%02X\n", sr.sr1, sr.sr2);
    }

    if (!cfg.protection_enabled) {
        TEST_FAIL("BP protection not enabled");
        return false;
    }

    /* sync allowed region，example */
    g_allowed_start = start;
    g_allowed_end   = end;

    TEST_OK("BP allowed region configured");
    return true;
}

/* expert success or expert */
static void example_probe_write(uint32_t addr, bool expect_success, const char *tag)
{
    int rc;
    uint8_t w = PATTERN_55;
    uint8_t r = 0;

    TEST_PRINT("%s: probe write @ 0x%08X", tag, addr);

    /* Option： erase */
    (void)hosal_flash_erase(HOSAL_FLASH_ERASE_SECTOR, addr);

    rc = hosal_flash_write(HOSAL_FLASH_WRITE_BYTE, addr, &w);

    if (expect_success) {
        if (rc != HOSAL_STATUS_SUCCESS) {
            printf("    rc=%d\n", rc);
            TEST_FAIL("Expected SUCCESS but failed");
            return;
        }

        rc = hosal_flash_read(HOSAL_FLASH_READ_BYTE, addr, &r);
        if (rc != HOSAL_STATUS_SUCCESS || r != w) {
            printf("    read rc=%d val=0x%02X\n", rc, r);
            TEST_FAIL("Write OK but verify failed");
            return;
        }

        TEST_OK("Write allowed as expected");
    } else {
        if (rc == HOSAL_STATUS_PROTECTED) {
            TEST_OK("Blocked as expected");
        } else if (rc == HOSAL_STATUS_SUCCESS) {
            TEST_FAIL("SECURITY ISSUE: write unexpectedly succeeded");
        } else {
            printf("    rc=%d\n", rc);
            TEST_FAIL("Unexpected result");
        }
    }
}

static void example_bp_allowed_region_matrix(void)
{
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf(" BP Allowed-Region Matrix (Usage Example)\n");
    printf("═══════════════════════════════════════════════════════════════\n");

    for (uint32_t i = 0; i < (uint32_t)(sizeof(g_bp_cases) / sizeof(g_bp_cases[0])); i++) {
        const bp_case_t *c = &g_bp_cases[i];

        printf("\n[CASE %u] %s\n", i, c->name);
        printf("    Allowed:   [0x%08X, 0x%08X)\n", c->start, c->end);
        printf("    Probe IN : 0x%08X (expect OK)\n", c->allowed_probe);
        printf("    Probe OUT: 0x%08X (expect BLOCK)\n", c->outside_probe);

        if (!example_bp_set_allowed_region(c->start, c->end)) {
            TEST_FAIL("Setup protection failed, skip");
            continue;
        }

        example_probe_write(c->allowed_probe,  true,  "IN-REGION");
        example_probe_write(c->outside_probe,  false, "OUT-REGION");
        example_probe_write(TEST_PROTECTED_ADDR, false, "PROTECTED-APP");
    }
}

/* ============================================================
 * 7) examples：Init -> BP Matrix -> basic operation -> summary
 * ============================================================ */

static void run_usage_example(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Flash HOSAL Usage Example                                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");

    /* Flash init */
    hosal_flash_init();

    /* Option) low voltage detect */
    #if defined(CONFIG_RT584H) || defined(CONFIG_RT584HA4) || defined(CONFIG_RT584L) || defined(CONFIG_RF1301)
    hosal_flash_vbat_detect(1900);
    uint32_t enable_lvd = 1;
    hosal_flash_ioctrl(HOSAL_FLASH_LVD_ENABLE,&enable_lvd);
    #endif
    /* Step 1: BP Allowed-Region Matrix */
    example_bp_allowed_region_matrix();

    /* Step 2:  Allowed Region  */
    (void)example_bp_set_allowed_region(DEFAULT_ALLOWED_START, DEFAULT_ALLOWED_END);


    /* Step 3: Verify on/off examples */
    example_write_with_verify(TEST_ALLOWED_ADDR + 0x3000, false, true);  /* verify off, page */
    example_write_with_verify(TEST_ALLOWED_ADDR + 0x4000, true,  true);  /* verify on,  page */
    example_write_with_verify(TEST_ALLOWED_ADDR + 0x5000, true,  false); /* verify on,  byte */

    /* Summary */
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  Summary                                                     ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
    printf("  Total: %u\n", (unsigned)(g_test_pass + g_test_fail));
    printf("  Pass : %u\n", (unsigned)g_test_pass);
    printf("  Fail : %u\n\n", (unsigned)g_test_fail);

    if (g_test_fail == 0) {
        printf("  ALL PASSED\n");
    } else {
        printf("  SOME FAILED\n");
    }
    printf("\n");
}


int main(void)
{
    uart_stdio_init();
    vHeapRegionsInt();
    hosal_dma_init();


    /* Remove BP*/
    (void)hosal_flash_ioctrl(HOSAL_FLASH_BP_REMOVE_ALL, NULL);

    run_usage_example();

    /* Remove BP*/
    (void)hosal_flash_ioctrl(HOSAL_FLASH_BP_REMOVE_ALL, NULL);

    while (1) {
        /* idle */
    }

    return 0;
}
