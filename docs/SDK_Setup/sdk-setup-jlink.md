# J-Link Setup Guide
## Recommended Versions

|Tool|Version|Notes|
| :-: | :-: | :-: |
|J-Link|v8.18(or later)|Recommended for flashing MCU firmware|

## 4. Download & Install J-Link

- J-Link Website: https://www.segger.com/downloads/jlink 

<img alt="drawing" src="picture/tool/DownloadJLinkPage.jpg" style="width:700px"/>

### J-Link Installation Instructions

1. Run the installer. You can keep the default options. Recommended settings:
   1. **Install J-Link Software and Documentation**: Checked
   1. **Adjusting your PATH environment**: Choose **Add J-Link to PATH**
   1. Keep other options as default
1. Click **Install** to start the installation.

### Verify Installed Version (Windows / Linux)

1. Open the command tool:
   1. **Windows** → Command Prompt
   1. **Linux** → Terminal
1. Type the command:

   ```
   JLinkExe
   ```

   Expected output:

   ```
   SEGGER J-Link Commander Vx.xx (Compiled Sep xx xx:xx:xx)
   ```

### Update JLinkDevices.xml

**Option A: Overwrite the File**

Copy the JLinkDevices.xml provided by the Rafael SDK into the J-Link installation directory, replacing the existing file:

```
copy "<SDK_PATH>\tools\JLink\JLinkDevices.xml" "C:\Program Files (x86)\SEGGER\JLink\JLinkDevices.xml"
```

> ⚠️ Caution: Make sure the Rafael-provided file already includes all other MCU definitions.
> Otherwise, overwriting the file may cause J-Link to lose support for other devices.

**Option B: Merge Manually (Recommended)**

If you want to preserve support for all other MCUs, manually merge only the `<Device>` entries for RT58x into the existing JLinkDevices.xml.

Steps:

1. Open the original `JLinkDevices.xml` in a text editor.
1. Open the `JLinkDevices.xml` from `<SDK_PATH>\tools\JLink\JLinkDevices.xml`.
1. Copy the `<Device>` entries related to RT58x RT584 series from the SDK file and paste them inside the `<Database>` section of the original file.
1. Save the file.

> ✅ This approach keeps support for all other devices while adding RT58x and RT584 series definitions.

### Reference the picture below:

<img src="picture/tool/jlink_devicexml.jpg" alt="drawing" style="width:700px"/>

---

## Configure J-Link Path in VS Code

Modify `.vscode/settings.json` to configure the J-Link path in VS Code.

Open your SDK folder, locate `.vscode/settings.json`, and add the following settings:

```json
{
  "cortex-debug.JLinkGDBServerPath": "C:/Program Files/SEGGER/JLink/JLinkGDBServerCL.exe"
}
```
