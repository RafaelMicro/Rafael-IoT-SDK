# Tool Folder

This folder contains all the necessary tools and utilities required for Rafael SDK 2.0 development, including debuggers, flash algorithms, evaluation tools, production tools, and related documentation. Users can find the installation packages, configuration files, and reference manuals needed to set up the development environment and perform MCU programming and testing.
---
### 1. Debugger
- **OpenOCD**  
- **JLink**  
---

### 2. Flash Algorithm

| MCU Family | Model           | Flash Size |
| ---------- | --------------- | ---------- |
| RT58x      | RT581 / RT582   | 1MB        |
| RT58x      | RT583           | 2MB        |
| RT584      | RT584S          | 1MB        |
| RT584      | RT584H / RT584L | 2MB        |

---
### 3. Rafael Extension

- `rafael-extension-0.0.1.vsix`

### 4. Requirement

  #### 1. VS Code Extensions

| Extension Name        | Publisher / ID                  | Description                               |
| --------------------- | ------------------------------- | ----------------------------------------- |
| Python                | ms-python.python                | Python development support                |
| Pylance               | ms-python.vscode-pylance        | Python intelligent suggestions & analysis |
| CMake Tools           | ms-vscode.cmake-tools           | CMake project support                     |
| C/C++                 | ms-vscode.cpptools              | C/C++ development support                 |
| VSCode Serial Monitor | ms-vscode.vscode-serial-monitor | Serial port monitoring tool               |
| Cortex-Debug          | marus25.cortex-debug            | Cortex-M MCU debugging support            |

  #### 2. Python Packages

| Package Name   | Version / Notes | Description                         |
| -------------- | --------------- | ----------------------------------- |
| cmake          | latest          | Build tool for C/C++ projects       |
| ninja          | latest          | Fast build system                   |
| kconfiglib     | latest          | Configuration file processing       |
| requests       | latest          | HTTP / network request support      |
| urllib3        | latest          | HTTP request support                |
| chardet        | latest          | Character encoding detection        |
| pyelftools     | latest          | ELF file parsing and processing     |
| windows-curses | latest          | Windows command-line curses support |

### 5. RT58x Tools Reference

| Tool Name            | Reference PDF File                                         |
| -------------------- | ---------------------------------------------------------- |
| IOT Evaluation Tool  | [Tool_01] RT58x_ISP_Tool_User_Guide_V1.9.pdf               |
| Mass Production Tool | [Tool_04] RT58x_MPTCB_and_MP_Tool_User_Guide_V1.10.pdf     |
| Security Boot Tool   | [Tool_03] RT58x_Security_Boot_Tool_User_Guide_V1.0.pdf     |
| UART Break Tool      | [Tool_07] Uart_Break_Tool_User_Guide_V1.0.pdf              |
| Zigbee Tool          | [Tool_09] RT58x_Zigbee_Gateway_PC_Tool_User_Guide_V1.1.pdf |



