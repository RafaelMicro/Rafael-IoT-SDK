message("\t\t+ hci-bridge")
sdk_generate_library(hci_bridge)
sdk_add_include_directories(include)
sdk_library_add_sources(src/hci_bridge.c)
