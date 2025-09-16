sdk_generate_library(css)
sdk_add_include_directories(include)
sdk_library_add_sources(
    Src/uart_bridge.c
    Src/rfb_sf.c
    Src/dtm_mode.c
)