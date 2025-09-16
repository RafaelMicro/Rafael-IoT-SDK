# sdk_generate_component_tag()
sdk_generate_library(miu_port)

sdk_add_include_directories(
    include
)

sdk_library_add_sources(
    src/miu_alarm.c
    src/miu_crypto.c
    src/miu_diag.c
    src/miu_entropy.c
    src/miu_freertos.c
    src/miu_logging.c
    src/miu_misc.c
    src/miu_radio.c
    src/miu_settings.c
    src/miu_uart.c
)
sdk_library_add_sources(
    utils/mac_frame.cpp
    utils/mac_frame_gen.c
    utils/soft_source_match_table.c
)

sdk_library_add_sources(
    crypto/aes_alt.c
    crypto/ecjpake_alt.c
    crypto/sha256_alt.c
)