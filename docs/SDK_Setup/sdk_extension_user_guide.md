# Rafael IoT SDK Extension — User Guide

The Rafael IoT SDK Extension is a tool. It runs inside VS Code.

The tool helps you do four things:

1. Build firmware for a Rafael chip.
2. Send the firmware to a board.
3. Debug the firmware on the board.
4. Watch messages from the board.

You do not need to type build commands. You click buttons in VS Code.

## Contents

1. [Key words](#1-key-words)
2. [Build configuration](#2-build-configuration)
3. [Building](#3-building)
4. [The Kconfig editor](#4-the-kconfig-editor)
5. [Creating new projects](#5-creating-new-projects)
6. [Firmware stamping (SHA256)](#6-firmware-stamping-sha256)
7. [Downloading (programming the board)](#7-downloading-programming-the-board)
8. [Debugging](#8-debugging)
9. [Serial monitor](#9-serial-monitor)
10. [SDK manager](#10-sdk-manager)
11. [Documentation browser](#11-documentation-browser)
12. [Tools, toolchain manager, and Settings panel](#12-tools-toolchain-manager-and-settings-panel)
13. [Settings reference](#13-settings-reference-rafael)
14. [Troubleshooting](#14-troubleshooting)

---

## 1. Key words

This table explains words used in this guide. Read this table first.

| Word | Simple meaning |
|---|---|
| **SDK root** | The main folder of your `Rafael-IoT-SDK` copy. You open this folder in VS Code. |
| **`.config`** | One file, at the SDK root. It stores your current build settings: the example, the chip, every option. The SDK builds firmware from this one file only. |
| **defconfig** | A ready-made settings file for one chip and board. Example: `default-RT582-EVB.config`. When you pick a chip, the extension copies its defconfig into `.config`. |
| **Build configuration** | Three choices, together: a project, a chip/board, and a build type (Debug or Release). You always see these three choices in the status bar. |
| **Kconfig** | The SDK's settings system. Linux and Zephyr use the same system. Use the Kconfig editor (Section 4) to view and change settings. Do not edit `.config` by hand. |
| **Debug probe** | A small USB adapter. It connects your PC to the board. There are two kinds: **CMSIS-DAP** (works with the tool already inside the SDK) and **SEGGER J-Link** (works with SEGGER's own tool). |
| **ELF file** | Your compiled firmware, with debug information inside. |
| **SVD file** | A map of the chip's registers. The debugger uses this file to show register names, not just numbers. |

---

## 2. Build configuration

A **build configuration** is three choices: project, chip/board, and build
type. You see these three choices at the bottom of the window, in the status
bar:

```
[icon] helloworld | RT582-EVB | Release     Build     Download     Serial
```

- Click this status bar item. Or run **Rafael: Select Project / Chip / Build
  Type**. A picker opens. Choose a project. Then choose a chip/board. Then
  choose a build type.
- The extension finds projects for you. Any folder under `examples/` or
  `user_project/` with a `default-<chip>-<board>.config` file is a project.
- When you pick a chip, the extension copies that chip's defconfig over
  `.config`.
  - Your Kconfig edits stay, if you keep the **same** project.
  - If you switch to a **different** project, the extension asks you first.
    The switch will replace `.config` with the new project's own defconfig.

## 3. Building

- Click **Build**. You find this button in the status bar, and in the
  sidebar. Or run **Rafael: Build**. This step runs CMake, then Ninja.
- Errors and warnings show in the **Problems panel**. Click an error to jump
  to that line of code. The status bar shows build progress as a percentage.
- The build creates these files: `<project>/build/<name>_<CHIP>.elf`,
  `.bin`, `.hex`, and `.map`. After each build, the sidebar's **Output** row
  shows how much Flash and RAM the firmware uses.
- **Rebuild** deletes old build files first, then builds again from
  scratch.
- **Clean** only deletes the build output folder.
- Expand the **Sources** row under your project. This row lists every source
  file the current build uses. Click a file to open it. This row appears
  after your first build.
- Only one build can run at a time. The whole SDK shares one `.config` file.
- The build output panel prints the exact `cmake` and `ninja` commands it
  used. Copy these commands to run the same build in a plain terminal, or in
  a CI system. Two extra steps are needed there: copy the project's defconfig
  onto `<sdk>/.config` first, and add the SDK's `toolchain/` `bin` folders to
  `PATH`.
- You can also save one build configuration as a VS Code task, in
  `tasks.json`:
  ```json
  { "type": "rafael-build", "project": "examples/helloworld",
    "defconfig": "RT582-EVB", "buildType": "Release",
    "problemMatcher": "$rafael-gcc" }
  ```

---

## 4. The Kconfig editor

Run **Rafael: Open Kconfig Editor**. You can also open it from the sidebar's
**Project** group.

The editor has three parts: a category list on the left, that category's
options on the right, and a details panel at the bottom. The details panel
shows help text, the option's type, its current value, and why an option is
on or off.

**The files the editor changes:**

| File | What it is |
|---|---|
| `<sdk>/.config` | Your working settings file. **Save** writes to this file. The SDK builds from this file only. |
| `<project>/default-<chip>-<board>.config` | A **read-only** template file. The extension copies this file over `.config` when you switch project or chip. The editor does not write to this file, unless you ask it to. |

A plain **Save** does not last forever. If you switch project or chip, the
extension copies the template over `.config` again. Your edit is gone.

**To make an edit permanent**, use **Save ▾ → "Save as this example's
default"**. This command copies `.config` back onto the example's own
`default-*.config` file. The editor asks you to confirm first. This file is
tracked by the SDK's git history.

If a build changes the defconfig while the editor is open, the editor
reloads automatically. If you have unsaved edits, the editor warns you instead.
It does not discard your work.

**Two modes.** A badge in the toolbar always shows the current mode.

- **`menuconfig`**. This is the full editor. It checks option dependencies
  live. It greys out options you cannot use. It shows help text. This mode
  needs `python` and `pip install kconfiglib`.
- **`text mode (limited)`**. This is a simple fallback mode. The editor uses
  this mode when it cannot find Python or `kconfiglib`. This mode only shows
  options already in `.config`. It does not check dependencies. It has no
  help text.
  If Python is installed but not found, set `rafael.kconfig.pythonPath` to
  its full path, then click **Reload**.

## 5. Creating new projects

Run **Rafael: Create / Copy Project (template or example)**. A wizard opens
with three steps:

1. Type a name for your project.
2. Pick a starting point: an empty template
   (`examples/template/application`), or a copy of any existing example.
3. Pick a chip.

The extension creates your project under `user_project/<name>`. This is the
only folder the build system supports for your own projects. The extension
sets up the defconfig for you. Your new project becomes the active build
configuration right away.

## 6. Firmware stamping (SHA256)

After every successful build, the extension writes two values into the
firmware image: the image size, and a SHA256 checksum of the whole image.
Both the `.bin` file and the `.hex` file get these values. This step is
automatic. You do not need to turn it on.

This step is safe. It only writes to an image that already has an empty
`BUILD_INFO` space ready for these two values. A project without
`Version_Information` is left alone. An image that is already stamped is
left alone too.

Check the Output panel for the line `SHA256 stamped: <digest>`. This line
confirms the step ran. You see a warning if only the `.bin` file could be
stamped, or if you try to download a `.hex` file that was never stamped
(Download uses the `.hex` file by default).

---

## 7. Downloading (programming the board)

- Click **Download**. This step sends firmware to the board. It shows three
  stages, one after another: erase, program, verify.
- **Set Download Target Binary** lets you pick a different file to send.
- Download and Erase run inside a **Rafael Download** task. This task reuses
  one terminal tab. Build uses its own **Rafael Build** tab.

**Which probe the extension uses.** The sidebar's **Debug Probe** row
always names the probe the extension will use. This is either the probe you
picked, or the first probe the extension finds (for example,
`COM106 (auto)`). Run **Rafael: Select Debug Probe**, or click this row, to
pick a probe yourself. This choice lasts only for the current VS Code
window. VS Code forgets the choice when you close it. The picker always
offers an **Auto** option, so you can always clear your choice.

A picked probe only matters when two or more probes are connected. If you
unplug the picked probe, and only one probe is left, the extension uses that
one probe. The row tells you this happened (for example,
`COM9 (pinned ABC123 not attached)`). There is no second board to send
firmware to by mistake. If two or more probes are still connected, the
extension cannot guess which one you meant. Download and Debug stop. The row
turns into a warning. Click the row to choose again.

If you want a probe choice that survives a VS Code restart, set
`rafael.debug.probeSerial` in Settings by hand. A choice you make in one
window overrides this setting, until you reload the window.

**CMSIS-DAP and SEGGER J-Link.** CMSIS-DAP works with the tool already inside
the SDK (OpenOCD). You do not need to install anything extra. For a J-Link
probe, install SEGGER's J-Link Software Pack. If the extension cannot find
it, set `rafael.tools.jlinkPath` to the **installation folder**. On Windows,
you must install the J-Link pack. Without it, SEGGER's own driver blocks
another tool from reaching the probe. On Linux, the J-Link pack is optional.
Without it, the SDK's bundled OpenOCD drives the J-Link probe instead. If
the extension finds no probe, run **Rafael: Probe Diagnostics** to check
what your computer can see.

**If you have no debug probe, use ISP Lite.** ISP Lite sends firmware over
the board's USB-to-serial (UART) connection. You do not need an extra tool.
Run **Rafael: Download via UART (ISP Lite)**. This has two steps:

1. Pick the port. Click **Connect**. Then power-cycle the board: turn it
   off, then on again. ISP mode is only open for about 50 milliseconds after
   the board resets. The status dot turns green. The panel shows the flash
   size, chip ID, and chip series.
2. Click **Download**. Do not reset the board while this runs. One
   **Connect** step lets you click **Download** many times.

The **Download** and **Erase Chip…** buttons stay off until you connect. The
connection ends if you click **Disconnect**, close the panel, or an
operation fails. To connect again, power-cycle the board again. The
**Application image** row uses your current build by default. The
**Bootloader image** row is optional and empty by default. Writing a new
bootloader replaces the one thing that lets the board start up. The
extension never fills in this row for you. A `.bin` file needs an address (a
`.hex` file already has one built in). The extension guesses this address
from your build configuration or the connected chip. You can change it.

If the connected board's chip does not match your build configuration, the
panel shows a warning and `⚠ Connected`. It still lets you continue. Your
firmware was built for the configured chip. Treat this warning as a check on
which board is plugged in.

**Erase whole area** (a checkbox):

| Checkbox | What gets erased |
|---|---|
| Off (default) | Only the flash sectors your image actually uses |
| On | The whole application area |
| On, with a bootloader image | The bootloader area, and the application area |

None of these ever erase the MP calibration sector. **Erase Chip…** is a
separate action. It only erases. It does not write anything.

---

## 8. Debugging

Click **Debug** in the sidebar. Or run **Rafael: Start Debug**.

1. The extension finds your connected probe automatically. It checks again every
   3 seconds while the **Debug** group is open.
2. OpenOCD starts (or SEGGER's J-Link GDB Server, for a J-Link probe). This
   program acts as a GDB server. The `cortex-debug` extension connects to
   it, using your build's ELF file and the chip's SVD file. This is why you
   see register names, not just numbers.
3. `cortex-debug` is required. It installs automatically together with this
   extension. If you removed it, **Start Debug** tells you, and offers a
   button to install it again.

The `F5` key also starts a debug session. It uses a template file the SDK
writes automatically, `.vscode/launch.json`. But `F5` does not pick a
specific probe. **If more than one probe is connected, use Start Debug from
the sidebar**, not `F5`.

## 9. Serial monitor

Click **Serial** in the status bar. Or run **Rafael: Open Serial Monitor**.
Pick a port, then a baud rate. A terminal tab opens. You can split this tab,
or open more than one port at the same time.

- The Enter key sends a line ending. The setting `rafael.serial.lineEnding`
  controls which one. The default is CR.
- Turn on `rafael.serial.localEcho` to see your own typed characters on
  screen. Use this setting if your device does not echo them back to you.
- **Disconnect** closes the port. The panel stays open. **Reconnect** opens
  the port again. Use **Reconnect** after you unplug and replug the board
  too.
- **Break** sends a serial break signal.
- **Clear** only clears the screen. The connection and any file logging
  keep running.
- Click **Log** (or run **Rafael: Serial Monitor: Start/Stop Logging**) to
  write everything the board sends to a file with a timestamp in its name.
  You can open this file to read it while it records. Do not **save** the
  file from an editor while it is recording. Saving replaces everything the
  file already holds.

## 10. SDK manager

The **SDK Manager** sidebar view lists SDK versions from GitHub. From here
you can install a version, update its submodules, open it as a workspace, or
remove it. The install location comes from *SDK Install Path*, in
**Rafael Settings**.

To switch versions, open a **different folder**. There is no switch button
that changes the current folder in place. Right-click a version, then choose
**Open as Workspace** (opens a new window) or **Open SDK Folder in
Explorer**. Each window remembers its own build configuration.

`user_project/` folders do **not** move to a new SDK version by themselves.
Copy them by hand if you want the same custom project on a newer SDK
version.

## 11. Documentation browser

The **Documentation** sidebar view shows the SDK's own `docs/` folder as a
two-level list. This folder holds setup guides, and guides for BLE, Zigbee,
and peripheral drivers. Click a file to open it in VS Code's Markdown
preview. This view is empty until you open an SDK folder. It updates by
itself when `docs/` changes.

## 12. Tools, toolchain manager, and Settings panel

**Tools** (inside **Tools & SDK**) lists every tool in use: ARM GCC, CMake,
Ninja, OpenOCD, and SEGGER J-Link. Each row shows a version and a source
(**SDK bundled**, **from settings**, or **from PATH**). Right-click a row:

- **Change path…** sets one specific install. This choice always wins over
  the SDK's own bundled toolchain.
- **Reset to auto** removes your choice. The extension goes back to finding
  the tool automatically.

If a row shows no version, the extension cannot find that tool. Install
ARM GCC, CMake, and Ninja yourself, and add each one to the PATH. See the
[Setup guide, "Install the ARM toolchain"](sdk_setup.md#3-install-the-arm-toolchain-and-add-it-to-the-path).
This is the most reliable fix, on every platform.

As an alternative for CMake and Ninja only, run
**Rafael: Install / Manage Toolchain**. This installs them through your
computer's own package manager (winget, Homebrew, apt, dnf, pacman, or
zypper). This command does not install ARM GCC. **CMake must be version
3.24 through 3.31.** CMake 4.x breaks the SDK's built-in mbedTLS. The
toolchain manager only offers supported versions. If you already have a
too-new CMake installed, the manager warns you. Use **Set CMake Path…** to
fix it.

The **Settings panel** edits every setting from Section 13 on one page. Open
it from the gear icon in the **Rafael SDK** sidebar group, or run
**Rafael: Open Settings Panel**.

---

## 13. Settings reference (`rafael.*`)

| Setting | Meaning |
|---|---|
| `sdk.path` | The SDK root folder, set by hand. Leave empty to let the extension find it. |
| `kconfig.pythonPath` | The Python program the Kconfig editor's menuconfig mode uses (needs `kconfiglib`). Leave empty to use `python3` or `python` from `PATH`. |
| `tools.gccPath` / `cmakePath` / `ninjaPath` / `openocdPath` | A tool's path, set by hand. The extension checks in this order: this setting, then the SDK's bundled copy, then `PATH`. |
| `tools.jlinkPath` | The SEGGER J-Link install **folder** (not the program file itself). Only needed for a J-Link probe. CMSIS-DAP needs nothing installed. Leave empty to auto-detect the newest install found. |
| `build.configureOnSelect` | After you pick a project, chip, or build type, run a quick CMake configure step (about 1 second). This fills in the **Sources** list right away. Turn this off if you edit `.vscode/launch.json` by hand. A configure step rewrites that file. |
| `flash.openocdArgs` | Extra arguments added before the Download or Erase command. |
| `serial.baudRate` | The default baud rate for the **Serial Monitor only**. ISP Lite ignores this setting. Its bootloader always uses a fixed rate. Valid range: 1200 to 4000000. |
| `serial.lineEnding` / `localEcho` | Serial Monitor behavior, described in Section 9. |
| `debug.gdbPath` | The default value is `arm-none-eabi-gdb`, meaning: use the `gdb` program next to the resolved ARM GCC, or `PATH` if none is found. Any other value is used exactly as typed. |
| `debug.svdPath` | Overrides the SVD file the extension picks automatically. |
| `debug.openocdArgs` | Extra arguments for the debug session's OpenOCD (separate from `flash.openocdArgs`). |
| `debug.interfaceCfg` / `gdbPort` / `adapterSpeed` | OpenOCD debug session options. `adapterSpeed` sets the SWD clock speed. This also applies to J-Link. Set it to `0` to let the target script decide. `gdbPort` accepts values from 1024 to 65535. |
| `debug.probeSerial` | A probe choice, by serial number, that survives a restart. Useful for a bench that always uses the same probes. Leave empty to use the first probe found. **Rafael: Select Debug Probe** does **not** change this setting. That command's choice lasts for one window only. It overrides this setting while that window is open. |

The **Settings panel** (Section 12) edits all of these settings on one page.
The extension still reads old settings under the `mcuBuild.*` name, from
before the extension's rename. Old settings keep working.

---

## 14. Troubleshooting

| Problem | Fix |
|---|---|
| Board does not respond after firmware that sleeps (`Error connecting DP: cannot read IDR`) | Deep sleep turns off the debug port too. Only a reset brings it back. Download, Erase, and Debug retry automatically with a reset-and-connect step. Look for `woke the target on attempt 1` in the Output panel. In most cases you do not need to do anything. If the problem stays, click (do not hold) the board's **RESET** button, then retry right away. As another option, use **ISP Lite**, which never uses the debug port. Erase still needs the debug port, so ISP Lite cannot replace it for that action. **This problem does not affect J-Link users.** SEGGER's own tools can reach a sleeping board without this workaround. |
| "No Rafael IoT SDK detected" | Your workspace folder needs a `cmake/entry.cmake` file inside it. Or set `rafael.sdk.path` by hand. |
| Build fails because a tool is missing, or the **Tools** section (Section 12) shows no version for ARM GCC, CMake, or Ninja | Install that tool yourself and add it to the PATH. See the [Setup guide, "Install the ARM toolchain"](sdk_setup.md#3-install-the-arm-toolchain-and-add-it-to-the-path). For CMake and Ninja only, you can also run **Rafael: Install / Manage Toolchain** instead. |
| Wrong tool version, or you want a different compiler | Open the **Tools** section (Section 12). It shows each tool's version and source. Right-click a tool, then choose **Change path…** or **Reset to auto**. |
| "The .config does not select any example" | No specific example is selected yet. In the Kconfig editor, under **Select Application**, pick one specific example. Picking only a category (Bluetooth, Zigbee, and so on) is not enough. |
| Build errors do not show in the Problems panel | Check that the build ran through the extension, or through a `rafael-build` task, not through a plain terminal. |
| A "Select a Configure Preset" or "Select a Kit" popup appears when you open the folder | This popup comes from a different extension, **CMake Tools**. It reacts to the SDK's `CMakeLists.txt` file. Add `"cmake.configureOnOpen": false` to the SDK folder's workspace settings to stop it. |
| The Kconfig editor badge says "text mode (limited)" | The extension cannot find a working Python, or cannot find `kconfiglib`. Install Python 3, then run `pip install kconfiglib`. Or set `rafael.kconfig.pythonPath` by hand, then click **Reload**. |
| "I saved my change, but it disappeared" | **Save** writes to `.config`. The next time you switch project or chip, the example's own `default-*.config` file overwrites `.config` again. Use **Save ▾ → "Save as this example's default"** to keep your change (see Section 4). |
| Download shows "No OpenOCD target script known" | The defconfig's `CONFIG_CHIP` value is not one the extension knows. The extension expects RT58x, RT584, or RF1301. |
| The extension cannot find a J-Link, or Download/Debug fails with a J-Link | Check that the J-Link Software Pack is installed. If it is not in the usual place, set `rafael.tools.jlinkPath` to point at its install **folder**. Run **Rafael: Probe Diagnostics** to check what your computer can see. |
| A company firewall or proxy blocks SDK downloads | The version list falls back to the `git ls-remote` command, which uses git's own proxy and certificate settings. Set `git config --global http.proxy` (and `http.sslCAInfo`, if your company's proxy intercepts HTTPS). If this still fails, install the SDK by hand, then use **Open Folder**. |
| "SDK is incomplete: its components/ submodule failed to download" | Click **Retry Submodules** on the message. Or right-click the version in **SDK Manager**, then choose **Update Submodules**. |
| The serial port is busy | Only one program can use a COM port at a time. If this extension's Serial Monitor is using it, click **Disconnect** first. |
| On Linux: "Permission denied" when you open a port, or OpenOCD cannot see the probe | Your `udev` rule or `dialout` group is not set up yet. See the SDK's Linux setup guide. Or ask a colleague who has already set this up. |
| Multi-root workspaces | The SDK folder does not need to be the first folder in your workspace. The extension always writes `.vscode/launch.json` and `c_cpp_properties.json` into the folder that actually holds the SDK. |
