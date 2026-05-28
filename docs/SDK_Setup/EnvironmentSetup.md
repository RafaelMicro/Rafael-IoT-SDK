# Environment Setup

## Setup Building tool and toolchain

- Clone Project
    Using website or git command Download Rafael SDK
  - Download source code from [**Source URL**](https://github.com/RafaelMicro/Rafael-IoT-SDK)
  - Using git command clone

    ``` bash
    git clone https://github.com/RafaelMicro/Rafael-IoT-SDK
    ```

### Windows

- Install tools
    1. VScode : [**VSCodeUserSetup-x64-1.93.1.exe**](https://support.rafaelmicro.com:8088/attachments/9248/VSCodeUserSetup-x64-1.93.1.exe)
    2. GCC: [**arm-gnu-toolchain-13.3.rel1-mingw-w64-i686-arm-none-eabi.exe**](https://support.rafaelmicro.com:8088/attachments/9249/arm-gnu-toolchain-13.3.rel1-mingw-w64-i686-arm-none-eabi.exe)
    </br>**Please check "Add path to environment variable" in the last window when installing GCC.**
- Install VSCode's extensions
    1. [**C/C++, id: ms-vscode.cpptools**](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
    2. [**C/C++ Extension Pack, id: ms-vscode.cpptools-extension-pack**](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
    3. [**C/C++ Themes, id: ms-vscode.cpptools-themes**](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-themes)
    4. [**CMake, id: twxs.cmake**](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
    5. [**CMake Tools, id: ms-vscode.cmake-tools**](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
    <img src="../picture/vscode_extension.png" alt="drawing" style="width:400px;"/>

---

## Build Project

1. Open VSCode (select File -> Open Folder...)
2. Copy Project config File from project Folder to top level, and change file name ".config"
3. Click CMake icon on left bar
<img src="../picture/CMake_icon.png" alt="drawing" style="width:400px;"/>
4. On [No Kit Selected] click pencil icon (please select "GCC 13.3.1 arm-none-eabi")
<img src="../picture/CMake_pancil_icon.png" alt="drawing" style="width:400px;"/>

<img src="../picture/CMake_select_toolchain.png" alt="drawing" style="width:800px;"/>
5. Build Project
<img src="../picture/CMake_build.png" alt="drawing" style="width:400px;"/>
<img src="../picture/build.png" alt="drawing" style="width:400px;"/>
6. Compile completed
<img src="../picture/CMake_compile.png" alt="drawing" style="width:800px;"/>
7. Firmware directory
<img src="../picture/bin_file.png" alt="drawing" style="width:400px;"/>

---

## Flash Application

In System Program (ISP) tool, which packaged in Rafael IoT Evaluation Tool, please download from the [Rafael Customer Support Portal](https://support.rafaelmicro.com:8088/).
<img src="../picture/ISP.png" alt="drawing" style="width:400px;"/>

Flash code step:

1. Setup COM port:
<img src="../picture/ISP2.png" alt="drawing" style="width:400px;"/>
2. Connect device:
<img src="../picture/ISP1.png" alt="drawing" style="width:400px;"/>
3. Select bootloader and DUT bin file
<img src="../picture/ISP3.png" alt="drawing" style="width:400px;"/>
4. Setup EVK to ISP mode (clicked reser buttom)
<img src="../picture/ISP4.png" alt="drawing" style="width:400px;"/>
5. Erase and download image
<img src="../picture/ISP5.png" alt="drawing" style="width:400px;"/>

---

## UART debug

This example use "Tera Term" for UART debug terminial

1. Connect device:
<img src="../picture/teraterm_connect.png" alt="drawing" style="width:400px;"/>
2. Change setting:
<img src="../picture/teraterm_change_setting.png" alt="drawing" style="width:200px;"/>
<img src="../picture/change_setting.png" alt="drawing" style="width:400px;"/>
3. Check log
<img src="../picture/helloworld.png" alt="drawing" style="width:400px;"/>

---

## JLink Debug

1. Setup Debug Tool
<img src="../picture/env_path.png" alt="drawing" style="width:600px;"/>
2. Start Debug
<img src="../picture/launch_debug.png" alt="drawing" style="width:800px;"/>
3. Work with Debug
<img src="../picture/debug_ui.png" alt="drawing" style="width:600px;"/>
