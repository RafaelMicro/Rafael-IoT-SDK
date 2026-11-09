# VS Code Debug Reference

To start a debug session, use the Rafael extension. See
[Rafael Extension, "Debug"](sdk_extension.md#debug). This page lists the debug toolbar buttons
and panels that appear when a session is running.

## Debug toolbar

| Button | Function | Shortcut |
|---|---|---|
| Reset Device | Reset program execution | |
| Continue | Continue program execution | F5 |
| Step Over | Run the current line, then stop at the next line in this function | F10 |
| Step Into | Step into the called function, one line at a time | F11 |
| Step Out | Run until the current function returns | Shift+F11 |
| Restart | Restart the debug session | Ctrl+Shift+F5 |
| Stop | Stop the debug session | Shift+F5 |

## Debug panels

- **Variables** — shows the current variable values in scope. You can inspect or change them
  while the program runs.
- **Watch** — tracks the values of variables or expressions you select.
- **Call Stack** — shows the sequence of active function calls.
- **Breakpoints** — lists all breakpoints. You can enable or disable each one.
- **XPeripherals** — shows the values of MCU peripheral registers, by name. It reads the
  register names from the chip's SVD file.
- **Debug Console** — an interactive console for debugger commands and program output.

## Optional extras

These extras are not required.

- **Memory View** — a hex memory viewer. Use this if you want more than the Debug Console's
  `-exec x/10x <addr>` command. It needs the separate `mcu-debug.memory-view` extension.
- For the serial console, use the Rafael extension's own **Serial Monitor**. See
  [Rafael Extension, "Serial Monitor"](sdk_extension.md#serial-monitor). This does not need a
  separate extension.
