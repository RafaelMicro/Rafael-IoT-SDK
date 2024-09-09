#include <EnhancedFlashDataset.h>

#include "efd_init.h"

#if !defined(EFD_START_ADDR)
#error "Please configure backup area start address (in efd_cfg.h)"
#endif

#if !defined(EFD_ERASE_MIN_SIZE)
#error "Please configure minimum size of flash erasure (in efd_cfg.h)"
#endif

/**
 * Enhanced FlashDataset system initialize.
 *
 * @return result
 */
EfErrCode enhanced_flash_dataset_init(void) {
    size_t default_env_set_size = 0;
    const efd_env* default_env_set;
    EfErrCode result = EFD_NO_ERR;

    result = efd_port_init(&default_env_set, &default_env_set_size);
    if (result != EFD_NO_ERR)
        return result;

#ifdef EFD_USING_ENV
    result = efd_env_init(default_env_set, default_env_set_size);
    if (result != EFD_NO_ERR)
        return result;
#endif

#ifdef EFD_USING_IAP
    result = efd_iap_init();
    if (result != EFD_NO_ERR)
        return result;
#endif

#ifdef EFD_USING_LOG
    result = efd_log_init();
    if (result != EFD_NO_ERR) {
        EFD_INFO("EnhancedFlashDataset v%s is initialize fail.\r\n",
                 EFD_SW_VERSION);
        return result;
    }
#endif

    EFD_INFO("EnhancedFlashDataset v%s is initialize success.\r\n",
             EFD_SW_VERSION);
    return result;
}
