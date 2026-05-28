# WSL + OpenOCD + VS Code Debugging Guide

**For RT58x (RT58x/RT584x) Chips and Rafael-IOT-SDK Development**

---

## Quick Start

If you're already familiar with the basic workflow, follow these steps to get started quickly:

```bash
# 1. Windows: Attach debugger to WSL
usbipd attach --wsl --busid <BUSID>

# 2. WSL: Verify device
lsusb | grep "0d28:0204"

# 3. Start OpenOCD
cd /path/to/matter_sdk
./openocd_rt58x.sh

# 4. VS Code: Press F5 to start debugging
```

⚠️ **First time user?** Please read this document completely, especially sections 2-4.

---

## Table of Contents

1. [System Architecture](#1-system-architecture)
2. [Prerequisites Check](#2-prerequisites-check)
3. [Windows Configuration](#3-windows-configuration)
4. [WSL Configuration](#4-wsl-configuration)
5. [Matter SDK Setup](#5-matter-sdk-setup)
6. [Firmware Flashing](#6-firmware-flashing)
7. [OpenOCD Startup](#7-openocd-startup)
8. [OpenOCD Configuration Optimization](#8-openocd-configuration-optimization)
9. [VS Code Debug Configuration](#9-vs-code-debug-configuration)
10. [Python Environment Setup](#10-python-environment-setup)
11. [Debugging Workflow](#11-debugging-workflow)
12. [Troubleshooting](#12-troubleshooting)
13. [Common Errors Quick Reference](#13-common-errors-quick-reference)
14. [Appendix](#14-appendix)

---

## 1. System Architecture

### 1.1 Overall Architecture

```
┌────────────────────────────────────────────────────────────┐
│                        Windows Host                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ CMSIS-DAP    │  │ usbipd-win   │  │ VS Code          │  │
│  │ Debugger     │─▶│ USB Forward  │  │ (Frontend UI)    │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
└───────────────────────────┬────────────────────────────────┘
                            │ USB-over-IP
┌───────────────────────────▼─────────────────────────────────┐
│                    WSL2 (Ubuntu 22.04)                      │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ OpenOCD                                              │   │
│  │ - CMSIS-DAP Driver                                   │   │
│  │ - GDB Server (Port 50000)                            │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ ARM Toolchain                                        │   │
│  │ - arm-none-eabi-gdb (Debugger)                       │   │
│  │ - arm-none-eabi-objdump (Disassembler)               │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ Rafael-IoT-SDK                                       │   │
│  │ - Source Code                                        │   │
│  │ - Build Output (.elf, .bin)                          │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │ VS Code cortex-debug Extension                       │   │
│  │ - Connect to GDB                                     │   │
│  │ - Source-level Debug Interface                       │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 Workflow

1. **USB Forwarding**: CMSIS-DAP on Windows is forwarded to WSL2 via usbipd
2. **OpenOCD Connection**: OpenOCD in WSL2 connects to CMSIS-DAP and establishes GDB Server
3. **GDB Debugging**: arm-none-eabi-gdb connects to GDB Server through port 50000
4. **VS Code Frontend**: cortex-debug extension provides graphical debugging interface

---

## 2. Prerequisites Check

### 2.1 Hardware Requirements

- [ ] CMSIS-DAP compatible debugger (e.g., DAPLink)
- [ ] RT58x/RT584x development board
- [ ] USB cable (ensure it's a data cable, not charge-only)
- [ ] Proper SWD wiring:
  - VTref (target reference voltage)
  - SWDIO (data line)
  - SWCLK (clock line)
  - GND (ground)

### 2.2 Software Requirements

#### Windows Side
- [ ] Windows 10 (version 1903+) or Windows 11
- [ ] WSL2 enabled
- [ ] Ubuntu 22.04 LTS (recommended)
- [ ] usbipd-win installed
- [ ] VS Code installed

#### WSL Side
- [ ] OpenOCD (SDK/tools/Debugger/OpenOCD)
- [ ] ARM Toolchain (SDK/toolchain)
- [ ] libusb and related dependencies
- [ ] Python 3.8+

### 2.3 Permission Requirements

- [ ] Windows: Administrator privileges (for usbipd)
- [ ] WSL: sudo privileges (for package installation)

---

## 3. Windows Configuration

### 3.1 Enable WSL2

If WSL2 is not yet enabled, execute in PowerShell with administrator privileges:

```powershell
# Enable WSL
wsl --install

# Set WSL 2 as default version
wsl --set-default-version 2

# Install Ubuntu 22.04
wsl --install -d Ubuntu-22.04
```

After restarting the computer, complete Ubuntu's initial setup (create username and password).

### 3.2 Install usbipd-win

#### Method 1: Using winget (Recommended)

```powershell
# Execute in PowerShell with administrator privileges
winget install usbipd
```

#### Method 2: Manual Download

1. Visit official project: https://github.com/dorssel/usbipd-win/releases
2. Download the latest `.msi` installer
3. Run the installer as administrator

After installation, **restart PowerShell** to make the command effective.

### 3.3 Configure USB Device Sharing

#### 3.3.1 List USB Devices

Execute in PowerShell with administrator privileges:

```powershell
usbipd list
```

**Expected output example:**

```
BUSID  VID:PID    DEVICE                                                        STATE
1-1    17ef:608d  USB Input Device                                              Not shared
1-2    0461:0010  USB Input Device                                              Not shared
1-5    0bda:0153  Realtek USB 2.0 Card Reader                                   Not shared
1-8    0403:6015  USB Serial Converter                                          Not shared
1-9    0d28:0204  USB Mass Storage Device, USB Serial Device (COM5), USB Input Not shared
```

🔍 **Find your CMSIS-DAP device**:
- Look for device with `VID:PID` of `0d28:0204`
- Note the corresponding `BUSID` (e.g., `1-9` in the example)

#### 3.3.2 Bind Device

```powershell
# Set device as shareable (only need to execute once)
usbipd bind --busid 1-9
```

**Expected output:**

```
usbipd: info: Device with busid '1-9' was already shared.
```

Or when executing for the first time:

```
usbipd: info: Device with busid '1-9' successfully shared.
```

#### 3.3.3 Attach Device to WSL

```powershell
# Attach device to WSL (need to execute after each unplug)
usbipd attach --wsl --busid 1-9
```

**Expected output:**

```
usbipd: info: Using WSL distribution 'Ubuntu-22.04' to attach; the device will be available in all WSL 2 distributions.
usbipd: info: Detected networking mode 'nat'.
usbipd: info: Using IP address 172.25.176.1 to reach the host.
```

⚠️ **Important Reminders**:
- **After each USB device unplug/plug**, you must re-execute `usbipd attach`
- Device power-off or restart also requires re-attach
- If you see errors, try `usbipd detach --busid 1-9` first, then re-attach

### 3.4 Verify Device Connection

Open WSL terminal and execute:

```bash
lsusb
```

**Expected output should include:**

```
Bus 001 Device 002: ID 0d28:0204 NXP ARM mbed
```

✅ If you see this line, the device has been successfully attached to WSL.

❌ If not visible, check:
1. Whether `usbipd attach` was executed on Windows
2. Whether USB cable is properly connected
3. Whether device has power

---

## 4. WSL Configuration

### 4.1 Update System

```bash
sudo apt update
sudo apt upgrade -y
```

### 4.2 Install OpenOCD Dependencies

⚠️ **This step is critical**. Missing these libraries will cause OpenOCD to fail or produce mysterious errors.

```bash
sudo apt install -y \
  libusb-1.0-0 \
  libusb-1.0-0-dev \
  libhidapi-hidraw0 \
  libhidapi-dev \
  libjaylink0 \
  libgpiod2 \
  libftdi1-2 \
  libcapstone4
```

### 4.3 Verify Installation

```bash
# Check if critical libraries are installed
dpkg -l | grep -E "libusb|libhidapi|libjaylink"
```

**Expected output should include:**

```
ii  libhidapi-hidraw0:amd64  0.11.0-1  amd64  ...
ii  libjaylink0:amd64        0.2.0-1   amd64  ...
ii  libusb-1.0-0:amd64       2:1.0.26  amd64  ...
```

---

## 5. Rafael-Iot-SDK Setup


### 5.1 Required Files Checklist

From **Rafael-Iot-SDK**, you need to copy the following directories:

```
Rafael-Iot-SDK/tools/
├── Debugger/OpenOCD/
│   ├── bin/
│   │   └── linux/
│   │       └── openocd          # OpenOCD executable
│   └── script/
│       ├── interface/
│       │   └── cmsis-dap.cfg    # CMSIS-DAP interface config
│       └── target/
│           └── rt58x.cfg         # RT58x chip config
└── toolchain/arm/
    └── Linux/
        └── bin/
            ├── arm-none-eabi-gdb       # GDB debugger
            ├── arm-none-eabi-objdump   # Disassembly tool
            ├── arm-none-eabi-nm        # Symbol table tool
            └── ... (other toolchain files)
```

### 5.2 Test OpenOCD

```bash
# Test if OpenOCD can execute normally
./tools/Debugger/OpenOCD/bin/linux/openocd --version
```

**Expected output:**

```
Open On-Chip Debugger 0.12.0
Licensed under GNU GPL v2
...
```

❌ **If errors occur**:
- `cannot execute binary file`: Architecture mismatch, ensure using Linux version
- `error while loading shared libraries`: Missing dependency libraries, return to section 4.2 to reinstall

---

## 6. Firmware Flashing

### 6.1 Flash Command

Execute in Rafael-Iot-SDK directory:

```bash
./tools/Debugger/OpenOCD/bin/linux/openocd \
  -f ./tools/Debugger/OpenOCD/script/interface/cmsis-dap.cfg \
  -f ./tools/Debugger/OpenOCD/script/target/rt58x.cfg \
  -s ./tools/Debugger/OpenOCD/script \
  -c "program ./build/helloworld_RT582.elf 0x8000" \
  -c "reset run" \
  -c "shutdown"
```

### 6.2 Command Explanation

| Parameter                    | Description                         |
| ---------------------------- | ----------------------------------- |
| `-f interface/cmsis-dap.cfg` | Specify debugger interface config   |
| `-f target/rt58x.cfg`        | Specify target chip config          |
| `-s <path>`                  | Script search path                  |
| `-c "program <file> <addr>"` | Flash firmware to specified address |
| `-c "reset run"`             | Reset and run                       |
| `-c "shutdown"`              | Close OpenOCD after flashing        |

### 6.3 Common Flash Addresses

| Chip Model | Firmware Address | Bootloader Address |
| ---------- | ---------------- | ------------------ |
| RT58x      | 0x00008000       | 0x0000             |
| RT584x     | 0x10010000       | 0x0000             |

### 6.4 Flash Output Example

**Successful flash output:**

```
Open On-Chip Debugger 0.12.0
...
Info : CMSIS-DAP: Interface ready
Info : clock speed 100 kHz
Info : SWD DPIDR 0x0bc11477
Info : [rt58x.cpu] Cortex-M0 r0p0 processor detected
Info : [rt58x.cpu] target has 4 breakpoints, 2 watchpoints
...
** Programming Started **
Info : Padding image section 0 at 0x00008000 with 256 bytes
** Programming Finished **
** Verify Started **
** Verified OK **
** Resetting Target **
shutdown command invoked
```

✅ Seeing `Verified OK` indicates successful flashing.

❌ **Common errors**:
- `Error: init mode failed`: See section 12 Troubleshooting
- `Error: timed out while waiting for target halted`: Clock speed too fast, need to reduce

---

## 7. OpenOCD Startup


### 7.1 Set Execute Permission

```bash
chmod +x ./tools/Debugger/OpenOCD/bin/linux/oepnocd
chmod +x ./tools/Debugger/OpenOCD/script/openocd_rt58x.sh
```

### 7.2 Start OpenOCD

```bash
# Method 1: Execute script directly
/tools/Debugger/OpenOCD/script/openocd_rt58x.sh (rt581/rt582/rt583/rt584h/rt584l/rt584ha4/rf1301)

```

### 7.3 Verify OpenOCD Started Successfully

**Successful output example:**

```
Open On-Chip Debugger 0.12.0
Licensed under GNU GPL v2
...
Info : Listening on port 50001 for tcl connections
Info : Listening on port 50002 for telnet connections
Info : CMSIS-DAP: Interface ready
Info : clock speed 100 kHz
Info : SWD DPIDR 0x0bc11477
Info : [rt58x.cpu] Cortex-M0 r0p0 processor detected
Info : [rt58x.cpu] target has 4 breakpoints, 2 watchpoints
Info : starting gdb server for rt58x.cpu on 50000
Info : Listening on port 50000 for gdb connections
```

✅ Seeing `Listening on port 50000 for gdb connections` indicates GDB Server is ready.

⚠️ **Keep this terminal open**, OpenOCD needs to run continuously for debugging.

### 7.4 Stop OpenOCD

Press `Ctrl + C` to stop OpenOCD.

---

## 8. OpenOCD Configuration Optimization

### 8.1 SWD Clock Speed Adjustment (Important)

**Problem symptom:**

```
Error: Error connecting DP: cannot read IDR
```

**Root cause:**

RT58x chip's SWD interface is sensitive to clock speed. The default 1000 kHz may be too fast, causing communication failure.


### 8.2 Speed Recommendations

| Speed (kHz) | Use Case                                   | Stability                  |
| ----------- | ------------------------------------------ | -------------------------- |
| 1000        | Ideal conditions (short wires, good power) | ⚠️ Unstable                 |
| 500         | General conditions                         | ⚠️ May be unstable          |
| 100         | **Recommended** (works for most cases)     | ✅ Stable                   |
| 50          | Long wires, noisy environment              | ✅ Very stable (but slower) |

### 8.3 Adjust Based on Environment

If 100 kHz is still unstable, you can further reduce:

```tcl
adapter speed 50
```

### 8.5 Other Possible Optimizations

If problems persist, check the following factors:

1. **VTref voltage**:
   ```tcl
   # Add to rt58x.cfg (if supported)
   adapter_nsrst_delay 100
   jtag_ntrst_delay 100
   ```

2. **Reset configuration**:
   ```tcl
   # Ensure correct reset configuration
   reset_config srst_only srst_nogate
   ```

3. **Hardware check**:
   - Dupont wire length < 20cm
   - Use shielded wires (if possible)
   - Ensure good ground connection

---

## 9. VS Code Debug Configuration

### 9.1 Install Cortex-Debug Extension

1. Open VS Code
2. Press `Ctrl + Shift + X` to open Extensions panel
3. Search for `Cortex-Debug`
4. Install **marus25.cortex-debug**

### 9.2 Configuration Explanation

| Config Item              | Description                                   |
| ------------------------ | --------------------------------------------- |
| `gdbTarget`              | OpenOCD's GDB port (default 50000)            |
| `gdbPath`                | Full path to arm-none-eabi-gdb                |
| `executable`             | .out file containing debug symbols            |
| `servertype: "external"` | Use external GDB Server (OpenOCD)             |
| `postAttachCommands`     | GDB commands to auto-execute after connection |

### 9.3 Start Debugging

1. **Ensure OpenOCD is running in another terminal** (see section 7)
2. In VS Code, press `F5` or click the start button in debug panel
3. Select project and board
4. Debugger will connect to target and halt at `main` function

### 9.4 Debug Operations

| Operation | Shortcut            | Description                              |
| --------- | ------------------- | ---------------------------------------- |
| Continue  | `F5`                | Continue program execution               |
| Step Over | `F10`               | Execute next line (don't enter function) |
| Step Into | `F11`               | Enter function                           |
| Step Out  | `Shift + F11`       | Exit current function                    |
| Restart   | `Ctrl + Shift + F5` | Restart debugging                        |
| Stop      | `Shift + F5`        | Stop debugging                           |

### 9.5 Setting Breakpoints

- **Line breakpoint**: Click on the left side of line number
- **Conditional breakpoint**: Right-click breakpoint → Edit Breakpoint → Enter condition expression
- **Logpoint**: Right-click breakpoint → Add Logpoint → Enter message

---

## 10. Python Environment Setup

Some tools in Rafael-Iot-SDK depend on Python environment.

### 10.1 Install Python and Basic Tools

```bash
sudo apt update
sudo apt install -y \
  python3 \
  python3-pip \
  python3-venv \
  python3-setuptools \
  python3-wheel
```

### 10.2 Install Required Python Packages for Matter

```bash
# Use --user flag to install to user directory
pip3 install --user \
  click \
  pyserial \
  cryptography \
  cbor2 \
  coloredlogs \
  construct
```

### 10.3 Verify Installation

```bash
# Check Python version
python3 --version

# Check installed packages
pip3 list | grep -E "click|pyserial|cryptography"
```

**Expected output:**

```
Python 3.10.12
click          8.1.7
cryptography   41.0.5
pyserial       3.5
```

---

## 11. Debugging Workflow

### 11.1 Complete Debug Flow

```
┌─────────────────────────────────────────────────────────┐
│ 1. Connect Hardware                                     │
│    - Connect CMSIS-DAP to PC                            │
│    - Connect CMSIS-DAP to RT58x board (SWD interface)   │
│    - Power on the board                                 │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│ 2. Windows: Forward USB to WSL                          │
│    PowerShell (Admin):                                  │
│    > usbipd list                                        │
│    > usbipd attach --wsl --busid 1-9                    │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│ 3. WSL: Verify device                                   │
│    $ lsusb                                              │
│    Should see: ID 0d28:0204 NXP ARM mbed                │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│ 4. Start OpenOCD                                        │
│    $ cd ~/Rafael-IoT-SDK                                │
│    $ ./openocd_rt58x.sh rt58x                           │
│     **Keep this terminal open                           │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│ 5. VS Code: Start debugging                             │
│    - Open Matter SDK workspace                          │
│    - Press F5 to start debugging                        │
│    - Select project and board                           │
│    - Debugger halts at main                             │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│ 6. Debug                                                │
│    - Set breakpoints                                    │
│    - Step through code                                  │
│    - Inspect variables                                  │
│    - View call stack                                    │
└─────────────────────────────────────────────────────────┘
```

### 11.2 Debug Session Management

#### Start New Debug Session

```bash
# Terminal 1: Start OpenOCD (if not already running)
./openocd_rt58x.sh rt58x

# VS Code: Press F5 to start debugging
```

#### End Debug Session

```bash
# VS Code: Shift + F5 to stop debugging
# Terminal 1: Ctrl + C to stop OpenOCD (if needed)
```

#### Reconnect Debugger

If debugger disconnects:

1. Stop VS Code debugging (`Shift + F5`)
2. Stop OpenOCD (`Ctrl + C`)
3. Restart OpenOCD (`./openocd_rt58x.sh`)
4. Restart VS Code debugging (`F5`)

### 11.3 Common Debug Tasks

#### View Variable Values

```gdb
# In VS Code's Debug Console
-exec print variable_name
-exec p variable_name

# View struct
-exec p *struct_ptr

# View array
-exec p array[0]@10    # View first 10 elements
```

#### View Memory

```gdb
-exec x/10x 0x20000000    # View 10 words in hex
-exec x/10c 0x20000000    # View 10 bytes as characters
```

#### View Registers

```gdb
-exec info registers      # View all registers
-exec p $r0              # View specific register
```

#### View Call Stack

```gdb
-exec bt                 # Show backtrace
-exec frame 2            # Switch to frame 2
```

---

## 12. Troubleshooting

### 12.1 USB Device Issues

#### Issue: CMSIS-DAP Not Visible in WSL

**Symptom:**

```bash
$ lsusb
# Don't see 0d28:0204
```

**Solution:**

1. **Check if Windows has attached:**
   ```powershell
   # PowerShell (Admin)
   usbipd list
   # Confirm device state is "Attached - Ubuntu-22.04"
   ```

2. **If state is "Not shared" or "Not attached":**
   ```powershell
   usbipd bind --busid 1-9
   usbipd attach --wsl --busid 1-9
   ```

3. **If problem persists:**
   ```powershell
   # Try detach then attach
   usbipd detach --busid 1-9
   usbipd attach --wsl --busid 1-9
   ```

4. **Last resort: Re-plug USB device:**
   - Unplug CMSIS-DAP
   - Wait 5 seconds
   - Plug back in
   - Execute `usbipd attach --wsl --busid 1-9`

### 12.2 OpenOCD Connection Issues

#### Issue: Error connecting DP: cannot read IDR

**Symptom:**

```
Error: Error connecting DP: cannot read IDR
```

**Root cause:**
- SWD clock speed too fast
- VTref voltage insufficient
- Wiring issues

**Solution:**

1. **Reduce SWD clock speed (most common solution):**
   ```bash
   # Edit rt58x.cfg
   nano tools/Debugger/OpenOCD/script/target/rt58x.cfg
   
   # Change adapter speed to 100 or lower
   adapter speed 100
   ```

2. **Check hardware connections:**
   - Confirm VTref pin is connected
   - Use multimeter to measure VTref voltage (should be 3.3V)
   - Check SWDIO, SWCLK, GND connections

3. **Check target board power:**
   - Confirm target board is powered on
   - Check if LED indicators are lit

4. **Try reset:**
   ```bash
   # While OpenOCD is running, execute in another terminal
   telnet localhost 50002
   > reset halt
   > exit
   ```

#### Issue: timed out while waiting for target halted

**Symptom:**

```
Error: timed out while waiting for target halted
```

**Solution:**

1. **Reduce clock speed:**
   ```tcl
   adapter speed 50
   ```

2. **Increase timeout:**
   Add to `rt58x.cfg`:
   ```tcl
   adapter speed 100
   jtag configure rt58x.cpu -event examine-end {
       targets rt58x.cpu
       halt
       wait_halt 2000
   }
   ```

3. **Check target board state:**
   - Target board may be in low-power mode
   - Try manually resetting target board

### 12.3 GDB Connection Issues

#### Issue: Connection refused (port 50000)

**Symptom:**

```
Error: Unable to connect to GDB server at localhost:50000
Connection refused
```

**Root cause:**
OpenOCD not running or GDB Server not started

**Solution:**

1. **Check if OpenOCD is running:**
   ```bash
   ps aux | grep openocd
   ```

2. **Check if port is listening:**
   ```bash
   netstat -tuln | grep 50000
   # or
   ss -tuln | grep 50000
   ```

3. **Restart OpenOCD:**
   ```bash
   # Stop existing OpenOCD
   sudo killall openocd
   
   # Restart
   ./openocd_rt58x.sh rt584h
   ```

4. **Check firewall settings:**
   ```bash
   sudo ufw status
   # If enabled, allow port 50000
   sudo ufw allow 50000/tcp
   ```

#### Issue: GDB encodings error

**Symptom:**

```
Python Exception <class 'LookupError'> unknown encoding: ANSI_X3.4-1968
```

**Root cause:**
Python environment encoding settings issue

**Solution:**

```bash
# Add to ~/.bashrc
echo 'export LC_ALL=C.UTF-8' >> ~/.bashrc
echo 'export LANG=C.UTF-8' >> ~/.bashrc

# Reload configuration
source ~/.bashrc

# Restart VS Code and WSL
```

### 12.4 VS Code Debug Issues

#### Issue: Executable File Not Found

**Symptom:**

```
Error: Unable to find executable file at ./build/<project name>_<chip>.elf
```

**Solution:**

1. **Confirm file path is correct:**
   ```bash
   ls -lh ./build/<project name>_<chip>.elf
   ```

#### Issue: Breakpoint Cannot Be Set

**Symptom:**
Breakpoint shows as gray circle or warning symbol

**Solution:**

1. **Confirm compilation includes debug symbols:**
   ```bash
   # Check if compilation options include -g
   # Ensure using debug mode when recompiling
   ```

2. **Check if source file path matches:**
   - File path opened in VS Code should match path used during compilation

3. **Use `set breakpoint pending on`:**
   This command is already included in `postAttachCommands` in `launch.json`

### 12.5 Rafael-IoT SDK Specific Issues


#### Issue: Python Dependencies Missing

**Symptom:**

```
ModuleNotFoundError: No module named 'click'
```

**Solution:**

Reinstall Python dependencies (see section 10):

```bash
pip3 install --user click pyserial cryptography cbor2 coloredlogs
```

---

## 13. Common Errors Quick Reference

| Error Message                          | Possible Cause                                  | Solution                                  | Section Reference |
| -------------------------------------- | ----------------------------------------------- | ----------------------------------------- | ----------------- |
| `cannot read IDR`                      | SWD clock too fast / VTref issue / wiring error | Reduce adapter speed to 100 or 50         | 8.2, 12.2         |
| `timed out while waiting`              | Target not ready / clock too fast               | Reduce clock speed / increase timeout     | 12.2              |
| `CMSIS-DAP not found`                  | Not executed usbipd attach                      | Re-execute attach command                 | 3.3.3, 12.1       |
| `Connection refused (50000)`           | OpenOCD not running                             | Start OpenOCD                             | 7.3, 12.3         |
| `GDB encodings error`                  | Python encoding settings issue                  | Set LC_ALL and LANG environment variables | 12.3              |
| `No such file openocd`                 | OpenOCD not copied or insufficient permissions  | Recopy and set execute permission         | 5.3, 12.5         |
| `error while loading shared libraries` | Missing dependency libraries                    | Install OpenOCD dependencies              | 4.2               |
| `Unable to find executable file`       | File path error / not compiled                  | Check path or recompile                   | 12.4              |
| `Breakpoint cannot be set`             | Missing debug symbols / path mismatch           | Compile in debug mode / check path        | 12.4              |
| `ModuleNotFoundError`                  | Python dependencies missing                     | Install required Python packages          | 10.2, 12.5        |

---

## 14. Appendix

### 14.1 Reference Resources

#### Official Documentation
- **usbipd-win project**: https://github.com/dorssel/usbipd-win
- **OpenOCD official documentation**: http://openocd.org/documentation/
- **ARM Cortex-M development guide**: https://developer.arm.com/documentation/

#### Matter SDK
- **Matter official repository**: https://github.com/project-chip/connectedhomeip
- **Matter development guide**: https://project-chip.github.io/connectedhomeip-doc/

#### VS Code Extensions
- **Cortex-Debug**: https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug

### 14.2 Port Usage

| Port  | Protocol | Purpose       | Notes                         |
| ----- | -------- | ------------- | ----------------------------- |
| 50000 | TCP      | GDB Server    | GDB connection port           |
| 50001 | TCP      | TCL Server    | OpenOCD script interface      |
| 50002 | TCP      | Telnet Server | Interactive command interface |

### 14.3 File Path Quick Reference

```
Rafael-IoT-sdk/
├── .vscode/
│   └── launch.json                    # VS Code debug configuration
├── tools/
│   ├── Debugger/
│   │   └── OpenOCD/
│   │       ├── bin/linux/openocd      # OpenOCD executable
│   │       └── script/
│   │           ├── interface/
│   │           │   └── cmsis-dap.cfg  # CMSIS-DAP configuration
│   │           │── target/
│   │           │   └── rt58x.cfg      # RT58x chip configuration
│   │           └── openocd_rt58x.sh   # OpenOCD startup script
│   └── toolchain/
│       └── arm/Linux/bin/
│           └── arm-none-eabi-gdb      # GDB debugger
├── build/
│   └── <project>-<chip>.elf  # Debug symbols file
│       
└── 
```

### 14.4 Common Commands Quick Reference

#### Windows (PowerShell, Administrator)

```powershell
# List USB devices
usbipd list

# Bind device
usbipd bind --busid 1-9

# Attach to WSL
usbipd attach --wsl --busid 1-9

# Detach
usbipd detach --busid 1-9
```

#### WSL

```bash
# Verify USB device
lsusb | grep "0d28:0204"

# Start OpenOCD
./openocd_rt58x.sh

# Stop all OpenOCD processes
sudo killall openocd

# Check if OpenOCD is running
ps aux | grep openocd

# Check port listening
netstat -tuln | grep 5000
```

#### OpenOCD Telnet Commands

```bash
# Connect to OpenOCD
telnet localhost 50002

# Common commands
> halt                 # Halt target
> resume               # Resume execution
> reset halt           # Reset and halt
> reset run            # Reset and run
> reg                  # View registers
> mdw 0x20000000 10    # View memory (10 words)
> exit                 # Exit telnet
```

### 14.5 Hardware Wiring Reference

#### CMSIS-DAP to RT58x Board

| CMSIS-DAP Pin    | RT58x Pin    | Description                                          |
| ---------------- | ------------ | ---------------------------------------------------- |
| VTref            | VDD / 3V3    | Target reference voltage (detection only, not power) |
| SWDIO            | SWDIO        | Data line                                            |
| SWCLK            | SWCLK        | Clock line                                           |
| GND              | GND          | Ground (must connect)                                |
| RESET (optional) | RESET / nRST | Hardware reset (recommended)                         |

⚠️ **Important Notes:**
- VTref is for voltage detection only, cannot be used for power supply
- Ensure good GND connection - this is a common issue source
- Dupont wire length recommended < 20cm, too long introduces interference
- RESET wire is optional but strongly recommended for more stable debugging

### 14.6 Version Compatibility

| Component | Recommended | Minimum    | Notes                               |
| --------- | ----------- | ---------- | ----------------------------------- |
| Windows   | 11          | 10 (1903+) | Must support WSL2                   |
| WSL       | 2           | 2          | WSL1 doesn't support USB forwarding |
| Ubuntu    | 22.04 LTS   | 20.04 LTS  | Recommend using LTS versions        |
| OpenOCD   | 0.12.0      | 0.11.0     | From RT58x SDK                      |
| VS Code   | Latest      | 1.70.0     | Must support cortex-debug extension |
| Python    | 3.10+       | 3.8+       | Matter SDK requirement              |

### 14.7 Performance Optimization Recommendations

1. **SWD clock speed**:
   - Development debugging: 100 kHz (recommended)
   - After stable: Can try increasing to 500 kHz
   - Mass production flashing: 1000 kHz (ensure stability)

2. **GDB optimization**:
   Add to `preLaunchCommands` in `launch.json`:
   ```jsonc
   "preLaunchCommands": [
     "set mem inaccessible-by-default off",
     "set remotetimeout 60",
     "set print pretty on"
   ]
   ```

3. **OpenOCD log level**:
   ```bash
   # Reduce log output to improve performance
   sudo ./tools/Debugger/OpenOCD/bin/linux/openocd \
     -s ./tools/Debugger/OpenOCD/script \
     -f interface/cmsis-dap.cfg \
     -f target/rt58x.cfg \
     -c "gdb_port 50000; tcl_port 50001; telnet_port 50002" \
   ```

### 14.8 Security Recommendations

1. **Permission management**:
   - Use sudo only when necessary
   - Consider using udev rules to avoid sudo (see below)

2. **USB access without sudo (optional configuration)**:

   Create udev rules:
   ```bash
   sudo nano /etc/udev/rules.d/99-cmsis-dap.rules
   ```

   Add content:
   ```
   # CMSIS-DAP debugger
   SUBSYSTEM=="usb", ATTR{idVendor}=="0d28", ATTR{idProduct}=="0204", MODE="0666", GROUP="plugdev"
   ```

   Reload rules:
   ```bash
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   
   # Add current user to plugdev group
   sudo usermod -a -G plugdev $USER
   
   # Logout and login to apply group changes
   ```

3. **Network security**:
   - GDB Server only listens on 127.0.0.1 (localhost)
   - Don't expose ports 50000-50002 to external networks

### 14.9 License Information

- **OpenOCD**: GPL v2
- **ARM Toolchain**: According to ARM license
- **usbipd-win**: GPL v3
- **cortex-debug**: MIT License

### 14.10 Contribution and Feedback

If you find any errors in this document or have improvement suggestions, please provide feedback through:

1. Submit Issue to project repository
2. Send email to technical support team
3. Raise in team meetings

---

## Conclusion

This guide covers the complete workflow for debugging RT58x chips using OpenOCD in WSL environment. By following the steps in this document, you should be able to:

✅ Successfully configure Windows + WSL2 development environment  
✅ Use CMSIS-DAP debugger to connect to RT58x  
✅ Perform source-level debugging through VS Code  
✅ Resolve common connection and configuration issues  

---

