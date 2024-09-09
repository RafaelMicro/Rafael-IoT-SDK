# sdk_generate_component_tag()
sdk_generate_library(openthread_port)



sdk_add_include_directories(
    include
)

if(CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT)
    add_definitions(-DOPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT=1)
else()
    add_definitions(-DOPENTHREAD_CONFIG_RADIO_SUBGHZ_OQPSK_SUPPORT)
endif()
sdk_library_add_sources(
    src/ot_alarm.c
    src/ot_diag.c
    src/ot_entropy.c
    src/ot_freertos.c
    src/ot_logging.c
    src/ot_misc.c
    src/ot_radio.c
    src/ot_settings.c
    utils/mac_frame.cpp    
    utils/mac_frame_gen.c    
    utils/soft_source_match_table.c
)
if(CONFIG_OT_RCP_EZMESH)
    sdk_library_add_sources(ncp/ncp.c)
    sdk_library_add_sources(ncp/ncp_cpc.cpp)
    sdk_library_add_sources(src/ot_uart.c)
else()
    sdk_library_add_sources(src/ot_uart.c)
endif()