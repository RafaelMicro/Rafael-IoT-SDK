sdk_generate_library(utility)
sdk_add_include_directories(Inc)
sdk_library_add_sources(fsm.c util_list.c util_string.c util_queue.c util_fcs.c)