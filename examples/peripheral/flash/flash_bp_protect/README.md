# Flash HOSAL BP Protect & Read-Back Verify 使用說明


**Rafael Microelectronics Inc.**\
適用平台 / Applicable Platforms: RF1301 / RT584H / RT584L / RT584HA4

------------------------------------------------------------------------

# 1. hosal flash bp 說明

本文件說明 HOSAL Flash 驅動中：

-   Flash Block Protect (BP) 保護機制
-   Allowed Region（允許寫入區域）設定
-   Read-back Verify（讀回驗證）
-   Low Voltage Detect (LVD) 保護

> 注意 : Note: 最後 8KB 為MP Data區域，不建議設定使用。
> The last 8KB is reserved (MP/factory area) and should not be modified.

------------------------------------------------------------------------

# 2. BP 保護機制 

只有設定的 Allowed Region 可寫入，其餘區域自動受保護。

------------------------------------------------------------------------

# 3. API 說明 

#### 3.1 清除既有保護 

``` c
hosal_flash_ioctrl(HOSAL_FLASH_BP_REMOVE_ALL, NULL);
```

#### 3.2 設定 Allowed Region / Configure Allowed Region

``` c
hosal_flash_allowed_region_t region = {
    .start = start_address,
    .end   = end_address,
};

hosal_flash_ioctrl(HOSAL_FLASH_SET_ALLOWED_REGION, &region);
```

#### 3.3 確認保護狀態

``` c
flash_protection_config_t cfg;
hosal_flash_ioctrl(HOSAL_FLASH_GET_PROTECTION_CONFIG, &cfg);
```

#### 3.4 Write/Read Verify（寫入讀回驗證）

啟用後，每次寫入完成會自動讀回並比對資料。\
若不一致，API 會回傳：

    HOSAL_STATUS_VERIFY_FAIL

#### 啟用 / Enable

``` c
hosal_flash_ioctrl(HOSAL_FLASH_ENABLE_VERIFY, NULL);
```

#### 關閉 / Disable

``` c
hosal_flash_ioctrl(HOSAL_FLASH_DISABLE_VERIFY, NULL);
```

#### 3.5 低電壓保護 / Low Voltage Detect (LVD)。

``` c
hosal_flash_vbat_detect(1900); //1.9v  可根據使用的產品調整低電壓數值.

uint32_t enable = 1;
hosal_flash_ioctrl(HOSAL_FLASH_LVD_ENABLE, &enable);
```

------------------------------------------------------------------------

© 2025 Rafael Microelectronics Inc. All rights reserved.
