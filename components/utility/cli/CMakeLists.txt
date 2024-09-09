sdk_generate_library(cli)
sdk_add_include_directories(Inc)
sdk_library_add_sources(
    cli.c
    shell.c 
)