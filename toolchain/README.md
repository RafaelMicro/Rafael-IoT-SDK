# Toolchain Setup

This directory contains the toolchains and build tools required for compiling ARM projects.  
The following provides installation instructions, recommended versions, and licensing information.

ARM GCC has no automatic installer. Always install it by hand, as below. You can install CMake
and Ninja the same way. Or you can install them automatically. First install the Rafael VS Code
extension. Then run **Rafael: Install / Manage Toolchain**. This command installs both tools
through your system's package manager: winget, Homebrew, apt, dnf, pacman, or zypper. We
recommend the automatic method for CMake and Ninja. See
[sdk_extension.md](../docs/SDK_Setup/sdk_extension.md#tools-toolchain-manager-and-settings).

**Recommended: install outside the SDK checkout, on the PATH.** The steps below unzip tools into
this `toolchain/` folder, as an example. This was the old method, from before the extension could
detect a PATH install. It meant every fresh `git clone` of the SDK needed its own copy of the
tools. The extension still detects tools placed here. Its **Tools** panel shows this source as
"SDK bundled". This method still works, but we no longer recommend it. Instead, unzip ARM GCC to
a permanent folder outside any SDK checkout. Add it to the PATH. This install then serves every
SDK clone you make in the future. You do not need to redo it.

---

## Usage Instructions

1. Download the toolchain and build tools for your platform.
2. Unzip the files to a permanent location outside the SDK checkout. This is the recommended
   method. Or unzip them under this `toolchain/` folder, for the older, per-clone method.
3. Add the `bin` directory to your system's PATH. This lets you use the tools from the terminal
   or the command prompt. The example PATH commands below use the current directory (`%CD%` or
   `$(pwd)`). They assume you unzipped the tools under this `toolchain/` folder, and that you run
   the commands from the SDK root. If you unzipped the tools elsewhere, use that path instead.
   After this step, close VS Code. Open it again. The Rafael extension then picks up the new
   PATH.
4. **Linux users note**: After you unzip the files, run `chmod +x` to set the executable
   permission.
5. Verify the installation:
   ```bash
   arm-none-eabi-gcc --version
   cmake --version
   ninja --version

---

## Recommended Versions

| Tool              | Recommended Version | Notes                                    |
| ----------------- | ------------------- | ---------------------------------------- |
| ARM GNU Toolchain | 14.2                | Official precompiled binaries            |
| CMake             | 3.24                | Requires support for `Presets` and Ninja |
| Ninja             | 1.11                | Typically used with CMake                |

---

## ARM GNU Toolchain

- **Source**: [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)  
- **Description**:  
  The `arm` folder contains official precompiled binaries of the Arm GNU Toolchain for multiple platforms. No modifications have been made.  
- **License & Attribution**:  
  Arm GNU Toolchain is based on open-source licenses ([GPL](https://www.gnu.org/licenses/gpl-3.0.en.html), [LGPL](https://www.gnu.org/licenses/lgpl-3.0.en.html), etc.).  
  For more information, visit the [official Arm GNU Toolchain page](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain).  

**Downlaod toolchain**:

  - **Linux**: [14.2.rel1](https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz)  
    Unzip to path: `toolchain/arm/Linux/`  
    **Set environment variable**:  
    ```bash
    export PATH=$PATH:$(pwd)/toolchain/arm/Linux/bin
    ```
    **Set executable permission**:  
    ```bash
    chmod +x toolchain/arm/Linux/bin/*
    ```

  - **Windows**: [14.2.rel1](https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-arm-none-eabi.zip)  
    Unzip to path: `toolchain/arm/Windows/`  
    **Set environment variable**:  
    ```powershell
    setx PATH "%PATH%;%CD%\toolchain\arm\Windows\bin"
    ```
---

## CMake

- **Source**: [CMake Downloads](https://cmake.org/download/)  
- **Description**:  
  CMake is a cross-platform build system generator, used to produce Makefiles or Ninja build scripts.  
- **License & Attribution**:  
  CMake is released under the [BSD 3-Clause License](https://cmake.org/licensing/), allowing free use, modification, and redistribution.  
  More information at [CMake official page](https://cmake.org/).  

**Download cmake**:  
    
  - **Linux**: [v3.24.0](https://cmake.org/files/v3.24/cmake-3.24.0-linux-x86_64.sh)  
    Unzip to path: `toolchain/cmake/Linux/`  
    ```bash
    export PATH=$PATH:$(pwd)/toolchain/cmake/Linux/bin
    chmod +x toolchain/cmake/Linux/bin/*
    ```

  - **Windows**: [v3.24.0](https://cmake.org/files/v3.24/cmake-3.24.0-windows-x86_64.zip)  
    Unzip to path: `toolchain/cmake/Windows/`  
    ```powershell
    setx PATH "%PATH%;%CD%\toolchain\cmake\Windows\bin"
    ```

---

## Ninja

- **Source**: [Ninja Releases](https://github.com/ninja-build/ninja/releases)  
- **Description**:  
  Ninja is a small, fast build system focused on speed, often used with CMake (`-G Ninja`).  
- **License & Attribution**:  
  Ninja is released under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0), allowing free use and redistribution.  
  More information at [Ninja GitHub page](https://github.com/ninja-build/ninja).  


**Download ninja**: 

  - **Linux**: [v1.11.1](https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip)  
    Unzip to path: `toolchain/ninja/Linux/`  
    ```bash
    export PATH=$PATH:$(pwd)/toolchain/ninja/Linux
    chmod +x toolchain/ninja/Linux/*
    ```

  - **Windows**: [v1.11.1](https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-win.zip)  
    Unzip to path: `toolchain/ninja/Windows/`  
    ```powershell
    setx PATH "%PATH%;%CD%\toolchain\ninja\Windows"
    ```
---
## License Overview

| Tool              | License                                 |
| ----------------- | --------------------------------------- |
| ARM GNU Toolchain | GPL / LGPL / Other Open-Source Licenses |
| CMake             | BSD 3-Clause                            |
| Ninja             | Apache 2.0                              |
