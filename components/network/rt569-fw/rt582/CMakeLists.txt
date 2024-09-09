sdk_generate_library(rt569_fw)
sdk_add_include_directories(include)
sdk_library_add_sources(
    Src/rt569mp_fw.c
    Src/rt569mp_init.c 
)
