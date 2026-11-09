# SDK Document Quick Started

Use this page to find the correct document for your task.

## Document classification

### Platform

1. [Platform_Getting_Started.md](Platform_Getting_Started.md) — development environment, architecture, directory layout, build/flash/debug, adding an example
2. [SDK_Application_Overview.md](SDK_Application_Overview.md) — application architecture, task/queue model, flash map, RTOS API
3. [SDK_Setup/sdk_setup.md](../SDK_Setup/sdk_setup.md) — how to install VS Code, the toolchain, and the Rafael extension
4. [SDK_Setup/sdk_extension.md](../SDK_Setup/sdk_extension.md) — how to use the Rafael extension

### Bootloader

1. Bootloader source code — `examples/bootloader-app/bootloader-app/` (`main.c`, `bootloader.c`,
   `isp.c`, `slip.c`). `components/platform/soc/<family>/<family>_bootloader/` holds only the
   linker script and startup code for it.
2. How it works, and how FOTA updates are applied — see
   [docs/Fota/RT58x_Bootloader_FOTA_Guide.md](../Fota/RT58x_Bootloader_FOTA_Guide.md).
3. How to flash it — use the Rafael extension's Download panel. If you have no debug probe, use
   ISP Lite instead. See [SDK_Setup/sdk_extension.md](../SDK_Setup/sdk_extension.md). A
   standalone ISP GUI tool is also available, under `tools/IOT_Evaluation_Tool/`.

### BLE

1. `docs/BLE/RT58x_BLE_SDK_User_Guide.md`
2. `docs/BLE/RT58x_BLE_SDK_Application_Guide.md`
3. `docs/BLE/RT58x_BLE_SDK_OTA_Guide.md`
4. `docs/BLE/RT58x_BLE_SDK_Service_Profile_Guide.md`

### BLE Mesh

- `docs/BLE/RT58x_BLE_Mesh_SDK_Application_Guide.md`
- `docs/BLE/RT58x_BLE_Mesh_Gateway_Command_Manual.md`

### Zigbee

- `docs/Zigbee/zigbee_application_note.md`
- `docs/Easy_Gateway/` — Easy Gateway command manual, PDF only

### Thread

- `docs/Thread/Thread_User_Guid.md`

### Multi-Protocol

- `docs/Multi_Protocol/RT58x_Multi_Protocol_Application_Guide.md`

### Sub-G

- `docs/SubG/README.md`

## Reference questions and answers

| Question | Answer |
|---|---|
| How do I flash the bootloader? | Use the Rafael extension's Download panel, or `tools/IOT_Evaluation_Tool/`. |
| How do I set up VS Code and start building? | See [SDK_Setup/sdk_setup.md](../SDK_Setup/sdk_setup.md) and [Platform_Getting_Started.md](Platform_Getting_Started.md). |
| Where is the EVK button and ICE connector layout? | See [Platform_Getting_Started.md](Platform_Getting_Started.md), section 1. |
| Where is the MCU memory layout? | See [Flash_Layout_Overview.md](Flash_Layout_Overview.md). |
| Where is the MCU flash programming algorithm? | See `tools/Flash_Programming_Algorithm/`. |
| How do I start a BLE application? | See `docs/BLE/RT58x_BLE_SDK_Application_Guide.md`. |
| How do I start a Zigbee application? | See `docs/Zigbee/zigbee_application_note.md`. |
| Where is the calibration tool? | See `tools/Mass_Production_Tool/` and `docs/MP/`. |
