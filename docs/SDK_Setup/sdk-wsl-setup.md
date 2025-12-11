# Windows WSL（Windows Subsystem for Linux）


## Windows version requirements :  
  - Windows 10 version 2004 (Build 19041) or later
  - Windows 11  
  
#### Open Command Prompt Using the Run Dialog

---
   - Press Win + R on your keyboard. 
        → This opens the Run dialog box.
   - Type cmd in the text field.
   - Press Enter or click OK.
        → The Command Prompt window will open. 
<img src="../SDK_Setup/picture/wsl/wsl-win-r.jpg" alt="drawing" style="width:200px  height=200"/> 
---  

#### Check WSL

---
   - **wsl --version**   
---  
<img src="../SDK_Setup/picture/wsl/wsl-version.jpg" alt="drawing" style="width:200px  height=200"/> 

## Windows Command Line Install :

---
### List available distributions: 
   - **wsl --list --online**  
  
---  

<img src="../SDK_Setup/picture/wsl/wsl-list-online.jpg" alt="drawing" style="width:200px  height=200"/> 

---
### Install Linux: 
   - **wsl --install buntu**
---  

<img src="../SDK_Setup/picture/wsl/wsl-install.jpg" alt="drawing" style="width:200px  height=200"/> 


---
### Open VS Code : 
   - Press Extension Icon
   - Search WSL extension
   - Install WSL
---  

<img src="../SDK_Setup/picture/wsl/wsl-vscode-install-wsl1.jpg" alt="drawing" style="width:200px  height=200"/> 


---
### Remote Exploer : 

---  
<img src="../SDK_Setup/picture/wsl/wsl-remote-explorer.jpg" alt="drawing" style="width:200px  height=200"/> 

---

### Connect WSL : 
   - Press Ctrl+Shiht+P 
   - Select **WSL : Open Folder in WSL**
   - Open **SDK Folder**
---  

<img src="../SDK_Setup/picture/wsl/wsl-open-folder-wsl.jpg" alt="drawing" style="width:200px  height=200"/> 


---

### Install Rafael Extension : 

---
      code --install-extension tools/rafael_extension/rafael-extension-0.0.2.vsix
 <img src="../SDK_Setup/picture/wsl/wsl-install-rafael-extension-v2.jpg" alt="drawing" style="width:200px  height=200"/>

---  

---
     Run apt update whenever an error message occurs.
 <img src="../SDK_Setup/picture/wsl/wsl-install-rafaelextension-error.jpg" alt="drawing" style="width:200px  height=200"/>

---  

## Install Tool in Ubuntu Environment : 

---
  - **sudo apt update**  
 <img src="../SDK_Setup/picture/wsl/wsl-sudo-update.jpg" alt="drawing" style="width:200px  height=200"/>

---  

---
  - **sudo apt install -y python3 python3-pip**  
 <img src="../SDK_Setup/picture/wsl/wsl-sudo-update.jpg" alt="drawing" style="width:200px  height=200"/>

---  

---
  - **sudo apt update**  
  - **sudo apt install -y snapd**  
  - **sudo snap install powershell --classic**  
  
 <img src="../SDK_Setup/picture/wsl/wsl-sudo-snap-powerhsell.jpg" alt="drawing" style="width:200px  height=200"/>

---

---
  - **sudo apt update**  
  - **sudo apt install python3-tk**  
  - **sudo apt install python3-kconfiglib**  
  
   <img src="../SDK_Setup/picture/wsl/wsl-sudo-python3-kconfiglib.jpg" alt="drawing" style="width:200px  height=200"/>

   <img src="../SDK_Setup/picture/wsl/wsl-sudo-python3-tk.jpg" alt="drawing" style="width:200px  height=200"/>
---

## Start Rafael Extension :  

  - [Rafael Extension Operation](../SDK_Setup/sdk_extension.md) 


