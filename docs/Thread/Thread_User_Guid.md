# Thread User Guide

This document provides an overview of the Thread SDK architecture and points you to usage examples for different Thread device roles:

* [`ot-cli-ftd`](../../examples/thread/ot-cli-ftd): Full Thread Device
* [`ot-cli-mtd`](../../examples/thread/ot-cli-mtd-sleep): Minimal Thread Device
* [`ot-cli-ncp`](../../examples/thread/ot-cli-ncp): Network Co-Processor

---

## 📘 What is Thread?

[Thread](https://openthread.io/) is a low-power, secure, and IPv6-based wireless mesh networking protocol for IoT and smart home applications. It supports self-healing mesh topology, low-latency communication, and is designed to work reliably even in harsh RF environments.

---

## 🧱 SDK Architecture Overview

The Thread SDK typically contains the following example applications under `examples/thread/`:

```
examples/thread/
├── ot-cli-ftd/   # Full Thread Device example
├── ot-cli-mtd/   # Minimal Thread Device example
└── ot-cli-ncp/   # Network Co-Processor example
```

### Thread Device Types

| Type | Description                                                         | Suitable For                              |
| ---- | ------------------------------------------------------------------- | ----------------------------------------- |
| FTD  | Full Thread Device. Can become Leader, Router, or Child. Always-on. | Mains-powered, always-connected devices   |
| MTD  | Minimal Thread Device. Can only be a Child. Sleep-capable.          | Low-power, battery devices                |
| NCP  | Network Co-Processor. Controlled by external host.                  | Host/NCP architecture like Linux gateways |

---

## 🔍 ot-cli-ftd

`ot-cli-ftd` is a Full Thread Device example that can operate as a Leader, Router, or Child. It is always on and suitable for devices that need to maintain full mesh routing capabilities.

📄 Full usage guide: `examples/thread/ot-cli-ftd/README.md`

**Note:**

* Suitable for use as the network's root or routing node.
* Not suitable for low-power devices.

---

## 💥 ot-cli-mtd

`ot-cli-mtd` is a Minimal Thread Device example. MTDs are designed for low-power, battery-operated devices and only function as Child nodes.

📄 Full usage guide: `examples/thread/ot-cli-mtd-sleep/README.md`

**Note:**

* Use `ot pollperiod` to configure polling interval.
* MTD cannot route traffic and relies on FTD routers.

---

## 🔌 ot-cli-ncp

`ot-cli-ncp` is a Network Co-Processor example used when the Thread stack runs on a separate MCU (NCP), controlled by a host system via the Spinel protocol.

📄 Full usage guide: `examples/thread/ot-cli-ncp/README.md`

**Note:**

* Use tools like `wpantund` or `pyspinel` on the host to communicate with NCP.
* Ensure UART/SPI is properly configured between host and NCP.

### System Diagram

```
+----------------+         UART/SPI         +----------------+
|   Host System  | <----------------------> |   NCP (RT58x)  |
|  (Linux/RTOS)  |      Spinel Protocol     |  ot-cli-ncp     |
+----------------+                          +----------------+
```

---

## 📌 Related Resources

* [OpenThread CLI Reference](https://github.com/openthread/openthread/blob/main/src/cli/README.md)
* [OpenThread GitHub](https://github.com/openthread/openthread)
* [Thread Primer](https://openthread.io/guides/thread-primer)
* [Spinel Protocol](https://github.com/openthread/openthread/blob/main/src/lib/spinel)
