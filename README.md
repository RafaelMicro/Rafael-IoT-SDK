![image](/docs/SDK_Overview/picture/logo.JPG)

![Date](https://img.shields.io/badge/Date-2026--03--18-green) ![Release](https://img.shields.io/badge/Release-v2.2.0-blue)

# Rafael IoT SDK

- ### [Release Notes](docs/RELEASE_NOTES.md)
---

## Supported ICs List<a id="supportic"></a>
- **[RT581, RT582, RT583, RT584L, RT584H, RF1301](#installation)**

---

## 📑 Table of Contents

- 📌 [Features](#features)
- 📦 [Installation and Development Environment Setup](#installation)
- 🚀 [Build and Run Examples](#build-and-run-examples)
- 💻 [Evaluation Kit (EVK)](#evaluation-kit-evk)
- 📂 [Project Structure](#project-structure)
- 🔗 [Reference Documents and Links](#links)

---

## 📌 Features <a id="features"></a>

**Protocol Support**  
- Bluetooth Low Energy (BLE)  
- Zigbee  
- Mesh Networking  
- Sub-G Wireless Communication  

**Hardware Support**  
- High-performance Cortex-M microcontroller  
- Full RF driver support  
- Rich peripheral support (UART, SPI, I2C, PWM, ADC, etc.)  

**Developer Tools**  
- Example applications included  
- Supports external debug tools (Rafael CMSIS-DAPLink, SEGGER J-Link ICE)  
  - **Rafael CMSIS-DAP**  
  ![image](/docs/SDK_Overview/picture/rafael-cmsis-dap.jpg)    
  - **SEGGER J-Link Debugger**  
  - J-Link Debugger Website: :[https://www.segger.com/products/debug-probes/j-link/](https://www.segger.com/products/debug-probes/j-link/)

- Supports firmware flashing, debugging, and real-time trace  

---

## 📦 Installation and Development Environment Setup<a id="installation"></a>

### Clone the repository (Recommended)
```bash
git clone --recursive https://github.com/RafaelMicro/Rafael-IoT-SDK.git
```

If you already cloned without submodules:  
```bash
git submodule update --init --recursive
```

Download ZIP Archive

Download ZIP
⚠️ Note: ZIP archive does not include submodules. You must download and add them manually.

###  Development Environment Setup
- [Environment Setup Guide](docs/SDK_Setup/sdk-env-setup.md) 
- [Setup Guide](docs/SDK_Setup/sdk_setup.md)
- [Extension Guide](docs/SDK_Setup/sdk_extension.md)   
 Install ARM GCC Toolchain, CMake, and Ninja   
 Ensure the PATH environment variable is properly configured  
 Recommended IDEs: VS Code (with C/C++ and Cortex-Debug extensions)    

---  
## 🚀 Build and Run Examples  <a id="build-and-run-examples"></a>
- [Build Examples Guide](docs/SDK_Setup/sdk_build_examples.md) 
- [Debug Guide](docs/SDK_Setup/sdk_debug.md)  
  **Flashing to the Development Board**
Use the 20-pin SWD interface together with a CMSIS-DAP or SEGGER J-Link probe to flash and debug the firmware.   
On Windows, add the SDK directory to the Windows Defender exclusion list to improve build performance. Otherwise can build the project in a **WSL** or **Linux** environment for significantly faster compilation times.



---  

## 💻 Evaluation Kit (EVK) <a id="evaluation-kit-evk"></a>

The Rafael EVK provides a standard 20-pin SWD connector:

- Functions: Firmware flashing, debugging, real-time trace
- Compatibility: ARM standard pinout, works with CMSIS-DAP and SEGGER J-Link ICE
- Applications: IoT wireless communication, sensor integration, low-power applications
 ![image](/docs/SDK_Overview/picture/RT58X_EVK.jpg)  
 ---  

## 📂 Project Structure  <a id="project-structure"></a>

- Rafael-IoT-SDK/  
  ├── **cmake/**  
      CMake scripts and build configurations for project compilation across different platforms and toolchains.  

  ├── **components/**  
      SDK drivers and middleware (BLE, Zigbee, Sub-GHz, UART, SPI, I2C, GPIO, ADC, etc.) with ready-to-use APIs.  

  ├── **config/**  
      Project configuration files, including board settings, MCU options, flash layouts, and build parameters.  

  ├── **docs/**  
      Documentation, user guides, API references, release notes, and tutorials.  

  ├── **examples/**  
      Sample applications demonstrating BLE, Zigbee, and Sub-GHz use cases.  

  ├── **toolchain/**  
      Compiler toolchain binaries and scripts (ARM GCC, CMake support, and utilities).  

  └── **tools/**  
      Development tools for flashing, debugging, testing, and monitoring MCU applications (production programming, secure boot, UART monitoring, etc.).


---  

## 🔗 Reference Documents and Links <a id="links"></a>

- [Enviroment Setup Guide](docs/SDK_Setup/sdk-env-setup.md) 
- [Setup Guide](docs/SDK_Setup/sdk_setup.md)  
- [Extension Guide](docs/SDK_Setup/sdk_extension.md)  
- [Build Examples Guide](docs/SDK_Setup/sdk_build_examples.md)  
- [Debug Guide](docs/SDK_Setup/sdk_debug.md)  


| Resources                         | Reference Link                                                                                                                                                                                                         |
| --------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Rafael Micro Official Website** | [![Visit Rafael Micro](https://img.shields.io/badge/Rafael--Micro-Website-blue?style=for-the-badge)](https://www.rafaelmicro.com)                                                                                     |
| **GitHub Repository**             | [![Visit GitHub](https://img.shields.io/badge/GitHub-Repository-black?logo=github&style=for-the-badge)](https://github.com/RafaelMicro/Rafael-IoT-SDK)                                                                |
| **ARM Toolchain**                 | [![Download ARM Toolchain](https://img.shields.io/badge/Download-ARM--GCC-blue?logo=arm&style=for-the-badge)](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) |
| **CMake**                         | [![Download CMake](https://img.shields.io/badge/Download-CMake-orange?logo=cmake&style=for-the-badge)](https://cmake.org/download/)                                                                                   |
| **Git**                           | [![Download Git](https://img.shields.io/badge/Download-Git-red?logo=git&style=for-the-badge)](https://git-scm.com/downloads)                                                                                          |
| **SEGGER J-Link Software**        | [![Download J-Link](https://img.shields.io/badge/Download-J--Link-blue?logo=segger&style=for-the-badge)](https://www.segger.com/downloads/jlink)                                                                      |
| **ARM CMSIS**                     | [![Download CMSIS](https://img.shields.io/badge/Download-CMSIS-lightgrey?logo=arm&style=for-the-badge)](https://www.arm.com/why-arm/technologies/cmsis)                                                               |
| **Ninja**                         | [![Download Ninja](https://img.shields.io/badge/Download-Ninja-green?logo=ninja&style=for-the-badge)](https://ninja-build.org/)                                                                                       |
| **Python**                        | [![Download Python](https://img.shields.io/badge/Download-Python-yellow?logo=python&style=for-the-badge)](https://www.python.org/downloads/)                                                                          |
| **VS Code**                       | [![Download VS Code](https://img.shields.io/badge/Download-VS--Code-blue?logo=visual-studio-code&style=for-the-badge)](https://code.visualstudio.com/Download)                                                        |

---
