sdk_generate_library(EnhancedFlashDataset)

sdk_add_include_directories(
    Src
    Inc
)

sdk_library_add_sources(
    Src/efd_iap.c
    Src/efd_kv.c
    Src/efd_log.c
    Src/efd_port.c
    Src/efd_utils.c
    Src/EnhancedFlashDataset.c
)