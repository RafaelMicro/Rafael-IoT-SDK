# Zigbee Application Note
## Introduction
This article primarily introduces the information required for developing Zigbee applications using the RT58x platform, including:

* Rafael Zigbee Application Software Flow
* Explanation of the Zigbee Example Source Code File Structure
* The Zigbee device examples provided by the SDK
* Operations of the RT58x Evaluation Board
* Compilation method for the Zigbee Example

This note covers the Zigbee device examples under [`examples/zigbee/`](../../examples/zigbee/) —
lights, switches, plugs, locks and sensors.

## Rafael Zigbee Application Software Flow
```
  +---------------------------- User Application ----------------------------+
  |                                                                          |
  |   Application input: button / sensor / timer   (device_api.c)            |
  |        |                                                                 |
  |        |  ZIGBEE_APP_NOTIFY / ZIGBEE_APP_NOTIFY_ISR                      |
  |        |  (set a bit in g_zb_app_evt_var, then signal the task)          |
  |        v                                                                 |
  |   app_main_loop()   "app-zigbee"                                         |
  |        ^                                                                 |
  |        |  ZB_APP_EVENT_INIT / ZB_APP_EVENT_NOT_JOINED / _JOINED          |
  |        |                                                                 |
  |   zboss_signal_handler()        zigbee_zcl_msg_handler()                 |
  |                                       |                                  |
  |                                       |  start / stop                    |
  |                                       v                                  |
  |                                  FreeRTOS timer callbacks                |
  |                                  identify / dimming / timed-off          |
  +-----|---|----------------------------------------------------------------+
        |   ^
        |   |   down: ZBOSS API calls, wrapped in ZB_THREAD_SAFE(...)
        |   |   up:   endpoint handler and stack signal callbacks
        v   |
  +-----|---|-------------------- Zigbee Stack ------------------------------+
  |   zbStackTask   "zb-thread"                                              |
  |   Zigbee 3.0 and network stack                                           |
  |   holds the stack lock (zb_extLock) while it runs                        |
  +-----|--------------------------------------------------------------------+
        |   IEEE 802.15.4 frames
        v
  +-----|------------------------ RF Function -------------------------------+
  |   __rf_proc   "hoasl-rf"                                                 |
  |   IEEE 802.15.4 MAC and radio control                                    |
  +-----|--------------------------------------------------------------------+
        |
        v
  +-----|-------------------- Peripheral Function ---------------------------+
  |   HOSAL peripheral drivers   GPIO, UART, PWM, Flash, WDT                 |
  |   Also reached directly by app_main_loop, the ZCL handler and the        |
  |   timer callbacks (pwm_ctl_set_level, efd_*, log), and by the            |
  |   stack for NVRAM (zb_osif_nvram_*)                                      |
  +-----|--------------------------------------------------------------------+
        |
        v
  +--------------------------------------------------------------------------+
  |   RT58x hardware platform                                                |
  +--------------------------------------------------------------------------+
```

This workflow diagram illustrates the software interaction within a Zigbee-based application, structured in a layered architecture from application level down to hardware platform. The flow demonstrates how user interaction is processed and managed through various software layers and finally transmitted via hardware.

When an application-specific input occurs — a button press, a sensor reading or a software timer, depending on the example — an event notification (ZIGBEE_APP_NOTIFY or ZIGBEE_APP_NOTIFY_ISR) is triggered and sent to the application's main loop (app_main_loop), which operates as a task within a Real-Time Operating System (RTOS). This notification triggers the app_main_loop, which operates as an RTOS task that serves as the primary entry point and handler for user-level application logic. Both macros set a bit in the application event variable and then signal the application task; use the ISR variant from interrupt context. Examples that are driven by physical buttons (`switch`, `wall-switch`, `door-lock`, `door-sensor`, `thermostat`) raise this notification from the GPIO callback in their `device_api.c`.

The app_main_loop is responsible for distributing messages or events to various handlers, such as the Zigbee Cluster Library (ZCL) message handler (zigbee_zcl_msg_handler) or the Zigbee Stack signal handler (zboss_signal_handler). These handlers interact with the Zigbee Stack task (zbStackTask), which also runs as an RTOS task.

The Zigbee Stack includes the Zigbee 3.0 and network stack, which processes messages from the upper-layer application and handles protocol-specific tasks. The Zigbee 3.0 & Network Stack implements Zigbee protocol functionality, managing network communication, packet routing, and data handling based on the Zigbee 3.0 specification. The zbStackTask interacts with the underlying RF processing task (__rf_proc), which handles IEEE802.15.4 wireless communication. The zbStackTask communicates bidirectionally with both upper-layer handlers (zigbee_zcl_msg_handler and zboss_signal_handler) and lower-layer peripheral and RF functionality.

Additionally, app_main_loop, the ZCL message handler and the application's software-timer callbacks access hardware resources through the HOSAL peripheral drivers, managing GPIO, UART, PWM, Flash and other hardware functions. In `lighting-app` those timer callbacks are what produce the identify blink (`t_id`, 200 ms), the dimming ramp (`t_lv`), the off-with-effect fade (`t_off`) and the on-with-timed-off countdown (`t_ow`); each of them ends in `pwm_ctl_set_level()`.

The application-event path is entered from two places only: the example's own input handling, and `zboss_signal_handler()` reporting a change of network state. The ZCL message handler acts on commands directly — it updates attributes, drives the hardware and answers over the air — rather than raising application events.

The lowest layer, labeled as RT58X Hardware Platform, represents the physical hardware components and integrated circuits that perform the actual signal transmission, peripheral operations, and RF communications as instructed by the software tasks described above.

Rafael Zigbee Application Software Flow design enables Zigbee applications to efficiently translate user actions into concrete device behavior and communicate with other devices within the wireless network, fulfilling the goals of smart connected device applications.

### Calling ZBOSS APIs from the Application Task
The Zigbee stack runs in its own task (zbStackTask), so ZBOSS APIs called from `app_main_loop` — or
from any other application task — must be serialised against it. Wrap every such call in
`ZB_THREAD_SAFE()`, which takes and releases the stack lock (`zbLock()` / `zbUnlock()`):

```c
ZB_THREAD_SAFE(bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);)

ZB_THREAD_SAFE(g_panid      = zb_get_pan_id();
               g_short_addr = zb_get_short_address();)
```

`ZB_THREAD_SAFE_RET(ret, ...)` is the variant to use when the call returns a value. Both macros are
defined in
[`components/network/zigbee/zboss_port/include/zigbee_platform.h`](../../components/network/zigbee/zboss_port/include/zigbee_platform.h).
Code that already runs in stack context — the ZCL message handler and `zboss_signal_handler()` —
holds the lock and must not take it again.

## Zigbee Example Source Code File Structure
```
examples/zigbee/lighting-app/
├── CMakeLists.txt
├── Kconfig
├── default-RT582-EVB.config
├── default-RT583-EVB.config
├── default-RT584H-EVB.config
├── default-RT584HA4-EVB.config
├── default-RT584L-EVB.config
├── include
│   ├── device_api.h
│   ├── zigbee_api.h
│   └── zigbee_zcl_msg_handler.h
├── rtos
│   ├── CMakeLists.txt
│   └── config
│       ├── rt582
│       │   └── FreeRTOSConfig.h
│       └── rt584
│           └── FreeRTOSConfig.h
└──  src
    ├── device_api.c
    ├── main.c
    ├── zcl_construction.c
    ├── zigbee_api.c
    └── zigbee_zcl_msg_handler.c
```
A Zigbee example consists of the files listed above, and every Zigbee device example in
[`examples/zigbee/`](../../examples/zigbee/) shares this structure. The content of each file varies
depending on the Zigbee device type. Here, the explanation is based on the files related to ```./examples/zigbee/lighting-app/```.

* main.c
This file serves as the entry point (main) of the Zigbee application. It initializes the Zigbee stack, RF hardware, and checks device reboot conditions. The main application loop (app_main_loop) handles Zigbee events like network initialization, network joining processes, and managing device states upon different Zigbee stack events. The start-up sequence is `hosal_rf_init()` → `zigbee_app_init()` (which creates the application task) → `zbStart()` → `vTaskStartScheduler()`; `app_main_loop()` then registers the device context with `ZB_AF_REGISTER_DEVICE_CTX()` and installs the ZCL handler for each endpoint with `ZB_AF_SET_ENDPOINT_HANDLER()` before entering its event loop.

* device_api.c
This file implements device-specific control functions, primarily for managing PWM (Pulse Width Modulation) operations. It initializes PWM settings (pwm_ctl_init) with specified frequency and output pins, and provides a function (pwm_ctl_set_level) to adjust PWM duty cycles to control device brightness levels​.

* zcl_construction.c
This source file contains attribute definitions and Zigbee Cluster Library (ZCL) configurations, including basic device attributes, OTA (Over-The-Air) update attributes, and definitions for Zigbee endpoints and clusters. It describes the supported functionality, including basic device information, firmware details, and OTA upgrade parameters.

* zigbee_api.c
Defines the Zigbee-specific API functions responsible for managing network startup (zigbee_app_nwk_start), OTA update checks, handling scene databases, startup configurations, and responding to Zigbee stack signals. The file contains event-handling mechanisms essential for Zigbee operations and interactions with hardware component.

* zigbee_zcl_msg_handler.c
This file handles ZCL messages received from other Zigbee devices, processing commands for managing device states, scenes, groups, and on/off operations. It includes detailed implementations of timers to manage device state transitions, dimming effects, and various ZCL command processes like adding, removing, storing, or recalling scenes​.

* device_api.h
A header file defining the prototypes for device-specific control functions, primarily for PWM control (pwm_ctl_init, pwm_ctl_set_level). It serves as the interface to the corresponding ```.c``` implementation for device-level hardware control.

* zigbee_api.h
A header file providing declarations of essential data structures (scene_entry_t, startup_entry_t) and function prototypes that interface with Zigbee network management, event handling, scene management, and startup configurations. This header serves as an essential reference for the Zigbee-related application logic and interactions.

* zigbee_zcl_msg_handler.h
Declares the function prototype for handling Zigbee Cluster Library (ZCL) messages (zigbee_zcl_msg_handler). This handler processes incoming ZCL commands from the Zigbee stack and ensures appropriate reactions or actions in the application context.

* CMakeLists.txt
The build description of the example: the source files to compile, the application-specific compile
options, the flash addresses used by EnhancedFlashDataset, the Zigbee role macros (`ZB_ED_ROLE` /
`ZB_USE_SLEEP`, selected by `CONFIG_HOSAL_SOC_IDLE_SLEEP`), the main file
(`sdk_set_main_file()`) and finally `setup_project()`, which links the image.

* Kconfig
The example's own configuration menu, sourced by
[`config/examples/zigbee_examples.config`](../../config/examples/zigbee_examples.config) when the
example is selected. For the Zigbee device examples this file is currently an empty placeholder;
add `config` entries here when an application needs its own `menuconfig` options, which then become
`CONFIG_*` compile definitions.

* default-\<CHIP\>-EVB.config
One pre-generated configuration file per supported chip (RT582, RT583, RT584H, RT584HA4, RT584L).
This file is the only input the CMake tree reads: it selects the chip, the board, the enabled
components and the example itself, and is passed to CMake through `-DCUSTOM_CONFIG_DIR`. It also
fixes the Zigbee role of the shipped build via `CONFIG_HOSAL_SOC_IDLE_SLEEP`.

* rtos/
The FreeRTOS configuration used by the example: `rtos/CMakeLists.txt` selects the
`FreeRTOSConfig.h` matching the target family (`config/rt582/` or `config/rt584/`). Adjust task
sizes, tick rate and heap settings here.

## Zigbee Device Examples
The table below lists the Zigbee device examples and the endpoint definition each one declares in
its `zcl_construction.c`. Every example additionally declares a second endpoint carrying only the
OTA Upgrade cluster, and supports all five EVB targets. The role column is the role configured in
the shipped `default-*.config` files; it is chosen by `CONFIG_HOSAL_SOC_IDLE_SLEEP`, so either role
can be built from any of these examples.

| Example | HA device ID | Main clusters (application endpoint) | Default role |
|---|---|---|---|
| [`lighting-app`](../../examples/zigbee/lighting-app/) | 0x0101 Dimmable Light | Basic, Identify, Scenes, Groups, On/Off, Level Control | Router |
| [`wall-switch`](../../examples/zigbee/wall-switch/) | 0x0103 On/Off Light Switch | Basic, Identify, Groups, On/Off, Level Control (client) | Router |
| [`switch`](../../examples/zigbee/switch/) | 0x0103 On/Off Light Switch | Basic, Identify, Groups, On/Off (client) | Sleepy end device |
| [`smart-plug`](../../examples/zigbee/smart-plug/) | 0x0051 Smart Plug | Basic, Identify, Groups, On/Off, Metering, Electrical Measurement | Sleepy end device |
| [`door-lock`](../../examples/zigbee/door-lock/) | 0x000A Door Lock | Basic, Identify, Groups, Power Configuration, IAS Zone, Door Lock | Sleepy end device |
| [`door-sensor`](../../examples/zigbee/door-sensor/) | 0x0402 IAS Zone | Basic, Identify, Groups, Power Configuration, IAS Zone | Sleepy end device |
| [`pir-sensor`](../../examples/zigbee/pir-sensor/) | 0x0402 IAS Zone | Basic, Identify, Groups, Power Configuration, IAS Zone | Sleepy end device |
| [`temperature-sensor`](../../examples/zigbee/temperature-sensor/) | 0x0302 Temperature Sensor | Basic, Identify, Groups, Power Configuration, Temperature Measurement, Relative Humidity Measurement | Sleepy end device |
| [`illuminance-sensor`](../../examples/zigbee/illuminance-sensor/) | 0x0106 Light Sensor | Basic, Identify, Groups, Power Configuration, Illuminance Measurement | Sleepy end device |
| [`thermostat`](../../examples/zigbee/thermostat/) | 0x0301 Thermostat | Basic, Identify, Groups, Thermostat | Sleepy end device |

To create a new device type, start from the example whose cluster set is closest to the target
device and modify `zcl_construction.c` (attributes, clusters, endpoints), `zigbee_zcl_msg_handler.c`
(command handling) and `device_api.c` (hardware control).

## RT58x Evaluation Board Operation
* GPIO availability per chip is listed in the
  [GPIO pinout table](../SDK_Overview/pin_out.md). The pins an example drives are compile-time
  defaults in its `device_api.c` — `lighting-app` drives its PWM output on
  `CONFIG_APP_LIGHTING_PWM_PIN` (GPIO20) using `CONFIG_APP_LIGHTING_PWM_ID` (PWM0), each defined
  with an `#ifndef` fallback so it can be overridden by a Kconfig option or a compile definition.
* The debug console is the stdio UART configured in the example's `.config`
  (`CONFIG_UART_STDIO_PORT=0`, TX GPIO17, RX GPIO16, 115200 8N1 by default). See
  [Serial Monitor](../SDK_Setup/sdk_extension.md#serial-monitor) for the terminal setup.
* See [Download (Flash the Board)](../SDK_Setup/sdk_extension.md#download-flash-the-board) for
  flashing.
* See [J-Link setup](../SDK_Setup/sdk-setup-jlink.md) for JLink debugging.

## Compilation and Image update Procedure
Each Zigbee example is built by pointing CMake at one of its `default-<CHIP>-EVB.config` files:

```bash
cmake -DCUSTOM_CONFIG_DIR="examples/zigbee/lighting-app/default-RT582-EVB.config" -S . -B build_zigbee -G Ninja
cmake --build build_zigbee -j16
```

Make sure `arm-none-eabi-gcc`, `cmake`, and `ninja` are on the PATH. See the
[Setup guide](../SDK_Setup/sdk_setup.md). CMake sets the generator when it first creates the build
directory. To change the generator later, delete the build directory. Run the cmake command
again.

The image is produced in the build directory as `<CONFIG_BUILD_PROJECT>_<CHIP>.elf/.bin/.hex`. To
change the configuration, run `python3 -m menuconfig` from the repository root and save the result
back over the example's `.config` file; the Zigbee examples are selected by
`CONFIG_ZIGBEE_EXAMPLE` together with the per-example symbol (for instance
`CONFIG_ZIGBEE_LIGHTING_APP`), and the Zigbee stack itself by `CONFIG_BUILD_COMPONENT_ZIGBEE`.

For the toolchain setup, the VS Code Rafael extension build flow, and the image update (flashing)
procedure, please refer to:
* [Setup guide](../SDK_Setup/sdk_setup.md): toolchain, build, flash, UART and JLink debug
* [Build Examples](../SDK_Setup/sdk_build_examples.md) — building and programming from the VS Code Rafael extension
* [SDK Configuration System (Kconfig)](<../SDK_Development/SDK_ConfigurationSystem(Kconfig).md>) — how the `.config` files are generated and consumed
