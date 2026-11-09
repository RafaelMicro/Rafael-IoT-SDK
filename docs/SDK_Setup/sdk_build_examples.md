# Build Your First Example (Hello World)

This page shows how to build and flash the `helloworld` example on an RT582 EVB. It uses the
Rafael VS Code extension. See [Rafael Extension](sdk_extension.md) for a description of each
button.

1. Install VS Code and the Rafael extension. See the [Setup guide](sdk_setup.md).
2. Open the SDK folder in VS Code.
3. Click the extension's **Build Configuration** entry in the status bar. Select these values:
   - **Project**: `examples/helloworld`
   - **Chip / Board**: `RT582-EVB`
   - **Build Type**: `Release` or `Debug`
4. Click **Build**. For the first build, click **Rebuild** instead. When the build finishes, the
   sidebar's **Output** row shows the Flash and RAM usage, and the built `.bin` and `.hex` files.
5. Connect the EVK's SWD header to a CMSIS-DAP probe or a J-Link probe.
6. Click **Download** to flash the board.
7. Open **Serial Monitor**. Select the board's COM port. Set the baud rate to 115200.
8. Reset the board. The Hello World example prints its output to the console.

If you have no debug probe, use **Rafael: Download via UART (ISP Lite)** instead. See
[Rafael Extension, "Download"](sdk_extension.md#download-flash-the-board).

## Building from the command line

You do not need the extension or VS Code to build. Every build runs a plain CMake configure
step. This step reads one Kconfig-generated `.config` file. Use the `-DCUSTOM_CONFIG_DIR` option
to select this file. Each example has one `default-<CHIP>-<BOARD>.config` file for each supported
chip, in the example's own folder. Point `-DCUSTOM_CONFIG_DIR` at that file:

```sh
cmake -DCUSTOM_CONFIG_DIR="examples/helloworld/default-RT582-EVB.config" -S . -B build -G Ninja
cmake --build build
```

Make sure `arm-none-eabi-gcc`, `cmake`, and `ninja` are on the PATH. See the
[Setup guide](sdk_setup.md#3-install-the-arm-toolchain-and-add-it-to-the-path). The build writes
its output files to the build directory, as `<project>_<CHIP>.elf/.bin/.hex/.map`.
