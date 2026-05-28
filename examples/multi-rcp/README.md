# How to build

## Change directory to Rafael-IoT-SDK-Internal

>   cd Rafael-IoT-SDK-Internal

## Generate cmake files

* Board : EVB

> cmake -S . -B build -DCUSTOM_CONFIG_DIR=examples/multi-rcp/hci_otrcp_zgw_b500k.config

* Board : Dongle

> cmake -S . -B build -DCUSTOM_CONFIG_DIR=examples/multi-rcp/hci_otrcp_zgw_dg_b500k.config

* For Amigo

> cmake -S . -B build -DCUSTOM_CONFIG_DIR=examples/multi-rcp/hci_otrcp_zbgw_amigo.config

## Build binary

> cmake --build build
