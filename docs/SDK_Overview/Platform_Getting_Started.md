# SoC Platform — Getting Started

This page tells you how to set up your development environment. It also explains how the SDK is
organized, and how to build your first project.

## Supported chips

| Family | Core | Flash |
|---|---|---|
| RT581, RT582 | Cortex-M3 | 1 MB |
| RT583 | Cortex-M3 | 2 MB |
| RF1301 | Cortex-M33 | 1 MB |
| RT584H, RT584L | Cortex-M33 | 2 MB |
| RT584HA4 | Cortex-M33 | 4 MB |

The SDK supports these protocols: BLE, BLE Mesh, Zigbee, OpenThread, Sub-G, and multi-protocol RCP.

## 1. What you need

- A debug probe. Use a SEGGER J-Link adapter, or a Rafael CMSIS-DAP probe.
- An EVK for your target chip. Connect the EVK to the probe with the 20-pin SWD connector.
- VS Code, with the Rafael VS Code extension installed.
- The ARM toolchain, CMake, and Ninja, on the PATH. Windows, native Linux, and WSL each have a
  separate PATH. Set the PATH in every system you use.

For full, step-by-step instructions with pictures, see the [Setup guide](../SDK_Setup/sdk_setup.md).
If you use a J-Link probe, also see [J-Link setup](../SDK_Setup/sdk-setup-jlink.md).

## 2. How the SDK is organized

The SDK has four software layers:

```
Application            your example or project code
        |
Middleware              protocol stacks: BLE, BLE Mesh, Zigbee, OpenThread, Sub-G, 802.15.4 MAC
        |
HOSAL                    portable peripheral API (UART, timer, GPIO, ...)
        |
Chip driver              register-level driver for your specific chip
```

FreeRTOS runs below all four layers. The application and each protocol stack run as FreeRTOS
tasks. Application code must use the HOSAL API for peripherals. Do not access a chip's registers
directly. This lets the same application code run on different chips.

This structure appears in the SDK directory tree:

```
Rafael-IoT-SDK-Internal/
├── cmake/            build-system scripts (CMake DSL, toolchain, compiler flags)
├── components/       common/  cpc/  libc/  network/ (protocol stacks)  platform/ (HOSAL + chip drivers)  utility/
├── config/            Kconfig files that build up your project's .config
├── docs/              you are here
├── examples/          one directory per protocol family, each example has a default-<CHIP>-<BOARD>.config
├── thirdparty/        FreeRTOS, TensorFlow Lite Micro
├── tools/             flashing, debugging, and production tools, plus the Rafael VS Code extension
└── user_project/      your own project, outside the examples directory
```

## 3. Build, flash, and debug

Do these steps in the Rafael extension sidebar:

1. In **Build Configuration**, select your project, your chip, and your board.
2. Click **Build**.
3. Click **Download** to flash the board.
4. Click **Debug** to start a debug session.

See [Rafael Extension](../SDK_Setup/sdk_extension.md) for full details. For everyday work, you do
not need to type build commands by hand. The extension runs the same CMake configure step and
Ninja build for you. You can also build from the command line, without the extension. See
[Building examples, "Building from the command line"](../SDK_Setup/sdk_build_examples.md#building-from-the-command-line).

For an example, see [Building examples](../SDK_Setup/sdk_build_examples.md). It shows how to
build and flash the `helloworld` example.

## 4. Start your own application

Flash the bootloader before you flash an application. An empty chip cannot start an application
image without a bootloader.

1. Build the `examples/bootloader-app` project for your chip. This is the bootloader's source
   (`main.c`, `bootloader.c`, `isp.c`, `slip.c`).
   `components/platform/soc/<family>/<family>_bootloader/` only holds its linker script and
   startup code.
2. In the Rafael extension, open the **Download** panel.
3. Enter the bootloader image in the **Bootloader image** field.
4. Click **Download**.

To create your own project:

1. Run the command **Rafael: Create / Copy Project**.
2. Select a blank template, or select a copy of an existing example.
3. Select your chip.

The extension creates the project in `user_project/<name>`. It also sets up the build. See
[Rafael Extension, "Create a New Project"](../SDK_Setup/sdk_extension.md#create-a-new-project)
for more information.

Adding a new example to the SDK's own `examples/` directory is a different task from creating
your own project in `user_project/`. Only SDK maintainers do this task.
