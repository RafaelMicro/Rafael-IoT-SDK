# How to build

## Change directory to Rafael-IoT-SDK-Internal

> cd Rafael-IoT-SDK-Internal

## Generate cmake files

* Board : EVB

> cmake -S . -B build -DCUSTOM_CONFIG_DIR=examples/peripheral/qspi_dma_loopback/default.config

## Build binary

> cmake --build build
