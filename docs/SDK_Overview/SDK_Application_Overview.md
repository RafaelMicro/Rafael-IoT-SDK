# SDK Application Overview

This page explains the software layers of a Rafael IoT SDK application. It also explains how an
application talks to a protocol stack, and where to find flash and pin information.

## 1. Introduction

The SDK gives you protocol libraries and application examples. You can use them to build
products with BLE, BLE Mesh, Zigbee, OpenThread, Sub-G, or multi-protocol RCP. All applications
run on FreeRTOS. An application can use one task or many tasks.

Flash the bootloader to your EVK before you run an example. See
[Platform_Getting_Started.md, "Start your own application"](Platform_Getting_Started.md#4-start-your-own-application).

## 2. Application architecture

```
Application            your example or project code
        |
Protocol Stack API      BLE Host / BLE Mesh / Zigbee / OpenThread / Sub-G / 802.15.4 MAC
        |
HOSAL                    portable peripheral API (UART, timer, GPIO, ...)
        |
Hardware Peripheral Driver    register-level driver for your specific chip
```

FreeRTOS runs below the whole stack. The same FreeRTOS instance schedules your application task
and each protocol stack task. There are no separate "OS" and "middleware" layers.

## 3. How the application and a protocol stack talk to each other

The application and a protocol stack, for example the BLE host, each run in their own FreeRTOS
task. The application creates a FreeRTOS queue. The application and the protocol stack use this
queue to send events to each other. A typical application does this at startup:

```c
g_app_msg_q = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_t));
xTaskCreate(app_main_entry, "main", APP_STACK_SIZE, NULL, APP_TASK_PRIORITY, NULL);
```

The protocol stack sends events into the queue. The application's task loop reads them with
`xQueueReceive`, then reacts to them. This is plain FreeRTOS. The SDK does not add its own queue
or task wrapper. See `examples/bluetooth-le/peripheral/periph-hrs/` for a full, working example
of this pattern.

## 4. Flash memory map

Bootloader size is different for each chip. The application area can hold one application image,
or a Primary/Secondary pair for OTA updates. This also depends on your chip and board. See
[Flash_Layout_Overview.md](Flash_Layout_Overview.md) for the address ranges and sizes for each
chip, and for the MP Sector layout.

## 5. GPIO pinout

See [pin_out.md](pin_out.md) for the GPIO numbers available on each chip.

## 6. RTOS and memory API

Use the FreeRTOS API directly for tasks, queues, and memory:

| Need | API |
|---|---|
| Create a task | `xTaskCreate` |
| Create a message queue | `xQueueCreate` |
| Send / receive a message | `xQueueSend` (or `xQueueSendFromISR`) / `xQueueReceive` |
| Allocate / free heap memory | `pvPortMalloc` / `vPortFree` |

FreeRTOS itself is in `thirdparty/freertos`. Edit `components/common/rtos_port/app_hooks.c` to
change a FreeRTOS hook function. Examples of hook functions: what happens on stack overflow,
malloc failure, tick, or idle. This file implements FreeRTOS's standard weak hooks:
`vApplicationStackOverflowHook`, `vApplicationMallocFailedHook`, `vApplicationIdleHook`, and
`vApplicationTickHook`. It also sets up the heap regions for the SDK's two-region heap.
