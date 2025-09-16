# Toolchain Setup

This directory contains the toolchains and build tools required for compiling ARM projects.  
The following provides installation instructions, recommended versions, and licensing information.

---

## Usage Instructions

1. Download the appropriate toolchain and build tools for your platform.
2. Unzip the files to the specified directories.
3. Add the `bin` directory to your system's PATH, so the tools can be used from the terminal or command prompt.
4. **Linux users note**: After unzipping, use `chmod +x` to set executable permissions.
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
| CMake             | 3.24 or later       | Requires support for `Presets` and Ninja |
| Ninja             | 1.11 or later       | Typically used with CMake                |

---

## ARM GNU Toolchain

- **Source**: [Arm GNU Toolchain Downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)  
- **Description**:  
  The `arm` folder contains official precompiled binaries of the Arm GNU Toolchain for multiple platforms. No modifications have been made.  
- **License & Attribution**:  
  Arm GNU Toolchain is based on open-source licenses ([GPL](https://www.gnu.org/licenses/gpl-3.0.en.html), [LGPL](https://www.gnu.org/licenses/lgpl-3.0.en.html), etc.).  
  For more information, visit the [official Arm GNU Toolchain page](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain).  

**Downlaod toolchain**:

  - **Linux**: [14.2.rel1](https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/gcc-arm-14.2.rel1-x86_64-arm-none-eabi.tar.xz)  
    Unzip to path: `toolchain/arm/Linux/`  
    **Set environment variable**:  
    ```bash
    export PATH=$PATH:$(pwd)/toolchain/arm/Linux/bin
    ```
    **Set executable permission**:  
    ```bash
    chmod +x toolchain/arm/Linux/bin/*
    ```

  - **Windows**: [14.2.rel1](https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/gcc-arm-14.2-win32.zip)  
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
    
  - **Linux**: [v3.30.4](https://cmake.org/files/v3.24/cmake-3.24.0-linux-x86_64.sh)  
    Unzip to path: `toolchain/cmake/Linux/`  
    ```bash
    export PATH=$PATH:$(pwd)/toolchain/cmake/Linux/bin
    chmod +x toolchain/cmake/Linux/bin/*
    ```

  - **Windows**: [v3.30.4](https://cmake.org/files/v3.24/cmake-3.24.0-windows-x86_64.zip)  
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

