# Rafael Extension

The extension adds a full workbench for Rafael Micro **RT58x / RT584 /
RF1301** chips to VS Code. Pick an example. Build it. Flash it to your
board. Debug it. You do this from the sidebar. You do not type build
commands.

## Install

1. Find the `.vsix` file in the SDK checkout, at
   `tools/rafael_extension/rafael-iot-<version>.vsix`.
2. Run this command from the SDK root, to install the file:
   ```sh
   code --install-extension tools/rafael_extension/rafael-iot-<version>.vsix
   ```
3. VS Code installs the `cortex-debug` debugger extension at the same time.
   This happens automatically. You do not need a separate step.
4. Open your `Rafael-IoT-SDK` checkout as the VS Code workspace folder
   (**File > Open Folder…**). The extension finds the file
   `cmake/entry.cmake` at the workspace root and activates itself. Its icon
   then appears in the Activity Bar, the strip of icons on the left side of
   the window.
5. Open the **Tools & SDK** area and check the **Tools** list. If ARM GCC,
   CMake, or Ninja shows no version, install it yourself before you build.
   See [Tools, Toolchain Manager, and Settings](#tools-toolchain-manager-and-settings).

If you are new to the extension, open **Help > Welcome > Walkthroughs**, then
select **Get started with Rafael IoT SDK**. This walkthrough shows the same
steps, with a checklist.

## Sidebar Overview

The Rafael icon in the Activity Bar opens a sidebar. The sidebar has these
areas:

| Sidebar area | What it does |
|---|---|
| **Project** | Pick an example or your own project. Choose the chip and board. See which source files your build compiles, under **Sources**. |
| **Build & Download** | Build, Rebuild, Clean, and Download (flash) the current build configuration. |
| **Debug** | Start a debug session with the connected probe. |
| **Serial Monitor** | Open a terminal for the board's UART console. |
| **Tools & SDK** | See which versions of the compiler, CMake, Ninja, OpenOCD, and J-Link are in use, and where each one comes from. Install missing tools. |
| **SDK Manager** | Download, update, and switch between versions of the Rafael IoT SDK. |
| **Documentation** | Browse the SDK's own `docs/` folder without leaving VS Code. |

A gear icon opens the **Settings Panel**. This panel edits every `rafael.*`
setting on one page.

## Build Configuration

A **build configuration** has three parts: a project, a chip/board, and a
build type (Debug, Release, or another type). The status bar shows all
three parts at all times:

```
[icon] helloworld | RT582-EVB | Release     Build     Download     Serial
```

Click this status bar entry to open a picker. You can also run
**Rafael: Select Project / Chip / Build Type**. The picker has three steps:
project, then chip/board, then build type. The extension finds projects
automatically. Any folder under `examples/` or `user_project/` that contains
a `default-<chip>-<board>.config` file counts as a project.

When you select a chip, the extension copies that chip's template settings
(`default-<chip>-<board>.config`) over the SDK's root `.config` file. Your
Kconfig changes stay in place if you keep the **same** project. If you
switch to a **different** project, the extension asks you to confirm first.
The switch overwrites `.config` with that new project's own template.

## Build

- Click **Build**. You find this button in the status bar and in the
  sidebar. You can also run **Rafael: Build**. This step runs a CMake
  configure step when needed, then runs Ninja.
- Errors and warnings appear in the **Problems panel**. Click one to jump to
  that line of code. The status bar shows build progress as a percentage.
- The build writes output files to
  `<project>/build/<name>_<CHIP>.{elf,bin,hex,map}`. After each successful
  build, the sidebar's **Output** row shows Flash and RAM usage.
- **Rebuild** runs a clean build from scratch. **Clean** deletes the build
  output directory.
- Expand the **Sources** row under your project. This row lists exactly
  which source files the current configuration compiles. The extension
  reads this list from `compile_commands.json`. It does not guess. Click a
  file to open it. This row appears after your first build or configure
  step.
- The build output panel prints the exact `cmake` and `ninja` commands it
  ran. Copy these commands to reproduce the same build in a plain terminal
  or in CI.

## Download (Flash the Board)

- Click **Download**. This step flashes the current build configuration's
  firmware to the board. Progress moves through three stages: erase,
  program, and verify.
- Use **Set Download Target Binary** to pick a different file to flash.
- Download and Erase run inside the **Rafael Download** task. This task
  reuses one terminal tab. Build uses its own **Rafael Build** tab.

**Which probe the extension uses.** The sidebar's **Debug Probe** row always
names the probe the extension will use. This is either the probe you
selected, or the first probe the extension finds automatically. Run
**Rafael: Select Debug Probe**, or click this row, to pick a probe. This
choice lasts only for the current VS Code window.

The extension supports two probe types: **CMSIS-DAP** and **SEGGER
J-Link**. CMSIS-DAP works with the OpenOCD tool already bundled in the SDK.
It needs no extra install. For a J-Link probe, install SEGGER's J-Link
Software Pack. If the extension does not detect it, set
`rafael.tools.jlinkPath` to the install folder. On Windows, you must
install the J-Link Software Pack. Without it, SEGGER's own driver blocks
OpenOCD from reaching the probe. On Linux, the J-Link Software Pack is
optional. Without it, the SDK's bundled OpenOCD drives the J-Link probe
instead.

**If you have no debug probe, use ISP Lite.** ISP Lite is a built-in UART
downloader. It needs no external tool. Run
**Rafael: Download via UART (ISP Lite)**:

1. Select the port, then click **Connect**. Power-cycle the board after
   this. ISP mode stays open for about 50 milliseconds after a reset, so
   connect before you reset the board. The status dot turns green once the
   extension connects.
2. Click **Download**. Do not reset the board while this runs. One
   **Connect** session supports many downloads.

## Debug

Click **Debug** in the sidebar. You can also run **Rafael: Start Debug**:

1. The extension detects the connected probe automatically.
2. OpenOCD starts as a GDB server. For a J-Link probe, SEGGER's own J-Link
   GDB Server starts instead. The `cortex-debug` extension connects to this
   server. It uses your build's ELF file and the chip's SVD file. The SVD
   file gives the peripheral registers their names.
3. You get breakpoints, stepping, variable inspection, and live views of
   peripheral registers.

Pressing `F5` also starts a debug session. This uses a template
`launch.json` file that the SDK regenerates on every configure step. If
more than one probe is attached, use **Start Debug** from the sidebar
instead of `F5`. **Start Debug** always picks the selected probe correctly.

## Serial Monitor

Click **Serial** in the status bar. You can also run
**Rafael: Open Serial Monitor**. Pick a port, then a baud rate. The monitor
opens as a terminal tab that uses `xterm.js`. You can split this tab. You
can also open more than one port at the same time.

- **Disconnect** releases the port. The panel stays open. **Reconnect**
  opens the port again.
- **Break** sends a serial break signal.
- **Clear** only clears the view. The connection and any logging keep
  running.
- Click **Log** (or run **Rafael: Serial Monitor: Start/Stop Logging**) to
  write incoming data to a file with a timestamp in its name.

## Create a New Project

Run **Rafael: Create / Copy Project (template or example)**. This opens a
three-step wizard: name, then base, then chip. For the base, pick a blank
template (`examples/template/application`) or a copy of any existing
example.

The extension creates the project under `user_project/<name>`. This is the
only supported location for custom projects. The extension sets up the
defconfig automatically. The new project becomes your current build
configuration right away.

## The Kconfig Editor

Run **Rafael: Open Kconfig Editor**. You can also open it from the
sidebar's **Project** area. This editor changes the SDK's root `.config`
file. This is the only file the build reads. A plain **Save** keeps your
change only while you build the *same* project. If you switch projects or
chips, the extension reapplies that project's template. Your change is
then lost. To make a change permanent, use
**Save ▾ → "Save as this example's default"**. This command writes your
change back into that example's own `default-*.config` file.

The editor has two modes. A badge in the toolbar always shows the current
mode:

- **`menuconfig`** — the full editor. It checks option dependencies live. It
  greys out options you cannot use. It shows help text. This mode needs
  `python` and `pip install kconfiglib`.
- **`text mode (limited)`** — a fallback mode. The editor uses this mode
  when it cannot find Python or `kconfiglib`. This mode shows only options
  already present in `.config`. It does not check dependencies. It has no
  help text.

## Tools, Toolchain Manager, and Settings

**Tools** (under **Tools & SDK**) lists every tool in use: ARM GCC, CMake,
Ninja, OpenOCD, and SEGGER J-Link. Each row shows a version and a source
(**SDK bundled**, **from settings**, or **from PATH**). Right-click a row
for **Change path…** or **Reset to auto**.

If a row shows no version, the extension cannot find that tool. Install ARM
GCC, CMake, and Ninja yourself, and add each one to the PATH. See the
[Setup guide, "Install the ARM toolchain"](sdk_setup.md#3-install-the-arm-toolchain-and-add-it-to-the-path).
As an alternative for CMake and Ninja only, run
**Rafael: Install / Manage Toolchain**. This installs them through your
system's own package manager: winget, Homebrew, apt, dnf, pacman, or
zypper. This command does not install ARM GCC.

The **Settings Panel** edits every `rafael.*` setting on one page. Open it
from the gear icon in the sidebar, or run **Rafael: Open Settings Panel**.

---

For the full settings reference and troubleshooting guide, see
[Rafael IoT SDK Extension — User Guide](sdk_extension_user_guide.md).
